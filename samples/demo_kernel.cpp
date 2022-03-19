#include <string.h>
#include "microkernel/microkernel.hpp"
#include "microkernel/driver_extend.hpp"
#include "microkernel/router_externd.hpp"
#include "microkernel/extend.hpp"

using namespace bio;
using namespace bio::microkernel;

class Ext1 : public Extend {
public:
	Ext1(Kernel* kernel)
	: Extend(kernel, "ext1", "v1.0.0") 
	{
	}

	virtual void init(void) override {
		extend_subscribe_ipc(kernel_, info_.name_, "/testipc/ext2");
		extend_subscribe_ipc(kernel_, info_.name_, "/testipc/ext3");
	}

	virtual void finit(void) override {
		extend_unsubscribe_ipc(kernel_, info_.name_, "/testipc/ext2");
		extend_unsubscribe_ipc(kernel_, info_.name_, "/testipc/ext3");
	}

	// 被驱动模块调用的消息
	virtual void user_yield(void) override {
		static int i = 0;
		char push_msg[128] = "";
		sprintf(push_msg, "I am Ext1 - %d", i);
		i++;
		DefaultPubMsg *msg = (DefaultPubMsg*)extend_alloc_pub_msg_ipc(kernel_, (uint32_t)strlen(push_msg) + 1);
		memcpy(msg->msg_ctx, push_msg, strlen(push_msg));
		msg->topic = "/testipc/ext1";
		msg->module_name = info_.name_;
		extend_publish_ipc(kernel_, msg);
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	// 平台订阅消息回调处理
	virtual void submsg_router_callback(void* msg) override {
		
	}

	// 模块间消息处理
	virtual void submsg_router_ipc(void* msg) override {
		DefaultSubMsg* submsg = (DefaultSubMsg*)msg;
		KLOG_I("recv from [%s] : topic[%s], message[%s]\n", submsg->module_name.c_str(), submsg->topic.c_str(), (char*)submsg->msg_ctx);
	}

	// rpc短链接消息处理
	virtual void rpc_service(void* msg) override {
		DefualtExtendMsg* rpcmsg = (DefualtExtendMsg*)msg;
		KLOG_I("recv from [%s] :, message[%s]\n", rpcmsg->src_extend_name.c_str(), (char*)rpcmsg->req_msg);
		rpcmsg->res_msg = malloc(1024);
		rpcmsg->req_msg_len = 1024;
		std::string res = "rpc res : ";
		res += info_.name_;
		snprintf((char*)rpcmsg->req_msg, rpcmsg->req_msg_len, "%s", res.c_str());
		rpcmsg->res_msg_len = res.size();
	}

	// 内核消息通知
	virtual void event_notice(void* msg) override {

	}

	// 广播消息处理
	virtual void broadcast_notice(void* msg) override {
		DefaultBroadcastMsg* bmsg = (DefaultBroadcastMsg*)msg;
		KLOG_I("recv from [%s] :, message[%s]\n", bmsg->broadcast_name.c_str(), (char*)bmsg->msg);
	}

	virtual ~Ext1() {}
};

class Ext2 : public Extend {
public:
	Ext2(Kernel* kernel)
	: Extend(kernel, "ext2", "v1.0.0") 
	{

	}
	virtual void init(void) override {
		extend_subscribe_ipc(kernel_, info_.name_, "/testipc/ext1");
		extend_subscribe_ipc(kernel_, info_.name_, "/testipc/ext3");
	}

	virtual void finit(void) override {
		extend_unsubscribe_ipc(kernel_, info_.name_, "/testipc/ext1");
		extend_unsubscribe_ipc(kernel_, info_.name_, "/testipc/ext3");
	}

