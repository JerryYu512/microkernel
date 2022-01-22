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
#include "extend.hpp"
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

	int start(void) {
		#if 0
		初始化驱动模块
		init driver extend
		sub msg
		// 初始化常规模块
		init common extend

		初始化路由模块线程池
		  
		#endif
	}

	void stop(void);

	// 添加模块
	void AddExtend(std::shared_ptr<Extend> extend) {
		if (!driver_extend_) {
			throw std::logic_error("driver extend must loading at first.");
		}

		if (extends_.end() != extends_.find(extend->info_.name_)) {
			throw std::invalid_argument("extend exsit.");
		}

		extends_[extend->info_.name_] = extend;
	}

	// init extend
	// fini extend

private:
	friend DriverExtend;
	void set_driver_extend(DriverExtend* driver) {
		if (driver_extend_) {
			throw std::logic_error("driver extend setted.");
		}
		driver_extend_ = driver;
	}

	DriverExtend* driver_extend_;	///< 驱动模块
	RouterExtend* router_extend_;	///< 路由模块
	// router_extend 路由模块
	std::map<std::string, std::shared_ptr<Extend>> extends_;	///< 扩展模块
};

} // namespace microkernel

} // namespace ars
