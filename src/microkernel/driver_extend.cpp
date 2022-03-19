
#include "driver_extend.hpp"
#include "microkernel.hpp"
#include "extend.hpp"

namespace bio {

namespace microkernel {

const std::string DriverExtend::driver_extend_name = "bio#microkernel#driver";
const std::string DriverExtend::driver_extend_version = "1.0.0";

DriverExtend::DriverExtend(Kernel* kernel)
	: kernel_(kernel), st_(ST_NOT_INIT), stop_(false) {
	KLOG_D("DriverExtend constructor.\n");
}

void DriverExtend::init(void) {
	#if 0
	1. 基本接入验证
	2. 订阅平台消息
	3. 订阅内部消息
	#endif
	KLOG_T("DriverExtend init\n");
	st_ = ST_INITED;
}

void DriverExtend::finit(void) {
	if (st_ != ST_NOT_INIT || st_ != ST_STOPED || st_ != ST_STOPING) {
		st_ = ST_STOPING;
		KLOG_T("DriverExtend stopping\n");
		stop_ = true;
		loop_thread_.join();
		st_ = ST_STOPED;
		stop_ = false;
		KLOG_T("DriverExtend stopped\n");
	}
}

void DriverExtend::loop_task(void) {
	auto extends = kernel_->extends();

	st_ = ST_RUNNING;
	while (!stop_) {
		for (auto &item : extends) {
			item.second->user_yield();
		}
	}
	KLOG_T("loop task exit\n");
	// st_ = ST_STOPED;
	// stop_ = false;
}

void DriverExtend::loop(void) {
	if (st_ != ST_INITED) {
		return;
	}
	loop_thread_ = std::thread(std::bind(&DriverExtend::loop_task, this));
}

void DriverExtend::kernel_msg(void* msg) {
	// TODO:
}

} // namespace microkernel

} // namespace bio
