#include "nlab.h"
#include "stack.h"
#include "extension.h"

void on_error(const char* s)
{
   fprintf(stderr, "%s\n", s);
   exit(EXIT_FAILURE);
}

void* ncalloc(int n, size_t size)
{
   void* v = calloc(n, size);
   if(v==NULL){
      on_error("Cannot calloc() space");
   }
   return v;
}

void* nremalloc(void* p, int bytes)
{
   void* n = realloc(p, bytes);
   if(n==NULL){
      on_error("Cannot malloc() space");
   }
   return n;
}

stack* stack_init(void)
{
   stack *s = (stack*) ncalloc(sizeof(stack), 1);
   s->arr = (stacktype**) ncalloc(sizeof(stacktype*), FIXEDSIZE);
   s->size = 0;
   s->capacity= FIXEDSIZE;
   s->print_result = true;
   return s;
}

bool stack_extend(stack* s)
{
   if(s==NULL || s->capacity==0){
      return false;
   }
   s->arr = (stacktype**) nremalloc(s->arr,
      sizeof(stacktype*)*s->capacity*SCALEFACTOR);
   s->capacity = s->capacity*SCALEFACTOR;
   return true;
}

bool stack_push(stack* s, const char* d)
{
   if(s==NULL || d==NULL || strsame(d, "")){
      return false;
   }
   if(s->size >= s->capacity){
      if(stack_extend(s)!=1){
         return false;
      }
   }
   stacktype* a;
   char tmp[10];
   strcpy(tmp, d);

   if(tmp[0]>='0' && tmp[0]<='9' ){
      a = stack_push_int(s, tmp);
   }
   else if(tmp[0]=='-'){
      a = stack_push_int(s, tmp);
   }
   else if(tmp[0]=='$'){
      a = stack_push_var(s, tmp);
   }
   else{
      return false;
   }
   if(a==NULL){
      return false;
   }
   s->arr[s->size] = a ;
   s->size = s->size + 1;
   return true;
}

bool stack_push_readfile(stack* s, const char* d)
{
   if(valid_Str(d)==false){
      return false;
   }
   if(s==NULL){
      return false;
   }

   stacktype* a;
   a = stack_push_file(s, d);
   if(a==NULL){
      return false;
   }

   if(s->size >= s->capacity){
      if(stack_extend(s)!=1){
         return false;
      }
   }
   s->arr[s->size] = a ;
   s->size = s->size + 1;

   return true;
}

stacktype* stack_push_file(stack* s, const char* d)
{
   if(s==NULL || d==NULL){
      stacktype* a = NULL;
      return a;
   }
   if(valid_Str(d)==false){
      stacktype* a = NULL;
      return a;
   }
   char file_name[ELEMSIZE];
   if(_transfer_str(file_name, d)!=true){
      stacktype* a = NULL;
      return a;
   }
   char full_name[ELEMSIZE+6]="matrix/";
   strcat(full_name,file_name);
   FILE* fp = fopen(full_name, "r");
   if(fp==NULL){
      fclose(fp);
      ERROR("cannot read file", __LINE__);
      exit(EXIT_FAILURE);
   }

   stacktype* a = (stacktype*) ncalloc(1, sizeof(stacktype));
   if(a==NULL){
      free(a);
      fclose(fp);
      ERROR("calloc failed", __LINE__);
      exit(EXIT_FAILURE);
   }

   int row = 0;
   int column = 0;

   if(fscanf(fp, "%d %d", &row, &column) != 2){
      free(a);
      fclose(fp);
      stacktype* a = NULL;
      char str_error[ELEMSIZE*SCALEFACTOR];
      strcpy(str_error, "Cannot read row or column of ");
      strcat(str_error, file_name);
      ERROR(str_error, __LINE__);
      return a;
   }
   if(row <= 1 || column <= 1 || 
      row > ELEMSIZE || column > ELEMSIZE){
      free(a);
      fclose(fp);
      stacktype* a = NULL;
      ERROR("INVALID ROW OR COLUMN", __LINE__);
      return a;
   }

   strcpy(a->vname, file_name);
   a->vrow = row;
   a->vcolumn = column;   // len includes extra-""

   for(int j=0; j<a->vrow ;++j){
      for(int i=0; i<a->vcolumn; ++i){
         if(fscanf(fp, "%d", &a->value[j][i]) != 1){
            ERROR("error", __LINE__);
            free(a);
            fclose(fp);
            stacktype* a = NULL;
            return a;
         }
      }
   }

   fclose(fp);
   return a;
}

