#include "nlab.h"
#include "stack.h"
#include "extension.h"

// "PRINT" ":=" <SENTENCELIST>
bool print_Sentence(Program *p)
{
   p->cw = p->cw + 1;
   if(SentenceList(p)!=1){
      return false;
   }
   return true;
}

// <SENTENCELIST> ::= <SENTENCE> <SENTENCELIST> | ";"
bool SentenceList(Program *p)
{
   if(strsame(p->wds[p->cw], ";")==1){
      #ifdef INTERP
      if(p->print_error_code==true){
         printf("\n");
      }
      #endif
      return true;
   }
   if(Sentence(p)!=1){
      return false;
   }
   p->cw = p->cw + 1;
   if(SentenceList(p)!=1){
      return false;
   }
   return true;
}

bool Sentence(Program *p)
{
   if(valid_Var(p->wds[p->cw])==1){
      #ifdef INTERP
      if(_print_var_insentence(p->s, p->wds[p->cw])!=1){
         ERRORC("cannot print invalid variable", p, __LINE__);
         return false;
      }
      #endif
      return true;
   }
   if(valid_Str(p->wds[p->cw])==1){
      #ifdef INTERP
      if(_print_str(p->s, p->wds[p->cw])!=1){
         ERRORC("cannot print invalid string", p, __LINE__);
         return false;
      }
      if(p->print_error_code==true){
         printf(" ");
      }
      #endif
      return true;
   }
   else{
      ERRORC("invalid variable/string is found", p, __LINE__);
      return false;
   }
}

// <SENTENCE> ::= <VARNAME> | <STRING>
bool _print_var_insentence(stack* s, const char* d)
{
   if((s==NULL) || (s->size <= 0) || strsame(d,"")){
      return false;
   }
   if(valid_Var(d)==false){
      return false;
   }
   if(stack_find(s, d)==false){
      return false;
   }
   stacktype* g1 = (stacktype*) ncalloc(1, sizeof(stacktype));
   if(stack_pop(s, g1)!=1){
      free(g1);
      return false;
   }
   if(g1->vrow!=1 || g1->vcolumn!=1){
      free(g1);
      return false;
   }
   if(s->print_result==true){
      for(int j=0; j<g1->vrow ;++j){
         for(int i=0; i<g1->vcolumn; ++i){
            printf("%d ", g1->value[j][i]);
         }
      }
   }
   free(g1);
   return true;
}

// <COMPAROP> ::= "C-SAME" | "C-DIFF"
bool ComparOP(Program *p)
{
   if(p==NULL){
      ERRORC("no C-operation is found", p,__LINE__);
      return false;
   }
   if(!strsame(p->wds[p->cw], "C-SAME") && \
      !strsame(p->wds[p->cw], "C-DIFF") ){
      ERRORC("invalid C-operation is found", p, __LINE__);
      return false;
   }
   #ifdef INTERP
   if(p->op_cnt<2){
      ERRORC("less variable/integer for C-operation (at least 2)", p, __LINE__);
      return false;
   }
   if(_compar_op(p->s, p->wds[p->cw])!=1){
      ERRORC("invalid result in C-operation", p, __LINE__);
      return false;
   }
   #endif

   return true;
}

// <COMPAROP> ::= "C-SAME" | "C-DIFF"
bool _compar_op(stack* s, const char* d)
{
   if(s==NULL || strsame(d,"")){
      return false;
   }
   stacktype* g1 = (stacktype*) ncalloc(1, sizeof(stacktype));
   stacktype* g2 = (stacktype*) ncalloc(1, sizeof(stacktype));

   if(stack_pop(s, g2)!=1){
      free(g1);
      free(g2);
      return false;
   }
   if(stack_pop(s, g1)!=1){
      free(g1);
      free(g2);
      return false;
   }
   stacktype* a = (stacktype*) ncalloc(1, sizeof(stacktype));
   strcpy(a->vname, "#C");
   a->vrow = 1;
   a->vcolumn = 1;
   a->value[0][0]= -1;

   if(_c_diff_op(a, g1, g2, d)!=1){
      free(a);
      free(g1);
      free(g2);
      return false;
   }
   s->arr[s->size] = a;
   s->size = s->size + 1;

   free(g1);
   free(g2);
   return true;
}

bool _c_diff_op(stacktype* g, stacktype* g1, stacktype* g2, const char* d)
{
   if(g!=NULL && g1!=NULL && g2!=NULL){
      if(strsame(d, "C-SAME")){
         if(_c_same(g, g1, g2)!=1){
            return false;
         }
      }
      else if(strsame(d, "C-DIFF")){
         if(_c_diff(g, g1, g2)!=1){
            return false;
         }
      }
      else{
         return false;
      }
      return true;
   }
   return false;
}

