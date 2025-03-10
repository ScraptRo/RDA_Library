# RDA_Library

The `RDA_Library` is a C++ library that provides various utilities and components designed to simplify and enhance C++ development. This library is designed to work with C++20.

## Features

- Custom shared pointer implementation (`sh_obj`)
- A variant of the shared pointer that allocates data as a buffer, not an object ('sh_all')
- A combination of shared pointer and linked list, made for graphic buffer handling ('obj_list' and 'obj_ref')
- A variable wrapper that let elements connect into a linked list('stack_list' and 'stack element')

## Getting Started

### Prerequisites

- C++20 compatible compiler
- CMake (optional, for building the library)