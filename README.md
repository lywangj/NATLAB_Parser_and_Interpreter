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
  In this programming language, there is a title at the beginning of our instructions. Following this, all the instructions must be within a pair of curly brackets.
```
<PROG> ::== "BEGIN" { <INSTRCLIST>
<INSTRCLIST> ::= "}" | <INSTR> <INSTRCLIST>
```  
  Five types of functional actions are triggered by these keyworks.
```
<INSTR> ::= <PRINT> | <SET> | <CREATE> | <LOOP> | <IFSTMT>
```
  **Print**: 
```
<PRINT> ::= "PRINT" <VARNAME> | "PRINT" <STRING> | "PRINT" ":=" <SENTENCELIST>
<SENTENCELIST> ::= <SENTENCE> <SENTENCELIST> | ";"
<SENTENCE> ::= <VARNAME> | <STRING>
```
  **Set**: 
<SET> ::= "SET" <VARNAME> ":=" <POLISHLIST>
<POLISHLIST> ::= <POLISH> <POLISHLIST> | ";"

<POLISH> ::= <PUSHDOWN> | <UNARYOP> | <BINARYOP> | <COMPAROP>
<PUSHDOWN> ::= <VARNAME> | <INTEGER>
  
following Postfix notation, comparing two integers/variables
- C-SAME : if two elements are the same, generate an 1 for result. If different, 0 for result.
- C-DIFF : opposite to C-SAME
  
```
<COMPAROP> ::= "C-SAME" | "C-DIFF"
<UNARYOP> ::= "U-NOT" | "U-EIGHTCOUNT"
<BINARYOP> :: "B-AND" | "B-OR" | "B-GREATER" | "B-LESS" | "B-ADD" | "B-TIMES" | "B-EQUALS"|
              "B-SUB" | "B-DIV" |"B-MODULUS"
<COMPAROP> ::= "C-SAME" | "C-DIFF"
```
  
  **Create**: 
  
```
<CREATE> ::= "ONES" <ROWS> <COLS> <VARNAME> | "READ" <FILENAME> <VARNAME>
<ROWS> ::= <INTEGER>
<COLS> ::= <INTEGER>
<FILENAME> ::= <STRING>
```
  
  **Loop**: 
  
```
<LOOP> ::= "LOOP" <VARNAME> <INTEGER> "{" <INSTRCLIST>
```
  
  **If-else**: 
  - <IF>     : if statement
  - (<ELSE>) : optional, but only allowed after <IF>
  - IF : if ( condition ) { statement } for if structure
  - ELSE : else { statement } for else structure
As if-condition gets the result with 1 (meaning condition is true), if-statement will be followed.
IF if-condition is 0, if-statement will be skipped, then else-statement or other INSTRC will be followed.
  
```
<IFSTMT> ::= <IF> (<ELSE>)
<IF> ::= "IF" <PUSHDOWN> <PUSHDOWN> <COMPAROP> "{" <INSTRCLIST>
<ELSE> ::= "ELSE" "{" <INSTRCLIST>
```


