#include "ast_visitor.h"
#include "node.h"
#include "cfg.h"

class CFGBuilder : public ASTVisitor {
    CFG cfg;

    std::vector<std::string> variableNames;
    int tempVarCounter = 0;
    std::string currentResultVar;

    std::vector<std::unique_ptr<Instruction>> pendingInstructions;

    void visit(StatementBlock& node) override{
        for (const auto& s : node.statements)
            s->accept(*this);
    };

    void visit(AssignStatement& node) override{
        auto instr = std::make_unique<AssignInstr>();
        instr->dest = node.target;
        auto operand = std::make_unique<VariableOperand>();

        node.expression->accept(*this);

        operand->name = currentResultVar;
        instr->src = std::move(operand);

        pendingInstructions.push_back(std::move(instr));
    };

    void visit(BinaryArithmeticExpression& node) override{


        auto instr = std::make_unique<BinOpInstr>();
        
        auto operand1 = std::make_unique<VariableOperand>();
        node.leftExpression->accept(*this);
        operand1->name = currentResultVar; //"t" + std::to_string(tempVarCounter);

        auto operand2 = std::make_unique<VariableOperand>();
        node.rightExpression->accept(*this);
        operand2->name = currentResultVar; // "t" + std::to_string(tempVarCounter++);

        instr->lhs = std::move(operand1);
        instr->rhs = std::move(operand2);
        instr->dest = "t" + std::to_string(tempVarCounter++);
        instr->op = node.operation;

        currentResultVar = instr->dest;

        pendingInstructions.push_back(std::move(instr));
    };

    // All remaining cases TODO
};