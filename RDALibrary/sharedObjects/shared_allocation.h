#pragma once
#include <cstdlib> // For malloc and free
#include <iostream>
#include <mutex>

namespace RDA {
	// It works as a shared pointer but for buffer of data
	// not just for an object
	// Maybe it would need some work for optimizing, and better way of synchronizing the data
	struct sh_all {
		struct sha_header {
			uint16_t counter;
		};
		sh_all() = default;
		sh_all(size_t pSize) : _size(pSize) {
			_all = malloc(pSize + sizeof(sha_header));
			if (!_all)
				throw std::bad_alloc();
			auto header = reinterpret_cast<sha_header*>(_all);
			header->counter = 1;
		}
		sh_all(const sh_all& other) {
			this->_all = other._all;
			this->_size = other._size;
			if (_all) {
				sha_header* t = reinterpret_cast<sha_header*>(_all);
				t->counter++;
			}
		}
		~sh_all() {
			if (!_all) {
				return;
			}
			auto header = getHeader();
			header->counter--;
			if (header->counter == 0) {
				free(_all);
			}
			_all = nullptr;
			_size = 0;
		}
		sh_all& operator=(const sh_all& other) {
			if (this == &other) return *this;

			auto header = getHeader();
			auto other_header = other.getHeader();
			if (_all) {
				header->counter--;
				if (header->counter == 0) {
					free(_all);
				}
			}
			_all = other._all;
			_size = other._size;
			if (_all) {
				other_header->counter++;
			}
			return *this;
		}
		void* operator->() const {
			return reinterpret_cast<sha_header*>(_all) + 1;
		}
		void* getRawPointer() {
			return reinterpret_cast<sha_header*>(_all) + 1;
		}
	private:
		sha_header* getHeader() const {
			return reinterpret_cast<sha_header*>(_all);
		}
		void* _all = nullptr;
		size_t _size = 0;
	};
}