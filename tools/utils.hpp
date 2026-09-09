#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <fstream>
#include <iostream>

std::string makeBufferFrom(char* file)
{
    std::ifstream in(file, std::ios::binary | std::ios::ate);
    
    if (!in) return "";

    std::streamsize size = in.tellg();
    in.seekg(0, std::ios::beg);

    std::string buffer(size, ' ');
    if (!in.read(buffer.data(), size)) return "";
    
    in.close();

    return std::move(buffer);
}

void printArgs(char* argv[], size_t argc)
{
    for (size_t i = 0; i <= argc; ++i)
    {
        std::cerr << "arg" << i << ": " << argv[i] << std::endl;
    }
}
#endif