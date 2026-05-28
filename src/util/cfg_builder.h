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
        finishBlock();
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

    void visit(BoolLiteralExpression& node) override{
        auto operand = std::make_unique<ConstantBoolOperand>();
        operand->value = node.value;
        currentOperand = std::move(operand);
    };


    void visit(UnaryBooleanExpression& node) override{
        auto instr = std::make_unique<UnOpInstr>();
        
        node.operand->accept(*this);

        instr->operand = std::move(currentOperand);
        instr->dest = "t" + std::to_string(tempVarCounter++);
        instr->op = node.operation;

        auto operand = std::make_unique<VariableOperand>();
        operand->name = instr->dest;
        currentOperand = std::move(operand);

        pendingInstructions.push_back(std::move(instr));
    };

    void visit(BinaryBooleanExpression& node) override{

        auto instr = std::make_unique<BinOpInstr>();
        
        
        node.leftExpression->accept(*this);
        instr->lhs = std::move(currentOperand);

        node.rightExpression->accept(*this);
        instr->rhs = std::move(currentOperand);

        instr->dest = "t" + std::to_string(tempVarCounter++);
        instr->op = node.operation;

        auto operand = std::make_unique<VariableOperand>();
        operand->name = instr->dest;
        currentOperand = std::move(operand);

        pendingInstructions.push_back(std::move(instr));
    };

    void visit(BinaryRelationalExpression& node) override{

        auto instr = std::make_unique<BinOpInstr>();
        
        
        node.leftExpression->accept(*this);
        instr->lhs = std::move(currentOperand);

        node.rightExpression->accept(*this);
        instr->rhs = std::move(currentOperand);

        instr->dest = "t" + std::to_string(tempVarCounter++);
        instr->op = node.operation;

        auto operand = std::make_unique<VariableOperand>();
        operand->name = instr->dest;
        currentOperand = std::move(operand);

        pendingInstructions.push_back(std::move(instr));
    };

    void visit(IfStatement& node) override{

        node.guard->accept(*this);
        auto* condTerm = new ConditionalTerminatorInstr();
        condTerm->guard = std::move(currentOperand);
        pendingInstructions.push_back(std::unique_ptr<Instruction>(condTerm));
        finishBlock();


        int thenBlockIdx = cfg.blocks.size();
        node.thenStatement->accept(*this);
        auto thenTerminator = std::make_unique<UnconditionalTerminatorInstr>();
        

        int elseBlockIdx = cfg.blocks.size();
        if(node.elseStatement.has_value()){
            node.elseStatement.value()->accept(*this);
            auto elseTerminator = std::make_unique<UnconditionalTerminatorInstr>();
            elseTerminator->target = cfg.blocks.size();
            cfg.blocks[elseBlockIdx].instructions.push_back(std::move(elseTerminator));
        }

        int mergeIdx = cfg.blocks.size();
        thenTerminator->target = mergeIdx;
        cfg.blocks[thenBlockIdx].instructions.push_back(std::move(thenTerminator));

        condTerm->trueTarget = thenBlockIdx;
        condTerm->falseTarget = node.elseStatement.has_value() ? elseBlockIdx : mergeIdx;
    };

    void finishBlock() {
        Block block;
        block.instructions = std::move(pendingInstructions);
        cfg.blocks.push_back(std::move(block));
        pendingInstructions.clear();
    };

public:
    CFG build(StatementBlock& root) {
        visit(root);
        return std::move(cfg);
    }

private:
    void visit(DeclarationStatement& node) override{};
    void visit(AssertionStatement& node) override{};
    void visit(WhileStatement& node) override{};
};