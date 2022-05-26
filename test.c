#include "nlab.h"
#include "stack.h"
#include "extension.h"

#define TESTTOKENS 20

void test_func(void)
{
   assert(valid_Int("")==false);
   assert(valid_Int("&A")==false);
   assert(valid_Int("$0")==false);
   assert(valid_Int("$a")==false);
   assert(valid_Int("01")==true);
   assert(valid_Int("0")==true);
   assert(valid_Int("9")==true);
   assert(valid_Int("1234567890")==true);
   assert(valid_Int("$Aa")==false);
   #ifndef EXTENSION
   assert(valid_Int("-0")==false);
   assert(valid_Int("-01")==false);
   assert(valid_Int("-1")==false);
   assert(valid_Int("-123")==false);
   #else
   assert(valid_Int("-0")==true);
   assert(valid_Int("-01")==true);
   assert(valid_Int("-1")==true);
   assert(valid_Int("-123")==true);
   #endif

   assert(char_toInt("")==-999);
   assert(char_toInt("&A")==-999);
   assert(char_toInt("$0")==-999);
   assert(char_toInt("$a")==-999);
   assert(char_toInt("01")==1);
   assert(char_toInt("0")==0);
   assert(char_toInt("9")==9);
   assert(char_toInt("1234567890")==1234567890);
   assert(char_toInt("$Aa")==-999);
   #ifndef EXTENSION
   assert(char_toInt("-0")==-999);
   assert(char_toInt("-01")==-999);
   assert(char_toInt("-1")==-999);
   assert(char_toInt("-123")==-999);
   #else
   assert(char_toInt("-0")==0);
   assert(char_toInt("-01")==-1);
   assert(char_toInt("-1")==-1);
   assert(char_toInt("-123")==-123);
   #endif
   assert(valid_Var("")==false);
   assert(valid_Var("&A")==false);
   assert(valid_Var("$")==false);
   assert(valid_Var("$1")==false);
   assert(valid_Var("$a")==false);
   assert(valid_Var("$A")==true);
   assert(valid_Var("$Aa")==false);

   char ts_str[ELEMSIZE];
   char ts_tgt[ELEMSIZE];
   ts_tgt[0] = '"';
   ts_tgt[1] = '\0';
   strcat(ts_tgt, "aB3@");
   int len = strlen(ts_tgt);
   ts_tgt[len] = '"';
   ts_tgt[len+1] = '\0';

   assert(strsame(ts_tgt, "aB3@")==false);

   assert(valid_Str("''")==false);
   assert(valid_Str("@@")==false);
   assert(valid_Str("abc")==false);
   assert(valid_Str(ts_tgt)==true);
   
   /* valid to store "aB3@" to aB3@ */
   assert(_transfer_str(ts_str, ts_tgt));
   assert(strsame(ts_str, "aB3@"));
   assert(valid_Str(ts_str)==false);

   /* invalid to store aB3@ to string */
   assert(!_transfer_str(ts_tgt, ts_str));

}

