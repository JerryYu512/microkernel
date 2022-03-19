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
 * @file typedef.hpp
 * @brief 
 * @author Jerry.Yu (jerry.yu512@outlook.com)
 * @version 1.0.0
 * @date 2021-10-10
 * 
 * @copyright MIT License
 * 
 */
#pragma once
#include <stdio.h>
#include <string>
#include <time.h>
#include <functional>

namespace bio {

namespace microkernel {

static inline std::string debug_time_string(void) {
	char buf[64] = {0};
	time_t seconds = time(nullptr);
    struct tm tm_time;
    gmtime_r(&seconds, &tm_time);

	snprintf(buf, sizeof(buf), "%4d-%02d-%02d %02d:%02d:%02d", tm_time.tm_year + 1900,
                 tm_time.tm_mon + 1, tm_time.tm_mday, tm_time.tm_hour, tm_time.tm_min,
                 tm_time.tm_sec);
	
	return buf;
}

#define KLOG_V(fmt, args...) printf(fmt, ##args)
#define KLOG_T(fmt, args...) printf("%s [T][%s:%s:%d] " fmt, debug_time_string().c_str(), __FILE__, __FUNCTION__, __LINE__, ##args)
#define KLOG_D(fmt, args...) printf("%s [D][%s:%s:%d] " fmt, debug_time_string().c_str(), __FILE__, __FUNCTION__, __LINE__, ##args)
#define KLOG_I(fmt, args...) printf("%s [I][%s:%s:%d] " fmt, debug_time_string().c_str(), __FILE__, __FUNCTION__, __LINE__, ##args)
#define KLOG_W(fmt, args...) printf("%s [W][%s:%s:%d] " fmt, debug_time_string().c_str(), __FILE__, __FUNCTION__, __LINE__, ##args)
#define KLOG_E(fmt, args...) printf("%s [E][%s:%s:%d] " fmt, debug_time_string().c_str(), __FILE__, __FUNCTION__, __LINE__, ##args)
#define KLOG_F(fmt, args...) printf("%s [F][%s:%s:%d] " fmt, debug_time_string().c_str(), __FILE__, __FUNCTION__, __LINE__, ##args)

typedef enum {
	kCodeSuccess = 0,
} KernelError;

///< 状态
enum ExtendStatus {
	ST_NOT_INIT = 0,	///< 未初始化
	ST_INITED = 1,		///< 已初始化
	ST_STOPING = 2,		///< 停止中
	ST_STOPED = 3,		///< 已停止
	ST_RUNNING = 4,		///< 运行中
};

class Kernel;

using extend_subscribe_callback = std::function<void(void*)>;

} // namespace microkernel

} // namespace ars
