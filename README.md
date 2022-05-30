# NATLAB
- Develop programming language allowing Matrix-manipulation, written in C
- 

### Manipulations
- setting variable with integer or matrix
- read matrix from a file
- handle operations for integer or matrix
  - arithmetic operators
    - add, times, sub, divide, modulus
  - logical operators
    - and, or
  - relational operators
    - equal, greater, less
  - unit operators
    - not, count 
- if-else statement
- loop
- print-out integer, matrix & sentence

### Example
#### case1. 

#### case2. Hailstone

### Error Code Displayment for Exceptions


### Verifying Methodology
1. unit testing for general functions             // test_func()
2. white-box testing for stack functions          // test_stack()
3. white-box testing for programming functions    // test_prog() & test_prog_ext()
4. black-box testing from testers                 // test_file()

### Grammar list
<PROG> ::= "BEGIN" "{" <INSTRCLIST>

<INSTRCLIST> ::= "}" | <INSTRC> <INSTRCLIST>
<INSTRC> ::= <PRINT> | <SET> | <CREATE> | <LOOP>

- Print array or one-word string to stdout
<PRINT> ::= "PRINT" <VARNAME> | "PRINT" <STRING>

- One of the 26 possible (upper-case) variables
<VARNAME> ::= "$"[A-Z]     # e.g. $A, $B, $Z etc.

- Because of the assumption that a program is just a list of words,
strings can't have spaces in them (for simplicity)
<STRING> :: Double-quoted string e.g. "../../doof.arr", "Hello!" etc.

<SET> ::= "SET" <VARNAME> ":=" <POLISHLIST>
<POLISHLIST> ::= <POLISH><POLISHLIST>  | ";"

<POLISH> ::= <PUSHDOWN> | <UNARYOP> | <BINARYOP>
<PUSHDOWN> ::== <VARNAME> | <INTEGER>

- A non-negative integer
<INTEGER> ::= [0-9]+    # e.g. 1, 250,  3

- Pop one array, push the result.
- U-NOT : Flip the Boolean values of an array
- U-EIGHTCOUNT : Returns the numbers of true values around each cell in the array in its
- Moore 8-neighbourhood (north, south, west, east, NE, NW, SW, SE).
<UNARYOP> ::= "U-NOT" | "U-EIGHTCOUNT"

- Pop 2 arrays, push the resultant array
- If both arrays are bigger than 1x1, they must be the same size
- If one array is a 1x1 scalar, apply this value to each cell of the other array in turn
- B-TIMES operates on corresponding cells in turn (it is not a full matrix multiplication).
<BINARYOP> :: "B-AND" | "B-OR" | "B-GREATER" | "B-LESS" | "B-ADD" | "B-TIMES" | "B-EQUALS"

- Create an array full of ones, or read from a file
<CREATE> ::= "ONES" <ROWS> <COLS> <VARNAME> | "READ" <FILENAME> <VARNAME>
<ROWS> ::= <INTEGER>
<COLS> ::= <INTEGER>
<FILENAME> ::= <STRING>

- Loop using a variable to count from 1 (!) to <= <INTEGER>
- IF the variable
<LOOP> ::= "LOOP" <VARNAME> <INTEGER> "{" <INSTRCLIST>
