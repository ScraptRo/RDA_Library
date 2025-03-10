#pragma once
#include <cstring>

namespace RDA {
	// An unifinished copy of vector
	// I wanted to create it for some kind of reason... but i forgot, and now im keeping it here for later
	template<typename T>
	struct dyn_arr{
		dyn_arr() = default;
		dyn_arr(size_t size) : _dt(new T[size]), _size(size) {};
		dyn_arr(T* ptr, size_t size) : _dt(ptr), _size(size) {};

		dyn_arr& operator==(const dyn_arr& other) {
			memcpy(_dt, other._dt, sizeof(T) * other._size);
			_size = other._size;
		}

		T* data() { return _dt; }
		size_t size() { return _size; }

		void push();
		void pop();
		void pop_back();

	private:
		T* _dt = nullptr;
		size_t _size = 0;
	};
}