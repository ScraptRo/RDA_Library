#include "arrays/AESBuffer.h"
#include <iostream>
#include <cstring>
#include <cassert>

void testWriteAndRead() {
    RDA::AESBuffer buffer(10);

    char writeData[] = "Hello";
    buffer.write(writeData, strlen(writeData));

    char readData[6] = { 0 };
    buffer.read(readData, strlen(writeData));

    assert(strcmp(readData, "Hello") == 0);
    std::cout << "testWriteAndRead passed!" << std::endl;
}

void testWriteWrapAround() {
    RDA::AESBuffer buffer(10);

    char writeData1[] = "Hello";
    buffer.write(writeData1, strlen(writeData1));

    char writeData2[] = "World";
    buffer.write(writeData2, strlen(writeData2));

    char readData1[6] = { 0 };
    buffer.read(readData1, strlen(writeData1));
    assert(strcmp(readData1, "Hello") == 0);

    char readData2[6] = { 0 };
    buffer.read(readData2, strlen(writeData2));
    assert(strcmp(readData2, "World") == 0);

    std::cout << "testWriteWrapAround passed!" << std::endl;
}

RDA::AESBuffer* buf;
void threadTestFunct() {
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    char data[6] = { 0 };
    buf->write(data, 5);
}

void testReadBeforeWrite() {
    RDA::AESBuffer buffer(10);
    buf = &buffer;
    std::thread th(threadTestFunct);

    char readData[6];
    buffer.read(readData, 5);
    th.join();
    assert(strcmp(readData, "") == 0);
    std::cout << "testReadBeforeWrite passed!" << std::endl;
}

void testWriteAndReadLargeData() {
    RDA::AESBuffer buffer(10);

    char writeData[] = "This is a test of the AESBuffer";
    buffer.write(writeData, strlen(writeData));

    char readData[35] = { 0 };
    buffer.read(readData, strlen(writeData));

    assert(strcmp(readData, "This is a test of the AESBuffer") == 0);
    std::cout << "testWriteAndReadLargeData passed!" << std::endl;
}

int main() {
    testWriteAndRead();
    testWriteWrapAround();
    testReadBeforeWrite();
    testWriteAndReadLargeData();

    std::cout << "All tests passed!" << std::endl;
    return 0;
}
