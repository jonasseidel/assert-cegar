#include <iostream>
#include <fstream>
#include <string>
#include "lexer/lexer.h"
#include "parser/parser.h"

#include "util/ast_printer.h"
#include "util/cfg_builder.h"
#include "util/cfg_printer.h"

std::string fileToString(const std::string& filename) {
    std::ifstream file(filename);
    if(!file.is_open()){
        throw std::runtime_error("Cannot open provided file: " + filename);
    }{
        return std::string((std::istreambuf_iterator<char>(file)),
                            std::istreambuf_iterator<char>());
    }
}


int main(int argc, char* argv[]) {

    if(argc > 1) {
        std::string filename = argv[1];
        std::string source = fileToString(filename);
        std::cout << "Source file: " << filename << ":\n";
        Lexer lexer(source);
        auto tokens = lexer.tokenize();
        Parser parser(tokens);
        auto ast = parser.parse();
        if (!ast) {
            std::cerr << "Error parsing the source file\n";
            return 1;
        }
        std::cout << "Parsed AST:\n";

        ASTPrinter printer(std::cout);
        ast->get()->accept(printer);

        std::cout << "\nCFG:\n";
        CFGBuilder builder;
        CFG cfg = builder.build(*dynamic_cast<StatementBlock*>(ast->get()));
        printCFG(cfg, std::cout);

        
    } else {
        std::cout << "Missing input file as argument." << std::endl;
    }
}