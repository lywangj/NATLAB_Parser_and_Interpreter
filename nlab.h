#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <math.h>

// #define INTERP
// #define LOOPCOUNTER 1
#define MINUMTOKENS 3
#define MAXNUMTOKENS 500
#define MAXTOKENSIZE 20
#define strsame(A,B) (strcmp(A, B)==0)


struct prog{
   char wds[MAXNUMTOKENS][MAXTOKENSIZE];
   int wds_cnt;
   int cw; // Current Word
   struct stack* s;
   int op_cnt;
   bool print_error_code;
};
typedef struct prog Program;

void ERROR(char* error_str, int line);
void ERRORC(char* error_str, Program* p, int line);
bool test_input(char* input, char store[][MAXTOKENSIZE]);
void test_prog(void);
void test_prog_ext(void);
void test_file(void);
bool open_file(Program* p, char* d);
Program* prog_init(void);
bool Prog(Program *p);
bool InstrcList(Program *p);
bool Instr(Program *p);
bool Loop(Program *p);
bool Loop_InstrcList(Program *p, char* str, int i);
bool loop_recurse(Program *p, char* str, int i);
bool CreateOnes(Program *p);
bool ones_array(Program *p);
bool CreateRead(Program *p);
bool Print(Program *p);
bool print_Varname(Program *p, const char* str);
bool print_String(Program *p, const char* str);
bool Set(Program *p);
bool PolishList(Program *p);
bool Polish(Program *p);
bool UnaryOP(Program *p);
bool BinaryOP(Program *p);
bool Pushdown(Program *p);
bool VarName(Program *p);
bool Integer(Program *p);

bool valid_Var(const char* str);
bool valid_Int(const char* str);
bool valid_Str(const char* str);
