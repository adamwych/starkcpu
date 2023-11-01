#include "SourceMapWriter.hpp"

SourceMapWriter::SourceMapWriter(const string &file) {
    filePath = file;
    buffer = "";
    position = 0;
}

void SourceMapWriter::Write(uint32 binOffset, uint32 sourceLine) {
    buffer += to_string(binOffset) + "|" + to_string(sourceLine) + "\n";
}

void SourceMapWriter::Flush() {
    FILE *file = fopen(filePath.c_str(), "wb+");
    fwrite(buffer.c_str(), 1, buffer.length(), file);
    fclose(file);
}