stacktype* stack_push_int(stack* s, const char* d)
{
   if(s==NULL){
      ERROR("stack failed", __LINE__);
   }
   if(valid_Int(d)==false){
      stacktype* a = NULL;
      return a;
   }
   int num = char_toInt(d);
   stacktype* a = (stacktype*) ncalloc(1, sizeof(stacktype));

   strcpy(a->vname, d);
   a->vrow = 1;
   a->vcolumn = 1;
   a->value[0][0] = num;
   return a;
}

stacktype* stack_push_var(stack* s, const char* d)
{
   if(s==NULL){
      ERROR("stack failed", __LINE__);
   }
   if(valid_Var(d)==false){
      stacktype* a = NULL;
      return a;
   }

   stacktype* a = (stacktype*) ncalloc(1, sizeof(stacktype));
   stacktype* g = (stacktype*) ncalloc(1, sizeof(stacktype));
   if(stack_pop(s, g)!=true){
      free(a);
      free(g);
      return false;
   }
   if(stack_copy(a,g)!=true){
      free(a);
      free(g);
      return false;
   }
   strcpy(a->vname, d);

   free(g);
   return a;
}

bool stack_copy(stacktype* d, stacktype* t)
{
   if(d==NULL || t==NULL){
      return false;
   }
   if(strsame(t->vname,"")){
      return false;
   }   
   strcpy(d->vname, t->vname);
   d->vrow = t->vrow;
   d->vcolumn = t->vcolumn;

   for(int j=0; j<t->vrow ;++j){
      for(int i=0; i<t->vcolumn; ++i){
         d->value[j][i] = t->value[j][i];
      }
   }
   return true;
}

bool stack_pop(stack* s, stacktype* d)
{
   if((s == NULL) || (s->size < 1)){
      return false;
   }
   s->size = s->size - 1;
   stacktype *t = s->arr[s->size];

   if(stack_copy(d,t)!=true){
      return false;
   }
   free(s->arr[s->size]);
   return true;
}

bool stack_find(stack* s, const char* d)
{
   if((s==NULL) || s->size <= 0 || strsame(d, "")){
      /* Stack is Empty */
      return false;
   }
   if(valid_Var(d)==false){
      return false;
   }
   char tmp[ELEMSIZE];
   strcpy(tmp, d);

   int posi = s->size-1;
   stacktype* p = s->arr[posi];
   while(strsame(p->vname,d)==false && posi>0){
      posi--;
      p = s->arr[posi];
   }
   // if found, push "$A"
   if(strsame(p->vname,d)){
      if(s->size >= s->capacity){
         if(stack_extend(s)!=1){
            return false;
         }
      }
      stacktype* t = s->arr[posi];
      stacktype* d = (stacktype*) ncalloc(1, sizeof(stacktype));
      if(stack_copy(d, t)!=1){
         free(d);
         return false;
      }
      s->arr[s->size] = d;
      s->size = s->size + 1;
   }

   // if not found, set ERROR, return false
   if(strsame(p->vname,d)==false){
      return false;
   }

   return true;
}

bool _create_array(stack* s, const char* d)
{
   if(valid_Var(d)==false){
      return false;
   }
   if((s==NULL) || (s->size <= 0)){
      return false;
   }
   stacktype* g1 = (stacktype*) ncalloc(1, sizeof(stacktype));
   stacktype* g2 = (stacktype*) ncalloc(1, sizeof(stacktype));

   if(stack_pop(s, g1)!=1){   // COLS
      free(g1);
      free(g2);
      return false;
   }
   if(stack_pop(s, g2)!=1){   // ROWS
      free(g1);
      free(g2);
      return false;
   }
   if(g1->vrow!=1 || g1->vcolumn!=1 || 
      g2->vrow!=1 || g2->vcolumn!=1 || 
      g1->value[0][0]==0 || g2->value[0][0]==0) {
      free(g1);
      free(g2);
      return false;
   }

// <CREATE> ::= "ONES" <ROWS> <COLS> <VARNAME>
   stacktype* a = (stacktype*) ncalloc(1, sizeof(stacktype));
   strcpy(a->vname, d);
   a->vrow = g2->value[0][0];
   a->vcolumn = g1->value[0][0];
   for(int j=0; j<a->vrow ;++j){
      for(int i=0; i<a->vcolumn; ++i){
         a->value[j][i] = 1;
      }
   }
   s->arr[s->size] = a;
   s->size = s->size + 1;
   free(g1);
   free(g2);
   return true;
}

