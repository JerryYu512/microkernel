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
 * @file kernel_event.hpp
 * @brief 
 * @author Jerry.Yu (jerry.yu512@outlook.com)
 * @version 1.0.0
 * @date 2021-10-10
 * 
 * @copyright MIT License
 * 
 */
#pragma once

namespace bio {

namespace microkernel {

#if 0
typedef enum : unsigned int {
    kOnline,                         	///< 设备上线
    kBreak,                          	///< 设备离线
    kSwitchover,                    	///< 接入地址切换
    kInvalidAuth,                   	///< 验证码不合规
    kRuntimeError,                  	///< 运行时错误
    kReconnectSuccess,              	///< 重连成功
    kHeartbeatIntervalChanged,     		///< 心跳间隔改变
} EventType;
#endif

/**
 * @brief 内核事件消息
 * 
 */
typedef struct {
    unsigned int type;             		///< 事件类型，由loop-extend去定义，如iot设备的话，@c EventType
										///< 需要个消息过滤器，loop-exend会受到自身的消息出去
    void *event_cxt;                    ///< 事件内容
	void *udata;						///< 用户数据
} EventMsg;

} // namespace microkernel

} // namespace ars
