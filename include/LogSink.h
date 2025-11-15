/**
 * @file LogSink.h
 * @brief 实现日志的输出方式，并支持输出方式的扩展
 *          - 标准输出
 *          - 指定文件
 *          - 滚动文件
 * @author zch
 * @date 2025-11-02
 */

#ifndef LOGSINK_H__
#define LOGSINK_H__

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <memory>
//#include <json/json.h>

#include "util.hpp"
//#include "../include/MySQLConn.h"

namespace zch {

    // 日志输出方式基类
	class LogSink {
	public:
		using ptr = std::shared_ptr<LogSink>;
		// 日志输出接口, data 为日志的真实地址, len 为日志的长度
		virtual void log(const char* data, size_t len) = 0;
		virtual ~LogSink() {};
	};

    // 标准输出
	class StdOutSink : public LogSink {
	public:
		void log(const char* data, size_t len) override {
			std::cout.write(data, len);
		}
	};

    // 指定文件
	class FileSink : public LogSink {
	public:
		// 创建文件并打开
		FileSink(const std::string& pathname);

		void log(const char* data, size_t len) override {
			_ofs.write(data, len);
		}

	private:
		std::string _pathname;
		std::ofstream _ofs;
	};

    // 滚动文件(这里按照文件大小进行滚动)
	class RollBySizeSink : public LogSink {
	public:
		// 创建文件并打开
		RollBySizeSink(const std::string& basename, size_t max_size);

		void log(const char* data, size_t len) override;

	private:
		// 得到要生成的日志文件的名称
		// 通过基础文件名 + 时间组成 + 计数器生成真正的文件名
		std::string GetFileName();

	private:
        // 基础文件名
		std::string _basename;
		// 文件大小限制
		size_t _max_size;
		// 当前文件的大小
		size_t _cur_size;
		std::ofstream _ofs;
		// 文件名称计数器(防止一秒之内创建多个文件时，使用同一个名称)
		size_t _name_cout;
	};

    // // MySQL 服务器中
	// class MySQLSink : public LogSink {
	// public:
	// 	MySQLSink();
	// 	void log(const char* data, size_t len) override;

	// private:
	// 	MySQLConn _conn;
	// };

    // 落地方向类的工厂类  (通过此工厂类实现对落地方向的可扩展性)
	class SinkFactory {
	public:
		template<class SinkType, class ...Args>
		static LogSink::ptr create(Args&&... args) {
			return std::make_shared<SinkType>(std::forward<Args>(args)...);
		}
	};
}

#endif
