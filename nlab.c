#include "nlab.h"
#include "stack.h"
#include "extension.h"

bool open_file(Program* p, char* d)
{
   if(d==NULL || strsame(d, "")){
      ERRORC("cannot find file", p,__LINE__);
      return false;
   }
   strcpy(p->wds[0], d);

   FILE* fp = fopen(d, "r");
   if(fp==NULL){
      ERRORC("cannot read file", p,__LINE__);
      return false;
   }
   
   char dummy1[MAXTOKENSIZE];   // [20]
   char dummy2[MAXNUMTOKENS];   // [100]
   int i=1;
   do{
      if(fscanf(fp, "%s", dummy1)!=1){
         ERRORC("cannot read file", p,__LINE__);
         return false;
      }
      if(strsame(dummy1, "#")){
         if(fscanf(fp, "%[^\n]s", dummy2)!=1){
         }
      }
      else if(strsame(dummy1, "BEGIN")){
         strcpy(p->wds[i], "BEGIN");
         ++i;
      }
      else{
         strcpy(p->wds[i], dummy1);
         p->cw = 1;
         ++i;
         fclose(fp);
         ERRORC("failed to read file, expected 'BEGIN' at the beginning of file", p, __LINE__);
         return false;
      }
   }while(strsame(dummy1, "#"));

   while(fscanf(fp, "%s", p->wds[i++]) == 1){
   }
   fclose(fp);
   p->wds_cnt = i-1;

   if(i>=MAXNUMTOKENS){
      ERROR("too many words in file", __LINE__);
      return false;
   }
   if(p->wds_cnt<=MINUMTOKENS){
      ERROR("too few words in file", __LINE__);
      return false;
   }
   return true;
}

Program* prog_init(void)
{
   Program* p = (Program*) ncalloc(sizeof(Program), 1);
   p->wds_cnt = 0;
   p->cw = 0;
   p->op_cnt = 0;
   p->print_error_code = true;
   return p;
}

// <PROG> ::== "BEGIN" { <INSTRCLIST>
bool Prog(Program *p)
{
   if(p==NULL){
      return false;
   }
   p->cw = 1;
   if(!strsame(p->wds[p->cw], "BEGIN")){
      ERRORC("expected 'BEGIN' at the beginning of file", p, __LINE__);
      return false;
   }
   p->cw = p->cw + 1;
   if(!strsame(p->wds[p->cw], "{")){
      ERRORC("expected '{' after key word BEGIN", p, __LINE__);
      return false;
   }
   #ifdef INTERP
   stack* st = stack_init();
   if(st==NULL){
      ERRORC("stack failed", p, __LINE__);
      return false;
   }
   p->s = st;
   if(p->print_error_code==false){
      stack* sp = p->s;
      sp->print_result = false;
   }
   #endif
   p->cw = p->cw + 1;
   if(InstrcList(p)!=1){
      return false;
   }
   return true;
}

// <INSTRCLIST> ::= "}" | <INSTR> <INSTRCLIST>
bool InstrcList(Program *p)
{
   if(strsame(p->wds[p->cw], "}")){
      return true;
   }
   else if( p->cw == p->wds_cnt-1){
      ERRORC("expected '}' at the end of file", p, __LINE__);
      return false;
   }
   if(Instr(p)!=1){
      return false;
   }
   p->cw = p->cw + 1;

   if(InstrcList(p)!=1){
      return false;
   }
   return true;
}

// <INSTR> ::= <PRINT> | <SET> | <CREATE> | <LOOP> | <IFSTMT>
// <INSTR> ::= <PRINT> | <SET> | <CREATE> | <LOOP>
bool Instr(Program *p)
{
   if(strsame(p->wds[p->cw], "PRINT")){
      if(Print(p)!=1){
         return false;
      }
   }
   else if(strsame(p->wds[p->cw], "SET")){
      if(Set(p)!=1){
         return false;
      }
   }
   else if(strsame(p->wds[p->cw], "ONES")){
      if(CreateOnes(p)!=1){
         return false;
      }
   }
   else if(strsame(p->wds[p->cw], "READ")){
      if(CreateRead(p)!=1){
         return false;
      }
   }
   else if(strsame(p->wds[p->cw], "LOOP")){
      if(Loop(p)!=1){
         return false;
      }
   }
   #ifdef EXTENSION
   else if(strsame(p->wds[p->cw], "IF")){
      if(IfStmt(p)!=1){
         return false;
      }
   }
   #endif
   else{
      ERRORC("cannot find specific kw for INSTRC", p, __LINE__);
      return false;
   }
   return true;
}

