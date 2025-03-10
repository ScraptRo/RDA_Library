#pragma once
#include "shared_allocation.h"
#include <atomic>
#include <cstring>

namespace RDA {
    // A buffer made for handling incoming request from a TCP server
    // I've took inspiration from the C# TCP listener
    //
    // The write method is meant for the incoming message thread, and the read method is meant for the "main" thread
    // The structure was not tested for reading from multiple threads
    // Also, the write method is made for only ONE thread

    struct AESBuffer {
        AESBuffer(size_t startSize) : _size(startSize), _available(startSize) {
            _all = sh_all(startSize);
        }

        void write(char* buffer, size_t pAmount) {
            size_t cA = _available.load(std::memory_order_relaxed);
            size_t readC = readI.load(std::memory_order_acquire);
            size_t writeC = writeI.load(std::memory_order_acquire);
            // In case there is not enough data, it will add 2*pAmount to the buffer size
            if (cA < pAmount) {
                cA = _size + pAmount * 2;
                sh_all temp = sh_all(cA);
                // Implemented some weird logic here, but I'm to tired to do it better
                if (readC <= writeC) {
                    memcpy(temp.getRawPointer(), _all.getRawPointer(), writeC);
                    size_t tmp = _size - readC;
                    memcpy(reinterpret_cast<char*>(temp.getRawPointer()) + cA - tmp, reinterpret_cast<char*>(_all.getRawPointer()) + readC, tmp);
                }
                else {
                    memcpy(temp.getRawPointer(), _all.getRawPointer(), writeC);
                    memcpy(reinterpret_cast<char*>(temp.getRawPointer()) + cA - (_size - readC), reinterpret_cast<char*>(_all.getRawPointer()) + readC, _size - readC);
                }
                _all = temp;
                _size = cA;
            }
            char* bf = reinterpret_cast<char*>(_all.getRawPointer());
            cA -= pAmount;
            // If is at the end of the buffer, write till the end of the buffer
            // and then just get to the start and write the rest
            if (pAmount > _size - writeC) {
                memcpy(bf + writeC, buffer, _size - writeC);
                buffer += _size - writeC;
                pAmount -= _size - writeC;
                writeC = 0;
            }
            memcpy(bf + writeC, buffer, pAmount);
            writeC += pAmount;
            writeI.store(writeC, std::memory_order_release);
            _available.store(cA, std::memory_order_release);
            cv.notify_all();
        }

        void read(char* buffer, size_t pAmount) {
            // Make sure there is enough data in the buffer
            // This is meant to block the thread, its a feature, not a bug
            std::unique_lock<std::mutex> lock(mtx);
            cv.wait(lock, [&] { return _size - _available.load(std::memory_order_acquire) >= pAmount; });

            sh_all tAll(_all);
            size_t cA = _available.load(std::memory_order_relaxed);
            char* bf = reinterpret_cast<char*>(tAll.getRawPointer());
            size_t readC = readI.load(std::memory_order_acquire);
            cA -= pAmount;
            // If is at the end of the buffer, copy till the end of the buffer
            // and then just get to the start and copy the rest
            if (pAmount > _size - readC) {
                memcpy(buffer, bf + readC, _size - readC);
                buffer += _size - readC;
                pAmount -= _size - readC;
                readC = 0;
            }
            memcpy(buffer, bf + readC, pAmount);
            readC += pAmount;
            readI.store(readC, std::memory_order_release);
            _available.store(cA, std::memory_order_release);
        }

    private:
        sh_all _all; // The shared buffer
        std::atomic<size_t> _available; // Amount of free space
        size_t _size; // The size of the entire buffer
        std::atomic<size_t> readI = 0; // Read Index
        std::atomic<size_t> writeI = 0; // Write Index
        // Synchronization Objects
        std::mutex mtx;
        std::condition_variable cv;
    };
}
