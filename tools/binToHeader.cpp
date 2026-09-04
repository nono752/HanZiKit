#include <fstream>
#include <vector>
#include <iomanip>

/*
    Convert the cedict file into a cpp structure: const unsigned char cedictData[];
    save the size: const unsigned int cedictDataSize;
    save the entries count: const unsigned int cedictEntriesCount;

    CMake: write it in the file cedictData.hpp in the binary dir.
*/

int main(int argc, char* argv[]) 
{
    if (argc != 3) return 1;

    std::ifstream in(argv[1], std::ios::binary | std::ios::ate);
    std::ofstream out(argv[2]);
    if (!in || !out) return 1;

    std::streamsize size = in.tellg();
    in.seekg(0, std::ios::beg);

    std::vector<unsigned char> buffer(size);
    if (!in.read(reinterpret_cast<char*>(buffer.data()), size)) return 1;
    in.close();

    unsigned int lineCount = 0;

    out << "const unsigned char cedictData[] = {\n";
    
    out << std::hex << std::uppercase << std::setfill('0');
    
    for (std::streamsize i = 0; i < size; ++i) 
    {
        if (buffer[i] == '\n') lineCount++;

        out << "0x" << std::setw(2) << static_cast<int>(buffer[i]) << ",";
        if ((i + 1) % 16 == 0) out << "\n";
    }
    
    out << std::dec << "};\nconst unsigned int cedictDataSize = " << size << ";\n";

    out << "\nconst unsigned int cedictEntriesCount = " << lineCount << ";";
    
    return 0;
}