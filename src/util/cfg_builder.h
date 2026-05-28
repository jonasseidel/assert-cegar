#include "ast_visitor.h"
#include "node.h"
#include "cfg.h"

class CFGBuilder : public ASTVisitor {
    CFG cfg;

    std::vector<std::string> variableNames;
    int tempVarCounter = 0;
    std::string currentResultVar;
    std::unique_ptr<Operand> currentOperand;


    std::vector<std::unique_ptr<Instruction>> pendingInstructions;

    void visit(StatementBlock& node) override{
        for (const auto& s : node.statements)
            s->accept(*this);
    };

    void visit(AssignStatement& node) override{
        auto instr = std::make_unique<AssignInstr>();
        instr->dest = node.target;

        node.expression->accept(*this);

        // auto operand = std::make_unique<VariableOperand>();
        // operand->name = currentResultVar;

        instr->src = std::move(currentOperand);

        pendingInstructions.push_back(std::move(instr));
    };

    void visit(BinaryArithmeticExpression& node) override{


        auto instr = std::make_unique<BinOpInstr>();
        
        
        node.leftExpression->accept(*this);
        // auto operand1 = std::make_unique<VariableOperand>();
        // operand1->name = currentResultVar; //"t" + std::to_string(tempVarCounter);
        instr->lhs = std::move(currentOperand);

        // auto operand2 = std::make_unique<VariableOperand>();
        // operand2->name = currentResultVar; // "t" + std::to_string(tempVarCounter++);

        node.rightExpression->accept(*this);
        instr->rhs = std::move(currentOperand);

        instr->dest = "t" + std::to_string(tempVarCounter++);
        instr->op = node.operation;

        auto operand = std::make_unique<VariableOperand>();
        operand->name = instr->dest;
        currentOperand = std::move(operand);

        // currentResultVar = instr->dest;

        pendingInstructions.push_back(std::move(instr));
    };

    void visit(VariableExpression& node) override{
        auto operand = std::make_unique<VariableOperand>();
        operand->name = node.name;
        currentOperand = std::move(operand);
    };

    void visit(IntLiteralExpression& node) override{
        auto operand = std::make_unique<ConstantIntOperand>();
        operand->value = node.value;
        currentOperand = std::move(operand);
    };
    // All remaining cases TODO
};