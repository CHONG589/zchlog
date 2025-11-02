#include "../include/Formatter.h"

int main() {

    zch::LogMsg msg;
	msg._ctime = zch::Date::Now();
	msg._level = zch::LogLevel::Level::WARN;
	msg._file = "test.cpp";
	msg._line = 38;
	msg._logger = "root";
	msg._payload = "测试日志";
	zch::Formatter formatter("%d{%H:%M:%S}[%p][%c][%f:%l]%m%n");
	std::cout << formatter.Format(msg) << std::endl;

    return 0;
}