	// 被驱动模块调用的消息
	virtual void user_yield(void) override {
		static int i = 0;
		char push_msg[128] = "";
		sprintf(push_msg, "I am Ext2 - %d", i);
		i++;
		DefaultPubMsg *msg = (DefaultPubMsg*)extend_alloc_pub_msg_ipc(kernel_, (uint32_t)strlen(push_msg) + 1);
		memcpy(msg->msg_ctx, push_msg, strlen(push_msg));
		msg->topic = "/testipc/ext2";
		msg->module_name = info_.name_;
		extend_publish_ipc(kernel_, msg);
		// std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	// 平台订阅消息回调处理
	virtual void submsg_router_callback(void* msg) override {
		
	}

	// 模块间消息处理
	virtual void submsg_router_ipc(void* msg) override {
		DefaultSubMsg* submsg = (DefaultSubMsg*)msg;
		KLOG_I("recv from [%s] : topic[%s], message[%s]\n", submsg->module_name.c_str(), submsg->topic.c_str(), (char*)submsg->msg_ctx);
	}

	// rpc短链接消息处理
	virtual void rpc_service(void* msg) override {
		DefualtExtendMsg* rpcmsg = (DefualtExtendMsg*)msg;
		KLOG_I("recv from [%s] :, message[%s]\n", rpcmsg->src_extend_name.c_str(), (char*)rpcmsg->req_msg);
		rpcmsg->res_msg = malloc(1024);
		rpcmsg->req_msg_len = 1024;
		std::string res = "rpc res : ";
		res += info_.name_;
		snprintf((char*)rpcmsg->req_msg, rpcmsg->req_msg_len, "%s", res.c_str());
		rpcmsg->res_msg_len = res.size();
	}

	// 内核消息通知
	virtual void event_notice(void* msg) override {

	}

	// 广播消息处理
	virtual void broadcast_notice(void* msg) override {
		DefaultBroadcastMsg* bmsg = (DefaultBroadcastMsg*)msg;
		KLOG_I("recv from [%s] :, message[%s]\n", bmsg->broadcast_name.c_str(), (char*)bmsg->msg);
	}

	virtual ~Ext2() {}
};

class Ext3 : public Extend {
public:
	Ext3(Kernel* kernel)
	: Extend(kernel, "ext3", "v1.0.0") 
	{

	}
	virtual void init(void) override {
		extend_subscribe_ipc(kernel_, info_.name_, "/testipc/ext1");
		extend_subscribe_ipc(kernel_, info_.name_, "/testipc/ext2");
	}

	virtual void finit(void) override {
		extend_unsubscribe_ipc(kernel_, info_.name_, "/testipc/ext1");
		extend_unsubscribe_ipc(kernel_, info_.name_, "/testipc/ext2");
	}

	// 被驱动模块调用的消息
	virtual void user_yield(void) override {
		static int i = 0;
		char push_msg[128] = "";
		sprintf(push_msg, "I am Ext3 - %d", i);
		i++;
		DefaultPubMsg *msg = (DefaultPubMsg*)extend_alloc_pub_msg_ipc(kernel_, (uint32_t)strlen(push_msg) + 1);
		memcpy(msg->msg_ctx, push_msg, strlen(push_msg));
		msg->topic = "/testipc/ext3";
		msg->module_name = info_.name_;
		extend_publish_ipc(kernel_, msg);
		// std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	// 平台订阅消息回调处理
	virtual void submsg_router_callback(void* msg) override {
		
	}

	// 模块间消息处理
	virtual void submsg_router_ipc(void* msg) override {
		DefaultSubMsg* submsg = (DefaultSubMsg*)msg;
		KLOG_I("recv from [%s] : topic[%s], message[%s]\n", submsg->module_name.c_str(), submsg->topic.c_str(), (char*)submsg->msg_ctx);
	}

	// rpc短链接消息处理
	virtual void rpc_service(void* msg) override {
		DefualtExtendMsg* rpcmsg = (DefualtExtendMsg*)msg;
		KLOG_I("recv from [%s] :, message[%s]\n", rpcmsg->src_extend_name.c_str(), (char*)rpcmsg->req_msg);
		rpcmsg->res_msg = malloc(1024);
		rpcmsg->req_msg_len = 1024;
		std::string res = "rpc res : ";
		res += info_.name_;
		snprintf((char*)rpcmsg->req_msg, rpcmsg->req_msg_len, "%s", res.c_str());
		rpcmsg->res_msg_len = res.size();
	}

	// 内核消息通知
	virtual void event_notice(void* msg) override {

	}

	// 广播消息处理
	virtual void broadcast_notice(void* msg) override {
		DefaultBroadcastMsg* bmsg = (DefaultBroadcastMsg*)msg;
		KLOG_I("recv from [%s] :, message[%s]\n", bmsg->broadcast_name.c_str(), (char*)bmsg->msg);
	}

	virtual ~Ext3() {}
};

int main(void) {
	RouterSimpleExtendOpts opts = {
		2048,
		128,
		128 * 1024 * 1024,
		6,
		60 * 60,
	};

	Kernel kernel;
	std::shared_ptr<DriverExtend> driver(new DriverExtend(&kernel));
	std::shared_ptr<RouterSimpleExtend> router(new RouterSimpleExtend(&kernel, opts));

	kernel.set_driver_extend(driver);
	kernel.set_router_extend(router);
	kernel.AddExtend(std::make_shared<Ext1>(&kernel));
	kernel.AddExtend(std::make_shared<Ext2>(&kernel));
	kernel.AddExtend(std::make_shared<Ext3>(&kernel));

	kernel.start();

	while (true) {
		std::this_thread::sleep_for(std::chrono::seconds(5));
	}

	kernel.stop();

	return 0;
}
