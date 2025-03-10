#pragma once
#include <cstdlib> // For malloc and free
#include <iostream>

namespace RDA {
	// Shared ptr alternative
	// Need some tests and maybe some polish
	template<typename T>
	struct sh_obj {
		struct sh_obj_header {
			uint16_t _refCount = 0;
		};
		sh_obj() = default;
		sh_obj(sh_obj& other) {
			if (other.allocation){
				allocation = other.allocation;
				sh_obj_header* header = reinterpret_cast<sh_obj_header*>(allocation) - 1;
				header->_refCount++;
			}
		};
		constexpr sh_obj(const T& item) {
			sh_obj_header* temp = reinterpret_cast<sh_obj_header*>(malloc(sizeof(T) + sizeof(sh_obj_header)));
			if (!temp) throw std::bad_alloc();
			temp->_refCount++;
			allocation = reinterpret_cast<T*>(temp + 1);
			memcpy(allocation, reinterpret_cast<const void*>(&item), sizeof(T));
		};
		~sh_obj() {
			if (allocation){
				sh_obj_header* header = reinterpret_cast<sh_obj_header*>(allocation) - 1;
				header->_refCount--;
				if (header->_refCount == 0){
					free(header);
				}
				allocation = nullptr;
			}
		}
		constexpr sh_obj& operator=(const sh_obj& other) {
			if (allocation){
				sh_obj_header* header = reinterpret_cast<sh_obj_header*>(allocation) - 1;
				header->_refCount--;
				if (header->_refCount == 0) {
					free(header);
				}
			}
			allocation = other.allocation;
			sh_obj_header* header = reinterpret_cast<sh_obj_header*>(allocation) - 1;
			header->_refCount++;
		}
		void deconnect() {
			if (allocation) {
				sh_obj_header* header = reinterpret_cast<sh_obj_header*>(allocation) - 1;
				header->_refCount--;
				if (header->_refCount == 0) {
					free(header);
				}
				allocation = nullptr;
			}
		}
		T* operator->() const { return allocation; }
		T* GetRawPointer() const { return allocation; }
	private:
		T* allocation = nullptr;
	};
}