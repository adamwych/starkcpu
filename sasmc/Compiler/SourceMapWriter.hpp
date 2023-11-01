#pragma once

#include "../Common.hpp"

class SourceMapWriter {
public:
    explicit SourceMapWriter(const string& file);

    void Write(uint32 binOffset, uint32 sourceLine);

    void Flush();

private:
    string filePath;
    string buffer;
    uint32 position;
};