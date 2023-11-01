#pragma once

#include "../Common.hpp"

class SourceFile {
public:
    SourceFile(const string& path, const string& contents);

    string GetPath() const;
    string GetContents() const;

    static shared_ptr<SourceFile> LoadFromPath(const string& path);

private:
    string path;
    string contents;
};