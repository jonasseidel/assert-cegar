#pragma once

class StatementBlock;
class VariableExpression;
class IntLiteralExpression;
class BoolLiteralExpression;
class BinaryArithmeticExpression;
class UnaryBooleanExpression;
class BinaryBooleanExpression;
class BinaryRelationalExpression;
class DeclarationStatement;
class AssertionStatement;
class AssignStatement;
class IfStatement;
class WhileStatement;

class ASTVisitor {

public:
    virtual void visit(StatementBlock& node) = 0;
    virtual void visit(VariableExpression& node) = 0;
    virtual void visit(IntLiteralExpression& node) = 0;
    virtual void visit(BoolLiteralExpression& node) = 0;
    virtual void visit(BinaryArithmeticExpression& node) = 0;
    virtual void visit(UnaryBooleanExpression& node) = 0;
    virtual void visit(BinaryBooleanExpression& node) = 0;
    virtual void visit(BinaryRelationalExpression& node) = 0;
    virtual void visit(DeclarationStatement& node) = 0;
    virtual void visit(AssertionStatement& node) = 0;
    virtual void visit(AssignStatement& node) = 0;
    virtual void visit(IfStatement& node) = 0;
    virtual void visit(WhileStatement& node) = 0;

};