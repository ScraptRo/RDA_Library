#pragma once
#include "shared_allocation.h"
#include <atomic>
#include <cstring>

namespace RDA {
    // A buffer made for handling incoming request from a TCP server
    // I've took inspiration from the C# TCP listener
    //
    // This is just a SPSC buffer with the ability of resizing on demand. For now it's only growing, and there is no way of making it shrink again
    // I've tried to make it with no blocking at all, but it didn't quite work
    //
    // HOW TO USE IT
    // Use the write() method inside the CallBack function, and the read() method where you need data
    // The read() method blocks the thread till it got the amount it requested
    // Use the Trigger_Interrupt() for stopping the AESBuffer from reading

    struct AESBuffer {
        AESBuffer(size_t startSize)
            : _all(startSize), _size(startSize), _available(startSize), _writeIndex(0), _readIndex(0) {
        }

        void write(const char* buffer, size_t pAmount) {
            std::unique_lock<std::mutex> lock(mtx);

            // Check if we need to resize the buffer
            if (pAmount > _available) {
                size_t newSize = _size + (pAmount - _available) + _size; // Add extra space to avoid frequent resizing
                sh_all newBuffer(newSize);
                // Copy existing data to new buffer
                size_t dataSize = _size - _available;
                if (dataSize > 0) {
                    if (_readIndex < _writeIndex) {
                        // Data is contiguous
                        memcpy(reinterpret_cast<char*>(newBuffer.getRawPointer()),
                            reinterpret_cast<const char*>(_all.getRawPointer()) + _readIndex,
                            dataSize);
                    }
                    else if (_readIndex > _writeIndex) {
                        // Data wraps around
                        size_t tailSize = _size - _readIndex;
                        memcpy(reinterpret_cast<char*>(newBuffer.getRawPointer()),
                            reinterpret_cast<const char*>(_all.getRawPointer()) + _readIndex,
                            tailSize);
                        memcpy(reinterpret_cast<char*>(newBuffer.getRawPointer()) + tailSize,
                            reinterpret_cast<const char*>(_all.getRawPointer()),
                            _writeIndex);
                    }
                }
                // Update buffer state
                _all = newBuffer;
                _size = newSize;
                _available = newSize - dataSize;
                _readIndex = 0;
                _writeIndex = dataSize;
            }

            // Write data to buffer
            if (_writeIndex + pAmount <= _size) {
                // Data fits without wrapping
                memcpy(reinterpret_cast<char*>(_all.getRawPointer()) + _writeIndex, buffer, pAmount);
            }
            else {
                // Data needs to wrap around
                size_t firstPart = _size - _writeIndex;
                memcpy(reinterpret_cast<char*>(_all.getRawPointer()) + _writeIndex, buffer, firstPart);
                memcpy(reinterpret_cast<char*>(_all.getRawPointer()), buffer + firstPart, pAmount - firstPart);
            }

            _writeIndex = (_writeIndex + pAmount) % _size;
            _available -= pAmount;

            cv.notify_all();
        }

        void read(char* buffer, size_t pAmount) {
            std::unique_lock<std::mutex> lock(mtx);
            cv.wait(lock, [this, pAmount] { return (_size - _available) >= pAmount || _interrupted; });
            if (_interrupted){
                buffer = nullptr;
                return;
            }
            // Read data from buffer
            if (_readIndex < _writeIndex) {
                // Data is contiguous
                memcpy(buffer, reinterpret_cast<const char*>(_all.getRawPointer()) + _readIndex, pAmount);
            }
            else {
                // Data might wrap around
                size_t tailSize = _size - _readIndex;
                if (tailSize >= pAmount) {
                    // All data is in the tail
                    memcpy(buffer, reinterpret_cast<const char*>(_all.getRawPointer()) + _readIndex, pAmount);
                }
                else {
                    // Data wraps around
                    memcpy(buffer, reinterpret_cast<const char*>(_all.getRawPointer()) + _readIndex, tailSize);
                    memcpy(buffer + tailSize, reinterpret_cast<const char*>(_all.getRawPointer()), pAmount - tailSize);
                }
            }

            _readIndex = (_readIndex + pAmount) % _size;
            _available += pAmount;
        }

        size_t availableData() const {
            std::lock_guard<std::mutex> lock(mtx);
            return _size - _available;
        }

        size_t availableSpace() const {
            std::lock_guard<std::mutex> lock(mtx);
            return _available;
        }

        void Trigger_Interrupt() {
            std::lock_guard<std::mutex> lock(mtx);
            cv.notify_all();
            _interrupted = true;
        }

        bool Is_Interrupted() {
            std::lock_guard<std::mutex> lock(mtx);
            return _interrupted;
        }
    private:
        sh_all _all;                    // The shared buffer
        size_t _size;                   // The size of the entire buffer
        size_t _available;              // Amount of free space
        size_t _writeIndex;             // Write Index
        size_t _readIndex;              // Read Index

        bool _interrupted = false;

        // Synchronization Objects
        mutable std::mutex mtx;
        std::condition_variable cv;
    };
}
