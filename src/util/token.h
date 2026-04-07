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


// // Typed token for AST nodes
// template <typename T>
// struct Token {
//     TokenType type;
//     T value;
//     std::string_view lexeme;
// };

// We will use a variant to hold the different value types of tokens:
// More flexible in the lexer. In the parser we construct corresponding (typed) AST nodes
using TokenValue = std::variant<std::monostate, int, bool, std::string_view>;
struct Token {
    TokenType type;
    TokenValue value;
    std::string_view lexeme;
};