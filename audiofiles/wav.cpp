// Wav file experiments
// Remove the JUNK chunk

#include <iostream>
#include <fstream>

const uint32_t BUFFER_SIZE = 1024 * 32;  // 32K buffer

struct RiffChunk // declare RiffChunk structure
{
    char chunkId[4];    // ‘RIFF’
    uint32_t chunkSize; // 4 byte size of the traditional RIFF/WAVE file
    char riffType[4];   // ‘WAVE’
};

struct JunkChunk // declare JunkChunk structure
{
    char chunkId[4];     // ‘JUNK’
    uint32_t chunkSize;  // 4 byte size of the ‘JUNK’ chunk. This must be at least 28 if the chunk is intended as a
                         // place-holder for a ‘ds64’ chunk.
    //char chunkData[];    // dummy bytes
};

int main()
{
    using namespace std;

    ifstream inFile("tunnel.wav", ios::binary);
    if (!inFile) {
        return -1;
    }

    RiffChunk riffChnk;
    inFile.read((char*)&riffChnk, 12);
    if (!inFile || inFile.gcount() < 12) {
        return -2;
    }
    if (strncmp(riffChnk.chunkId, "RIFF", 4) != 0) {
        return -3;
    }

    ofstream outFile("out.wav", ios::binary);
    if (!outFile) {
        return -4;
    }

    JunkChunk junkChnk;
    inFile.read((char*)&junkChnk, 8);
    if (!inFile || inFile.gcount() < 8) {
        return -5;
    }

    // Adjust & write new RIFF header
    riffChnk.chunkSize -= 8;   // for JUNK & 4 byte junk chunk length
    riffChnk.chunkSize -= junkChnk.chunkSize;
    outFile.write((char*)&riffChnk, 12);
    if (!outFile) {
        return -6;
    }

    // Skip junk
    inFile.seekg(junkChnk.chunkSize, ios_base::cur);
    if (!inFile) {
        return -7;
    }

    //char fmtID[4];
    //inFile.read(fmtID, 4);

    char buffer[BUFFER_SIZE];
    while (!inFile.eof()) {
        inFile.read(buffer, BUFFER_SIZE);
        auto readCount = inFile.gcount();        
        outFile.write(buffer, readCount);
        if (!outFile) {
            return -9;
        }
    }

    // Check output file size
    streamsize expectedFileSize = riffChnk.chunkSize + 8; // total file size includes the 4 byte id RIFF & 4 byte size of RIFF chunk
    outFile.seekp(0, ios_base::beg);
    auto beg = outFile.tellp();
    outFile.seekp(0, ios_base::end);
    auto outFileSize = outFile.tellp() - beg;
    if (outFileSize < expectedFileSize || outFileSize > expectedFileSize) {
        return -10;
    }

    outFile.close();
    inFile.close();
    return 0;
}