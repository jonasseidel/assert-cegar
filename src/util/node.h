#pragma once
#include <vector>
#include <memory>
#include <optional>
#include <ostream>
#include <string>
#include "token.h"

inline const char* tokenTypeToString(TokenType t) {
    switch (t) {
        case TokenType::PLUS:  return "+";
        case TokenType::MINUS: return "-";
        case TokenType::MUL:   return "*";
        case TokenType::EQEQ:  return "==";
        case TokenType::NEQ:   return "!=";
        case TokenType::LT:    return "<";
        case TokenType::GT:    return ">";
        case TokenType::AND:   return "and";
        case TokenType::OR:    return "or";
        case TokenType::NOT:   return "not";
        default:               return "?";
    }
}

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

    virtual void print(std::ostream& os) const = 0;
    virtual void dump(std::ostream& os, int indent = 0) const = 0;
};

class Statement : public Node {
};

class Expression : public Node {
};

class StatementBlock : public Statement {
public:
    std::vector<std::unique_ptr<Statement>> statements;

    void print(std::ostream& os) const override {
        for (const auto& s : statements)
            s->print(os);
    }
    void dump(std::ostream& os, int indent = 0) const override {
        dumpIndent(os, indent); os << "StatementBlock\n";
        for (const auto& s : statements)
            s->dump(os, indent + 1);
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

    void print(std::ostream& os) const override {
        os << name;
    }
    void dump(std::ostream& os, int indent = 0) const override {
        dumpIndent(os, indent); os << "Var: " << name << "\n";
    }
};

class IntLiteralExpression : public ArithmeticExpression {
public:
    int value;

    explicit IntLiteralExpression(int value) : value(value) {}

    void print(std::ostream& os) const override {
        os << value;
    }
    void dump(std::ostream& os, int indent = 0) const override {
        dumpIndent(os, indent); os << "Int: " << value << "\n";
    }
};

class BoolLiteralExpression : public BooleanExpression {
public:
    bool value;

    explicit BoolLiteralExpression(bool value) : value(value) {}

    void print(std::ostream& os) const override {
        os << (value ? "true" : "false");
    }
    void dump(std::ostream& os, int indent = 0) const override {
        dumpIndent(os, indent); os << "Bool: " << (value ? "true" : "false") << "\n";
    }
};

class BinaryArithmeticExpression : public ArithmeticExpression {
public:
    TokenType operation;
    std::unique_ptr<ArithmeticExpression> leftExpression;
    std::unique_ptr<ArithmeticExpression> rightExpression;

    BinaryArithmeticExpression(TokenType operation,
        std::unique_ptr<ArithmeticExpression> leftExpression,
        std::unique_ptr<ArithmeticExpression> rightExpression)
        : operation(operation),
          leftExpression(std::move(leftExpression)),
          rightExpression(std::move(rightExpression)) {}

    void print(std::ostream& os) const override {
        leftExpression->print(os);
        os << tokenTypeToString(operation);
        rightExpression->print(os);
    }
    void dump(std::ostream& os, int indent = 0) const override {
        dumpIndent(os, indent); os << "BinaryArith: " << tokenTypeToString(operation) << "\n";
        leftExpression->dump(os, indent + 1);
        rightExpression->dump(os, indent + 1);
    }
};

class UnaryBooleanExpression : public BooleanExpression {
public:
    std::unique_ptr<BooleanExpression> operand;

    explicit UnaryBooleanExpression(std::unique_ptr<BooleanExpression> operand)
        : operand(std::move(operand)) {}

    void print(std::ostream& os) const override {
        os << "not ";
        operand->print(os);
    }
    void dump(std::ostream& os, int indent = 0) const override {
        dumpIndent(os, indent); os << "UnaryBool: not\n";
        operand->dump(os, indent + 1);
    }
};

class BinaryBooleanExpression : public BooleanExpression {
public:
    TokenType operation;
    std::unique_ptr<BooleanExpression> leftExpression;
    std::unique_ptr<BooleanExpression> rightExpression;