void test_stack(void)
{
   stack* ts_s = stack_init();
   assert(ts_s->size==0);
   assert(ts_s->capacity==16);
   ts_s->print_result = false;
   // ts_s->print_result = true;

   assert(stack_push_int(ts_s, "")==NULL);
   assert(stack_push_int(ts_s, "abc")==NULL);
   assert(stack_push_var(ts_s, "")==NULL);
   assert(stack_push_var(ts_s, "abc")==NULL);
   assert(stack_extend(NULL)==false);
   assert(stack_push(NULL, "1")==false);
   assert(stack_push(ts_s, "")==false);
   assert(stack_push(ts_s, "abc")==false);

   assert(stack_push(ts_s, "1"));       // store 1
   assert(ts_s->size==1);
   assert(stack_push(ts_s, "2"));       // store 2
   assert(ts_s->size==2);

   stacktype* ts_p;
   ts_p = ts_s->arr[0] ; 
   assert(strsame(ts_p->vname, "1"));
   assert(ts_p->vrow==1);
   assert(ts_p->vcolumn==1);
   assert(ts_p->value[0][0]==1);
   ts_p = ts_s->arr[1] ; 
   assert(strsame(ts_p->vname, "2"));
   assert(ts_p->vrow==1);
   assert(ts_p->value[0][0]==2);

   /* variable must be started with '$' */
   assert(_create_array(ts_s, "abc")==false);    //a ?
   assert(_create_array(NULL, "$A")==false);

   // assert(stack_push_file(ts_s, "abc")==NULL);
   assert(stack_push_file(NULL, "")==NULL);
   assert(stack_push_file(ts_s, "")==NULL);
   assert(stack_push_file(ts_s, "wrong")==NULL);

   assert(stack_push_readfile(NULL, "")==false);         // no need ?
   assert(stack_push_readfile(ts_s, "")==false);         // no need ?
   assert(stack_push_readfile(ts_s, "wrong")==false);         // no need ?

   assert(stack_find(NULL, "$A")==false);
   assert(stack_find(ts_s, "")==false);
   /* variable must be started with '$' */
   assert(stack_find(ts_s, "abc")==false);
   /* cannot find variable $ZZZ, return false */
   assert(stack_find(ts_s, "$ZZZ")==false);

   /* cannot find invalid variable, return false */
   assert(_print_var(NULL, "$A")==false);
   assert(_print_var(ts_s, "abc")==false);
   assert(_print_var(ts_s, "$ZZZ")==false);
   #ifdef EXTENSION
   assert(_print_var_insentence(NULL, "$A")==false);
   assert(_print_var_insentence(ts_s, "abc")==false);
   assert(_print_var_insentence(ts_s, "$ZZZ")==false);
   #endif

   /* cannot print empty string, reutn false */
   assert(_print_str(NULL, "abc")==false);
   assert(_print_str(ts_s, "")==false);


   stacktype* g1 = (stacktype*) ncalloc(1, sizeof(stacktype));
   stacktype* g2 = (stacktype*) ncalloc(1, sizeof(stacktype));
   stacktype* g3 = (stacktype*) ncalloc(1, sizeof(stacktype));

   assert(_unary_op(NULL, "U-NOT")==false);
   assert(_unary_op(ts_s, "")==false);
   assert(_u_diff_op(NULL, NULL, "U-NOT")==false);
   assert(_u_diff_op(g1, g2, "U-NOT")==false);
   assert(_binary_op(NULL, "B-ADD")==false);
   assert(_binary_op(ts_s, "")==false);
   assert(_b_diff_op(NULL, NULL, NULL, "B-ADD")==false);
   assert(_b_diff_op(g1, g2, g3, "B-ADD")==false);
   assert(_valid_b_rules(NULL, NULL)==0);

   assert(stack_copy(NULL, NULL)==false);
   assert(stack_copy(g2, g1)==false);
   assert(stack_pop(NULL, NULL)==false);
   assert(stack_pop(NULL, g1)==false);

   assert(stack_pop(ts_s, g1));          // pop 2
   assert(ts_s->size==1);
   assert(g1->vrow==1);
   assert(g1->value[0][0]==2);

   assert(stack_pop(ts_s, g2));          // pop 1
   assert(ts_s->size==0);
   assert(g2->vrow==1);
   assert(g2->value[0][0]==1);

   assert(stack_copy(g3, g2));          // copy g2 to g1
   // g1 & g2 must be the same
   assert(strsame(g3->vname, g2->vname));
   assert(g3->vrow==g2->vrow);
   assert(g3->vcolumn==g2->vcolumn);
   assert(g3->value[0][0]==g2->value[0][0]);

   g3->value[0][0]= 0;

/* g1:result */   /* g2 = 1 */   /* g3 = 0 */
   assert(_b_and(g1,g2,g3)==true);
   assert(g1->value[0][0]==0);
   assert(_b_or(g1,g2,g3)==true);
   assert(g1->value[0][0]==1);
   assert(_b_greater(g1,g2,g3)==true);
   assert(g1->value[0][0]==1);
   assert(_b_less(g1,g2,g3)==true);
   assert(g1->value[0][0]==0);
   assert(_b_add(g1,g2,g3)==true);
   assert(g1->value[0][0]==1);
   assert(_b_times(g1,g2,g3)==true);
   assert(g1->value[0][0]==0);
   assert(_b_equals(g1,g2,g3)==true);
   assert(g1->value[0][0]==0);
   assert(_b_diff_op(g1,g2,g3, "B-XXXX")==false);
   assert(_b_diff_op(g1,g2,g3, "B-ADD")==true);
   assert(g1->value[0][0]==1);
   assert(_b_diff_op(g1,g2,g3, "B-LESS")==true);
   assert(g1->value[0][0]==0);
   #ifdef EXTENSION
   assert(_b_sub(g1,g2,g3)==true);
   assert(g1->value[0][0]==1);
   assert(_b_sub(g1,g3,g2)==true);
   assert(g1->value[0][0]==-1);
   assert(_b_div(g1,g2,g3)==false);       // not allowed divided by two
//    assert(g1->value[0][0]==2);
   assert(_b_div(g1,g3,g2)==true);       // not allowed divided by two
   assert(g1->value[0][0]==0);
   assert(_b_modulus(g1,g2,g3)==false);
//    assert(g1->value[0][0]==1);
   assert(_b_modulus(g1,g3,g2)==true);
   assert(g1->value[0][0]==0);
   assert(_b_diff_op(g1,g2,g3, "B-XXXX")==false);
   assert(_b_diff_op(g1,g2,g3, "B-DIV")==false);
//    assert(g1->value[0][0]==0);
   assert(_b_diff_op(g1,g3,g2, "B-DIV")==true);
   assert(g1->value[0][0]==0);
   assert(_c_same(g1,g2,g3)==true);
   assert(g1->value[0][0]==0);
   assert(_c_diff(g1,g2,g3)==true);
   assert(g1->value[0][0]==1);
   assert(_c_diff_op(g1,g3,g2, "C-SAME")==true);
   assert(g1->value[0][0]==0);
   #endif
   assert(_u_not(g1,g2)==true);
   assert(g1->value[0][0]==0);
   assert(_u_eightcount(g1,g2)==true);
   assert(g1->value[0][0]==0);
   assert(_u_diff_op(g1,g2, "U-XXXX")==false);
   assert(_u_diff_op(g1,g2, "U-NOT")==true);
   assert(g1->value[0][0]==0);
   assert(_u_diff_op(g1,g2, "U-EIGHTCOUNT")==true);
   assert(g1->value[0][0]==0);


   strcpy(g3->vname, "$F");
   g3->vrow = 3;
   g3->vcolumn = 3;      /* g3 =  1 0 0  */
   g3->value[0][0]=1;    /*       0 0 0  */
   g3->value[2][2]=1;    /*       0 0 1  */

   assert(_count_true_value(g1,-1,-1)==-999);
   assert(_count_true_value(NULL,0,0)==-999);
   assert(_count_true_value(g1,0,0)==0);         /* g1 = 1 */
   assert(_count_true_value(g3,0,0)==0);
   assert(_count_true_value(g3,1,0)==1);
   assert(_count_true_value(g3,1,1)==2);

   assert(_valid_b_rules(g2,g3)==1);
   assert(_valid_b_rules(g3,g2)==2);
   assert(_valid_b_rules(g3,g3)==3);
/* g1:result */   /* g2 = 1 */   /* g3 =  1 0 0  */
                                 /*       0 0 0  */
                                 /*       0 0 1  */
   assert(_b_and(g1,g2,g3)==true);
   assert(g1->value[0][0]==1);
   assert(g1->value[1][1]==0);
   assert(_b_or(g1,g2,g3)==true);
   assert(g1->value[0][0]==1);
   assert(g1->value[1][1]==1);
   assert(_b_greater(g1,g2,g3)==true);
   assert(g1->value[0][0]==0);
   assert(g1->value[1][1]==1);
   assert(_b_less(g1,g2,g3)==true);
   assert(g1->value[0][0]==0);
   assert(g1->value[1][1]==0);
   assert(_b_add(g1,g2,g3)==true);
   assert(g1->value[0][0]==2);
   assert(g1->value[1][1]==1);
   assert(_b_times(g1,g2,g3)==true);
   assert(g1->value[0][0]==1);
   assert(g1->value[1][1]==0);
   assert(_b_equals(g1,g2,g3)==true);
   assert(g1->value[0][0]==1);
   assert(g1->value[1][1]==0);
   #ifdef EXTENSION
   assert(_b_sub(g1,g2,g3)==true);
   assert(g1->value[0][0]==0);
   assert(g1->value[1][1]==1);
   assert(_b_div(g1,g2,g3)==false);       // not allowed divided by two
   assert(_b_modulus(g1,g2,g3)==false);
   assert(_c_same(g1,g2,g3)==true);
   assert(g1->value[0][0]==0);
   assert(_c_diff(g1,g2,g3)==true);
   assert(g1->value[0][0]==1);
   #endif
   assert(_u_not(g1,g3)==true);
   assert(g1->value[0][0]==0);
   assert(g1->value[1][0]==1);
   assert(g1->value[1][1]==1);
   assert(_u_eightcount(g1,g3)==true);
   assert(g1->value[0][0]==0);
   assert(g1->value[1][1]==2);

   _swap(g2, g3);
/* g1:result */   /* g2 =  1 0 0  */   /* g3 = 1 */
                  /*       0 0 0  */
                  /*       0 0 1  */
   assert(_b_greater(g1,g2,g3)==true);
   assert(g1->value[0][0]==0);
   assert(g1->value[1][1]==0);
   _swap(g2, g3);
   assert(_b_less(g1,g2,g3)==true);
   assert(g1->value[0][0]==0);
   assert(g1->value[1][1]==1);
   _swap(g2, g3);
   #ifdef EXTENSION
   assert(_b_sub(g1,g2,g3)==true);
   assert(g1->value[0][0]==0);
   assert(g1->value[1][1]==-1);
   _swap(g2, g3);
   assert(_b_div(g1,g2,g3)==true);       // not allowed divided by two
   assert(g1->value[0][0]==1);
   assert(g1->value[1][1]==0);
   _swap(g2, g3);
   assert(_b_modulus(g1,g2,g3)==true);
   assert(g1->value[0][0]==0);
   assert(g1->value[1][1]==0);
   _swap(g2, g3);
   #endif

   free(g1);
   free(g2);
   free(g3);

   assert(stack_push(ts_s, "10"));       // SET $I := 10 ;
   assert(ts_s->size==1);
   assert(stack_push(ts_s, "$I"));       // SET $I := 10 ;
   assert(ts_s->size==1);
   ts_p = ts_s->arr[0] ; 
   assert(strsame(ts_p->vname, "$I"));
   assert(ts_p->vrow==1);
   assert(ts_p->vcolumn==1);
   assert(ts_p->value[0][0]==10);
   assert(_print_var(ts_s, "$A")==false);       // PRINT "$A"
   assert(_print_var(ts_s, "$I")==true);       // PRINT "$I"

   assert(stack_push(ts_s, "3"));        // store 3
   assert(ts_s->size==2);
   assert(_unary_op(ts_s, "U-NOT"));  // 3 U-NOT
   assert(ts_s->size==2);
   ts_p = ts_s->arr[1] ; 
   // assert(strsame(ts_p->vname, "#U"));
   assert(ts_p->vrow==1);
   assert(ts_p->vcolumn==1);
   assert(ts_p->value[0][0]==0);

   assert(stack_find(ts_s, "$I"));       // 3 U-NOT $I
   assert(ts_s->size==3);
   assert(_binary_op(ts_s, "B-ADD"));  // 3 U-NOT $I B-ADD
   assert(ts_s->size==2);
   assert(stack_push(ts_s, "$A"));       // SET $A := 3 U-NOT $I B-ADD ;
   assert(ts_s->size==2);
   ts_p = ts_s->arr[1] ; 
   assert(strsame(ts_p->vname, "$A"));
   assert(ts_p->vrow==1);
   assert(ts_p->vcolumn==1);
   assert(ts_p->value[0][0]==10);

   assert(stack_push(ts_s, "6"));        // store 6
   assert(ts_s->size==3);
   assert(stack_push(ts_s, "5"));        // store 5
   assert(ts_s->size==4);

   assert(_create_array(ts_s, "$C"));    // ONES 6 5 $C
   assert(ts_s->size==3);
   ts_p = ts_s->arr[2] ; 
   assert(strsame(ts_p->vname, "$C"));
   assert(ts_p->vrow==6);
   assert(ts_p->vcolumn==5);
   assert(ts_p->value[0][0]==1);
   assert(ts_p->value[ts_p->vrow-1][ts_p->vcolumn-1]==1);
  
   assert(stack_push(ts_s, "3"));        // store 3
   assert(ts_s->size==4);
   assert(stack_find(ts_s, "$X")==false);       // connot find $X
   assert(stack_find(ts_s, "$A"));       // 3 $A
   assert(ts_s->size==5);
   #ifdef EXTENSION
   assert(_compar_op(ts_s, "C-SAME"));   // 3 $A C-SAME
   assert(ts_s->size==4);
   assert(stack_push(ts_s, "$B"));       // SET $B := 3 $A C-SAME ;
   assert(ts_s->size==4);
   #endif

   stack_free(ts_s);
}

