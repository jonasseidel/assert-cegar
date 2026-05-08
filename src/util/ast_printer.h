#include "ast_visitor.h"

class ASTPrinter : public ASTVisitor {
    int indent = 0;
    std::ostream& os;

    inline void dumpIndent(std::ostream& os, int indent) {
        for (int i = 0; i < indent; ++i) os << "  ";
    }

public:
    ASTPrinter(std::ostream& output_stream) : os(output_stream) {}

    void visit(StatementBlock& node) override{
        dumpIndent(os, indent); 
        os << "StatementBlock\n";
        ++indent;
        for (const auto& s : node.statements)
            s->accept(*this);
        --indent;
    };
    void visit(VariableExpression& node) override{
        dumpIndent(os, indent); 
        os << "Var: " << node.name << "\n";
        
    };
    void visit(IntLiteralExpression& node) override{
        dumpIndent(os, indent); 
        os << "Int: " << node.value << "\n";
    };
    void visit(BoolLiteralExpression& node) override{
        dumpIndent(os, indent); 
        os << "Bool: " << (node.value ? "true" : "false") << "\n";
    };
    void visit(BinaryArithmeticExpression& node) override{
        dumpIndent(os, indent); 
        os << "BinaryArith: " << tokenTypeToString(node.operation) << "\n";
        ++indent;
        node.leftExpression->accept(*this);
        node.rightExpression->accept(*this);
        --indent;
    };
    void visit(UnaryBooleanExpression& node) override{
        dumpIndent(os, indent); 
        os << "UnaryBool: not\n";
        ++indent;
        node.operand->accept(*this);
        --indent;
    };
    void visit(BinaryBooleanExpression& node) override{
        dumpIndent(os, indent); 
        os << "BinaryBool: " << tokenTypeToString(node.operation) << "\n";
        ++indent;
        node.leftExpression->accept(*this);
        node.rightExpression->accept(*this);
        --indent;
    };
    void visit(BinaryRelationalExpression& node) override{
        dumpIndent(os, indent); 
        os << "BinaryRel: " << tokenTypeToString(node.operation) << "\n";
        ++indent;
        node.leftExpression->accept(*this);
        node.rightExpression->accept(*this);
        --indent;
    };
    void visit(DeclarationStatement& node) override{
        dumpIndent(os, indent); 
        os << "Decl: " << node.name << "\n";
    };
    void visit(AssertionStatement& node) override{
        dumpIndent(os, indent); 
        os << "Assert\n";
        ++indent;
        node.assertion->accept(*this);
        --indent;
    };
    void visit(AssignStatement& node) override{
        dumpIndent(os, indent); 
        os << "Assign: " << node.target << "\n";
        ++indent;
        node.expression->accept(*this);
        --indent;
    };
    void visit(IfStatement& node) override{
        dumpIndent(os, indent); 
        os << "If\n";
        ++indent;
        dumpIndent(os, indent); 
        os << "guard:\n";
        ++indent;
        node.guard->accept(*this);
        --indent;
        dumpIndent(os, indent); 
        os << "then:\n";
        ++indent;
        node.thenStatement->accept(*this);
        --indent;
        dumpIndent(os, indent); 
        os << "else:\n";
        ++indent;
        if (node.elseStatement.has_value()) {
            node.elseStatement->get()->accept(*this);
        }
        --indent;
        --indent;
    };
    void visit(WhileStatement& node) override{
        dumpIndent(os, indent); 
        os << "While\n";
        ++indent;
        dumpIndent(os, indent); 
        os << "guard:\n";
        ++indent;
        node.guard->accept(*this);
        --indent;
        dumpIndent(os, indent); 
        os << "body:\n";
        ++indent;
        node.bodyStatement->accept(*this);
        --indent;
        --indent;
    };
};