#include "arrays/AESBuffer.h"
#include <iostream>
#include <cstring>
#include <cassert>
#include <any>

RDA::AESBuffer* buf;
unsigned int c1 = 0;
unsigned int c2 = 0;

void funct1() {
	const char* data = "Hello, World!";
	for (int i = 0; i < 1000000; ++i) {
		buf->write(data, 13);
		c1++;
	}
	std::cout << "Thread1 Ended with: " << c1 << std::endl;
}

void funct2() {
	char buffer[14];
	buffer[13] = '\0'; // Ensure null-termination
	for (int i = 0; i < 1000000; ++i) {
		buf->read(buffer, 13);
		assert(strcmp(buffer, "Hello, World!") == 0);
		c2++;
	}
	std::cout << "Thread2 Ended with: " << c1 << std::endl;
}

int main() {
	buf = new RDA::AESBuffer(16);
	//const char* data = "Hello, World!";
	//char buffer[14];
	//buffer[13] = '\0'; // Ensure null-termination
	//for (size_t i = 0; i < 10000; i++){
	//	buf->write(data, 13);
	//	buf->read(buffer, 13);
	//	assert(strcmp(buffer, data) == 0);
	//}
	try
	{
		std::thread t1(funct1);
		std::thread t2(funct2);
		t1.join();
		t2.join();
	}
	catch (const std::exception&)
	{
		std::cerr << "Exception caught!" << std::endl;
		std::cout << "c1: " << c1 << std::endl;
		std::cout << "c2: " << c2 << std::endl;
	}
	catch (...)
	{
		std::cerr << "Unknown exception caught!" << std::endl;
		std::cout << "c1: " << c1 << std::endl;
		std::cout << "c2: " << c2 << std::endl;
	}
    std::cout << "All tests passed!" << std::endl;
	delete buf;
    return 0;
}
