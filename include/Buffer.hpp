/**
 * @file Buffer.hpp
 * @brief 异步线程缓冲区的设计
 * @author zch
 * @date 2025-11-09
 */

#ifndef BUFFER_H__
#define BUFFER_H__

#include <iostream>
#include <vector>
#include <assert.h>

namespace zch {
    // 缓冲区默认大小
	const size_t default_buffer_size = 1 * 1024 * 1024;
	// 阈值
	const size_t threshold = 2 * 1024 * 1024;
	// 大于阈值以后每次扩容的自增值
	const size_t increament = 1 * 1024 * 1024;

    class Buffer {
    public:
        Buffer(size_t buffer_size = default_buffer_size) 
				: _buffer(buffer_size)
				, _read_idx(0)
				, _write_idx(0) {}

        // 返回可写空间大小 
		size_t WriteableSize() { return _buffer.size() - _write_idx; }

        // 返回可读空间大小
		size_t ReadableSize() { return _write_idx - _read_idx; }

        // 移动可读位置
		void MoveReadIdx(size_t len) {
			// 防止外界传入的参数不合法
			if (len > ReadableSize()) {
				_read_idx = ReadableSize();
			} else {
				_read_idx += len;
			}
		}

        // 返回可读数据的起始地址 
		const char* Start() { return &_buffer[_read_idx]; }

        // 重置缓冲区
		void reset() {
			_write_idx = 0; 
            _read_idx = 0;
		}

        // 交换缓冲区
		void swap(Buffer& buf) {
            // 首先交换读写指针
			std::swap(_write_idx, buf._write_idx);
			std::swap(_read_idx, buf._read_idx);

            // 交换两个 std::vector 对象所管理的内存区域，
            // 它是非复制的。它交换的是指向底层数据、大小和
            // 容量的内部指针，而不是复制大量的元素，所以它
            // 的时间复杂度是常数级的 O(1)。
			_buffer.swap(buf._buffer);
		}

        // 数据判空
		bool Empty() { return _write_idx == _read_idx; }
        
        // 将数据放入缓冲区中
		inline void Push(const char* data, size_t len) {
			// 1. 判断空间是否足够
			if (len > WriteableSize()) {
                Resize(len);
            }

			// 2. 将数据写入缓冲区
			std::copy(data, data + len, &_buffer[_write_idx]);

			// 3. 更新数据的写入位置
			MoveWriteIdx(len);
		}

    private:
        // 移动可写位置
		void MoveWriteIdx(size_t len) {
			assert(_write_idx + len <= _buffer.size());
			_write_idx += len;
		}

        // 扩容
		void Resize(size_t len) {
			size_t new_size;
			if (_buffer.size() < threshold) {
				new_size = _buffer.size() * 2 + len;
			} else {
				new_size = _buffer.size() + increament + len;
			}
			_buffer.resize(new_size);
		}
        
    private:
        // 日志缓存区
		std::vector<char> _buffer;
		// 可读位置的起始下标
		size_t _read_idx;
		// 可写位置的起始下标
		size_t _write_idx;
    };
}

#endif
