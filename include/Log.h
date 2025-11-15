/**
 * @file Log.h
 * @brief 日志宏
 * @author zch
 * @date 2025-11-15
 */

#ifndef LOG_H__
#define LOG_H__

#include "Logger.h"

namespace zch {

    // 1. 提供一个全局接口来得到指定的日志器对象
	const zch::Logger::ptr& GetLogger(std::string name);

    // 2. 提供一个全局接口来得到默认日志器对象
	const zch::Logger::ptr& DefaultLogger();

    // 3. 使用宏函数简化用户的传参，自动填充行号和文件名
    #define Debug(fmt, ...) Debug(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
    #define Info(fmt, ...) Info(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
    #define Warn(fmt, ...) Warn(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
    #define Error(fmt, ...) Error(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
    #define Fatal(fmt, ...) Fatal(__FILE__, __LINE__, fmt, ##__VA_ARGS__)

    // 4.给用户使用的宏函数
    #define DEBUG(fmt,...) zch::DefaultLogger()->Debug(fmt, ##__VA_ARGS__)
    #define INFO(fmt,...) zch::DefaultLogger()->Info(fmt, ##__VA_ARGS__)
    #define WARN(fmt,...) zch::DefaultLogger()->Warn(fmt, ##__VA_ARGS__)
    #define ERROR(fmt,...) zch::DefaultLogger()->Error(fmt, ##__VA_ARGS__)
    #define FATAL(fmt,...) zch::DefaultLogger()->Fatal(fmt, ##__VA_ARGS__)
}

#endif