// <LOOP> ::= "LOOP" <VARNAME> <INTEGER> "{" <INSTRCLIST>
bool Loop(Program *p)
{
   if(strsame(p->wds[p->cw], "LOOP")!=1){ 
      ERRORC("GRAMMAR ERROR ?", p, __LINE__);
      return false;
   }
   p->cw = p->cw + 1;
   if(valid_Var(p->wds[p->cw])==false){
      ERRORC("invalid variable, expected to get variables (e.g. $A)", p, __LINE__);
      return false;
   }
   #ifdef INTERP
   char str_var[20];
   strcpy(str_var, p->wds[p->cw]);                 // $A
   if(stack_push(p->s, "1")!=1){                   // 1
      return false;
   }
   if(stack_push(p->s, p->wds[p->cw])!=1){         // $A with 1
      return false;
   }
   #endif

   p->cw = p->cw + 1;
   char tmp[MAXTOKENSIZE];
   strcpy(tmp, p->wds[p->cw]);
   if(valid_Int(p->wds[p->cw])==false || tmp[0]=='-'){
      ERRORC("invalid integer, expected to get integer (e.g. 5)", p, __LINE__);
      return false;
   }
   #ifdef INTERP
   char str_int[20];
   strcpy(str_int, p->wds[p->cw]);    // 5
   int i = char_toInt(str_int);
   if(i<=0){
      ERRORC("invalid loop time (must be an integer equal or larger than 1)", p, __LINE__);
      return false;
   }
   #endif

   p->cw = p->cw + 1;
   if(!strsame(p->wds[p->cw], "{")){    // {
      ERRORC("expected '{' in LOOP INSTR", p, __LINE__);
      return false;
   }

   p->cw = p->cw + 1;
   #ifndef INTERP
   if(InstrcList(p)!=1){
      return false;
   }
   #endif
   #ifdef INTERP   
   if(Loop_InstrcList(p, str_var, i)!=1){        // <INSTRCLIST>
      return false;
   }
   #endif

   return true;
}

bool Loop_InstrcList(Program *p, char* str, int i)
{
   if(loop_recurse(p, str, i)!=1){
      return false;
   }
   return true;
}

bool loop_recurse(Program *p, char* str, int i)
{
   int loop_start = p->cw;
   if(InstrcList(p)!=1){
      return false;
   }
   if(stack_find(p->s, str)!=1){
      return false;
   }

   int lc = _get_value(p->s);
   if(lc==i){
      return true;
   }
   else if(lc>i){
      ERRORC("invalid setting for loop counter", p, __LINE__);
      return false;
   }

   if(stack_push(p->s, "1")!=1){
      return false;
   }
   if(_binary_op(p->s, "B-ADD")!=1){
      return false;
   }
   if(stack_push(p->s, str)!=1){
      return false;
   }
   p->cw = loop_start;
   if(loop_recurse(p, str, i)!=1){
      return false;
   }
   return true;
}

bool CreateOnes(Program *p)
{
   if(strsame(p->wds[p->cw], "ONES")!=1){ 
      ERRORC("GRAMMAR ERROR ?", p, __LINE__);
      return false;
   }
   p->cw = p->cw + 1;
   char tmp[MAXTOKENSIZE];
   strcpy(tmp, p->wds[p->cw]);
   if(tmp[0]<'0' || tmp[0]>'9'){       // not allowed '-'
      return false;
   }

   if(Integer(p)!=1){             // ROWS
      return false;
   }
   p->cw = p->cw + 1;

   strcpy(tmp, p->wds[p->cw]);
   if(tmp[0]<'0' || tmp[0]>'9'){
      return false;
   }
   if(Integer(p)!=1){             // COLS
      return false;
   }
   p->cw = p->cw + 1;

   strcpy(tmp, p->wds[p->cw]);
   if(valid_Var(tmp)==false){
      ERRORC("invalid variable, expected to get variables (e.g. $A)", p,__LINE__);
      return false;
   }
   #ifdef INTERP
   if(ones_array(p)!=1){
      return false;
   }
   #endif
   return true;
}

bool ones_array(Program *p)
{
   if(_create_array(p->s, p->wds[p->cw])!=1){
      return false;
   }
   return true;
}

