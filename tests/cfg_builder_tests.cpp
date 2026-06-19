#include <iostream>
#include <cassert>
#include "../src/lexer/lexer.h"
#include "../src/parser/parser.h"
#include "../src/util/cfg_builder.h"
#include "../src/util/cfg_printer.h"

static CFG buildFrom(const std::string& source) {
    auto tokens = Lexer(source).tokenize();
    Parser parser(tokens);
    auto ast = parser.parse();
    assert(ast);
    CFGBuilder builder;
    return builder.build(*dynamic_cast<StatementBlock*>(ast->get()));
}

static void test_straight_line() {
    CFG cfg = buildFrom("y=6 x=5*(y+1)");
    printCFG(cfg, std::cout);

    assert(cfg.blocks.size() == 1);
    auto& instrs = cfg.blocks[0].instructions;
    assert(instrs.size() == 4);

    auto* i0 = dynamic_cast<AssignInstr*>(instrs[0].get());
    assert(i0 && i0->dest == "y");
    assert(dynamic_cast<ConstantIntOperand*>(i0->src.get())->value == 6);

    auto* i1 = dynamic_cast<BinOpInstr*>(instrs[1].get());
    assert(i1 && i1->dest == "t0" && std::get<ArithOp>(i1->op) == ArithOp::ADD);
    assert(dynamic_cast<VariableOperand*>(i1->lhs.get())->name == "y");
    assert(dynamic_cast<ConstantIntOperand*>(i1->rhs.get())->value == 1);

    auto* i2 = dynamic_cast<BinOpInstr*>(instrs[2].get());
    assert(i2 && i2->dest == "t1" && std::get<ArithOp>(i2->op) == ArithOp::MUL);
    assert(dynamic_cast<ConstantIntOperand*>(i2->lhs.get())->value == 5);
    assert(dynamic_cast<VariableOperand*>(i2->rhs.get())->name == "t0");

    auto* i3 = dynamic_cast<AssignInstr*>(instrs[3].get());
    assert(i3 && i3->dest == "x");
    assert(dynamic_cast<VariableOperand*>(i3->src.get())->name == "t1");

    std::cout << "PASS cfg_straight_line\n";
}

static void test_if_else() {
    CFG cfg = buildFrom("x=1 if x==1 then y=2 else y=3 end");
    printCFG(cfg, std::cout);

    assert(cfg.blocks.size() == 4);

    auto& b0 = cfg.blocks[0].instructions;
    assert(b0.size() == 3);
    auto* i0 = dynamic_cast<AssignInstr*>(b0[0].get());
    assert(i0 && i0->dest == "x");
    assert(dynamic_cast<ConstantIntOperand*>(i0->src.get())->value == 1);
    auto* i1 = dynamic_cast<BinOpInstr*>(b0[1].get());
    assert(i1 && std::get<RelOp>(i1->op) == RelOp::EQ);
    assert(dynamic_cast<VariableOperand*>(i1->lhs.get())->name == "x");
    assert(dynamic_cast<ConstantIntOperand*>(i1->rhs.get())->value == 1);
    auto* i2 = dynamic_cast<ConditionalTerminatorInstr*>(b0[2].get());
    assert(i2 && i2->trueTarget == 1 && i2->falseTarget == 2);

    auto& b1 = cfg.blocks[1].instructions;
    assert(b1.size() == 2);
    auto* j0 = dynamic_cast<AssignInstr*>(b1[0].get());
    assert(j0 && j0->dest == "y");
    assert(dynamic_cast<ConstantIntOperand*>(j0->src.get())->value == 2);
    auto* j1 = dynamic_cast<UnconditionalTerminatorInstr*>(b1[1].get());
    assert(j1 && j1->target == 3);

    auto& b2 = cfg.blocks[2].instructions;
    assert(b2.size() == 2);
    auto* k0 = dynamic_cast<AssignInstr*>(b2[0].get());
    assert(k0 && k0->dest == "y");
    assert(dynamic_cast<ConstantIntOperand*>(k0->src.get())->value == 3);
    auto* k1 = dynamic_cast<UnconditionalTerminatorInstr*>(b2[1].get());
    assert(k1 && k1->target == 3);

    assert(cfg.blocks[3].instructions.empty());

    std::cout << "PASS cfg_if_else\n";
}

static void test_while() {
    CFG cfg = buildFrom("x=10 while x>0 do x=x-1 end");
    printCFG(cfg, std::cout);

    // Block 0: pre-block (x=10, goto header)
    // Block 1: header (t0 = x > 0, if t0 goto 2 else 3)
    // Block 2: body (t1 = x - 1, x = t1, goto 1)  -- back-edge to header
    // Block 3: exit (empty)
    assert(cfg.blocks.size() == 4);

    auto& b0 = cfg.blocks[0].instructions;
    assert(b0.size() == 2);
    auto* i0 = dynamic_cast<AssignInstr*>(b0[0].get());
    assert(i0 && i0->dest == "x");
    assert(dynamic_cast<ConstantIntOperand*>(i0->src.get())->value == 10);
    auto* pre = dynamic_cast<UnconditionalTerminatorInstr*>(b0[1].get());
    assert(pre && pre->target == 1);

    auto& b1 = cfg.blocks[1].instructions;
    assert(b1.size() == 2);
    auto* g = dynamic_cast<BinOpInstr*>(b1[0].get());
    assert(g && std::get<RelOp>(g->op) == RelOp::GT);
    assert(dynamic_cast<VariableOperand*>(g->lhs.get())->name == "x");
    assert(dynamic_cast<ConstantIntOperand*>(g->rhs.get())->value == 0);
    auto* cond = dynamic_cast<ConditionalTerminatorInstr*>(b1[1].get());
    assert(cond && cond->trueTarget == 2 && cond->falseTarget == 3);

    auto& b2 = cfg.blocks[2].instructions;
    assert(b2.size() == 3);
    auto* d = dynamic_cast<BinOpInstr*>(b2[0].get());
    assert(d && std::get<ArithOp>(d->op) == ArithOp::SUB);
    auto* a = dynamic_cast<AssignInstr*>(b2[1].get());
    assert(a && a->dest == "x");
    auto* back = dynamic_cast<UnconditionalTerminatorInstr*>(b2[2].get());
    assert(back && back->target == 1);  // back-edge to header

    assert(cfg.blocks[3].instructions.empty());

    std::cout << "PASS cfg_while\n";
}

int main() {
    test_straight_line();
    test_if_else();
    test_while();
    std::cout << "All tests passed.\n";
}
