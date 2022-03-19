/**
 * MIT License
 * 
 * Copyright © 2021 <Jerry.Yu>.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the “Software”), to deal in the Software without
 * restriction, including without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
 * 
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 * 
 * @file driver_extend.hpp
 * @brief 用户自定义的核心业务扩展模块
 * @author Jerry.Yu (jerry.yu512@outlook.com)
 * @version 1.0.0
 * @date 2021-10-10
 * 
 * @copyright MIT License
 * 
 */

#pragma once
#include <string.h>
#include <string>
#include <vector>
#include <memory>
#include <string>
#include <thread>
#include "typedef.hpp"

namespace bio {

namespace microkernel {

class Kernel;

/**
 * @brief 驱动模块
 * 
 */
class DriverExtend : public std::enable_shared_from_this<DriverExtend> {
public:
	// 添加自身到内核中
	DriverExtend(Kernel* kernel);
	virtual ~DriverExtend() {
		finit();
	}

public:
	// 驱动模块名称
	static const std::string driver_extend_name;
	// 驱动模块版本
	static const std::string driver_extend_version;

private:
	friend class Kernel;

	virtual void init(void);

	virtual void finit(void);

	// 由kernel调用，用户重载该接口来实现自身的微内核核心业务，如设备认证接入，公共消息广播
	virtual void loop(void);
	// 1. 接入，认证，重连
	// 2. 扩展模块user函数驱动

	// 定义基本的内核消息通知，如：内核上线，内核下线，扩展模块载入，扩展模块卸载，异常消息等
	virtual void kernel_msg(void* msg);

	virtual void loop_task(void);

private:
	Kernel* kernel_;
	ExtendStatus st_;
	bool stop_;
	std::thread loop_thread_;
};

} // namespace microkernel

} // namespace bio
