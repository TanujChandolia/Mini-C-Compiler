# Mini-C++-Compiler
This project, a Mini Compiler for the C++ programming language, focuses on producing intermediate codes for unique language constructs.
It deals with conditional statements, loops, binary and arithmetic expressions, along with error handling. 
The program is written in C++ from scratch. No tools were used in the process.<br />
<br />
The project's main role is to generate optimised intermediate code which is machine independent from the provided C++ source code.
This is done using the following steps:<br />
  **Phase 1**: Lexical Analyser generates the list of tokens as well as the Symbol Table.<br />
  **Phase 2**: Syntax Analyser/ Parser, parses the input string based on the given grammar and generates the parse tree.<br />
  **Phase 3**: Semantic Analyser checks the code for semantic errors.<br />
  **Phase 4**: Intermediate Code Generation, generates Abstract Syntax Tree as well as Three Address Code and Quadruples.<br />
  **Phase 5**: Code Optimization, performs constant folding followed by dead code elimination.<br />
  
Our Mini C++ Compiler can handle the following constructs: 
  -> Simple ‘if’ statements.<br />
  -> ‘if - else’ statements.<br />
  -> ‘for’ loop.<br />
  -> ‘while’ loop.<br />
  -> Arithmetic operators : + , - , * , / <br />
  -> Relational Operators : > , < , <= , >=, ==, !=<br />
  -> Boolean Operators: && (and) , || (or)  , ! (not)<br />
  -> Arithmetic expressions.<br />
  -> Boolean expressions.<br />

