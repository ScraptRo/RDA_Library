# RDA_Library

The `RDA_Library` is a C++ library that provides various utilities and components designed to simplify and enhance C++ development. This library is designed to work with C++20.

It was created using Visual Studio 2022, but it should work with other compilers that support C++20.(not tested)

Some components are not fully tested, so use them with caution.
An example at this point would be the `AESBuffer` class, which is not fully tested and should be used with caution.(I've got an error in a program and I'm not sure if it's the AESBuffer or something else))

In rest, feel free to use the library as you wish.

## Features

- Custom shared pointer implementation (`sh_obj`)
- A variant of the shared pointer that allocates data as a buffer, not an object (`sh_all`)
- A combination of shared pointer and linked list, made for graphic buffer handling (`obj_list` and `obj_ref`)
- A variable wrapper that let elements connect into a linked list('stack_list' and 'stack element')