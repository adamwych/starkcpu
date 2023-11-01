#pragma once

#include "../Common.hpp"
#include <map>

class SourceFile;
class Compiler {
public:

    void Compile();

    /**
     * Adds a new source file to the internal filesystem.
     * @param sourceFile
     */
    void AddSourceFile(const shared_ptr<SourceFile>& sourceFile);

    /**
     * Checks whether specified path can be resolved to a source file.
     * @param path
     * @return
     */
    bool HasSourceFile(const string& path);

    /**
     * Attempts to find a source file associated with specified path.
     * @param path
     * @return
     */
    shared_ptr<SourceFile> ResolveSourceFile(const string& path);

private:

    void CompileFile(const shared_ptr<SourceFile>& sourceFile);

    /** List of all source files in the virtual filesystem. */
    map<string, shared_ptr<SourceFile>> sourceFiles;
};