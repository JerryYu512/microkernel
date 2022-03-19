#pragma once
#include <string>
#include <list>
#include <map>
#include <set>
#include <queue>
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>

namespace bio {

namespace microkernel {

class Kernel;

#define BIO_MICROKERNEL_IPC_URL "inipc:///bio_micro_ipc"
#define BIO_MICROKERNEL_IPC_TOPIC_PREFIX "/bio/microkernel/ipc"

/**
 * @brief 路由模块，可由用户自行实现
 * @details 需要实现是少2部分的消息，来自平台和内部
 */
class RouterExtend : public std::enable_shared_from_this<RouterExtend> {
public:
	RouterExtend(Kernel* kernel);
	virtual ~RouterExtend() {}

	virtual void init(void) = 0;
	virtual void finit(void) = 0;

	// 消息发送
	/**
	 * @brief 平台消息发布
	 * 
	 * @param msg 消息
	 * @param sync 同步消息
	 * @param tmout 超时时间
	 * @return int 
	 */
	virtual int publish(void* msg, bool sync, time_t tmout) = 0;

	/**
	 * @brief 平台消息订阅
	 * 
	 * @param topic 
	 * @return int 
	 */
	virtual int subscribe(const std::string& topic) = 0;
	virtual int unsubscribe(const std::string& topic) = 0;

	/**
	 * @brief 模块间消息内存分配
	 * 
	 * @param len 
	 * @return void* 
	 */
	virtual void* ipcmsg_alloc(uint32_t len) = 0;
	// 模块间消息内存释放
	virtual void ipcmsg_free(void* ptr) = 0;
	virtual void* ipcmsg_dup(void* ptr) = 0;
	// 模块间消息发送
	virtual int publish_ipc(void* msg, bool sync, time_t tmout) = 0;
	// 模块间消息订阅
	virtual int subscribe_ipc(const std::string& extend_name, const std::string& topic) = 0;
	virtual int unsubscribe_ipc(const std::string& extend_name, const std::string& topic) = 0;

	// rpc短链接调用
	virtual void rpc(void* msg) = 0;

	virtual void broadcast_msg(void* msg) = 0;

	// 订阅消息回调接口，该接口将消息塞入消息队列，由路由模块的线程池处理
	// 消息队列满，则直接分发
	virtual void sub_msg_callback(const std::string& name, void*) = 0;

	Kernel* kernel_;
};

///< 简单消息路由选项
struct RouterSimpleExtendOpts {
	uint64_t ipc_msg_max;				///< 内部消息缓存最大条数
	uint64_t ipc_single_msg_max_len;	///< 单条消息最大长度
	uint64_t ipc_single_extend_msg_max;	///< 单模块缓存的消息最大条数
	uint32_t ipc_parallel;				///< 并行数
	time_t ipc_msg_timeout;				///< 消息过期时间
	std::string ipc_url;				///< 内部url
	std::string access_url;				///< 接入url
	uint32_t access_parallel;			///< 并行数
	std::string access_user;			///< 接入用户
	std::string access_passwd;			///< 接入密码
	bool tls;							///< 加密使能
	std::string ca_file;
	std::string cer_file;
	std::string private_key_file;
	std::string private_key_pass;
};

class RouterSimpleExtend;

/**
 * @brief 简单订阅消息对象
 * 
 */
class IPCSimpleSubMsg {
public:
	IPCSimpleSubMsg(RouterSimpleExtend* router, void* msg)
	: router_(router),
	  msg_(msg) {}
	~IPCSimpleSubMsg();

	void *get_msg(void) { return msg_; }
private:
	RouterSimpleExtend* router_;
	void *msg_;			///< 消息实体
};

// first为订阅者的名称，后者为消息对象
// 消息对象为shared的原因为实现0拷贝，所有订阅者使用完后再进行释放
// using submsg_shared = std::pair<std::string, std::shared_ptr<IPCSimpleSubMsg>>;
struct submsg_shared {
	std::string name;
	std::shared_ptr<IPCSimpleSubMsg> msg;
};

///< 订阅消息队列
struct IPCSimpleSubMsgQueue {
	std::string topic;				///< 话题
	std::set<std::string> names;	///< 模块名称列表
	std::queue<submsg_shared> msgs;	///< 消息队列
};

///< ipc消息队列
class IPCSimpleMsgQueue {
public:
	IPCSimpleMsgQueue() : msg_cnt_(0) {}
	~IPCSimpleMsgQueue() {
		msgs_.clear();
	}

	void clear(void) {
		msgs_.clear();
		msg_cnt_ = 0;
	}

