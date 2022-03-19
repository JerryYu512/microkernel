#include "microkernel.hpp"
#include "router_externd.hpp"
#include "extend.hpp"

namespace bio {

namespace microkernel {

IPCSimpleSubMsg::~IPCSimpleSubMsg() {
	// 整个释放后，数量减一
	KLOG_T("delete message.\n");
	router_->ipc_queue_.msg_cnt_--;
	router_->ipcmsg_free(msg_);
	msg_ = nullptr;
}

RouterExtend::RouterExtend(Kernel* kernel) : kernel_(kernel) {
	KLOG_D("RouterExtend constructor.\n");
}

int RouterSimpleExtend::publish_ipc(void* msg, bool sync, time_t tmout)
{
	if (msg) {
		DefaultPubMsg *pubmsg = (DefaultPubMsg*)msg;
		std::unique_lock<std::mutex> lck(ipc_queue_.mtx_);
		std::string topic = pubmsg->topic;
		auto item_topic = ipc_queue_.msgs_.find(topic);
		// 没有订阅
		if (item_topic == ipc_queue_.msgs_.end()) {
			KLOG_E("no such topic - %s\n", topic.c_str());
			ipcmsg_free(msg);
			return -1;
		}
		// 超了
		if (ipc_queue_.msg_cnt_ >= opts_.ipc_msg_max) {
			KLOG_E("ipc msgs_cnt over max, %ld >= %ld\n", ipc_queue_.msg_cnt_.load(), opts_.ipc_msg_max);
			KLOG_I("distribute now.\n");
			for (auto &item : item_topic->second->names) {
				sub_msg_callback(item, msg);
			}
			ipcmsg_free(msg);
			return -1;
		}
		// 超了
		if (item_topic->second->msgs.size() >= opts_.ipc_single_extend_msg_max * item_topic->second->names.size()) {
			KLOG_E("ipc single msgs_cnt over max, %ld >= %ld\n", item_topic->second->msgs.size(), opts_.ipc_single_extend_msg_max * item_topic->second->names.size());
			ipcmsg_free(msg);
			return -1;
		}
		// 长度超了
		if (pubmsg->msg_ctx_len > opts_.ipc_single_msg_max_len) {
			KLOG_E("ipc msg_ctx_len over max, %u > %ld\n", pubmsg->msg_ctx_len, opts_.ipc_single_msg_max_len);
			ipcmsg_free(msg);
			return -1;
		}

		std::shared_ptr<IPCSimpleSubMsg> msg_temp(new IPCSimpleSubMsg(this, msg));
		for (auto &item : item_topic->second->names) {
			submsg_shared t = { item, msg_temp };
			item_topic->second->msgs.push(t);
		}
		ipc_queue_.msg_cnt_++;
		KLOG_D("ipc msg cnt = %ld\n", ipc_queue_.msg_cnt_.load());
		ipc_queue_.cond_.notify_all();
	}
	return 0;
}

int RouterSimpleExtend::subscribe_ipc(const std::string& extend_name, const std::string& topic)
{
	if (!kernel_->has_extend(extend_name)) {
		KLOG_E("has no such extend - %s\n", extend_name.c_str());
		return -1;
	}
	std::unique_lock<std::mutex> lck(ipc_queue_.mtx_);
	auto topic_item = ipc_queue_.msgs_.find(topic);
	if (topic_item != ipc_queue_.msgs_.end()) {
		topic_item->second->names.insert(extend_name);
	} else {
		KLOG_I("create new ipc topic - %s\n", topic.c_str());
		std::shared_ptr<IPCSimpleSubMsgQueue> q(new IPCSimpleSubMsgQueue);
		q->topic = topic;
		q->names.insert(extend_name);
		ipc_queue_.msgs_.insert(std::pair<std::string, std::shared_ptr<IPCSimpleSubMsgQueue>>(topic, q));
	}

	return 0;
}

int RouterSimpleExtend::unsubscribe_ipc(const std::string& extend_name, const std::string& topic)
{
	std::unique_lock<std::mutex> lck(ipc_queue_.mtx_);
	auto topic_item = ipc_queue_.msgs_.find(topic);
	if (topic_item != ipc_queue_.msgs_.end()) {
		topic_item->second->names.erase(topic);
		if (topic_item->second->names.empty()) {
			KLOG_I("delete ipc topic - %s\n", topic.c_str());
			ipc_queue_.msgs_.erase(topic_item);
		}
	}
	return 0;
}

void RouterSimpleExtend::broadcast_msg(void* msg)
{
	std::unique_lock<std::mutex> lck(broadcast_queue_.mtx_);
	broadcast_queue_.msgs_.push(msg);
	broadcast_queue_.cond_.notify_all();
}

void RouterSimpleExtend::msg_distribution_thread(RouterSimpleExtend* router)
{
	while (!router->stop_) {
		std::string name;
		std::shared_ptr<IPCSimpleSubMsg> msg;
		{
			std::unique_lock<std::mutex> lck(router->ipc_queue_.mtx_);
			router->ipc_queue_.cond_.wait(lck, [router]() -> bool {
				return router->ipc_queue_.msg_cnt_ > 0 || router->stop_;
			});
			for (auto &item : router->ipc_queue_.msgs_) {
				if (!item.second->msgs.empty()) {
					name = item.second->msgs.front().name;
					msg = item.second->msgs.front().msg;
					item.second->msgs.pop();
				}
			}
		}
		if (!name.empty()) {
			KLOG_I("call module[%s]\n", name.c_str());
			router->sub_msg_callback(name, msg->get_msg());
		}
	}
}

void RouterSimpleExtend::msg_broadcast_thread(RouterSimpleExtend* router) {
	while (!router->stop_)
	{
		std::unique_lock<std::mutex> lck(router->broadcast_queue_.mtx_);
		router->broadcast_queue_.cond_.wait(lck, [router]() -> bool {
			return !router->broadcast_queue_.msgs_.empty() || router->stop_;
		});
		void *msg = router->broadcast_queue_.msgs_.front();
		router->broadcast_queue_.msgs_.pop();
		lck.unlock();
		auto extends = router->kernel_->extends();
		for (auto &item : extends) {
			item.second->event_notice(msg);
		}
	}
}

void RouterSimpleExtend::sub_msg_callback(const std::string& name, void* msg)
{
	auto extend = kernel_->find_extend(name);
	if (extend) {
		extend->submsg_router_ipc(msg);
	}
}

void RouterSimpleExtend::rpc(void* msg) {
	if (!msg) {
		return;
	}

	DefualtExtendMsg* em = (DefualtExtendMsg*)msg;
	auto extend = kernel_->find_extend(em->dst_extend_name);
	if (extend) {
		extend->rpc_service(msg);
	}
}

} // namespace microkernel

} // namespace bio
