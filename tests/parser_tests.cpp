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
    auto trace = parser.parse();
    if (!trace) {
        std::cerr << "  FAIL [" << testName << "]: parse() returned nullopt. Here the log:\n";
        parser.dumpLog();
        ++failures;
        return;
    }
    if (*trace != expected) {
        std::cerr << "  FAIL [" << testName << "]: unexpected trace. Got:\n";
        for (int p : *trace) std::cerr << ' ' << p;
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
// ============================================================

// assign0.txt: x=1+1
static void test_assign() {
    expectParse("x=1+1", {14, 14, 9, 4, 1}, "assign0");
}

// ============================================================

int main() {
    test_assign();

    if (failures == 0)
        std::cout << "All tests passed.\n";
    else
        std::cout << failures << " test(s) FAILED.\n";

    return failures == 0 ? 0 : 1;
}