// "C-SAME"
bool _c_same(stacktype* g, stacktype* g1, stacktype* g2)
{
   if(g==NULL || g1==NULL || g2==NULL || \
      g1->vrow==0 || g1->vcolumn==0 || g2->vrow==0 || g2->vcolumn==0){
      return false;
   }
   int b_rules = _valid_b_rules(g1, g2);
   if(b_rules==0){
      return false;
   }
   else if(b_rules!=3){
      g->value[0][0]=0;
      return true;
   }
   g->value[0][0]=1;

   for(int j=0; j<g2->vrow ;++j){
      for(int i=0; i<g2->vcolumn; ++i){
         if(g1->value[j][i]!=g2->value[j][i]){
            g->value[0][0]=0;
         }
      }
   }
   return true;
}

// "C-DIFF"
bool _c_diff(stacktype* g, stacktype* g1, stacktype* g2)
{
   if(g==NULL || g1==NULL || g2==NULL || \
      g1->vrow==0 || g1->vcolumn==0 || g2->vrow==0 || g2->vcolumn==0){
      return false;
   }
   int b_rules = _valid_b_rules(g1, g2);
   if(b_rules==0){
      return false;
   }
   else if(b_rules!=3){
      g->value[0][0]=1;
      return true;
   }
   bool is_Same = true;

   for(int j=0; j<g2->vrow ;++j){
      for(int i=0; i<g2->vcolumn; ++i){
         if(g1->value[j][i]!=g2->value[j][i]){
            is_Same = false;
         }
      }
   }
   if(is_Same==false){
      g->value[0][0]=1;
   }
   else{
      g->value[0][0]=0;
   }
   return true;
}


// "B-SUB"
bool _b_sub(stacktype* g, stacktype* g1, stacktype* g2)
{
   if(g==NULL || g1==NULL || g2==NULL || \
      g1->vrow==0 || g1->vcolumn==0 || g2->vrow==0 || g2->vcolumn==0){
      return false;
   }
   int b_rules = _valid_b_rules(g1, g2);
   if(b_rules==0){
      return false;
   }
   else if(b_rules==2){
      _swap(g1, g2);
   }
   for(int j=0; j<g2->vrow ;++j){
      for(int i=0; i<g2->vcolumn; ++i){
         if(b_rules==1){
            g->value[j][i] = g1->value[0][0] - g2->value[j][i] ;
         }
         else if(b_rules==2){
            g->value[j][i] = g2->value[j][i] - g1->value[0][0] ;
         }
         else if(b_rules==3){
            g->value[j][i] = g1->value[j][i] - g2->value[j][i] ;
         }
      }
   }
   return true;
}

// "B-DIV"
bool _b_div(stacktype* g, stacktype* g1, stacktype* g2)
{
   if(g==NULL || g1==NULL || g2==NULL || \
      g1->vrow==0 || g1->vcolumn==0 || g2->vrow==0 || g2->vcolumn==0){
      return false;
   }
   int b_rules = _valid_b_rules(g1, g2);
   if(b_rules==0){
      return false;
   }
   else if(b_rules==2){
      _swap(g1, g2);
   }
   for(int j=0; j<g2->vrow ;++j){
      for(int i=0; i<g2->vcolumn; ++i){
         if(b_rules==1){
            if(g2->value[j][i]==0){
               return false;
            }
            g->value[j][i] = g1->value[0][0] / g2->value[j][i] ;
         }
         else if(b_rules==2){
            if(g1->value[0][0]==0){
               return false;
            }
            g->value[j][i] = g2->value[j][i] / g1->value[0][0] ;
         }
         else if(b_rules==3){
            if(g2->value[j][i]==0){
               return false;
            }
            g->value[j][i] = g1->value[j][i] / g2->value[j][i] ;
         }
      }
   }
   return true;
}

// "B-MODULUS"
bool _b_modulus(stacktype* g, stacktype* g1, stacktype* g2)
{
   if(g==NULL || g1==NULL || g2==NULL || \
      g1->vrow==0 || g1->vcolumn==0 || g2->vrow==0 || g2->vcolumn==0){
      return false;
   }
   int b_rules = _valid_b_rules(g1, g2);
   if(b_rules==0){
      return false;
   }
   else if(b_rules==2){
      _swap(g1, g2);
   }
   for(int j=0; j<g2->vrow ;++j){
      for(int i=0; i<g2->vcolumn; ++i){
         if(b_rules==1){
            if(g2->value[j][i]==0){
               return false;
            }
            g->value[j][i] = g1->value[0][0] % g2->value[j][i] ;
         }
         else if(b_rules==2){
            if(g1->value[0][0]==0){
               return false;
            }
            g->value[j][i] = g2->value[j][i] % g1->value[0][0] ;
         }
         else if(b_rules==3){
            if(g2->value[j][i]==0){
               return false;
            }
            g->value[j][i] = g1->value[j][i] % g2->value[j][i] ;
         }
      }
   }
   return true;
}

