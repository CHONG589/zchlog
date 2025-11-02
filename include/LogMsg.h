/**
 * @file LogMsg.h
 * @brief 日志消息类
 * @author zch
 * @date 2025-11-02
 */

#ifndef LOGMSG_H__
#define LOGMSG_H__

#include <ctime>
#include <string>
#include <thread>

#include "LogLevel.hpp"
#include "util.hpp"

namespace zch {

    struct LogMsg {
		time_t _ctime;				// 时间戳
		LogLevel::Level _level;		// 日志等级
		std::string _logger;		// 日志器名称
		std::thread::id _tid;		// 线程id
		std::string _file;			// 源码文件名
		size_t _line;				// 源码行号
		std::string _payload;		// 有效载荷
		LogMsg() {}

		LogMsg(LogLevel::Level level, const std::string logger, const std::string file,
			size_t line, const std::string payload)
			: _ctime(Date::Now())
			, _level(level)
			, _logger(logger)
			, _tid(std::this_thread::get_id())
			, _file(file)
			, _line(line)
			, _payload(payload) {}
	};
}

#endif
