#include "../include/Formatter.h"
#include "../include/LogSink.h"
#include "../include/Logger.h"

int main() {

	zch::LocalLoggerBuilder local_builder;
	// 切记日志器不能没有名字！
	local_builder.BuildName("local_logger");
	// 构建一个同步日志器
	local_builder.BuildType(LoggerType::Sync_Logger)
	// 直接构造日志器
	zch::Logger::ptr default_logger = local_builder.Build();
	// 进行日志输出
	default_logger->Error(__FILE__, __LINE__, "测试日志器建造者类");

    return 0;
}
