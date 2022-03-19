#include "extend.hpp"
#include "microkernel.hpp"
#include "router_externd.hpp"

namespace bio {

namespace microkernel {

KernelError extend_subscribe_ipc(Kernel* kernel, const std::string& extend_name, const std::string& topic)
{
	if (kernel) {
		KLOG_D("extend - %s subscribe topic - %s\n", extend_name.c_str(), topic.c_str());
		kernel->router_extend()->subscribe_ipc(extend_name, topic);
	}
	return kCodeSuccess;
}

KernelError extend_unsubscribe_ipc(Kernel* kernel, const std::string& extend_name, const std::string& topic)
{
	if (kernel) {
		KLOG_D("extend - %s unsubscribe topic - %s\n", extend_name.c_str(), topic.c_str());
		kernel->router_extend()->unsubscribe_ipc(extend_name, topic);
	}
	return kCodeSuccess;
}

KernelError extend_publish_ipc(Kernel* kernel, void* msg)
{
	if (kernel) {
		kernel->router_extend()->publish_ipc(msg, true, -1);
	}
	return kCodeSuccess;
}

///< 分配发布消息
void* extend_alloc_pub_msg_ipc(Kernel* kernel, uint32_t msg_len) {
	if (kernel) {
		return kernel->router_extend()->ipcmsg_alloc(msg_len);
	}
	return nullptr;
}

///< 复制发布消息
void* extend_dup_msg_ipc(Kernel* kernel, void* msg) {
	if (kernel) {
		return kernel->router_extend()->ipcmsg_dup(msg);
	}
	return nullptr;
}

KernelError extend_rpc(Kernel* kernel, void* msg) {
	kernel->router_extend()->rpc(msg);
	return kCodeSuccess;
}

// 扩展模块检测
bool extend_check(Kernel* kernel, const std::string& name) {
	return kernel->has_extend(name);
}

KernelError extend_broadcast_msg(Kernel* kernel, void* msg) {
	kernel->router_extend()->broadcast_msg(msg);
	return kCodeSuccess;
}

///< 平台消息订阅
KernelError extend_subscribe(Kernel* kernel, extend_subscribe_callback cb);

// 平台消息发布
KernelError extend_pusmsg(Kernel *kernel, const void* msg);

} // namespace microkernel
	
} // namespace bio
