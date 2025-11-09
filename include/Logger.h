/**
 * @file Logger.h
 * @brief 日志器模块：1. 完成对日志落地模块，格式化模块，日志消息模块的封装；2. 日志器模块能够完成不同的落地方式，因此根据基类日志器派生出同步日志器和异步日志器
 * @author zch
 * @date 2025-11-09
 */

#ifndef LOGGER_H__
#define LOGGER_H__

#include <vector>
#include <unordered_map>
#include <mutex>
#include <atomic>
#include <cstdarg>

#include "LogLevel.hpp"
#include "Formatter.h"
#include "LogSink.h"
#include "AsynLopper.h"

namespace zch {

    class Logger {
    public:
        using ptr = std::shared_ptr<Logger>;
        Logger(const std::string logger
                , LogLevel::Level level
                , Formatter::ptr formatter
                , std::vector<LogSink::ptr> sinks)
			    : _logger(logger)
                , _limit_level(level)
                , _formatter(formatter)
                , _sinks(sinks) {}

        // 以 Debug 等级进行输出
		void Debug(const std::string& file, size_t line, const char* fmt, ...);
        
        // 以 Info 等级进行输出
		void Info(const std::string& file, size_t line, const char* fmt, ...);

        // 以 Warn 等级进行输出
		void Warn(const std::string& file, size_t line, const char* fmt, ...);

		// 以 Error 等级进行输出
		void Error(const std::string& file, size_t line, const char* fmt, ...);

		// 以 Fatal 等级进行输出
		void Fatal(const std::string& file, size_t line, const char* fmt, ...);

        const std::string& GetLoggerName() {
            return _logger;
        }

    protected:
        // 通过 log 接口让不同的日志器支持同步落地或者异步落地
		virtual void log(const char* data, size_t len) = 0;

    protected:
        // 保护日志落地的锁
        std::mutex _mtx;
        // 日志器名称
        std::string _logger;
        // 日志限制等级
        LogLevel::Level _limit_level;
        // 格式化器
        Formatter::ptr _formatter;
        // 落地方向集合
        std::vector<LogSink::ptr> _sinks; 
    };

    // 同步日志器
    class SyncLogger : public Logger {
    public:
        SyncLogger(const std::string logger
                    , LogLevel::Level level
                    , Formatter::ptr formatter
                    , std::vector<LogSink::ptr> sinks)
			        : Logger(logger, level, formatter, sinks) {}

    protected:
        void log(const char* data, size_t len) override;  
    };

    // 异步日志器
    class AsyncLogger : public Logger {
	public:
		AsyncLogger(const std::string logger
                    , LogLevel::Level level
                    , Formatter::ptr formatter
                    , std::vector<LogSink::ptr> sinks
                    , ASYNCTYPE type)
			        : Logger(logger, level, formatter, sinks)
			        , _lopper(std::bind(&AsyncLogger::RealSink, this, std::placeholders::_1)
                    , type) {}
    
	protected:
		void log(const char* data, size_t len) override {
			// 将数据放入异步缓冲区(这个接口是线程安全的因此不需要加锁)
			_lopper.Push(data, len);
		}

		// 异步线程调用此函数，用于真正地将数据落地
		void RealSink(Buffer& buf);

	protected:
		// 异步工作器
		AsynLopper _lopper;
	};

    // 使用建造者模式建造日志器，简化日志器的构建，降低用户的使用复杂度定义一个建造
    // 者基类：
	// 类里面 build() 函数会根据用户传入的类型来进行构建同步日志器或异步日志器
	// 根据基类派生出子类建造类，通过不同的子类分别构建：局部日志器和全局日志器

	// 日志器类型，根据传入的类型不同来决定是构造同步日志器，还是异步日志器
    enum class LoggerType {
		// 同步日志器
		Sync_Logger,
		// 异步日志器
		Async_Logger
	};
}

#endif
