#include <iostream>
#include <string>
#include <vector>
#include <cassert>
#include "../src/lexer/lexer.h"

static int failures = 0;

static void expectEq(TokenType actual, TokenType expected, const std::string& msg) {
    if (actual != expected) {
        std::cerr << "  FAIL [" << msg << "]: got "
                  << static_cast<int>(actual) << ", expected "
                  << static_cast<int>(expected) << "\n";
        ++failures;
    }
}

static void expectEq(size_t actual, size_t expected, const std::string& msg) {
    if (actual != expected) {
        std::cerr << "  FAIL [" << msg << "]: got "
                  << actual << ", expected " << expected << "\n";
        ++failures;
    }
}

static void checkTypes(const std::string& source,
                       const std::vector<TokenType>& expected,
                       const std::string& testName)
{
    std::cout << "TEST: " << testName << "\n";
    auto tokens = Lexer(source).tokenize();

    expectEq(tokens.size(), expected.size(), "token count");

    size_t n = std::min(tokens.size(), expected.size());
    for (size_t i = 0; i < n; ++i)
        expectEq(tokens[i].type, expected[i], "token[" + std::to_string(i) + "]");
}

// ============================================================
// Tests
// ============================================================

// assign0.txt: x=1+1
static void test_assign() {
    checkTypes("x=1+1", {
        TokenType::IDENTIFIER,
        TokenType::EQ,
        TokenType::CONSTANT,
        TokenType::PLUS,
        TokenType::CONSTANT,
        TokenType::END_OF_FILE,
    }, "assign0");
}

// assert0.txt: x=2\nassert x==2
static void test_assert() {
    checkTypes("x=2\nassert x==2", {
        TokenType::IDENTIFIER,
        TokenType::EQ,
        TokenType::CONSTANT,
        TokenType::ASSERT,
        TokenType::IDENTIFIER,
        TokenType::EQEQ,
        TokenType::CONSTANT,
        TokenType::END_OF_FILE,
    }, "assert0");
}

// if0.txt: if true then\nx=1\ny=2\nend
static void test_if() {
    checkTypes("if true then\nx=1\ny=2\nend", {
        TokenType::IF,
        TokenType::TRUE,
        TokenType::THEN,
        TokenType::IDENTIFIER,
        TokenType::EQ,
        TokenType::CONSTANT,
        TokenType::IDENTIFIER,
        TokenType::EQ,
        TokenType::CONSTANT,
        TokenType::END,
        TokenType::END_OF_FILE,
    }, "if0");
}

// while0.txt: x= 10\nwhile x>0 do\nx= x-1\nend
static void test_while() {
    checkTypes("x= 10\nwhile x>0 do\nx= x-1\nend", {
        TokenType::IDENTIFIER,
        TokenType::EQ,
        TokenType::CONSTANT,
        TokenType::WHILE,
        TokenType::IDENTIFIER,
        TokenType::GT,
        TokenType::CONSTANT,
        TokenType::DO,
        TokenType::IDENTIFIER,
        TokenType::EQ,
        TokenType::IDENTIFIER,
        TokenType::MINUS,
        TokenType::CONSTANT,
        TokenType::END,
        TokenType::END_OF_FILE,
    }, "while0");
}

// Operators
static void test_operators() {
    checkTypes("a!=b", {
        TokenType::IDENTIFIER, TokenType::NEQ, TokenType::IDENTIFIER, TokenType::END_OF_FILE,
    }, "neq operator");

    checkTypes("a and b", {
        TokenType::IDENTIFIER, TokenType::AND, TokenType::IDENTIFIER, TokenType::END_OF_FILE,
    }, "and operator");

    checkTypes("a or b", {
        TokenType::IDENTIFIER, TokenType::OR, TokenType::IDENTIFIER, TokenType::END_OF_FILE,
    }, "or operator");

    checkTypes("not a", {
        TokenType::NOT, TokenType::IDENTIFIER, TokenType::END_OF_FILE,
    }, "not operator");

    checkTypes("x==y", {
        TokenType::IDENTIFIER, TokenType::EQEQ, TokenType::IDENTIFIER, TokenType::END_OF_FILE,
    }, "eqeq operator");

    checkTypes("x<y", {
        TokenType::IDENTIFIER, TokenType::LT, TokenType::IDENTIFIER, TokenType::END_OF_FILE,
    }, "lt operator");

    checkTypes("x>y", {
        TokenType::IDENTIFIER, TokenType::GT, TokenType::IDENTIFIER, TokenType::END_OF_FILE,
    }, "gt operator");
}

// Literals
static void test_literals() {
    checkTypes("true", {
        TokenType::TRUE, TokenType::END_OF_FILE,
    }, "true literal");

    checkTypes("false", {
        TokenType::FALSE, TokenType::END_OF_FILE,
    }, "false literal");

    checkTypes("42", {
        TokenType::CONSTANT, TokenType::END_OF_FILE,
    }, "constant literal");
}

// Whitespace handling
static void test_whitespace() {
    checkTypes("  x  =  1  ", {
        TokenType::IDENTIFIER, TokenType::EQ, TokenType::CONSTANT, TokenType::END_OF_FILE,
    }, "whitespace around tokens");

    checkTypes("x\n\n\ny", {
        TokenType::IDENTIFIER, TokenType::IDENTIFIER, TokenType::END_OF_FILE,
    }, "newlines between tokens");

    checkTypes("\t x \t", {
        TokenType::IDENTIFIER, TokenType::END_OF_FILE,
    }, "tabs");
}

// Keywords vs identifiers
static void test_keyword_vs_identifier() {
    checkTypes("iff", {
        TokenType::IDENTIFIER, TokenType::END_OF_FILE,
    }, "iff is identifier not if");

    checkTypes("done", {
        TokenType::IDENTIFIER, TokenType::END_OF_FILE,
    }, "done is identifier not do");

    checkTypes("ending", {
        TokenType::IDENTIFIER, TokenType::END_OF_FILE,
    }, "ending is identifier not end");

    checkTypes("whileTrue", {
        TokenType::IDENTIFIER, TokenType::END_OF_FILE,
    }, "whileTrue is identifier not while");

    checkTypes("integer", {
        TokenType::IDENTIFIER, TokenType::END_OF_FILE,
    }, "integer is identifier not int");

    checkTypes("notify", {
        TokenType::IDENTIFIER, TokenType::END_OF_FILE,
    }, "notify is identifier not not");
}

// Delimiters
static void test_delimiters() {
    checkTypes("(x+1)*y", {
        TokenType::LPAREN, TokenType::IDENTIFIER, TokenType::PLUS, TokenType::CONSTANT,
        TokenType::RPAREN, TokenType::MUL, TokenType::IDENTIFIER, TokenType::END_OF_FILE,
    }, "parens and arithmetic");
}

// ============================================================

int main() {
    test_assign();
    test_assert();
    test_if();
    test_while();
    test_operators();
    test_literals();
    test_whitespace();
    test_keyword_vs_identifier();
    test_delimiters();

    if (failures == 0)
        std::cout << "\nAll tests passed.\n";
    else
        std::cout << "\n" << failures << " test(s) FAILED.\n";

    return failures == 0 ? 0 : 1;
}
