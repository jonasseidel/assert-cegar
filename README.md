Implementation of a very simple programming language with assert statements. 
The compiler tries to prove every occuring assertion correct using counterexample guided abstraction refinement. 
If a counterexample is found, it is provided to the user.
If an assertion could not be proven within max iteration limit, user is notified and compilation fails
=> Only compiles if all assertions proven.

IDEAS
- uses predicates and info of proven assertions for following assertions
- ...

TODO
- implement simple language (assign, assert, skip, if else, while)
- implement predicate abstraction and abstract interpretation
- implement CEGAR loop
- ...
