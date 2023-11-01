#include "Compiler.hpp"
#include "CompilationWorker.hpp"
#include "SourceFile.hpp"
#include "OpcodeWriter.hpp"
#include "SourceMapWriter.hpp"
#include "../Parsing/Tokenizer.hpp"

void Compiler::Compile() {
    for (auto &entry : sourceFiles) {
        CompileFile(entry.second);
    }
}

string MakeOutputFilePath(const shared_ptr<SourceFile> &sourceFile) {
    return sourceFile->GetPath() + ".bin";
}

string MakeMapOutputFilePath(const shared_ptr<SourceFile> &sourceFile) {
    return sourceFile->GetPath() + ".map";
}

void Compiler::CompileFile(const shared_ptr<SourceFile> &sourceFile) {
    auto writer = make_shared<OpcodeWriter>(MakeOutputFilePath(sourceFile));
    auto mapWriter = make_shared<SourceMapWriter>(MakeMapOutputFilePath(sourceFile));
    auto tokens = Tokenizer::TokenizeFile(sourceFile);
    auto worker = make_shared<CompilationWorker>(tokens, writer, mapWriter);

    worker->Compile();
    writer->Flush();
    mapWriter->Flush();
}

void Compiler::AddSourceFile(const shared_ptr<SourceFile> &sourceFile) {
    if (HasSourceFile(sourceFile->GetPath())) {
        throw runtime_error("Source file is already present in the filesystem.");
    }

    sourceFiles[sourceFile->GetPath()] = sourceFile;
}

bool Compiler::HasSourceFile(const string &path) {
    return ResolveSourceFile(path) != nullptr;
}

shared_ptr<SourceFile> Compiler::ResolveSourceFile(const string &path) {
    return sourceFiles[path];
}