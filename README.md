WORK IN PROGRESS

GOAL
Implementation of a very simple programming language with assert statements. 
The compiler tries to prove every occurring assertion correct using counterexample guided abstraction refinement (CEGAR). If a counterexample is found, it is provided to the user.
If an assertion could not be proven within max iteration limit, user is notified and compilation fails
=> Only compiles if all assertions proven or by adding an option to ignore unchecked assertions.
Compiles to LLVM IR.

IMPLEMENTED
Custom frontend (lexer + parser) for simple language.
Easily extendable to more language construct using implemented codegen for both lexer DFA and parser action tables. 

NEXT UP
Translation to a CFG and from there to LLVM IR.
Then implement CEGAR verification on CFG-level and annotate CFG with resuls for LLVM-level optimisations.

PIPELINE
source -> lexer -> parser -> AST -> CEGAR -> LLVM IR

IDEAS
- uses predicates and info of proven assertions for following assertions
- Compile to LLVM IR
- Implement LLVM optimization passes using information from assertions/predicates from CEGAR (dead code, stronger constant propagation (llvm.assume), loop unrolling semantics guided)

BUILD
in ./build directory run "cmake --build ."
 