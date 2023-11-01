#include "SourceFile.hpp"
#include <fstream>

SourceFile::SourceFile(const string& _path, const string& _contents) {
    path = _path;
    contents = _contents;
}

string SourceFile::GetPath() const {
    return path;
}

string SourceFile::GetContents() const {
    return contents;
}

shared_ptr<SourceFile> SourceFile::LoadFromPath(const string &path) {
    auto stream = ifstream(path);
    std::string contents((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
    return make_shared<SourceFile>(path, contents);
}