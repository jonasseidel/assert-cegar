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

class IfStatement : public Statement{
public:
    std::unique_ptr<Expression> guard;
    std::unique_ptr<Statement> thenStatement;
    std::unique_ptr<Statement> elseStatement;
};

class WhileStatement :public Statement{
public:
    std::unique_ptr<Expression> guard;
    std::unique_ptr<Statement> bodyStatement;
};

class ArithmeticExpression : public Expression {
    
};

class BooleanExpression : public Expression {

};