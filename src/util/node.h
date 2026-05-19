#pragma once
#include <vector>
#include <memory>
#include <optional>
#include <ostream>
#include <string>

//#include "token.h"
#include "operations.h"

#include "ast_visitor.h"

// inline const char* tokenTypeToString(TokenType t) {
//     switch (t) {
//         case TokenType::PLUS:  return "+";
//         case TokenType::MINUS: return "-";
//         case TokenType::MUL:   return "*";
//         case TokenType::EQEQ:  return "==";
//         case TokenType::NEQ:   return "!=";
//         case TokenType::LT:    return "<";
//         case TokenType::GT:    return ">";
//         case TokenType::AND:   return "and";
//         case TokenType::OR:    return "or";
//         case TokenType::NOT:   return "not";
//         default:               return "?";
//     }
// }

inline void dumpIndent(std::ostream& os, int indent) {
    for (int i = 0; i < indent; ++i) os << "  ";
}

class Node {
public:
    virtual ~Node() = default;

    Node() = default;
    Node(const Node&) = delete;
    Node& operator=(const Node&) = delete;

    Node(Node&&) = default;
    Node& operator=(Node&&) = default;

    virtual void accept(ASTVisitor& v) = 0;
    virtual void print(std::ostream& os) const = 0;
};

class Statement : public Node {
};

class Expression : public Node {
};

class StatementBlock : public Statement {
public:
    std::vector<std::unique_ptr<Statement>> statements;

    void accept(ASTVisitor& v) override { v.visit(*this); }

    void print(std::ostream& os) const override {
        for (const auto& s : statements)
            s->print(os);
    }

};

class ArithmeticExpression : public Expression {
};

class BooleanExpression : public Expression {
};

class VariableExpression : public ArithmeticExpression {
public:
    std::string name;

    explicit VariableExpression(std::string name) : name(std::move(name)) {}

    void accept(ASTVisitor& v) override { v.visit(*this); }

    void print(std::ostream& os) const override {
        os << name;
    }

};

class IntLiteralExpression : public ArithmeticExpression {
public:
    int value;

    explicit IntLiteralExpression(int value) : value(value) {}

    void accept(ASTVisitor& v) override { v.visit(*this); }
    
    void print(std::ostream& os) const override {
        os << value;
    }

};

class BoolLiteralExpression : public BooleanExpression {
public:
    bool value;

    explicit BoolLiteralExpression(bool value) : value(value) {}

    void accept(ASTVisitor& v) override { v.visit(*this); }

    void print(std::ostream& os) const override {
        os << (value ? "true" : "false");
    }

};

class BinaryArithmeticExpression : public ArithmeticExpression {
public:
    ArithOp operation;
    std::unique_ptr<ArithmeticExpression> leftExpression;
    std::unique_ptr<ArithmeticExpression> rightExpression;

    BinaryArithmeticExpression(ArithOp operation,
        std::unique_ptr<ArithmeticExpression> leftExpression,
        std::unique_ptr<ArithmeticExpression> rightExpression)
        : operation(operation),
          leftExpression(std::move(leftExpression)),
          rightExpression(std::move(rightExpression)) {}

    void accept(ASTVisitor& v) override { v.visit(*this); }

    void print(std::ostream& os) const override {
        leftExpression->print(os);
        os << toString(operation);
        rightExpression->print(os);
    }

};

class UnaryBooleanExpression : public BooleanExpression {
public:
    UnaryBoolOp operation;
    std::unique_ptr<BooleanExpression> operand;

    explicit UnaryBooleanExpression(UnaryBoolOp operation, std::unique_ptr<BooleanExpression> operand)
        : operation(operation), operand(std::move(operand)) {}

    void accept(ASTVisitor& v) override { v.visit(*this); }

    void print(std::ostream& os) const override {
        os << toString(operation);
        operand->print(os);
    }

};

class BinaryBooleanExpression : public BooleanExpression {
public:
    BoolOp operation;
    std::unique_ptr<BooleanExpression> leftExpression;
    std::unique_ptr<BooleanExpression> rightExpression;

    BinaryBooleanExpression(BoolOp operation,
        std::unique_ptr<BooleanExpression> leftExpression,
        std::unique_ptr<BooleanExpression> rightExpression)
        : operation(operation),
          leftExpression(std::move(leftExpression)),
          rightExpression(std::move(rightExpression)) {}

    void accept(ASTVisitor& v) override { v.visit(*this); }

    void print(std::ostream& os) const override {
        leftExpression->print(os);
        os << toString(operation);
        rightExpression->print(os);
    }

};

class BinaryRelationalExpression : public BooleanExpression {
public:
    RelOp operation;
    std::unique_ptr<ArithmeticExpression> leftExpression;
    std::unique_ptr<ArithmeticExpression> rightExpression;

    BinaryRelationalExpression(RelOp operation,
        std::unique_ptr<ArithmeticExpression> leftExpression,
        std::unique_ptr<ArithmeticExpression> rightExpression)
        : operation(operation),
          leftExpression(std::move(leftExpression)),
          rightExpression(std::move(rightExpression)) {}
    
    void accept(ASTVisitor& v) override { v.visit(*this); }

    void print(std::ostream& os) const override {
        leftExpression->print(os);
        os << toString(operation);
        rightExpression->print(os);
    }

};

class DeclarationStatement : public Statement {
public:
    std::string name;

    explicit DeclarationStatement(std::string name) : name(std::move(name)) {}

    void accept(ASTVisitor& v) override { v.visit(*this); }

    void print(std::ostream& os) const override {
        os << "int " << name << std::endl;
    }

};

class AssertionStatement : public Statement {
public:
    std::unique_ptr<BooleanExpression> assertion;

    void accept(ASTVisitor& v) override { v.visit(*this); }

    void print(std::ostream& os) const override {
        os << "assertion: ";
        assertion->print(os);
        os << std::endl;
    }

};

class AssignStatement : public Statement {
public:
    std::string target;
    std::unique_ptr<ArithmeticExpression> expression;

    void print(std::ostream& os) const override {
        os << target << " = ";
        expression->print(os);
        os << std::endl;
    }
    void accept(ASTVisitor& v) override { v.visit(*this); }

};

class IfStatement : public Statement {
public:
    std::unique_ptr<BooleanExpression> guard;
    std::unique_ptr<Statement> thenStatement;
    std::optional<std::unique_ptr<Statement>> elseStatement;

    void accept(ASTVisitor& v) override { v.visit(*this); }

    void print(std::ostream& os) const override {
        os << "if ";
        guard->print(os);
        os << " then ";
        thenStatement->print(os);
        if (elseStatement.has_value()) {
            os << " else ";
            (*elseStatement)->print(os);
        }
        os << std::endl;
    }

};

class WhileStatement : public Statement {
public:
    std::unique_ptr<BooleanExpression> guard;
    std::unique_ptr<Statement> bodyStatement;

    void accept(ASTVisitor& v) override { v.visit(*this); }

    void print(std::ostream& os) const override {
        os << "while ";
        guard->print(os);
        os << " do ";
        bodyStatement->print(os);
        os << std::endl;
    }

};
