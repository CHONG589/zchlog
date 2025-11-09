#include "../include/Logger.h"

void zch::Logger::Debug(const std::string& file, size_t line, const char* fmt, ...) {
	// 1. 判断当前日志能否输出
	if (LogLevel::Level::DEBUG < _limit_level) {
		return;
	}

	// 2. 形成有效载荷字符串
	va_list ap;
	va_start(ap, fmt);
	char* payload = NULL;
    // 第一个参数：存储格式化后的字符串
    // 第二个参数：是格式化字符串，包含要打印的文本和格式说明符
    // 第三个参数：vs_list 类型的可变参数列表
    // vasprintf 会根据 fmt 字符串和可变参数列表 ap 的内容动
    // 态分配足够的内存来存储格式化后的字符串，并将地址存储在 payload 
    // 指向的指针中。如果成功，它会返回格式化后的字符串的长度；如果失败，
    // 它会返回 -1。
	if (vasprintf(&payload, fmt, ap) == -1) {
		perror("vasprintf fail: ");
		return;
	}
	va_end(ap);

	// 3. 形成 LogMsg 结构体
	LogMsg msg(LogLevel::Level::DEBUG, _logger, file, line, std::string(payload));
	// 4. 形成日志消息字符串
	std::string log_message = _formatter->Format(msg);
	// 5. 将日志消息字符串进行落地
	log(log_message.c_str(), log_message.size());
	free(payload);
}

void zch::Logger::Info(const std::string& file, size_t line, const char* fmt, ...) {
	// 1. 判断当前日志能否输出
	if (LogLevel::Level::INFO < _limit_level) {
		return;
	}

	// 2. 形成有效载荷字符串
	va_list ap;
	va_start(ap, fmt);
	char* payload = NULL;
	if (vasprintf(&payload, fmt, ap) == -1) {
		perror("vasprintf fail: ");
		return;
	}
	va_end(ap);

	// 3. 形成LogMsg结构体
	LogMsg msg(LogLevel::Level::INFO, _logger, file, line, std::string(payload));
	// 4. 形成日志消息字符串
	std::string log_message = _formatter->Format(msg);
	// 5. 将日志消息字符串进行落地
	log(log_message.c_str(), log_message.size());
	free(payload);
}

void zch::Logger::Warn(const std::string& file, size_t line, const char* fmt, ...) {
	// 1. 判断当前日志能否输出
	if (LogLevel::Level::WARN < _limit_level) {
		return;
	}

	// 2. 形成有效载荷字符串
	va_list ap;
	va_start(ap, fmt);
	char* payload = NULL;
	if (vasprintf(&payload, fmt, ap) == -1) {
		perror("vasprintf fail: ");
		return;
	}
	va_end(ap);

	// 3. 形成LogMsg结构体
	LogMsg msg(LogLevel::Level::WARN, _logger, file, line, std::string(payload));
	// 4. 形成日志消息字符串
	std::string log_message = _formatter->Format(msg);
	// 5. 将日志消息字符串进行落地
	log(log_message.c_str(), log_message.size());
	free(payload);
}

void zch::Logger::Error(const std::string& file, size_t line, const char* fmt, ...) {
	// 1. 判断当前日志能否输出
	if (LogLevel::Level::ERROR < _limit_level) {
		return;
	}

	// 2. 形成有效载荷字符串
	va_list ap;
	va_start(ap, fmt);
	char* payload = NULL;
	if (vasprintf(&payload, fmt, ap) == -1) {
		perror("vasprintf fail: ");
		return;
	}
	va_end(ap);

	// 3. 形成LogMsg结构体
	LogMsg msg(LogLevel::Level::ERROR, _logger, file, line, std::string(payload));
	// 4. 形成日志消息字符串
	std::string log_message = _formatter->Format(msg);
	// 5. 将日志消息字符串进行落地
	log(log_message.c_str(), log_message.size());
	free(payload);
}

void zch::Logger::Fatal(const std::string& file, size_t line, const char* fmt, ...) {
	// 1. 判断当前日志能否输出
	if (LogLevel::Level::FATAL < _limit_level) {
		return;
	}

	// 2. 形成有效载荷字符串
	va_list ap;
	va_start(ap, fmt);
	char* payload = NULL;
	if (vasprintf(&payload, fmt, ap) == -1) {
		perror("vasprintf fail: ");
		return;
	}
	va_end(ap);

	// 3. 形成LogMsg结构体
	LogMsg msg(LogLevel::Level::FATAL, _logger, file, line, std::string(payload));
	// 4. 形成日志消息字符串
	std::string log_message = _formatter->Format(msg);

	// 5. 将日志消息字符串进行落地
	log(log_message.c_str(), log_message.size());
	free(payload);
}

void zch::SyncLogger::log(const char* data, size_t len) {
	std::unique_lock<std::mutex> ulk(_mtx);
	for (auto& sink : _sinks) {
		if (sink.get() != nullptr) {
			sink->log(data, len);
		}
	}
}

void zch::AsyncLogger::RealSink(Buffer& buf) {
	// 异步线程根据落地方向进行数据落地
	for (auto& sink : _sinks) {
		if (sink.get() != nullptr) {
			sink->log(buf.Start(), buf.ReadableSize());
		}
	}
}

zch::Logger::ptr zch::LocalLoggerBuilder::Build() {
	// 不能没有日志器名称
	assert(!_logger_name.empty());

	// 如果用户没有手动设置过格式化器，就进行构造一个默认的格式化器
	if (_formatter.get() == nullptr) {
		_formatter = std::make_shared<Formatter>();
	}

	// 如果用户没有手动设置过落地方向数组，就进行默认设置一个的落地到标准输出的格式化器
	if (_sinks.empty()) {
		_sinks.push_back(SinkFactory::create<StdOutSink>());
	}

	// 根据日志器的类型构造相应类型的日志器
	if (_logger_type == LoggerType::Async_Logger) {
		return std::make_shared<AsyncLogger>(_logger_name, _limit, _formatter, _sinks, _async_type);
	}
	return std::make_shared<SyncLogger>(_logger_name, _limit, _formatter, _sinks);
}

zch::Logger::ptr zch::GlobalLoggerBuilder::Build() {
	assert(!_logger_name.empty());
	if (_formatter.get() == nullptr) {
		_formatter = std::make_shared<Formatter>();
	}

	if (_sinks.empty()) {
		_sinks.push_back(SinkFactory::create<StdOutSink>());
	}

	Logger::ptr logger;
	if (_logger_type == LoggerType::Async_Logger) {
		logger = std::make_shared<AsyncLogger>(_logger_name, _limit, _formatter, _sinks, _async_type);
	} else {
		logger = std::make_shared<SyncLogger>(_logger_name, _limit, _formatter, _sinks);
	}

	LogManager::GetInstance().AddLogger(logger);

	return logger;
}