// <CREATE> ::= "READ" <FILENAME> <VARNAME>
// <FILENAME> ::= <STRING>
bool CreateRead(Program *p)
{
   if(strsame(p->wds[p->cw], "READ")!=1){    // start with $ ??? like $A ???
      ERRORC("GRAMMAR ERROR ?", p, __LINE__);
      return false;
   }
   p->cw = p->cw + 1;    // filename
   if(valid_Str(p->wds[p->cw])==false){
      ERRORC("invalid file name, expected to read file (e.g. \"lglider.arr\")", p, __LINE__);
      return false;
   }
   #ifdef INTERP
   if(stack_push_readfile(p->s, p->wds[p->cw])!=1){
      return false;
   }
   #endif
   p->cw = p->cw + 1;
   if(valid_Var(p->wds[p->cw])==false){
      ERRORC("invalid variable, expected to get variables (e.g. $A)", p, __LINE__);
      return false;
   }
   #ifdef INTERP
   if(stack_push(p->s, p->wds[p->cw])!=1){
      return false;
   } 
   #endif
   return true;
}

// <PRINT> ::= "PRINT" <VARNAME> | "PRINT" <STRING> | "PRINT" ":=" <SENTENCELIST>
// <PRINT> ::= "PRINT" <VARNAME> | "PRINT" <STRING>
bool Print(Program *p)
{
   if(strsame(p->wds[p->cw], "PRINT")!=1){
      ERRORC("invalid INSTR keyword is found", p, __LINE__);
      return false;
   }
   p->cw = p->cw + 1;

   if(strsame(p->wds[p->cw], "")){
      ERRORC("invalid variable/keyword is found in PRINT INSTRC", p, __LINE__);
      return false;
   }
   
   char tmp[20];
   strcpy(tmp, p->wds[p->cw]);
   if(valid_Var(tmp)!=false){
      #ifdef INTERP
      if(print_Varname(p, tmp)!=1){ // <VARNAME>
         ERRORC("cannot print invalid variable", p, __LINE__);
         return false;
      }
      #endif
   }
   #ifdef EXTENSION
   else if(strsame(tmp, ":=")!=false){
      if(print_Sentence(p)!=1){ // "PRINT" ":=" <SENTENCELIST>
         return false;
      }
   }
   #endif
   else if(valid_Str(tmp)!=false){
      #ifdef INTERP
      if(print_String(p, tmp)!=1){ // <STRING>
         ERRORC("cannot print invalid string", p, __LINE__);
         return false;
      }
      if(p->print_error_code==true){
         printf("\n");
      }
      #endif
   }
   else{
      ERRORC("invalid variabl/string is found", p, __LINE__);
      return false;
   }
   return true;
}

bool print_Varname(Program *p, const char* str)
{
   if(p==NULL){
      ERRORC("no variable is found", p, __LINE__);
      return false;
   }
   if(p->print_error_code==true){
      if(_print_var(p->s, str)!=1){
         return false;
      }
   }
   return true;
}

bool print_String(Program *p, const char* str)
{
   if(p==NULL){
      ERRORC("no string is found", p,__LINE__);
      return false;
   }
   // print string anyway
   if(p->print_error_code==true){
      if(_print_str(p->s, str)!=1){
         return false;
      }
   }
   return true;
}

// <SET> ::= "SET" <VARNAME> ":=" <POLISHLIST>
bool Set(Program *p)
{
   if(strsame(p->wds[p->cw], "SET")!=1){
      ERRORC("expected keyword SET for INSTRC", p, __LINE__);
      return false;
   }
   p->op_cnt = 0;
   p->cw = p->cw + 1;

   // check grammar error, like not "$*"
   char tmp[20];
   strcpy(tmp, p->wds[p->cw]);
   if(valid_Var(tmp)==false){
      ERRORC("invalid variable, expected to get variables (e.g. $A)", p, __LINE__);
      return false;
   }
   p->cw = p->cw + 1;                     // ":="

   if(strsame(p->wds[p->cw], ":=")!=1){
      ERRORC("expected keyword := after SET INSTRC", p,__LINE__);
      return false;
   }
   p->cw = p->cw + 1;

   if(PolishList(p)!=1){
      return false;
   }
   #ifdef INTERP
   if(p->op_cnt!=1){
      ERRORC("more variable/integer have been set than actually operated", p, __LINE__);
      return false;
   }
   if(stack_push(p->s, tmp)!=1){
      return false;
   }
   p->op_cnt = 0;
   #endif
   return true;
}