void test_prog(void)
{
   Program* ts_prog = prog_init();
   ts_prog->print_error_code = false;
   // ts_prog->print_error_code = true;

   test_input("BEGIN { }", ts_prog->wds);

   assert(strsame(ts_prog->wds[0], "test.prog"));
   assert(strsame(ts_prog->wds[1], "BEGIN"));
   assert(strsame(ts_prog->wds[2], "{"));
   assert(strsame(ts_prog->wds[3], "}"));

   int start = 1;
   // BEGIN" "{" "}"
   test_input("B@GIN { }", ts_prog->wds);
   ts_prog->cw = start;
   assert(Prog(ts_prog)==false);

   test_input("BEGIN [ }", ts_prog->wds);
   ts_prog->cw = start;
   assert(Prog(ts_prog)==false);

   test_input("BEGIN { ]", ts_prog->wds);
   ts_prog->cw = start;
   assert(Prog(ts_prog)==false);
   stack_free(ts_prog->s);

   test_input("BEGIN { }", ts_prog->wds);
   ts_prog->cw = start;
   assert(Prog(ts_prog)==true);

   test_input("0", ts_prog->wds);
   ts_prog->cw = start;
   assert(Integer(ts_prog)==true);   // stored cnt: 1

   test_input("1", ts_prog->wds);
   ts_prog->cw = start;
   assert(Integer(ts_prog)==true);   // stored cnt: 2

   test_input("3", ts_prog->wds);
   ts_prog->cw = start;
   assert(Integer(ts_prog)==true);   // stored cnt: 3

   test_input("9", ts_prog->wds);
   ts_prog->cw = start;
   assert(Integer(ts_prog)==true);   // stored cnt: 4

   test_input(".", ts_prog->wds);
   ts_prog->cw = start;
   assert(Integer(ts_prog)==false);   // stored cnt: 4


   #ifndef INTERP
   test_input("$A", ts_prog->wds);
   ts_prog->cw = start;
   assert(VarName(ts_prog)==true);   // stored cnt: 4
   #endif

   #ifdef INTERP
   test_input("$A", ts_prog->wds);
   ts_prog->cw = start;
   assert(VarName(ts_prog)==false);   // stored cnt: 4

   test_input("$A", ts_prog->wds);
   ts_prog->cw = start;
   assert(ones_array(ts_prog)==true);   // stored cnt: 3
   #endif

   test_input("SET $A := 1 ;", ts_prog->wds);
   ts_prog->cw = start;
   assert(Set(ts_prog)==true);   // stored cnt: 2

   test_input("$A", ts_prog->wds);
   ts_prog->cw = start;
   assert(VarName(ts_prog)==true);   // stored cnt: 4

   // ts_prog->op_cnt = 2;
   test_input("$A", ts_prog->wds);
   ts_prog->cw = start;
   assert(Pushdown(ts_prog)==true);   // stored cnt: 3

   ts_prog->op_cnt = 1;
   test_input("U-NOT", ts_prog->wds);
   ts_prog->cw = start;
   assert(UnaryOP(ts_prog)==true);   // stored cnt: 3

   ts_prog->op_cnt = 2;
   test_input("B-TIMES", ts_prog->wds);
   ts_prog->cw = start;
   assert(BinaryOP(ts_prog)==true);   // stored cnt: 2

   ts_prog->op_cnt = 1;
   test_input("U-EIGHTCOUNT", ts_prog->wds);
   ts_prog->cw = start;
   assert(Polish(ts_prog)==true);   // stored cnt: 2

   ts_prog->op_cnt = 1;
   test_input("U-EIGHTCOUNT", ts_prog->wds);
   ts_prog->cw = start;
   assert(Polish(ts_prog)==true);   // stored cnt: 2

   ts_prog->op_cnt = 1;
   test_input(";", ts_prog->wds);
   ts_prog->cw = start;
   assert(PolishList(ts_prog)==true);   // stored cnt: 2

   ts_prog->op_cnt = 2;
   test_input("$A 1 B-EQUALS ;", ts_prog->wds);
   ts_prog->cw = start;
   assert(PolishList(ts_prog)==true);   // stored cnt: 2

   test_input("SET $A := 1 ;", ts_prog->wds);
   ts_prog->cw = start;
   assert(Set(ts_prog)==true);   // stored cnt: 2

   test_input("SET $B := $A ;", ts_prog->wds);
   ts_prog->cw = start;
   assert(Set(ts_prog)==true);


   test_input("SET $B := $X ;", ts_prog->wds);
   ts_prog->cw = start;
   #ifdef INTERP
   assert(Set(ts_prog)==false);
   #endif
   #ifndef INTERP
   assert(Set(ts_prog)==true);
   #endif

   test_input("SET $B := $_ ;", ts_prog->wds);
   ts_prog->cw = start;
   assert(Set(ts_prog)==false);

   test_input("SET $B := $A U-NOT ;", ts_prog->wds);
   ts_prog->cw = start;
   assert(Set(ts_prog)==true);

   test_input("SET $B := $A U-XXX ;", ts_prog->wds);
   ts_prog->cw = start;
   assert(Set(ts_prog)==false);

   test_input("SET $B := $A 1 B-LESS ;", ts_prog->wds);
   ts_prog->cw = start;
   assert(Set(ts_prog)==true);

   test_input("SET $B := $A 1 B-XXX ;", ts_prog->wds);
   ts_prog->cw = start;
   assert(Set(ts_prog)==false);


   ts_prog->op_cnt = 2;
   test_input("SET $B := $A B-TIMES ;", ts_prog->wds);
   ts_prog->cw = start;
   #ifdef INTERP
   assert(Set(ts_prog)==false);
   #endif
   #ifndef INTERP
   assert(Set(ts_prog)==true);
   #endif

   test_input("SET $B := 5 $A 2 B-TIMES U-NOT B-EQUALS ;", ts_prog->wds);
   ts_prog->cw = start;
   assert(Set(ts_prog)==true);

   test_input("PRINT $A", ts_prog->wds);
   ts_prog->cw = start;
   assert(Print(ts_prog)==true);

   test_input("ONES 3 5 $C", ts_prog->wds);
   ts_prog->cw = start;
   assert(CreateOnes(ts_prog)==true);

   #ifdef INTERP
   test_input("ONES 0 5 $C", ts_prog->wds);
   ts_prog->cw = start;
   assert(CreateOnes(ts_prog)==false);
   #endif

   test_input("LOOP $L 3 { }", ts_prog->wds);
   ts_prog->cw = start;
   assert(Loop(ts_prog)==true);

   #ifdef INTERP
   test_input("LOOP $L 0 { }", ts_prog->wds);
   ts_prog->cw = start;
   assert(Loop(ts_prog)==false);
   #endif

   ts_prog->op_cnt = 2;
   test_input("LOOP $L 3 { SET $B := $B 1 B-ADD ; }", ts_prog->wds);
   ts_prog->cw = start;
   assert(Loop(ts_prog)==true);

   ts_prog->op_cnt = 2;
   test_input("LOOP $L 3 { SET $B := $B 1 X-XXX ; }", ts_prog->wds);
   ts_prog->cw = start;
   assert(Loop(ts_prog)==false);

   test_input("LOOP $L 3 { .", ts_prog->wds);
   ts_prog->cw = start;
   assert(Loop(ts_prog)==false);

   test_input("SET $A := 1 ;", ts_prog->wds);
   ts_prog->cw = start;
   assert(Instr(ts_prog)==true);

   test_input("SET $A := $ ;", ts_prog->wds);
   ts_prog->cw = start;
   assert(Instr(ts_prog)==false);

   test_input("}", ts_prog->wds);
   ts_prog->cw = start;
   assert(InstrcList(ts_prog)==true);

   test_input("SET $A := 1 ; ONES 3 5 $C }", ts_prog->wds);
   ts_prog->cw = start;
   assert(InstrcList(ts_prog)==true);

   test_input("SET $A := 1 ; ONES 3 5 $C", ts_prog->wds);
   ts_prog->cw = start;
   assert(InstrcList(ts_prog)==true);

   test_input("SET $A := 1 ;", ts_prog->wds);
   ts_prog->cw = start;
   assert(InstrcList(ts_prog)==true);

   test_input("SET $A := $ ;", ts_prog->wds);
   ts_prog->cw = start;
   assert(InstrcList(ts_prog)==false);
   stack_free(ts_prog->s);

   test_input("BEGIN { SET $A := 1 ; ONES 3 5 $C }", ts_prog->wds);
   ts_prog->cw = start;
   assert(Prog(ts_prog)==true);

   stack_free(ts_prog->s);
   free(ts_prog);

}