// <IFSTMT> ::= <IF> (<ELSE>)
bool IfStmt(Program *p)
{
   if(strsame(p->wds[p->cw], "IF")!=1){ 
      ERRORC("GRAMMAR ERROR ?", p, __LINE__);
      return false;
   }

   int if_option = If(p) ;
   p->cw = p->cw + 1;
   if(if_option==0){
      return false;
   }
   #ifdef INTERP
   else if(if_option==1){
      // skip ELSE
      int cnt_left_brackets = 0;
      int cnt_right_brackets = 0;
      if(strsame(p->wds[p->cw], "ELSE")){       // ELSE or other INSTRC
         do{
            p->cw = p->cw + 1;
            if(strsame(p->wds[p->cw], "{")){
               cnt_left_brackets += 1;
            }
            if(strsame(p->wds[p->cw], "}")){
               cnt_right_brackets += 1;
            }
         }while(cnt_left_brackets!=cnt_right_brackets);
         return true;         // }
      }
      else{
         p->cw = p->cw - 1;
         return true;         // }
      }
   }
   #endif
   else{    // if_option==2
      if(strsame(p->wds[p->cw], "ELSE")){       // ELSE
         if(Else(p)!=1){
            return false;
         }
         return true;                        // }
      }
      else{                           // other INSTRC
         p->cw = p->cw - 1;
         return true;                        // }
      }
   }
}

// <IF> ::= "IF" <PUSHDOWN> <PUSHDOWN> <COMPAROP> "{" <INSTRCLIST>
int If(Program *p)
{
   if(strsame(p->wds[p->cw], "IF")!=1){ 
      ERRORC("GRAMMAR ERROR ?", p, __LINE__);
      return 0;
   }
   p->cw = p->cw + 1;
   if(Pushdown(p)!=1){
      return 0;
   }
   p->cw = p->cw + 1;
   if(Pushdown(p)!=1){
      return 0;
   }
   #ifdef INTERP
   p->op_cnt = 2;
   #endif
   p->cw = p->cw + 1;
   if(ComparOP(p)!=1){
      return 0;
   }
   #ifdef INTERP
   p->op_cnt = 0;
   #endif

   p->cw = p->cw + 1;
   if(!strsame(p->wds[p->cw], "{")){    // {
      ERRORC("expected '{' in IF INSTR", p, __LINE__);
      return false;
   }
   int compar_result = 1;
   #ifdef INTERP
   p->op_cnt = 0;
   compar_result = _get_value(p->s);
   #endif
   p->cw = p->cw + 1;                  // other INSTRC
   if(compar_result == 1){
      if(InstrcList(p)!=1){
         return 0;
      }
      return 1;     // }
   }
   else if(compar_result == 0){
      // skip InstrcList(p) in IF
      int cnt_left_brackets = 1;
      int cnt_right_brackets = 0;
      int cnt_max = p->wds_cnt-3;
      do{
         // p->cw = p->cw + 1;
         if(strsame(p->wds[p->cw], "{")){
            cnt_left_brackets += 1;
         }
         if(strsame(p->wds[p->cw], "}")){
            cnt_right_brackets += 1;
         }
         p->cw = p->cw + 1;
         if(p->cw == cnt_max){
            ERRORC("missing '}' at the end of IF INSTR", p, __LINE__);
            return 0;
         }
      }while(cnt_left_brackets!=cnt_right_brackets);
      p->cw = p->cw - 1;
      return 2;      // }
   }
   else{
      ERRORC("invalid result of IF condition", p, __LINE__);
      return 0;
   }
}

// <ELSE> ::= "ELSE" "{" <INSTRCLIST>
bool Else(Program *p)
{
   if(!strsame(p->wds[p->cw], "ELSE")){    // {
      ERRORC("expected '{' in IF INSTR", p, __LINE__);
      return false;
   }
   p->cw = p->cw + 1;
   if(!strsame(p->wds[p->cw], "{")){    // {
      ERRORC("expected '{' in IF INSTR", p, __LINE__);
      return false;
   }
   p->cw = p->cw + 1;
   if(InstrcList(p)!=1){
      return false;
   }
   return true;
}

/*
<PROG> ::== "BEGIN" { <INSTRCLIST>
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
<BINARYOP> :: "B-AND" | "B-OR" | "B-GREATER" | "B-LESS" | "B-ADD" | "B-TIMES" | "B-EQUALS"| "B-SUB" | "B-DIV" |"B-MODULUS"
<COMPAROP> ::= "C-SAME" | "C-DIFF"

<CREATE> ::= "ONES" <ROWS> <COLS> <VARNAME> | "READ" <FILENAME> <VARNAME>
<ROWS> ::= <INTEGER>
<COLS> ::= <INTEGER>
<FILENAME> ::= <STRING>

<LOOP> ::= "LOOP" <VARNAME> <INTEGER> "{" <INSTRCLIST>

<IFSTMT> ::= <IF> (<ELSE>)
<IF> ::= "IF" <PUSHDOWN> <PUSHDOWN> <COMPAROP> "{" <INSTRCLIST>
<ELSE> ::= "ELSE" "{" <INSTRCLIST>
*/

