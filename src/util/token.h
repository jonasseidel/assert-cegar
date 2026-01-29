#include <variant>
#include <string_view>

enum class TokenType {
ASSERT,
IF,
THEN,
ELSE,
WHILE,
DO,
END,
INT,

IDENTIFIER,

EQUALS,
CONSTANT,
PLUS,
MINUS,
MUL,
};

struct Token {

TokenType type;
std::string_view value;

};

