#include <fstream>
#include <vector>
#include <iomanip>
#include "utils.hpp"

/*
    Convert the cedict file into a cpp structure: const unsigned char cedictData[];
    save the size: const unsigned int cedictDataSize;
    save the lines count: const size_t cedictLinesCount;

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

    std::string inBuff;
    if (!makeBufferFrom(argv[1], inBuff))
    {
        std::cerr << "Error: failed to make buffer from input file\n";
        return 1;
    }

    std::ofstream out(argv[2]);
    if (!out)
    {
        std::cerr << "Error: cannot open output file\n";
        return 1;
    }

    out << "#ifndef CEDICT_DATA_H\n#define CEDICT_DATA_H\n\n";
    out << "const unsigned char cedictData[] = {\n";
    
    std::string outBuff;
    size_t lineCount = makeHexListString(inBuff, outBuff);
    
    out << outBuff;

    out << "};\nconst unsigned long long cedictDataSize = " << inBuff.size() << ";\n";

    out << "\nconst size_t cedictLinesCount = " << lineCount << ";\n\n#endif";
    
    if (out.fail())
    {
        std::cerr << "Error: ostream failed\n";
        return 1;
    }
    out.close();
    
    return 0;
}