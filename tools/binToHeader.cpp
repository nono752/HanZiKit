#include <fstream>
#include <vector>
#include <iomanip>
#include "utils.hpp"

/*
    Convert the cedict file into a cpp structure: const unsigned char cedictData[];
    save the size: const unsigned int cedictDataSize;
    save the entries count: const unsigned int cedictEntriesCount;

    Usage: binToHeader <cedict.u8> <out.hpp>
    Current CMake write it in the file cedictData.hpp in the binary dir.
*/

int main(int argc, char* argv[]) 
{
    if (argc != 3)
    {
        std::cerr << "Error: Usage: binToHeader <cedict.u8> <out.hpp>\n";
        printArgs(argv, argc);
        return 1;
    }
    std::ifstream in(argv[1], std::ios::binary | std::ios::ate);
    std::ofstream out(argv[2]);
    if (!in)
    {
        std::cerr << "Error: cannot open cedict file\n";
        return 1;
    }
    if (!out)
    {
        std::cerr << "Error: cannot open output file\n";
        return 1;
    }

    std::streamsize size = in.tellg();
    in.seekg(0, std::ios::beg);

    std::vector<unsigned char> inBuffer(size);
    if (!in.read(reinterpret_cast<char*>(inBuffer.data()), size)) return 1;
    in.close();

    unsigned int lineCount = 0;

    out << "#ifndef CEDICT_DATA_H\n#define CEDICT_DATA_H\n\n";
    out << "const unsigned char cedictData[] = {\n";
    
    std::string outBuffer;
    outBuffer.reserve(size * 6); 
    constexpr char hexChars[] = "0123456789ABCDEF";

    for (std::streamsize i = 0; i < size; ++i) 
    {
        if (inBuffer[i] == '\n') lineCount++;

        unsigned char byte = inBuffer[i];
        outBuffer += "0x"; 
        outBuffer += hexChars[(byte >> 4) & 0x0F]; // first 4 bits converted in the hex char equivalent
        outBuffer += hexChars[byte & 0x0F]; // same for next 4 bits
        // rm: 0x0F = 0000 1111
        outBuffer += ",";

        if ((i + 1) % 16 == 0) outBuffer += "\n";
    }
    
    out << outBuffer;

    out << "};\nconst unsigned long long cedictDataSize = " << size << ";\n";

    out << "\nconst unsigned int cedictEntriesCount = " << lineCount << ";\n\n#endif";
    
    if (out.fail())
    {
        std::cerr << "Error: ostream failed\n";
        return 1;
    }
    out.close();
    
    return 0;
}