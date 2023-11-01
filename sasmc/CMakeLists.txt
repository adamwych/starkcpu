cmake_minimum_required(VERSION 3.17)
project(sasmc CXX)

set(CMAKE_CXX_STANDARD 20)

add_executable(sasmc
        Main.cpp
        Compiler/Compiler.cpp
        Compiler/SourceFile.cpp
        Parsing/Tokenizer.cpp
        Compiler/CompilationWorker.cpp
        Compiler/OpcodeWriter.cpp
        Compiler/Diagnostics.cpp
        Compiler/Scope.cpp
        Compiler/SourceMapWriter.cpp)