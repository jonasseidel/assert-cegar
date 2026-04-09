#pragma once
#include <string>
#include <vector>
#include "../util/token.h"
#include "language.h"
#include "DFA.h"


class Lexer {
    std::string file;
    std::string::iterator curr;
    DFA<TokenType> automata;


    //Token recognizedToken;
    // returns false if no productive state.
    bool step(char c){
        return true; // TODO impl
    };

    Token nextToken(){
        std::string lexeme;
        std::string backtrackLexeme;

        std::string::iterator backtrackPos;
        std::optional<TokenType> recognizedType;

        while (curr != file.end() && (*curr == ' ' || *curr == '\n' || *curr == '\t' || *curr == '\r')) {
            curr++;
        }

        if(curr == file.end()){
            std::cout << "END_OF_FILE" << std::endl;
            return Token{TokenType::END_OF_FILE, {}, ""};
        }

        // Assumption: our DFA is always productive unless it is in the absorbing state
        // Should always be satisfiable, as these nonproductive states could be merged into the absorbing state.
        // Extra safety check necessary in a DFA_tests? (TODO)
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
        // We need to catch error case where end of string reached but we did not recognize a token..
        if(curr == file.end() && !recognizedType){
            //TODO error handling
            throw std::runtime_error("LexerError: Reached end of file but no valid token recognized for prefix " + lexeme+".");
        }
        // Backtrack to last accepting state if we ended in a non-accepting state
        if(automata.isAccepting() == false) {
            curr = backtrackPos;
        }

        //Error if no accepting state reached at all (First longest match not applicable or invalid input)
        if(!recognizedType){
            throw std::runtime_error("LexerError:" + lexeme + " is not a prefix of any valid token.");
        } 

        automata.reset(); // reset DFA for next token
        //TODO construct token with correct value...
        //std::cout << lexeme << std::endl;

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
};