void test_prog_ext(void)
{
   #ifdef EXTENSION
   Program* ts_prog = prog_init();
   ts_prog->print_error_code = false;
   // ts_prog->print_error_code = true;

   int start = 1;
   test_input("BEGIN { }", ts_prog->wds);
   ts_prog->cw = start;
   assert(Prog(ts_prog)==true);

   stack* ts_p = ts_prog->s;
   ts_p->print_result = false;

   test_input("SET $A := 1 ;", ts_prog->wds);
   ts_prog->cw = start;
   assert(Set(ts_prog)==true);
   test_input("SET $B := 9 ;", ts_prog->wds);
   ts_prog->cw = start;
   assert(Set(ts_prog)==true);
   test_input("ONES 3 5 $C", ts_prog->wds);
   ts_prog->cw = start;
   assert(CreateOnes(ts_prog)==true);

   test_input("C-XXX", ts_prog->wds);
   ts_prog->cw = start;
   assert(ComparOP(ts_prog)==false);

   test_input("SET $D := 1 $A C-SAME ;", ts_prog->wds);
   ts_prog->cw = start;
   assert(Set(ts_prog)==true);

   test_input("SET $D := 1 C-SAME ;", ts_prog->wds);
   ts_prog->cw = start;
   assert(Set(ts_prog)==false);

   test_input("SET $D := 1 $A B-SUB ;", ts_prog->wds);
   ts_prog->cw = start;
   assert(Set(ts_prog)==true);

   test_input("SET $D := 1 0 D-DIV ;", ts_prog->wds);
   ts_prog->cw = start;
   assert(Set(ts_prog)==false);

   test_input("SET $D := 1 0 D-MODULUS ;", ts_prog->wds);
   ts_prog->cw = start;
   assert(Set(ts_prog)==false);

   test_input("IF $A $A C-SAME { }", ts_prog->wds);
   ts_prog->cw = start;
   assert(If(ts_prog)==1);

   test_input("IF $A $B C-SAME { }", ts_prog->wds);
   ts_prog->cw = start;
   assert(If(ts_prog)==2);

   test_input("IF $A $B C-SAME { PRINT $A }", ts_prog->wds);
   ts_prog->cw = start;
   assert(If(ts_prog)==2);

   test_input("IF $A $B C-SAME }", ts_prog->wds);
   ts_prog->cw = start;
   assert(If(ts_prog)==0);

   test_input("IF $XXX $B C-SAME { }", ts_prog->wds);
   ts_prog->cw = start;
   assert(If(ts_prog)==0);

   ts_prog->wds_cnt = 20;
   test_input("IF $A $B C-SAME { ELSE { }", ts_prog->wds);
   ts_prog->cw = start;
   assert(If(ts_prog)==0);

   test_input("IF $A $B C-SAME { PRINT $A ", ts_prog->wds);
   ts_prog->cw = start;
   assert(If(ts_prog)==0);

   test_input("IF $A $B C-SAME { PRINT $A }", ts_prog->wds);
   ts_prog->cw = start;
   assert(If(ts_prog)==2);

   test_input("ELSE { }", ts_prog->wds);
   ts_prog->cw = start;
   assert(Else(ts_prog)==true);

   test_input("ELSE } }", ts_prog->wds);
   ts_prog->cw = start;
   assert(Else(ts_prog)==false);

   test_input("ELSE { LOOP", ts_prog->wds);
   ts_prog->cw = start;
   assert(Else(ts_prog)==false);

   test_input("IF $A $B C-SAME { } ELSE { }", ts_prog->wds);
   ts_prog->cw = start;
   assert(IfStmt(ts_prog)==true);

   test_input("IF $A $B C-SAME { } ELSE { LOOP", ts_prog->wds);
   ts_prog->cw = start;
   assert(IfStmt(ts_prog)==false);

   test_input("IF $A $B C-SAME { } ELSE { ONES 3 5 $D }", ts_prog->wds);
   ts_prog->cw = start;
   assert(IfStmt(ts_prog)==true);

   test_input("$A", ts_prog->wds);
   ts_prog->cw = start;
   assert(Sentence(ts_prog)==true);

   test_input("$@", ts_prog->wds);
   ts_prog->cw = start;
   assert(Sentence(ts_prog)==false);

   test_input(";", ts_prog->wds);
   ts_prog->cw = start;
   assert(SentenceList(ts_prog)==true);

   test_input("$A ;", ts_prog->wds);
   ts_prog->cw = start;
   assert(SentenceList(ts_prog)==true);

   test_input(":= $A ;", ts_prog->wds);
   ts_prog->cw = start;
   assert(print_Sentence(ts_prog)==true);

   test_input("PRINT := $A ;", ts_prog->wds);
   ts_prog->cw = start;
   assert(Print(ts_prog)==true);

   test_input("PRINT :X $A ;", ts_prog->wds);
   ts_prog->cw = start;
   assert(Print(ts_prog)==false);

   test_input("PRINT := $A $B ;", ts_prog->wds);
   ts_prog->cw = start;
   assert(Print(ts_prog)==true);

   test_input("PRINT := $A $B ;", ts_prog->wds);
   ts_prog->cw = start;
   assert(Print(ts_prog)==true);

   stack_free(ts_prog->s);
   free(ts_prog);
   #endif

}

void test_file(void)
{
   Program* ts_prog = prog_init();
   ts_prog->print_error_code = false;
   // ts_prog->print_error_code = true;

   assert(open_file(ts_prog, "test.nlb")==true);

   assert(Prog(ts_prog));

   stack_free(ts_prog->s);
   free(ts_prog);
}

bool test_input(char* input, char store[][MAXTOKENSIZE])
{

   int len = strlen(input);
   int wds_cnt = 0;
   int j = 0;

   strcpy(store[wds_cnt], "test.prog");
   ++wds_cnt;

   for(int i=0; i<len; ++i){
      if(input[i]==' '){
         store[wds_cnt][j] = '\0';
         wds_cnt++;
         j = 0;
         ++i;
      }
      store[wds_cnt][j] = input[i];
      j++;
   }
   store[wds_cnt][j] = '\0';

   return true;
}
