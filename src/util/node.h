#pragma once
#include <vector>
#include <memory>
#include <optional>
#include "token.h"

class Node {
public:
    virtual ~Node() = default; //default constructur virtual so that at runtime the correct destructor is chosen
    
    Node() = default;
    //No copies, only moves
    Node(const Node&) = delete;
    Node& operator=(const Node&) = delete;

    Node(Node&&) = default;
    Node& operator=(Node&&) = default;
    virtual void print(std::ostream& os) const = 0;
};

class Statement : public Node {
};

class Expression : public Node {
};

class StatementBlock : public Statement{
public:
    std::vector<std::unique_ptr<Statement>> statements;

    void print(std::ostream& os) const override;
};


class ArithmeticExpression : public Expression {
};

class BooleanExpression : public Expression {
};

class VariableExpression : public ArithmeticExpression{
public:
    Token<std::string_view> token;

    explicit VariableExpression(Token<std::string_view> identifier) : token(std::move(identifier)) {}

    void print(std::ostream& os) const override;
};

class IntLiteralExpression : public ArithmeticExpression{
public:
    Token<int> token;

    explicit IntLiteralExpression(Token<int> value) : token(std::move(value)) {}

    void print(std::ostream& os) const override{
    os << token.value;
    }
};

class BoolLiteralExpression : public BooleanExpression{
public:
    Token<bool> token;

    explicit BoolLiteralExpression(Token<bool> value) : token(std::move(value)) {}

    void print(std::ostream& os) const override{
    os << (token.value ? "true":"false" );
}
};


class BinaryArithmeticExpression: public ArithmeticExpression{
public:
    Token<std::string_view> operation;
    std::unique_ptr<ArithmeticExpression> leftExpression;
    std::unique_ptr<ArithmeticExpression> rightExpression;

    BinaryArithmeticExpression(Token<std::string_view> operation, 
        std::unique_ptr<ArithmeticExpression> leftExpression,
        std::unique_ptr<ArithmeticExpression> rightExpression) 
        : operation(std::move(operation)), 
        leftExpression(std::move(leftExpression)), 
        rightExpression(std::move(rightExpression)) {}

    void print(std::ostream& os) const override{
    leftExpression->print(os);
    os << operation.value;
    rightExpression->print(os);
    }
};

class BinaryBooleanExpression: public BooleanExpression{
public:
    Token<std::string_view> operation;
    std::unique_ptr<BooleanExpression> leftExpression;
    std::unique_ptr<BooleanExpression> rightExpression;
    BinaryBooleanExpression(Token<std::string_view> operation, 
        std::unique_ptr<BooleanExpression> leftExpression,
        std::unique_ptr<BooleanExpression> rightExpression) 
        : operation(std::move(operation)), 
        leftExpression(std::move(leftExpression)), 
        rightExpression(std::move(rightExpression)) {}

    void print(std::ostream& os) const override{
    leftExpression->print(os);
    os << operation.value;
    rightExpression->print(os);
    }
};

class BinaryRelationalExpression: public BooleanExpression{
public:
    Token<std::string_view> operation;
    std::unique_ptr<ArithmeticExpression> leftExpression;
    std::unique_ptr<ArithmeticExpression> rightExpression;
    BinaryRelationalExpression(Token<std::string_view> operation, 
        std::unique_ptr<ArithmeticExpression> leftExpression,
        std::unique_ptr<ArithmeticExpression> rightExpression) 
        : operation(std::move(operation)), 
        leftExpression(std::move(leftExpression)), 
        rightExpression(std::move(rightExpression)) {}

    void print(std::ostream& os) const override{
    leftExpression->print(os);
    os << operation.value;
    rightExpression->print(os);
    }
};

class AssertionStatement : public Statement {
public:
    //For now only boolean expressions. Later add more general predicates
    std::unique_ptr<BooleanExpression> assertion;

    void print(std::ostream& os) const override{
    os << "assertion: ";
    assertion->print(os);
    os << std::endl;
    }
};

class AssignStatement : public Statement{
public:
    Token<std::string_view> target;
    std::unique_ptr<ArithmeticExpression> expression;

    void print(std::ostream& os) const override{
    os << target.value;
    os << " = ";
    expression->print(os);
    os << std::endl;
    }
};

class IfStatement : public Statement{
public:
    std::unique_ptr<BooleanExpression> guard;
    std::unique_ptr<Statement> thenStatement;
    std::optional<std::unique_ptr<Statement>> elseStatement;

    void print(std::ostream& os) const override{
        os << "if ";
        guard->print(os);
        os << " then ";
        thenStatement->print(os);
        if(elseStatement.has_value()){
            os << " else ";
            (*elseStatement)->print(os);
        }
        os << std::endl;
    }
};

class WhileStatement :public Statement{
public:
    std::unique_ptr<BooleanExpression> guard;
    std::unique_ptr<Statement> bodyStatement;

    void print(std::ostream& os) const override{
        os << "while ";
        guard->print(os);
        os << " do ";
        bodyStatement->print(os);
        os << std::endl;
    }

};