#pragma once
#include <string>
#include <vector>
#include "../util/token.h"
#include "language.h"
#include "DFA.h"


class Lexer {
public:

    explicit Lexer(const std::string& input)  : file(input), curr(file.begin()), automata(createLexerDFA()) {
    };

    std::vector<Token> tokenize(){
        std::vector<Token> tokens;

        while (tokens.empty() || tokens.back().type != TokenType::END_OF_FILE) {
            Token token = nextToken();
            tokens.push_back(token);
        }
        return tokens;
    }
private:

    std::string file;
    std::string::iterator curr;
    DFA<TokenType> automata;

    Token nextToken(){
        std::string lexeme;
        std::string backtrackLexeme;

        std::string::iterator backtrackPos;
        std::optional<TokenType> recognizedType;

        while (curr != file.end() && (*curr == ' ' || *curr == '\n' || *curr == '\t' || *curr == '\r')) {
            curr++;
        }

        if(curr == file.end()){
            return Token{TokenType::END_OF_FILE, {}, ""};
        }

        // Step through automata until non-productive (longest match principle)
        while (automata.isAbsorbing() == false && curr != file.end()) {
            char c= *curr;
            curr++;
            automata.step(c);
            if(!recognizedType){
                lexeme += c;
            } else {
                backtrackLexeme += c;
            }
            if (automata.isAccepting()) {
                recognizedType = automata.getAcceptedValue(automata.getCurrentState());
                lexeme += backtrackLexeme;
                backtrackLexeme = "";
                backtrackPos = curr;
            }
        }
        
        if(!recognizedType){
            throw std::runtime_error("LexerError:" + lexeme + " is not a prefix of any valid token.");
        } 

        if(automata.isAccepting() == false) {
            curr = backtrackPos;
        }
        automata.reset(); 

        // Construct recognized token struct (util/token.h) 
        Token recognizedToken;
        switch (recognizedType.value()){
        case TokenType::IDENTIFIER:
            recognizedToken = Token{TokenType::IDENTIFIER, lexeme, lexeme};
            break;
        case TokenType::CONSTANT:
            recognizedToken = Token{TokenType::CONSTANT, std::stoi(lexeme), lexeme};
            break;

        default:
            recognizedToken = Token{recognizedType.value(), {}, lexeme};
            break;
        }
        return recognizedToken;
    };

};