void _swap(stacktype* g1, stacktype* g2)
{
   stacktype* tmp = (stacktype*) ncalloc(1, sizeof(stacktype));
   assert(stack_copy(tmp, g1));
   assert(stack_copy(g1, g2));
   assert(stack_copy(g2, tmp));
   free(tmp);
}

// <UNARYOP> ::= "U-NOT" | "U-EIGHTCOUNT"
bool _unary_op(stack* s, const char* d)
{
   if(s==NULL || strsame(d,"")){
      return false;
   }
   stacktype* g1 = (stacktype*) ncalloc(1, sizeof(stacktype));
   if(stack_pop(s, g1)!=1){
      free(g1);
      return false;
   }

   stacktype* a = (stacktype*) ncalloc(1, sizeof(stacktype));
   strcpy(a->vname, "#U");
   a->vrow = g1->vrow;
   a->vcolumn = g1->vcolumn;

   if(_u_diff_op(a, g1, d)!=1){
      free(a);
      free(g1);
      return false;
   }
   s->arr[s->size] = a;
   s->size = s->size + 1;

   free(g1);
   return true;
}

// <UNARYOP> ::= "U-NOT" | "U-EIGHTCOUNT"
bool _u_diff_op(stacktype* g, stacktype* g1, const char* d)
{
   if(g!=NULL && g1!=NULL && strsame(d,"")!=1){
      if(strsame(d, "U-NOT")){
         if(_u_not(g, g1)!=1){
            return false;
         }
      }
      else if(strsame(d, "U-EIGHTCOUNT")){
         if(_u_eightcount(g, g1)!=1){
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

// "U-NOT"
bool _u_not(stacktype* g, stacktype* g1)
{
   if(g==NULL || g1==NULL || g1->vrow==0 || g1->vcolumn==0){
      return false;
   }
   for(int j=0; j<g1->vrow ;++j){
      for(int i=0; i<g1->vcolumn; ++i){
         if(g1->value[j][i]!=0){
            g->value[j][i] = 0;
         }
         else{
            g->value[j][i] = 1;
         }
      }
   }
   return true;
}

// "U-EIGHTCOUNT"
bool _u_eightcount(stacktype* g, stacktype* g1)
{
   if(g==NULL || g1==NULL || g1->vrow==0 || g1->vcolumn==0){
      return false;
   }
   for(int j=0; j<g1->vrow ;++j){
      for(int i=0; i<g1->vcolumn; ++i){
         g->value[j][i] = _count_true_value(g1, j, i);
      }
   }
   return true;
}

int _count_true_value(stacktype* g1, int j, int i)
{
   if(g1==NULL || j<0 || i<0){
      return -999;
   }
   int count = 0;
   for(int jj=j-1; jj<=j+1; ++jj){
      for(int ii=i-1; ii<=i+1; ++ii){
         if(jj>=0 && ii>=0 && jj<g1->vrow && ii<g1->vcolumn){
            if(jj!=j || ii!=i){
               if(g1->value[jj][ii]!=0){
                  count++;
               }
            }
         }
      }
   }
   return count;
}

// <BINARYOP> :: "B-AND" | "B-OR" | "B-GREATER" | "B-LESS" | "B-ADD" | "B-TIMES" | "B-EQUALS"
bool _binary_op(stack* s, const char* d)
{
   if(s==NULL || strsame(d,"")){
      return false;
   }
   stacktype* g1 = (stacktype*) ncalloc(1, sizeof(stacktype));
   stacktype* g2 = (stacktype*) ncalloc(1, sizeof(stacktype));

   if(stack_pop(s, g2)!=1){   // COLS
      free(g1);
      free(g2);
      return false;
   }
   if(stack_pop(s, g1)!=1){   // ROWS
      free(g1);
      free(g2);
      return false;
   }

// SET $D := $F $E B-TIMES ;
   stacktype* a = (stacktype*) ncalloc(1, sizeof(stacktype));
   strcpy(a->vname, "#B");
   a->vrow = (g1->vrow > g2->vrow)? g1->vrow : g2->vrow;
   a->vcolumn = (g1->vcolumn > g2->vcolumn)? g1->vcolumn : g2->vcolumn;

   if(_b_diff_op(a, g1, g2, d)!=1){
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

bool _b_diff_op(stacktype* g, stacktype* g1, stacktype* g2, const char* d)
{
   if(g!=NULL && g1!=NULL && g2!=NULL && strsame(d,"")!=1){
      if(strsame(d, "B-AND")){
         if(_b_and(g, g1, g2)!=1){
            return false;
         }
      }
      else if(strsame(d, "B-OR")){
         if(_b_or(g, g1, g2)!=1){
            return false;
         }
      }
      else if(strsame(d, "B-GREATER")){
         if(_b_greater(g, g1, g2)!=1){
            return false;
         }
      }
      else if(strsame(d, "B-LESS")){
         if(_b_less(g, g1, g2)!=1){
            return false;
         }
      }
      else if(strsame(d, "B-ADD")){
         if(_b_add(g, g1, g2)!=1){
            return false;
         }
      }
      else if(strsame(d, "B-TIMES")){
         if(_b_times(g, g1, g2)!=1){
            return false;
         }
      }
      #ifdef EXTENSION
      else if(strsame(d, "B-SUB")){
         if(_b_sub(g, g1, g2)!=1){
            return false;
         }
      }
      else if(strsame(d, "B-DIV")){
         if(_b_div(g, g1, g2)!=1){
            return false;
         }
      }
      else if(strsame(d, "B-MODULUS")){
         if(_b_modulus(g, g1, g2)!=1){
            return false;
         }
      }
      #endif
      else if(strsame(d, "B-EQUALS")){
         if(_b_equals(g, g1, g2)!=1){
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

// "B-AND"
bool _b_and(stacktype* g, stacktype* g1, stacktype* g2)
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
         if(b_rules==1 || b_rules==2){
            if(g1->value[0][0]==0 || g2->value[j][i]==0){
               g->value[j][i] = 0;
            }
            else{
               g->value[j][i] = 1;
            }
         }
         else if(b_rules==3){
            if(g1->value[j][i]==0 || g2->value[j][i]==0){
               g->value[j][i] = 0;
            }
            else{
               g->value[j][i] = 1;
            }
         }
      }
   }
   return true;
}

// "B-OR"
bool _b_or(stacktype* g, stacktype* g1, stacktype* g2)
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
         if(b_rules==1 || b_rules==2){
            if(g1->value[0][0]==0 && g2->value[j][i]==0){
               g->value[j][i] = 0;
            }
            else{
               g->value[j][i] = 1;
            }
         }
         else if(b_rules==3){
            if(g1->value[j][i]==0 && g2->value[j][i]==0){
               g->value[j][i] = 0;
            }
            else{
               g->value[j][i] = 1;
            }
         }
      }
   }
   return true;
}

// "B-GREATER"
bool _b_greater(stacktype* g, stacktype* g1, stacktype* g2)
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
            if(g1->value[0][0]>g2->value[j][i]){
               g->value[j][i] = 1;
            }
            else{
               g->value[j][i] = 0;
            }
         }
         else if(b_rules==2){
            if(g1->value[0][0]<g2->value[j][i]){
               g->value[j][i] = 1;
            }
            else{
               g->value[j][i] = 0;
            }
         }
         else if(b_rules==3){
            if(g1->value[j][i]>g2->value[j][i]){
               g->value[j][i] = 1;
            }
            else{
               g->value[j][i] = 0;
            }
         }
      }
   }
   return true;
}

