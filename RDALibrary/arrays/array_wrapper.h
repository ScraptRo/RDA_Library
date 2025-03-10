#pragma once
#include <cstdlib>
namespace RDA {

	// Just a simple struct meant for passing info about an array from one function to another
	// It was made as an alternative for the vector, but i didn't offered enough time cause i've got lost with other things:))
	// I would implement some kind of system to keep track of the array owner and delete it when we don't need it anymore(or there are no more handles to it)
	template<typename T>
	struct array_wrapper {
		
		array_wrapper() = default;
		array_wrapper(T* pData, size_t pSize) : mData(pData), size(pSize) {}

		array_wrapper& operator=(const array_wrapper& other) {
			mData = other.mData;
			size = other.size;
		}

		void SetData(T* pData, size_t pSize) {
			mData = pData;
			size = pSize;
		}

		void Delete() {
			if (mData){
				delete[] mData;
				mData = nullptr;
				size = 0;
			}
		}

		size_t GetSize() { return size; }
		T* data() { return mData; }
		bool isValid() { return (mData) && (size); }

	private:
		T* mData = nullptr;
		size_t size = 0;
	};

}