// <POLISHLIST> ::= <POLISH> <POLISHLIST> | ";"
bool PolishList(Program *p)
{
   if(strsame(p->wds[p->cw], ";")==1){
      p->cw = p->cw -1 ;
      if(strsame(p->wds[p->cw], ":=")!=1){    // not allowed SET $A := ;
         p->cw = p->cw +1 ;
         return true;
      }
      else{
         p->cw = p->cw +1 ;
         return false;
      }
   }
   if(Polish(p)!=1){
      return false;
   }
   p->cw = p->cw + 1;

   if(PolishList(p)!=1){
      return false;
   }
   return true;
}

// <POLISH> ::= <PUSHDOWN> | <UNARYOP> | <BINARYOP> | <COMPAROP>
// <POLISH> ::= <PUSHDOWN> | <UNARYOP> | <BINARYOP>
bool Polish(Program *p)
{
   char tmp[20];
   strcpy(tmp, p->wds[p->cw]);
   if(tmp[0]=='U'){
      if(UnaryOP(p)!=1){
         return false;
      }
      #ifdef INTERP
      p->op_cnt = p->op_cnt ;
      #endif
   }
   else if(tmp[0]=='B'){
      if(BinaryOP(p)!=1){
         return false;
      }
      #ifdef INTERP
      p->op_cnt = p->op_cnt -1;
      #endif
   }
   #ifdef EXTENSION
   else if(tmp[0]=='C'){
      if(ComparOP(p)!=1){
         return false;
      }
      #ifdef INTERP
      p->op_cnt = p->op_cnt -1;
      #endif
   }
   #endif
   else{
      if(Pushdown(p)!=1){
         return false;
      }
      #ifdef INTERP
      p->op_cnt = p->op_cnt +1;
      #endif
   }
   return true;
}

// <PUSHDOWN> ::= <VARNAME> | <INTEGER>
bool Pushdown(Program *p)
{
   char tmp[20];
   strcpy(tmp, p->wds[p->cw]);
   if(tmp[0]=='$'){
      if(VarName(p)!=1){
         return false;
      }
   }
   else if(tmp[0]>='0' && tmp[0]<='9'){
      if(Integer(p)!=1){
         return false;
      }
   }
   else if(tmp[0]=='-'){
      if(Integer(p)!=1){
         return false;
      }
   }
   else{
      ERRORC("unrecognised keyword is found in POLISH", p, __LINE__);
      return false;
   }
   return true;
}

// <UNARYOP> ::= "U-NOT" | "U-EIGHTCOUNT"
bool UnaryOP(Program *p)
{
   if(p==NULL){
      ERRORC("no U-operation is found", p,__LINE__);
      return false;
   }
   if(!strsame(p->wds[p->cw], "U-NOT") && \
      !strsame(p->wds[p->cw], "U-EIGHTCOUNT") ){
      ERRORC("invalid U-operation is found", p, __LINE__);
      return false;
   }
   #ifdef INTERP
   if(p->op_cnt<1){
      ERRORC("less variable/integer for U-operation (at least 1)", p, __LINE__);
      return false;
   }
   if(_unary_op(p->s, p->wds[p->cw])!=1){
      ERRORC("invalid result in U-operation", p, __LINE__);
      return false;
   }
   #endif

   return true;
}

// <BINARYOP> :: "B-AND" | "B-OR" | "B-GREATER" | "B-LESS" | "B-ADD" | "B-TIMES" | "B-EQUALS"| "B-SUB" | "B-MODULUS"
// <BINARYOP> :: "B-AND" | "B-OR" | "B-GREATER" | "B-LESS" | "B-ADD" | "B-TIMES" | "B-EQUALS"
bool BinaryOP(Program *p)
{
   if(p==NULL){
      ERRORC("no B-operation is found", p, __LINE__);
      return false;
   }
   bool is_recognised = false;
   if(strsame(p->wds[p->cw], "B-AND") || strsame(p->wds[p->cw], "B-OR") ||
      strsame(p->wds[p->cw], "B-GREATER") || strsame(p->wds[p->cw], "B-LESS") ||
      strsame(p->wds[p->cw], "B-ADD") || strsame(p->wds[p->cw], "B-TIMES") ||
      strsame(p->wds[p->cw], "B-EQUALS") ){
      is_recognised = true;
   }
   #ifdef EXTENSION
   else if(strsame(p->wds[p->cw], "B-SUB") || strsame(p->wds[p->cw], "B-DIV") || 
           strsame(p->wds[p->cw], "B-MODULUS")){
      is_recognised = true;
   }
   #endif
   if(is_recognised == false){
      ERRORC("invalid B-operation is found", p, __LINE__);
      return false;
   }

   #ifdef INTERP
   if(p->op_cnt<2){
      ERRORC("less variable/integer for B-operation (at least 2)", p, __LINE__);
      return false;
   }
   if(_binary_op(p->s, p->wds[p->cw])!=1){
      ERRORC("invalid result in B-operation", p, __LINE__);
      return false;
   }
   #endif
   return true;
}