// "B-LESS"
bool _b_less(stacktype* g, stacktype* g1, stacktype* g2)
{
   if(g==NULL || g1==NULL || g2==NULL || \
      g1->vrow==0 || g1->vcolumn==0 || g2->vrow==0 || g2->vcolumn==0){
      return false;
   }
   // _copy_array()
   int b_rules = _valid_b_rules(g1, g2);
   if(b_rules==0){
      // ERROR("GRAMMAR ERROR ?");
      return false;
   }
   else if(b_rules==2){
      _swap(g1, g2);
   }
   for(int j=0; j<g2->vrow ;++j){
      for(int i=0; i<g2->vcolumn; ++i){
         if(b_rules==1){
            if(g1->value[0][0]<g2->value[j][i]){
               g->value[j][i] = 1;
            }
            else{
               g->value[j][i] = 0;
            }
         }
         else if(b_rules==2){
            if(g1->value[0][0]>g2->value[j][i]){
               g->value[j][i] = 1;
            }
            else{
               g->value[j][i] = 0;
            }
         }
         else if(b_rules==3){
            if(g1->value[j][i]<g2->value[j][i]){
               g->value[j][i] = 1;
            }
            else{
               g->value[j][i] = 0;
            }
         }
      }
   }
   return true;
}

// "B-EQUALS"
bool _b_equals(stacktype* g, stacktype* g1, stacktype* g2)
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
         if(b_rules==1 || b_rules==2){
            if(g1->value[0][0]!=g2->value[j][i]){
               g->value[j][i] = 0;
            }
            else{
               g->value[j][i] = 1;
            }
         }
         else if(b_rules==3){
            if(g1->value[j][i]!=g2->value[j][i]){
               g->value[j][i] = 0;
            }
            else{
               g->value[j][i] = 1;
            }
         }
      }
   }
   return true;
}

