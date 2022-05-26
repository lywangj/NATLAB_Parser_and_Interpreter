#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#define ELEMSIZE 20
#define FIXEDSIZE 16
#define SCALEFACTOR 2

struct stacktype {
   char vname[ELEMSIZE];           // [20]
   int value[ELEMSIZE][ELEMSIZE];   // [20][20]
   int vrow;
   int vcolumn;
};
typedef struct stacktype stacktype;

struct stack {
   stacktype** arr;
   int size;
   int capacity;
   bool print_result;
};
typedef struct stack stack;

void test_func(void);
void test_stack(void);
void on_error(const char* s);
void* ncalloc(int n, size_t size);
void* nremalloc(void* p, int bytes);
/* Create an empty stack */
stack* stack_init(void);
/* Extend the capacity of empty stack */
bool stack_extend(stack* s);
/* Add element to top */
bool stack_push(stack* s, const char* d);
stacktype* stack_push_int(stack* s, const char* d);
stacktype* stack_push_var(stack* s, const char* d);
bool stack_push_readfile(stack* s, const char* d);
stacktype* stack_push_file(stack* s, const char* d);

/* copy element */
bool stack_copy(stacktype* d, stacktype* t);
/* Take element from top */
bool stack_pop(stack* s, stacktype* d);
/* Find element & add it to top */
bool stack_find(stack* s, const char* d);

/* Clears all space used */
bool stack_free(stack* s);

/* Operations */
bool _create_array(stack* s, const char* d);
bool _unary_op(stack* s, const char* d);
bool _u_diff_op(stacktype* g, stacktype* g1, const char* d);
bool _u_not(stacktype* g, stacktype* g1);
bool _u_eightcount(stacktype* g, stacktype* g1);
int _count_true_value(stacktype* g1, int j, int i);
bool _binary_op(stack* s, const char* d);
bool _b_diff_op(stacktype* g, stacktype* g1, stacktype* g2, const char* d);
int _valid_b_rules(stacktype* g1, stacktype* g2);
void _swap(stacktype* g1, stacktype* g2);
bool _b_and(stacktype* g, stacktype* g1, stacktype* g2);
bool _b_or(stacktype* g, stacktype* g1, stacktype* g2);
bool _b_greater(stacktype* g, stacktype* g1, stacktype* g2);
bool _b_less(stacktype* g, stacktype* g1, stacktype* g2);
bool _b_equals(stacktype* g, stacktype* g1, stacktype* g2);
bool _b_add(stacktype* g, stacktype* g1, stacktype* g2);
bool _b_times(stacktype* g, stacktype* g1, stacktype* g2);

int _get_value(stack* s);

bool _print_var(stack* s, const char* d);
bool _print_str(stack* s, const char* d);

bool _transfer_str(char* des, const char* d);
int char_toInt(const char* s);