    BinaryBooleanExpression(TokenType operation,
        std::unique_ptr<BooleanExpression> leftExpression,
        std::unique_ptr<BooleanExpression> rightExpression)
        : operation(operation),
          leftExpression(std::move(leftExpression)),
          rightExpression(std::move(rightExpression)) {}

    void print(std::ostream& os) const override {
        leftExpression->print(os);
        os << tokenTypeToString(operation);
        rightExpression->print(os);
    }
    void dump(std::ostream& os, int indent = 0) const override {
        dumpIndent(os, indent); os << "BinaryBool: " << tokenTypeToString(operation) << "\n";
        leftExpression->dump(os, indent + 1);
        rightExpression->dump(os, indent + 1);
    }
};

class BinaryRelationalExpression : public BooleanExpression {
public:
    TokenType operation;
    std::unique_ptr<ArithmeticExpression> leftExpression;
    std::unique_ptr<ArithmeticExpression> rightExpression;

    BinaryRelationalExpression(TokenType operation,
        std::unique_ptr<ArithmeticExpression> leftExpression,
        std::unique_ptr<ArithmeticExpression> rightExpression)
        : operation(operation),
          leftExpression(std::move(leftExpression)),
          rightExpression(std::move(rightExpression)) {}

    void print(std::ostream& os) const override {
        leftExpression->print(os);
        os << tokenTypeToString(operation);
        rightExpression->print(os);
    }
    void dump(std::ostream& os, int indent = 0) const override {
        dumpIndent(os, indent); os << "BinaryRelational: " << tokenTypeToString(operation) << "\n";
        leftExpression->dump(os, indent + 1);
        rightExpression->dump(os, indent + 1);
    }
};

class DeclarationStatement : public Statement {
public:
    std::string name;

    explicit DeclarationStatement(std::string name) : name(std::move(name)) {}

    void print(std::ostream& os) const override {
        os << "int " << name << std::endl;
    }
    void dump(std::ostream& os, int indent = 0) const override {
        dumpIndent(os, indent); os << "Decl: " << name << "\n";
    }
};

class AssertionStatement : public Statement {
public:
    std::unique_ptr<BooleanExpression> assertion;

    void print(std::ostream& os) const override {
        os << "assertion: ";
        assertion->print(os);
        os << std::endl;
    }
    void dump(std::ostream& os, int indent = 0) const override {
        dumpIndent(os, indent); os << "Assert\n";
        assertion->dump(os, indent + 1);
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
    void dump(std::ostream& os, int indent = 0) const override {
        dumpIndent(os, indent); os << "Assign: " << target << "\n";
        expression->dump(os, indent + 1);
    }
};

class IfStatement : public Statement {
public:
    std::unique_ptr<BooleanExpression> guard;
    std::unique_ptr<Statement> thenStatement;
    std::optional<std::unique_ptr<Statement>> elseStatement;

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
    void dump(std::ostream& os, int indent = 0) const override {
        dumpIndent(os, indent); os << "If\n";
        dumpIndent(os, indent + 1); os << "guard:\n";
        guard->dump(os, indent + 2);
        dumpIndent(os, indent + 1); os << "then:\n";
        thenStatement->dump(os, indent + 2);
        if (elseStatement.has_value()) {
            dumpIndent(os, indent + 1); os << "else:\n";
            (*elseStatement)->dump(os, indent + 2);
        }
    }
};

class WhileStatement : public Statement {
public:
    std::unique_ptr<BooleanExpression> guard;
    std::unique_ptr<Statement> bodyStatement;

    void print(std::ostream& os) const override {
        os << "while ";
        guard->print(os);
        os << " do ";
        bodyStatement->print(os);
        os << std::endl;
    }
    void dump(std::ostream& os, int indent = 0) const override {
        dumpIndent(os, indent); os << "While\n";
        dumpIndent(os, indent + 1); os << "guard:\n";
        guard->dump(os, indent + 2);
        dumpIndent(os, indent + 1); os << "body:\n";
        bodyStatement->dump(os, indent + 2);
    }
};
