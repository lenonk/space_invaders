#!/bin/bash

cmake -DCMAKE_TOOLCHAIN_FILE=windows-toolchain.cmake -B build-windows
cmake --build build-windows
