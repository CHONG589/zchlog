#include "../include/Formatter.h"
#include "../include/LogSink.h"
#include "../include/Logger.h"
#include "../include/Log.h"

int main() {

	DEBUG("%s", "测试日志");
	INFO("%s", "测试日志");
	WARN("%s", "测试日志");
	ERROR("%s", "测试日志");
	FATAL("%s", "测试日志");

    return 0;
}