// "B-ADD"
bool _b_add(stacktype* g, stacktype* g1, stacktype* g2)
{
   int b_rules = _valid_b_rules(g1, g2);
   if(b_rules==0){
      return false;
   }
   else if(b_rules==2){
      _swap(g1, g2);
   }
   for(int j=0; j<g2->vrow ;++j){
      for(int i=0; i<g2->vcolumn; ++i){
         if(b_rules==1 || b_rules==2){
            g->value[j][i] = g1->value[0][0] + g2->value[j][i] ;
         }
         else if(b_rules==3){
            g->value[j][i] = g1->value[j][i] + g2->value[j][i] ;
         }
      }
   }
   return true;
}

// "B-TIMES"
bool _b_times(stacktype* g, stacktype* g1, stacktype* g2)
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
         if(b_rules==1 || b_rules==2){
            g->value[j][i] = g1->value[0][0] * g2->value[j][i] ;
         }
         else if(b_rules==3){
            g->value[j][i] = g1->value[j][i] * g2->value[j][i] ;
         }
      }
   }
   return true;
}

int _valid_b_rules(stacktype* g1, stacktype* g2)
{
   if(g1==NULL || g2==NULL || \
      g1->vrow==0 || g1->vcolumn==0 || g2->vrow==0 || g2->vcolumn==0){
      return 0;
   }
   if(g1->vrow==g2->vrow && g1->vcolumn==g2->vcolumn){
      return 3;
   }
   else if(g1->vrow==1 && g1->vcolumn==1) {
      return 1;
   }
   else if(g2->vrow==1 && g2->vcolumn==1) {
      return 2;
   }
   return 0;
}

bool _print_var(stack* s, const char* d)
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
   if(s->print_result==true){
      for(int j=0; j<g1->vrow ;++j){
         for(int i=0; i<g1->vcolumn; ++i){
            printf("%d ", g1->value[j][i]);
         }
         printf("\n");
      }
   }
   free(g1);
   return true;
}

bool _print_str(stack* s, const char* d)
{
   if((s==NULL) || (s->size <= 0) || strsame(d,"")){
      return false;
   }
   if(valid_Str(d)==false){
      return false;
   }
   char str_print[ELEMSIZE];
   if(_transfer_str(str_print, d)==false){
      return false;
   }
   if(s->print_result==true){
      printf("%s", str_print);
   }
   return true;
}

bool _transfer_str(char* des, const char* d)
{
   if(d==NULL){
      return false;
   }
   if(valid_Str(d)==false){
      return false;
   }
   int len = strlen(d);
   if(d[0]!='"' || d[len-1]!='"'){
      return false;
   }
   for(int i=1 ;i<len-1; ++i){
      des[i-1] = d[i];
   }
   des[len-2] = '\0';

   return true;
}

int char_toInt(const char* str)
{
   if(valid_Int(str)!=1){
      return -999;
   }
   int num = atoi(str);
   return num;
}

int _get_value(stack* s)
{
   if(s==NULL){
      return -999;
   }
   int value = 0;
   if(s->size==0){
      return -999;
   }
   stacktype* p = s->arr[s->size-1];

   if(p->vrow!=1 || p->vcolumn!=1){
      return -999;
   }

   value = p->value[0][0];
   return value;
}

bool stack_free(stack* s)
{
   if(s==NULL){
      return true;
   }
   if(s){
      for(int i=0; i<s->size; ++i){
         free(s->arr[i]);
      }
   }
   free(s->arr);
   free(s);
   return true;
}
