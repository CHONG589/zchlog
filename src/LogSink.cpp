#include <cassert>

#include "../include/LogSink.h"

zch::FileSink::FileSink(const std::string& pathname)
	                    : _pathname(pathname) {
	// 1.检查路径是否存在,不存在就创建
	if (!zch::File::IsExist(zch::File::GetDirPath(_pathname))) {
		zch::File::CreateDirectory(zch::File::GetDirPath(_pathname));
	}

	// 2. 创建并打开文件
	_ofs.open(_pathname, std::ios::binary | std::ios::app);
	if (!_ofs.is_open()) {
		std::cerr << "FileSink中文件打开失败" << std::endl;
		abort();
	}
}

zch::RollBySizeSink::RollBySizeSink(const std::string& basename
                                    , size_t max_size)
	                                :_basename(basename)
                                    , _max_size(max_size)
                                    , _cur_size(0)
                                    , _name_cout(0) {
	// 1.检查路径是否存在,不存在就创建
	if (!zch::File::IsExist(zch::File::GetDirPath(_basename))) {
		zch::File::CreateDirectory(zch::File::GetDirPath(_basename));
	}

	// 2. 获得文件名
	std::string filename = GetFileName();

	// 3. 创建并打开文件
	_ofs.open(filename, std::ios::binary | std::ios::app);
	assert(_ofs.is_open());
}

void zch::RollBySizeSink::log(const char* data, size_t len) {
	// 判断文件是否超出大小
	if (_cur_size >= _max_size) {
		// 关闭旧文件
		_ofs.close();

		std::string filename = GetFileName();
		_ofs.open(filename, std::ios::binary | std::ios::app);
		assert(_ofs.is_open());
		// 由于是新文件，所以将当前文件已写入的大小置 0
		_cur_size = 0;
	}
	_ofs.write(data, len);
	_cur_size += len;
}

std::string zch::RollBySizeSink::GetFileName() {
	std::stringstream ssm;
	struct tm t = Date::GetTimeSet();
	ssm << _basename;
	ssm << '-';
	ssm << t.tm_year + 1900;
	ssm << t.tm_mon + 1;
	ssm << t.tm_mday;
	ssm << '_';
	ssm << t.tm_hour;
	ssm << t.tm_min;
	ssm << t.tm_sec;
	ssm << '-';
	ssm << std::to_string(_name_cout++);
	ssm << ".log";
	return ssm.str();
}

// zch::MySQLSink::MySQLSink() {
// 	Json::Value root;
// 	Json::String err;
// 	Json::CharReaderBuilder rbuilder;
// 	std::ifstream ifs;
// 	// 加载文件
// 	ifs.open(R"(/home/pan/item/MySQLConfig.json)", std::ios::binary);
// 	if (!ifs.is_open()) {
// 		std::cerr << "找不到配置文件" << std::endl;
// 		exit(-1);
// 	}

// 	// 解析Json
// 	if (!Json::parseFromStream(rbuilder, ifs, &root, &err)) {
// 		std::cout << err << std::endl;
// 		exit(-1);
// 	}

// 	uint16_t m_port = root["port"].asUInt();
// 	std::string m_host = root["host"].asString();
// 	std::string m_db_name = root["db_name"].asString();
// 	std::string m_user = root["user"].asString();
// 	std::string m_passwd = root["passwd"].asString();

// 	if (!_conn.connect(m_host, m_port, m_user, m_passwd, m_db_name)) {
// 		std::cerr << "数据库连接失败" << std::endl;
// 		exit(-1);
// 	}
// }

// void zch::MySQLSink::log(const char* data, size_t len) {
// 	std::string payload(data, len);
// 	//std::cout << payload << std::endl;
// 	std::stringstream ss(payload);
// 	std::string line;

// 	while (std::getline(ss, line, '\n')) {
// 		// 可能需要对 line 进行处理以防止 SQL 注入和处理特殊字符
// 		std::string sql = "INSERT INTO log(log) VALUES('" + line + "');";
// 		_conn.update(sql);
// 	}
// }
