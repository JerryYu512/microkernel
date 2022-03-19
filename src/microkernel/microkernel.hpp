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
 * @file microkenel.hpp
 * @brief 
 * @author Jerry.Yu (jerry.yu512@outlook.com)
 * @version 1.0.0
 * @date 2021-10-10
 * 
 * @copyright MIT License
 * 
 */
#pragma once
#include <stdexcept>
#include <memory>
#include <map>
#include <sstream>
#include "extend.hpp"
#include "driver_extend.hpp"
#include "router_externd.hpp"

namespace bio {

namespace microkernel {

class DriverExtend;

class Kernel {
public:

	static const char* logo(void) {
		return R"(
    			_                           __                                  __
   ____ ___    (_)  _____   _____  ____    / /__  ___    _____   ____   ___    / /
  / __ `__ \  / /  / ___/  / ___/ / __ \  / //_/ / _ \  / ___/  / __ \ / _ \  / / 
 / / / / / / / /  / /__   / /    / /_/ / / ,<   /  __/ / /     / / / //  __/ / /  
/_/ /_/ /_/ /_/   \___/  /_/     \____/ /_/|_|  \___/ /_/     /_/ /_/ \___/ /_/   
                                                                                  
)";
	}

	Kernel() : stoped_(true) {}

	~Kernel() {
		stop();
	}

	// 启动
	int start(void) {
		if (!stoped_) {
			return -1;
		}
		KLOG_V("%s\n", logo());
		KLOG_I("bio-microkernel start...\n");
		stoped_ = false;
		KLOG_I("init driver-extend\n");
		driver_extend_->init();
		KLOG_I("init router-extend\n");
		router_extend_->init();
		for (auto &item : extends_) {
			KLOG_I("init extend - %s\n", item.first.c_str());
			item.second->init();
		}
		driver_extend_->loop();
		KLOG_I("bio-microkernel running...\n");

		return 0;
	}

	// 停止
	void stop(void) {
		if (stoped_) {
			return;
		}
		KLOG_I("bio-microkernel stop...\n");
		for (auto &item : extends_) {
			item.second->finit();
		}
		router_extend_->finit();
		driver_extend_->finit();
		stoped_ = true;
		KLOG_I("bio-microkernel stopped...\n");
	}

	// 添加模块
	void AddExtend(std::shared_ptr<Extend> extend) {
		if (!driver_extend_ || !router_extend_) {
			throw std::logic_error("driver/router extend must loading at first.");
		}

		if (extends_.end() != extends_.find(extend->info_.name_)) {
			std::stringstream ss;
			ss << "extend exsit [" << extend->info_.name_ << "]";
			throw std::invalid_argument(ss.str());
		}

		extends_[extend->info_.name_] = extend;
	}

	// 路由扩展
	std::shared_ptr<RouterExtend> router_extend(void) {
		return router_extend_;
	}

	// 是否有该扩展
	bool has_extend(const std::string& name) {
		return extends_.find(name) != extends_.end();
	}

	// 扩展模块列表
	const std::map<std::string, std::shared_ptr<Extend>>& extends(void) const {
		return extends_;
	}

	// 查找扩展模块
	std::shared_ptr<Extend> find_extend(const std::string& name) {
		auto item = extends_.find(name);
		if (item != extends_.end()) {
			return item->second;
		}
		return std::shared_ptr<Extend>(nullptr);
	}

	void set_driver_extend(std::shared_ptr<DriverExtend> driver) {
		if (driver_extend_) {
			throw std::logic_error("driver extend setted.");
		}
		driver_extend_ = driver;
	}
	void set_router_extend(std::shared_ptr<RouterExtend> router) {
		if (router_extend_) {
			throw std::logic_error("router extend setted.");
		}
		router_extend_ = router;
	}

private:
	friend DriverExtend;
	friend RouterExtend;
	std::shared_ptr<DriverExtend> driver_extend_;	///< 驱动模块
	std::shared_ptr<RouterExtend> router_extend_;	///< 路由模块
	std::map<std::string, std::shared_ptr<Extend>> extends_;	///< 扩展模块
	bool stoped_;
};

} // namespace microkernel

} // namespace bio
