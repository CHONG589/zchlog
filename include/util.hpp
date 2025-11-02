/*
一些零碎的功能接口:
- 获取系统时间
- 判断文件是否存在
- 获取文件的所在目录路径
- 创建目录
*/

#pragma once
#ifndef UTIL_H__
#define UTIL_H__

#include <iostream>
#include <string>
#include <ctime>
#include <sys/stat.h>
#include <sys/types.h>

namespace zch {

    class Date {
    public:
        // 该函数返回系统的当前日历时间，自 1970 年 1 月 1 日以来经过的秒数。
        // 如果系统没有时间，则返回 -1。
        static time_t Now() {
            return time(nullptr);
        }

        // 将 Now() 函数返回的当前时间以 tm 结构体的形式保存
        static struct tm GetTimeSet() {
            struct tm t;
            time_t time_stamp = Date::Now();
            localtime_r(&time_stamp, &t);
            return t;
        }
    };

    class File {
	public:
		// 判断文件是否存在
		static bool IsExist(const std::string& path) {
			struct stat st;
			if (stat(path.c_str(), &st) == 0) {
				return true;
			}
			else {
				return false;
			}
		}

		// 获取文件的所在目录路径
		static std::string GetPath(const std::string& path) {
			size_t pos = path.find_last_of("/\\");
			if (pos == std::string::npos) {
				return ".";
			}
			else {
				return path.substr(0, pos + 1);
			}
		}

		// 创建目录
		static void CreateDirectory(const std::string& path) {
			if (path.size() == 0) return;
			umask(0);
			// 测试样例：
			// /home/abc/test/  	/home/abc/test
			// text    				test/
			// ./test  				./test/
			size_t cur = 0, pos = 0;
			std::string parent_dir;

			while (cur < path.size()) {
				pos = path.find_first_of("/\\", cur);
				// 截取父级路径
				parent_dir = path.substr(0, pos);
				// 父级路径有效 && 目录不存在
				if ((parent_dir.size() != 0) && (!IsExist(parent_dir))) {
					mkdir(parent_dir.c_str(), 0775);
				}

				// 如果pos等于结束位置，说明目录创建完毕
				if (pos == std::string::npos) {
					break;
				}
				cur = pos + 1;
			}
		}
	};
}

#endif