	// uint64_t msg_record_;				///< 消息累计
	std::atomic_uint64_t msg_cnt_;		///< ipc消息数量
	std::mutex mtx_;
	std::condition_variable cond_;
	std::map<std::string, std::shared_ptr<IPCSimpleSubMsgQueue>> msgs_;
};

///< 广播消息队列
class BroadcastSimpleQueue {
public:
	BroadcastSimpleQueue() {}
	~BroadcastSimpleQueue() {}

	std::mutex mtx_;
	std::condition_variable cond_;
	std::queue<void*> msgs_;		///< 广播队列
};

///< 简单路由扩展
class RouterSimpleExtend : public RouterExtend {
public:
	RouterSimpleExtend(Kernel* kernel, const RouterSimpleExtendOpts& opts)
	: RouterExtend(kernel),
	  opts_(opts),
	  stop_(false) {
	}

	virtual ~RouterSimpleExtend() {
		finit();
	}

	virtual void init(void) override {
		for (uint32_t i = 0; i < opts_.ipc_parallel; i++) {
			ipc_threads_.emplace_back([this](){
				msg_distribution_thread(this);
			});
		}
		broadcast_thread_ = std::thread([this](){
			msg_broadcast_thread(this);
		});
	}

	virtual void finit(void) override {
		stop_ = true;
		if (!ipc_threads_.empty()) {
			ipc_queue_.cond_.notify_all();
			KLOG_I("join ipc thread pool.\n");
			for (auto &item : ipc_threads_) {
				item.join();
			}
			ipc_threads_.clear();
		}
		broadcast_queue_.cond_.notify_all();
		KLOG_I("join broadcast thread.\n");
		broadcast_thread_.join();
		stop_ = false;
		ipc_queue_.clear();
	}

	virtual void* ipcmsg_alloc(uint32_t len) override {
		if (len > opts_.ipc_single_msg_max_len) {
			KLOG_E("alloc msg over max len, %u > %ld.\n", len, opts_.ipc_single_msg_max_len);
			return nullptr;
		}
		DefaultPubMsg* msg = new DefaultPubMsg;
		msg->msg_ctx = new char[len];
		memset(msg->msg_ctx, 0, len);
		msg->msg_ctx_len = len;

		return msg;
	}
	virtual void ipcmsg_free(void* ptr) override {
		if (!ptr) {
			return;
		}

		DefaultPubMsg* msg = (DefaultPubMsg*)ptr;
		if (msg->msg_ctx) {
			delete [] (char*)msg->msg_ctx;
		}
		delete msg;
	}
	virtual void* ipcmsg_dup(void* ptr) override {
		if (!ptr) {
			return nullptr;
		}

		DefaultPubMsg* src_msg = (DefaultPubMsg*)ptr;
		DefaultPubMsg* msg = (DefaultPubMsg*)ipcmsg_alloc(src_msg->msg_ctx_len);
		void *ctx_ptr = msg->msg_ctx;

		*msg = *src_msg;
		memcpy(ctx_ptr, src_msg->msg_ctx, src_msg->msg_ctx_len);
		msg->msg_ctx = ctx_ptr;

		return msg;
	}

	// 消息发送
	virtual int publish(void* msg, bool sync, time_t tmout) override {
		return 0;
	}
	// 消息订阅
	virtual int subscribe(const std::string& topic) override {
		return 0;
	}
	virtual int unsubscribe(const std::string& topic) override {
		return 0;
	}

	// 消息发送
	virtual int publish_ipc(void* msg, bool sync, time_t tmout) override;
	// 消息订阅
	virtual int subscribe_ipc(const std::string& extend_name, const std::string& topic) override;
	virtual int unsubscribe_ipc(const std::string& extend_name, const std::string& topic) override;

	virtual void rpc(void* msg) override;

	virtual void broadcast_msg(void* msg) override;

	// 订阅消息回调接口，该接口将消息塞入消息队列，由路由模块的线程池处理
	// 消息队列满，则直接分发
	virtual void sub_msg_callback(const std::string& name, void*) override;

	// 消息分发线程
	static void msg_distribution_thread(RouterSimpleExtend* router);
	static void msg_broadcast_thread(RouterSimpleExtend* router);

	RouterSimpleExtendOpts opts_;			///< 选项
	bool stop_;								///< 停止标记
	std::vector<std::thread> ipc_threads_;	///< ipc线程池
	std::thread broadcast_thread_;			///< 广播线程
	IPCSimpleMsgQueue ipc_queue_;
	BroadcastSimpleQueue broadcast_queue_;
};

} // namespace microkernel

} // namespace bio
