#pragma once
#include <string>
#include <list>

namespace bio {

namespace microkernel {

class Kernel;

/**
 * @brief 路由模块，可由用户自行实现
 * 
 */
class RouterExtend {
public:
	RouterExtend(Kernel* kernel);
	virtual ~RouterExtend();

	// 消息发送
	virtual int publish(const std::string* msg, bool sync);
	// 消息订阅
	virtual int subscribe(const std::string& topic);

	// 订阅消息回调接口，该接口将消息塞入消息队列，由路由模块的线程池处理
	// 消息队列满，则直接分发
	virtual void sub_msg_callback(void);

	// 消息分发线程
	virtual void msg_distribution_thread(Kernel* kernel);

private:
	Kernel* kernel_;
	int thread_loop_;		///< 线程池
	std::list<int> router_threads_;			///< 分发线程
	std::list<std::string> recv_msg_queue_;	///< 接受消息队列
};

} // namespace microkernel

} // namespace bio