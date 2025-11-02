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
            // stat 是一个用于获取文件或文件夹状态信息的函数，
            // 通常在 C 编程中使用。它可以用于检查文件的存在性、
            // 文件类型、文件大小、修改时间等属性。
            // st 表示一个指向 struct stat 结构体的指针，用于
            // 存储获取到的文件状态信息，返回 0 表示获取成功，这个
            // 系统调用在文件不存在的情况下会调用失败，返回 -1.
			struct stat st;
			if (stat(path.c_str(), &st) == 0) {
				return true;
			}
			else {
				return false;
			}
		}

		// 获取文件的所在目录路径，如：/home/abc/test/a.txt，目录路径为：
        // /home/abc/test/ 或者 /home/abc/test
		static std::string GetDirPath(const std::string& path) {
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
			if (path.size() == 0) {
                return;
            }

			umask(0);
			// 测试样例：
			// /home/abc/test/  	/home/abc/test
			// text    				test/
			// ./test  				./test/
			size_t cur = 0, pos = 0;
			std::string parent_dir;

            // cur 是当前位置，pos 是目录分隔符位置
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
