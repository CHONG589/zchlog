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

    class LoggerBuilder {
	public:
		LoggerBuilder() :_logger_type(LoggerType::Sync_Logger)
			            , _limit(LogLevel::Level::DEBUG)
			            , _async_type(ASYNCTYPE::ASYNC_SAFE) {}

		// 开启非安全模式 
		void BuildEnableUnSafe() { _async_type = ASYNCTYPE::ASYNC_UN_SAFE; }

		// 构建日志器类型
		void BuildType(LoggerType logger_type = LoggerType::Sync_Logger) { _logger_type = logger_type; }

		// 构建日志器的名称
		void BuildName(const std::string& logger_name) { _logger_name = logger_name; }

		// 构建日志限制输出等级
		void BuildLevel(LogLevel::Level limit) { _limit = limit; }

		// 构建格式化器
		void BuildFormatter(const std::string& pattern = "[%d{%H:%M:%S}][%p][%f:%l]%m%n") {
			_formatter = std::make_shared<Formatter>(pattern);
		}

		// 构建落地方向数组
		template<class SinkType, class ...Args>
		void AddLogSink(Args&&... args) {
			LogSink::ptr sink = SinkFactory::create<SinkType>(std::forward<Args>(args)...);
			_sinks.push_back(sink);
		}

		// 构建日志器
		virtual Logger::ptr Build() = 0;

	protected:
		// 异步日志器的写入是否开启非安全模式
		ASYNCTYPE  _async_type;
		// 日志器的类型，同步 or 异步
		LoggerType _logger_type;
		// 日志器的名称 (每一个日志器的唯一标识)
		std::string _logger_name;
		// 日志限制输出等级
		LogLevel::Level _limit;
		// 格式化器
		Formatter::ptr	_formatter;
		// 日志落地方向数组
		std::vector<LogSink::ptr> _sinks;
	};

    // 局部日志器建造者
	class LocalLoggerBuilder : public LoggerBuilder {
	public:
		Logger::ptr Build() override;
	};

    // 日志器管理者
	class LogManager {
	public:
		// 得到实例化对象
		static LogManager& GetInstance() {
			static LogManager ins;
			return ins;
		}

		// 添加日志器
		void AddLogger(const Logger::ptr& logger) {
			if (HasLogger(logger->GetLoggerName())) {
				return;
			}

			std::unique_lock<std::mutex> ulk(_mtx_loggers);
			_loggers[logger->GetLoggerName()] = logger;
		}

		// 判断日志器集合中是否存在指定的日志器
		bool HasLogger(const std::string& logger_name) {
			std::unique_lock<std::mutex> ulk(_mtx_loggers);
			return _loggers.find(logger_name) != _loggers.end();
		}

		// 返回默认日志器
		const Logger::ptr& DefaultLogger() {
			return _default_logger;
		}

		// 返回指定日志器
		const Logger::ptr& GetLogger(const std::string& logger_name) {
			return _loggers[logger_name];
		}

	private:
		LogManager() {
			std::unique_ptr<LoggerBuilder> builder(new LocalLoggerBuilder());
			builder->BuildName("default");
			_default_logger = builder->Build();
			AddLogger(_default_logger);
		}

		LogManager(const LogManager&) = delete;

	private:
		// 用于保证_loggers(日志器对象集合)线程安全的锁
		std::mutex _mtx_loggers;
		// 默认 logger 日志器
		Logger::ptr _default_logger;
		// 日志器对象集合
		std::unordered_map<std::string, Logger::ptr> _loggers;
	};

    // 全局建造者,通过全局建造者建造出的对象会自动添加到 LogManager 对象中
	class GlobalLoggerBuilder : public LoggerBuilder {
	public:
		Logger::ptr Build() override;
	};
}

#endif