bool VarName(Program *p)
{
   if(p==NULL){
      ERRORC("GRAMMAR ERROR ?", p,__LINE__);
      return false;
   }
   char str[MAXTOKENSIZE];
   strcpy(str, p->wds[p->cw]);
   if(valid_Var(str)==false){
      ERRORC("invalid variable, expected to get variables (e.g. $A)", p, __LINE__);
      return false;
   }
   #ifdef INTERP
   if(stack_find(p->s, p->wds[p->cw])!=1){
      ERRORC("uninitialised variable", p, __LINE__);
      return false;
   }
   #endif

   return true;
}

bool Integer(Program *p)  // V
{
   if(p==NULL){
      ERRORC("GRAMMAR ERROR ?", p,__LINE__);
      return false;
   }
   char tmp[MAXTOKENSIZE];
   strcpy(tmp, p->wds[p->cw]);
   if(valid_Int(tmp)==false){
      ERRORC("invalid integer, expected integer (e.g. 5)", p,__LINE__);
      return false;
   }
   #ifdef INTERP
   if(stack_push(p->s, p->wds[p->cw])!=1){
      ERRORC("uninitialised integer", p, __LINE__);
      return false;
   }
   #endif
   return true;
}

bool valid_Var(const char* str)
{
   if(strsame(str, "")){
      return false;
   }
   if(str[0]!='$' || (int)strlen(str)!=2){
      return false;
   }
   if(str[1]<'A' || str[1]>'Z'){
      return false;
   }
   return true;
}

bool valid_Int(const char* str)
{
   if(strsame(str, "")){
      return false;
   }
   bool isvalid = true;
   int len = strlen(str);
   if(len==0){
      isvalid = false;
   }
   int ck_start = 0;
   if(str[0]>='0' && str[0]<='9'){    // allowed "01"
      ck_start = 0;
   }
   #ifdef EXTENSION
   else if(str[0]=='-'){
      ck_start = 1;
   }
   #endif
   else{
      isvalid = false;
   }
   for(int i=len-1; i>=ck_start ; --i){
      if(str[i]<'0' || str[i]>'9'){
         isvalid = false;
      }
   }
   if(isvalid == false){
      return false;
   }
   return true;
}

bool valid_Str(const char* str)
{
   if(strsame(str, "")){
      return false;
   }
   int len = strlen(str);
   if(len<3){
      return false;
   }
   if(str[0]!='"' || str[len-1]!='"'){
      return false;
   }
   return true;
}

void ERROR(char* error_str, int line) 
{
   fprintf(stderr, "Fatal Error: %s occurred in %s, line %d\n", \
          error_str, __FILE__, line); 
}

void ERRORC(char* error_str, Program* p, int line)
{ 
   if(p->print_error_code == true){
      fprintf(stderr, "%s, word %d: Error: %s, occurred in %s, line %d\n", \
            p->wds[0], p->cw, error_str, __FILE__, line);
   
     char mark1[MAXTOKENSIZE];
      strcpy(mark1, p->wds[p->cw-1]);
      char mark2[MAXTOKENSIZE];
      strcpy(mark2, p->wds[p->cw]);

      if(p->cw==1){
         char mark3[MAXTOKENSIZE];
         strcpy(mark3, p->wds[p->cw+1]);
         printf("   %s %s\n", mark2, mark3);
         printf("   %.*s\n", (int)strlen(mark2), "^^^^^^^^^^^^^^^^^^^^");      
      }
      else if(p->cw==p->wds_cnt){
         printf("   %s %s\n", mark1 , mark2);
         printf("   %.*s %.*s\n", (int)strlen(mark1), "                    ", \
                                 (int)strlen(mark2), "^^^^^^^^^^^^^^^^^^^^");
      }
      else{
         char mark3[MAXTOKENSIZE];
         strcpy(mark3, p->wds[p->cw+1]);
         printf("   %s %s %s\n", mark1 , mark2, mark3);
         printf("   %.*s %.*s\n", (int)strlen(mark1), "                    ", \
                                 (int)strlen(mark2), "^^^^^^^^^^^^^^^^^^^^");
      }
   }
}
