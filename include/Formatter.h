/**
 * @file Formatter.h
 * @brief 日志输出格式化类
 * @author zch
 * @date 2025-11-02
 */

#ifndef FORMATTER_H__
#define FORMATTER_H__

#include <iostream>
#include <sstream>
#include <memory>
#include <ctime>
#include <vector>
#include <cassert>
#include <unordered_set>

#include "LogMsg.h"

/**
 * 日志格式如：%d{%H:%M:%S}%T[%t]%T[%p]%T[%c]%T%f:%l%T%m%n
 * 
 * 日志格式符               描述
 *     %d                  日期
 *     %T                  缩进
 *     %t                 线程 id
 *     %p                 日志级别
 *     %c                日志器名称
 *     %f                  文件名
 *     %l                  行号
 *     %m                 日志消息
 *     %n                  换行
 */

namespace zch {

    // 格式化基类
    class FormatItem {
	public:
		using ptr = std::shared_ptr<FormatItem>;
		virtual void Format(std::ostream& out, const LogMsg& msg) = 0;
	};

    // 日期格式化子项
	class TimeFormatItem : public FormatItem {
	public:
		// 日期格式化子项比较特殊，我们在使用时还需要指定时分秒的格式
		TimeFormatItem(const std::string& fmt = "%H:%M:%S") : _time_fmt(fmt) {}

		// 重写父类的格式化接口
		void Format(std::ostream& oss, const LogMsg& msg) override {
			struct tm t;
			char buf[32] = { 0 };
			// 使用C库函数对时间戳进行格式化
			localtime_r(&msg._ctime, &t);
            // 将结构体 t 中存储的时间按照 _time_fmt 的格式进行存储到 buf 中。 
			strftime(buf, sizeof(buf), _time_fmt.c_str(), &t);
			// 放入流中
			oss << buf;
		}
	private:
		std::string _time_fmt;
	};

    // 日志等级格式化子项
	class LevelFormatItem : public FormatItem {
	public:
		void Format(std::ostream& oss, const LogMsg& msg) override {
			// 提取指定字段插入流中
			oss << LogLevel::ToString(msg._level);
		}
	};

    // 日志器名称格式化子项
	class LoggerFormatItem : public FormatItem {
	public:
		void Format(std::ostream& oss, const LogMsg& msg) override {
			// 提取指定字段插入流中
			oss << msg._logger;
		}
	};

    // 线程id格式化子项
	class ThreadFormatItem : public FormatItem {
	public:
		void Format(std::ostream& oss, const LogMsg& msg) override {
			// 提取指定字段插入流中
			oss << msg._tid;
		}
	};

    // 文件名称格式化子项
	class FileFormatItem : public FormatItem {
	public:
		void Format(std::ostream& oss, const LogMsg& msg) override {
			// 提取指定字段插入流中
			oss << msg._file;
		}
	};

    // 文件行号格式化子项
	class LineFormatItem : public FormatItem {
	public:
		void Format(std::ostream& oss, const LogMsg& msg) override {
			// 提取指定字段插入流中
			oss << msg._line;
		}
	};

    // 日志有效信息格式化子项
	class MsgFormatItem : public FormatItem {
	public:
		void Format(std::ostream& oss, const LogMsg& msg) override {
			// 提取指定字段插入流中
			oss << msg._payload;
		}
	};

    // 制表符格式化子项
	class TabFormatItem : public FormatItem {
	public:
		void Format(std::ostream& oss, const LogMsg& msg) override {
			// 提取指定字段插入流中
			oss << "\t";
		}
	};

    // 新行格式化子项
	class NLineFormatItem : public FormatItem {
	public:
		void Format(std::ostream& oss, const LogMsg& msg) override {
			// 提取指定字段插入流中
			oss << "\n";
		}
	};

    // 原始字符格式化子项
	// 由于原始字符是我们想要在日志中添加的字符，在LogMsg中不存在对应的字段，所以我们需要传递参数
	class OtherFormatItem : public FormatItem {
	public:
		// 想要在日志中添加的字符
		OtherFormatItem(const std::string& str) :_str(str) {}
		void Format(std::ostream& oss, const LogMsg& msg) override {
			// 将字符串添加到流中
			oss << _str;
		}

	private:
		std::string _str;
	};

    // 格式化器
	class Formatter {
	public:
		using ptr = std::shared_ptr<Formatter>;
		Formatter(const std::string& pattern = "[%d{%H:%M:%S}][%p][%f:%l]%m%n") : _pattern(pattern) {
			assert(ParsePattern());
		}

		// 将日志输出到指定的流中
		void Format(std::ostream& oss, const LogMsg& msg);

		// 将日志以返回值的形式进行返回
		std::string Format(const LogMsg& msg) {
			// 复用 Format(std::ostream& oss, const LogMsg& msg) 接口
			std::stringstream oss;
			Format(oss, msg);
			return oss.str();
		}

	private:
		// 解析格式化字符串并填充 items 数组
		bool ParsePattern();

		// 根据不同的格式化字符创建不同的格式化子类，是 ParsePattern 的子函数
		FormatItem::ptr CreateItem(const std::string& key, const std::string& val);

	private:
		std::string _pattern;                       // 格式化规则字符串
		std::vector<FormatItem::ptr> _items;        // 按顺序存储指定的格式化对象
	};
}

#endif
