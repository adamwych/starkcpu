#include "Compiler/Compiler.hpp"
#include "Compiler/SourceFile.hpp"

int main(int argc, char** argv) {
    auto compiler = make_shared<Compiler>();
    compiler->AddSourceFile(SourceFile::LoadFromPath("../../examples/test.sasm"));
    compiler->Compile();

    return 0;
}