#include <vector>
#include <memory>
#include "token.h"

class Node {
public:
    virtual ~Node() = default; //default destructor
};

class Statement : public Node {

};

class Expression : public Node {

};

class StatementBlock : public Statement{
public:
    std::vector<std::unique_ptr<Statement>> statements;
};

class AssertionStatement : public Statement {
public:
    //For now only boolean expressions. Later add more general predicates
    std::unique_ptr<BooleanExpression> assertion;
};

class AssignStatement : public Statement{
public:
    Token<std::string_view> target;
    std::unique_ptr<ArithmeticExpression> expression;
};

class IfStatement : public Statement{
public:
    std::unique_ptr<BooleanExpression> guard;
    std::unique_ptr<Statement> thenStatement;
    std::unique_ptr<Statement> elseStatement;
};

class WhileStatement :public Statement{
public:
    std::unique_ptr<BooleanExpression> guard;
    std::unique_ptr<Statement> bodyStatement;
};

class ArithmeticExpression : public Expression {
public:
    virtual ~ArithmeticExpression() = default;
};

class BooleanExpression : public Expression {
public:
    virtual ~BooleanExpression() = default;
};

class VariableExpression : public ArithmeticExpression{
public:
    Token<std::string_view> identifier;

    explicit VariableExpression(Token<std::string_view> identifier) : identifier(std::move(identifier)) {}
    //Move semantics
    VariableExpression(VariableExpression&&) = default;
    VariableExpression& operator=(VariableExpression&&) = default;
    //No accidential copies
    VariableExpression(VariableExpression&) = delete;
    VariableExpression& operator=(VariableExpression&) = delete;

};

class IntLiteralExpression : public ArithmeticExpression{
public:
    Token<int> value;

    explicit IntLiteralExpression(Token<int> value) : value(std::move(value)) {}
    //Move semantics
    IntLiteralExpression(IntLiteralExpression&&) = default;
    IntLiteralExpression& operator=(IntLiteralExpression&&) = default;
    //No accidential copies
    IntLiteralExpression(IntLiteralExpression&) = delete;
    IntLiteralExpression& operator=(IntLiteralExpression&) = delete;
};

class BoolLiteralExpression : public BooleanExpression{
    Token<bool> value;

    explicit BoolLiteralExpression(Token<bool> value) : value(std::move(value)) {}
    //Move semantics
    BoolLiteralExpression(BoolLiteralExpression&&) = default;
    BoolLiteralExpression& operator=(BoolLiteralExpression&&) = default;
    //No accidential copies
    BoolLiteralExpression(BoolLiteralExpression&) = delete;
    BoolLiteralExpression& operator=(BoolLiteralExpression&) = delete;
};


//TODO add constructors, remove copy constructor
class BinaryArithmeticExpression: public ArithmeticExpression{
public:
    Token<std::string_view> operation;
    std::unique_ptr<ArithmeticExpression> leftExpression;
    std::unique_ptr<ArithmeticExpression> rightExpression;
};

class BinaryBooleanExpression: public BooleanExpression{
public:
    Token<std::string_view> operation;
    std::unique_ptr<BooleanExpression> leftExpression;
    std::unique_ptr<BooleanExpression> rightExpression;

};

class BinaryRelationalExpression: public BooleanExpression{
public:
    Token<std::string_view> operation;
    std::unique_ptr<ArithmeticExpression> leftExpression;
    std::unique_ptr<ArithmeticExpression> rightExpression;
};