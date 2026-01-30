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

//Make this a template?
template <typename T>
struct Token {
TokenType type;
T value;
std::string_view lexeme;
};

