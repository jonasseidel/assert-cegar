#include <iostream>
#include <fstream>
#include <string>
#include "lexer/lexer.h"
#include "parser/parser.h"


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
        ast->get()->dump(std::cout);
    } else {
        std::cout << "Missing input file as argument." << std::endl;
    }
}