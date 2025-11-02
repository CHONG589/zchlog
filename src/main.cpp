#include "../include/Formatter.h"
#include "../include/LogSink.h"

int main() {

	zch::LogMsg msg;
	msg._ctime = zch::Date::Now();
	msg._level = zch::LogLevel::Level::WARN;
	msg._file = "test.cpp";
	msg._line = 38;
	msg._logger = "root";
	msg._payload = "测试日志";
	zch::Formatter formatter("[%d{%H:%M:%S}][%p][%c][%f:%l]%m%n");

	std::string s = formatter.Format(msg);
	zch::LogSink::ptr log_file = zch::SinkFactory::create<zch::FileSink>("../logfile/test");
	//log_file->log(s.c_str(), s.size());

	zch::LogSink::ptr log_rollfile = zch::SinkFactory::create<zch::RollBySizeSink>("../logfile/test", 1024 * 1024 * 1024);
	log_rollfile->log(s.c_str(), s.size());

    return 0;
}
