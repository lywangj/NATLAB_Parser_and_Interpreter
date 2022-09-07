# NATLAB
###
Coursework code for developing programming language allowing Matrix-manipulation, written in C
Grade Achieved - 76% (1st)

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
#
### Examples
#### case1. 
In a simple case, set an integer **5** in a variable **$I**, then print out the variable.
```
BEGIN {
   SET $I := 5 ;
   PRINT $I
}
```
**Output:**
```
5
```
#### case2. Matrix operating
For this programming language, user is able to generate a new matrix with instructions or store a matrix into a variable by reading it from files. In this case, a matrix from file **lglider.arr** is stored in the variable **$A**. Next, in 10-cycle loop, this input matrix will be operated by **U-NOT**, which turns **0** to **1** and turn others to **0**.
```
BEGIN {
   READ "lglider.arr" $A
   LOOP $I 10 {
      PRINT "ARRAY:"
      PRINT $A
      SET $A := $A U-NOT ;
   }
}
```
**Output:**
```
ARRAY:
0 0 0 0 0 
0 0 0 0 0 
0 1 1 1 0 
0 0 0 0 0 
0 0 0 0 0 
...
ARRAY:
1 1 1 1 1 
1 1 1 1 1 
1 0 0 0 1 
1 1 1 1 1 
1 1 1 1 1 
... (10 arrays printed out in total)
```

#### case3. Hailstone sequence of a number
In the main structure, the first part is to setup the targeted number (in this case, it is **11**), its reference **$A** and the counter for the length of sequence **$C**. The second part is for a while-loop to generate the sequence. This loop will keep operating, until number **1** is found, and then the loop will be ended. The final part is for printing out a final result.

```
BEGIN {
   // initially set up, the targeted number is "11"
   SET $T := 11 ;
   SET $A := $T ;
   SET $C := 1 ;
   PRINT := "Find" "the" "hailstone" "sequence" "of" $T ;
   PRINT $T

   LOOP $L 2 {
      // generate Hailstone sequence
      ...
      
      // print out the number in each round
      PRINT $A      
      
      // Loop will be ended when Hailstone seq reach number "1"
      IF $A 1 C-SAME {
         SET $L := 2 ;
      }
   }
   // print out the final result
   PRINT := $T "gets" "the" "hailstone" "sequence," "containing" $C "numbers." ;
}
```
Within the loop, the fist step is to check the number in each round is **even** or **odd**. If the number is even, the number will be **diveded by 2**, or, if the number is odd, the number will be **timed by 3** and then **added 1**. The result will pass to next round in loop.

```
   ... // initially set up
   LOOP $L 2 {
      // check the number is even
      SET $D := $A 2 B-MODULUS ;

      // if the number is even
      IF $D 0 C-SAME {
         SET $A := $A 2 B-DIV ;
         SET $C := $C 1 B-ADD ;
         SET $L := 1 ;
      }
      // if the number is odd
      ELSE {
         SET $A := $A 3 B-TIMES 1 B-ADD ;
         SET $C := $C 1 B-ADD ;
         SET $L := 1 ;
      }
      ...
   }
   ... // print out the final result
```
**Output:**
```
Find the hailstone sequence of 11 
11 
34 
17 
52 
26 
13 
40 
20 
10 
5 
16 
8 
4 
2 
1 
11 gets the hailstone sequence, containing 15 numbers. 
```

#
### Error Code Displayment for Exceptions
As Users input files, if any error is found during both parse and interp phase, it will generate ERROR Info, to highlight the error position and its error code.

- EXAMPLE.file
```
BEGIN {
   SET xI := 5 ;         // invalid variable, xI
}
```
**Generate error code:**
```
test.nlb, word 4: Error: invalid variable, expected to get variables (e.g. $A), occurred in nlab.c, line 441
   SET xI :=
       ^^
at least 1 error generated in test.nlb
```

#
### Verifying Methodology
1. unit testing for general functions             // test_func()
2. white-box testing for stack functions          // test_stack()
3. white-box testing for programming functions    // test_prog() & test_prog_ext()
4. black-box testing from testers                 // test_file()

#
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
  
```  
<SET> ::= "SET" <VARNAME> ":=" <POLISHLIST>
<POLISHLIST> ::= <POLISH> <POLISHLIST> | ";"

<POLISH> ::= <PUSHDOWN> | <UNARYOP> | <BINARYOP> | <COMPAROP>
<PUSHDOWN> ::= <VARNAME> | <INTEGER>
```

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


