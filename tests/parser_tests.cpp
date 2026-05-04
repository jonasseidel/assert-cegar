#include <iostream>
#include <string>
#include <vector>
#include <cassert>
#include "../src/lexer/lexer.h"
#include "../src/parser/parser.h"

static int failures = 0;

static void expectParse(const std::string& source,
                        const std::vector<int>& expected,
                        const std::string& testName) {
    auto tokens = Lexer(source).tokenize();
    Parser parser(tokens);
    auto ast = parser.parse();
    if (!ast) {
        std::cerr << "  FAIL [" << testName << "]: parse() returned nullopt. Here the log:\n";
        parser.dumpLog();
        ++failures;
        return;
    }
    const auto& trace = parser.get_derivation();
    if (trace != expected) {
        std::cerr << "  FAIL [" << testName << "]: unexpected trace. Got:\n";
        for (int p : trace) std::cerr << ' ' << p;
        std::cerr << "\n    Expected:";
        for (int p : expected) std::cerr << ' ' << p;
        std::cerr << "\nHere the log";
        parser.dumpLog();
        ++failures;
    } else {
        std::cout << "PASS " + testName + '\n';
    }
}

// ============================================================
// Tests
// ===========================================================

// assign0.txt: x=1+1
static void test_assign() {
    expectParse("x=1+1", {14, 14, 9, 4, 1}, "assign0");
    expectParse("x= x + 2*2", {13, 14, 14, 10, 9, 4, 1}, "assign1");
}

// Check AST structure for x=1+1:
//   StatementBlock[ AssignStatement("x", BinaryArith(PLUS, IntLit(1), IntLit(1))) ]
static void test_assign_ast() {
    const std::string testName = "assign0_ast";
    const std::string source = "x=1+1";
    auto tokens = Lexer(source).tokenize();
    Parser parser(tokens);
    auto ast = parser.parse();
    if (!ast) {
        std::cerr << "  FAIL [" << testName << "]: parse returned nullopt\n";
        ++failures; return;
    }
    auto* block = dynamic_cast<StatementBlock*>(ast->get());
    if (!block || block->statements.size() != 1) {
        std::cerr << "  FAIL [" << testName << "]: expected StatementBlock with 1 statement\n";
        ++failures; return;
    }
    auto* assign = dynamic_cast<AssignStatement*>(block->statements[0].get());
    if (!assign) {
        std::cerr << "  FAIL [" << testName << "]: statements[0] is not an AssignStatement (got type: " << typeid(*block->statements[0]).name() << ")\n";
        ++failures; return;
    }
    if (assign->target != "x") {
        std::cerr << "  FAIL [" << testName << "]: expected target 'x', got '" << assign->target << "'\n";
        ++failures; return;
    }
    auto* binExpr = dynamic_cast<BinaryArithmeticExpression*>(assign->expression.get());
    if (!binExpr || binExpr->operation != TokenType::PLUS) {
        std::cerr << "  FAIL [" << testName << "]: expected BinaryArithmeticExpression(PLUS)\n";
        ++failures; return;
    }
    auto* left  = dynamic_cast<IntLiteralExpression*>(binExpr->leftExpression.get());
    auto* right = dynamic_cast<IntLiteralExpression*>(binExpr->rightExpression.get());
    if (!left || left->value != 1 || !right || right->value != 1) {
        std::cerr << "  FAIL [" << testName << "]: expected IntLit(1) + IntLit(1)\n";
        ++failures; return;
    }
    std::cout << "PASS " << testName << '\n';
}

// ============================================================

int main() {
    test_assign();
    test_assign_ast();

    if (failures == 0)
        std::cout << "All tests passed.\n";
    else
        std::cout << failures << " test(s) FAILED.\n";

    return failures == 0 ? 0 : 1;
}
