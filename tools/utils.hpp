#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <fstream>
#include <iostream>

bool makeBufferFrom(char* file, std::string& out)
{
    out.clear();
    std::ifstream in(file, std::ios::binary | std::ios::ate);
    
    if (!in) return false;

    std::streamsize size = in.tellg();
    in.seekg(0, std::ios::beg);

    out.resize(size, '\0');
    if (!in.read(out.data(), size))
    {
        out.clear();
        return false;
    }

    if (in.fail())
    {
        out.clear();
        return false;
    }
    in.close();

    return true;
}

// return line count
size_t makeHexListString(const std::string& in, std::string& out)
{
    size_t size = in.size();
    size_t lineCount = 0;
    constexpr char hexChars[] = "0123456789ABCDEF";

    out.clear();
    out.reserve(size * 6); 

    for (size_t i = 0; i < size; ++i) 
    {
        if (in[i] == '\n') lineCount++;

        unsigned char byte = static_cast<unsigned char> (in[i]);
        out += "0x";
        out += hexChars[(byte >> 4) & 0x0F]; // first 4 bits converted in the hex char equivalent
        out += hexChars[byte & 0x0F]; // same for next 4 bits
        // rm: 0x0F = 0000 1111
        out += ",";

        if ((i + 1) % 16 == 0) out += "\n";
    }

    return lineCount;
}

void printArgs(char* argv[], size_t argc)
{
    for (size_t i = 0; i <= argc; ++i)
    {
        std::cerr << "arg" << i << ": " << argv[i] << std::endl;
    }
}
#endif