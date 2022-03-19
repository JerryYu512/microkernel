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
 * @file extend.hpp
 * @brief 
 * @author Jerry.Yu (jerry.yu512@outlook.com)
 * @version 1.0.0
 * @date 2021-10-10
 * 
 * @copyright MIT License
 * 
 */
#pragma once
#include <string>
#include <stdint.h>
#include <functional>
#include "kernel_event.hpp"
#include "typedef.hpp"

namespace bio {

namespace microkernel {

typedef enum {
    kernel_msg_response,    ///< 响应
    kernel_msg_request,     ///< 请求消息
} MsgType;

typedef enum {
    QOS_L1,     ///< 最多一次送到
    QOS_L2,     ///< 至少一次送到
    QOS_L3,     ///< 准确一次送到
} MsgQos;

/**
 * @brief 默认订阅消息
 * 
 */
struct DefaultSubPubMsg {
	MsgType msg_type;								///< 消息类型
	MsgQos qos;										///< 消息QOS
	std::string topic;
	std::string msg_version;						///< 消息版本
	std::string compress;							///< 压缩算法，gzip
	uint64_t seq;									///< 消息序列
	void* msg_ctx;									///< 消息内容
	uint32_t msg_ctx_len;							///< 消息长度
	std::string res_id;								///< 资源id
	std::string res_type;							///< 资源类型
	std::string module_name;						///< 模块名称
	std::string msg_attr;							///< 消息特性：event-事件(pub用到)，attr-属性，service-服务(能力查询)，info-信息，upload-上传(pub)，result-结果
	std::string msg_method;							///< 消息方法：report-上报(通知)，query-查询(参数，信息)，set_reply-设置响应(参数类)，operate-reply-操作响应(对于控制类)
	void* user_data;								///< 用户数据
};

using DefaultSubMsg = DefaultSubPubMsg;
using DefaultPubMsg = DefaultSubPubMsg;

/**
 * @brief 模块间短连接消息rpc
 * 
 */
struct DefualtExtendMsg {
	std::string src_extend_name;	///< 源模块名称
	std::string src_extend_version;	///< 源模块版本
	std::string dst_extend_name;	///< 目标模块名称
	std::string dst_extend_version;	///< 目标模块版本
	void* req_msg;					///< 请求消息
	uint32_t req_msg_len;			///< 请求消息长度
	void* res_msg;					///< 应答消息
	uint32_t res_msg_len;			///< 应答消息长度
};

///< 广播消息
struct DefaultBroadcastMsg {
	std::string broadcast_name;		///< 谁广播的
	void* msg;						///< 广播消息
	uint32_t msg_len;				///< 消息长度
};

/**
 * @brief 插件信息
 * 
 */
struct ExtendInfo {
	std::string name_;			///< 插件名称
	std::string version_;		///< 插件版本
	void* user_data_;			///< 用户数据
};

// 插件定义
class Extend {
public:
	Extend(Kernel* kernel, const ExtendInfo& info) : kernel_(kernel), info_(info) {}
	Extend(Kernel* kernel, const std::string& name, const std::string& version, void* data=nullptr)
	{
		kernel_ = kernel;
		info_.name_ = name;
		info_.version_ = version;
		info_.user_data_ = data;
	}
	virtual ~Extend() {}

	virtual void init(void) = 0;
	virtual void finit(void) = 0;

	// 被驱动模块调用的消息
	virtual void user_yield(void) = 0;

	// 平台订阅消息回调处理
	virtual void submsg_router_callback(void* msg) = 0;
	// 模块间消息处理
	virtual void submsg_router_ipc(void* msg) = 0;
	// rpc短链接消息处理
	virtual void rpc_service(void* msg) = 0;
	// 内核消息通知
	virtual void event_notice(void* msg) = 0;
	// 广播消息处理
	virtual void broadcast_notice(void* msg) = 0;

	friend class Kernel;

protected:
	Kernel* kernel_;	///< 挂载内核
	ExtendInfo info_;	///< 消息信息
};

///< 分配发布消息
void* extend_alloc_pub_msg_ipc(Kernel* kernel, uint32_t msg_len);
///< 复制发布消息
void* extend_dup_msg_ipc(Kernel* kernel, void* msg);

///< 订阅
KernelError extend_subscribe_ipc(Kernel* kernel, const std::string& extend_name, const std::string& topic);
KernelError extend_unsubscribe_ipc(Kernel* kernel, const std::string& extend_name, const std::string& topic);
///< 发布
KernelError extend_publish_ipc(Kernel* kernel, void* msg);

///< 模块间rpc调用
KernelError extend_rpc(Kernel* kernel, void* msg);

///< 平台消息订阅
KernelError extend_subscribe(Kernel* kernel, extend_subscribe_callback cb);

// 平台消息发布
KernelError extend_pusmsg(Kernel *kernel, const void* msg);

// 消息广播
KernelError extend_broadcast_msg(Kernel* kernel, void* msg);

// 扩展模块检测
bool extend_check(Kernel* kernel, const std::string& name);

} // namespace microkernel

} // namespace bio
