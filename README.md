# NATLAB
- Develop programming language allowing Matrix-manipulation, written in C
- 

### Manipulations
- setting variable with integer or matrix
- read matrix from a file
- handle operations for integer or matrix
  - **arithmetic** operators: add, times, sub, divide, modulus
  - **logical** operators: and, or
  - **relational** operators: equal, greater, less
  - unit operators: not, count 
- if-else statement
- loop
- print-out integer, matrix & sentence

### Example
#### case1. 

#### case2. Hailstone sequence of a number

### Error Code Displayment for Exceptions


### Verifying Methodology
1. unit testing for general functions             // test_func()
2. white-box testing for stack functions          // test_stack()
3. white-box testing for programming functions    // test_prog() & test_prog_ext()
4. black-box testing from testers                 // test_file()

### Grammar Summary


<PROG> ::== "BEGIN" { \<INSTRCLIST\>
  
  
<INSTRCLIST> ::= "}" | <INSTR> <INSTRCLIST>
<INSTR> ::= <PRINT> | <SET> | <CREATE> | <LOOP> | <IFSTMT>
<PRINT> ::= "PRINT" <VARNAME> | "PRINT" <STRING> | "PRINT" ":=" <SENTENCELIST>
<SENTENCELIST> ::= <SENTENCE> <SENTENCELIST> | ";"
<SENTENCE> ::= <VARNAME> | <STRING>

<SET> ::= "SET" <VARNAME> ":=" <POLISHLIST>
<POLISHLIST> ::= <POLISH> <POLISHLIST> | ";"

<POLISH> ::= <PUSHDOWN> | <UNARYOP> | <BINARYOP> | <COMPAROP>
<PUSHDOWN> ::= <VARNAME> | <INTEGER>
<UNARYOP> ::= "U-NOT" | "U-EIGHTCOUNT"
<BINARYOP> :: "B-AND" | "B-OR" | "B-GREATER" | "B-LESS" | "B-ADD" | "B-TIMES" | "B-EQUALS"|
              "B-SUB" | "B-DIV" |"B-MODULUS"
<COMPAROP> ::= "C-SAME" | "C-DIFF"

<CREATE> ::= "ONES" <ROWS> <COLS> <VARNAME> | "READ" <FILENAME> <VARNAME>
<ROWS> ::= <INTEGER>
<COLS> ::= <INTEGER>
<FILENAME> ::= <STRING>

<LOOP> ::= "LOOP" <VARNAME> <INTEGER> "{" <INSTRCLIST>

<IFSTMT> ::= <IF> (<ELSE>)
<IF> ::= "IF" <PUSHDOWN> <PUSHDOWN> <COMPAROP> "{" <INSTRCLIST>
<ELSE> ::= "ELSE" "{" <INSTRCLIST>
