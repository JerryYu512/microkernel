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
struct DefaultSubMsg {
	MsgType msg_type;								///< 消息类型
	MsgQos qos;										///< 消息QOS
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

/**
 * @brief 默认发布消息
 * 
 */
struct DefaultPubMsg {
	MsgType msg_type;								///< 消息类型
	MsgQos qos;										///< 消息QOS
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

/**
 * @brief 模块间短连接消息
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
// template <typename SubMsg, typename PubMsg>
class Extend {
public:
	Extend(Kernel* kernel, const ExtendInfo& info);
	Extend(Kernel* kernel, const std::string& name, const std::string& info, void* data=nullptr);
	virtual ~Extend();

	// 订阅消息路由
	virtual void submsg_router(void* msg);
	// 内核消息通知
	virtual void event_notice(void* msg);

	friend class Kernel;

private:
	Kernel* kernel_;	///< 挂载内核
	ExtendInfo info_;	///< 消息信息
};

// TODO:对插件进行偏特化

// 消息发布
// template <typename PubMsg>
KernelError extend_pusmsg(Kernel *kernel, const void* msg);

// 短连接消息发送
// template <typename SendMsg>
KernelError extend_sendmsg(Kernel *kernel, const void* msg);

// TODO:
// 1.广播消息
// 2.广播通知消息
// 3.扩展发现查询
// 4.消息重定向

} // namespace microkernel

} // namespace ars
