
bool print_Sentence(Program *p);
bool SentenceList(Program *p);
bool Sentence(Program *p);
bool ComparOP(Program *p);
bool IfStmt(Program *p);
int If(Program *p);
bool Else(Program *p);

bool _print_var_insentence(stack* s, const char* d);

bool _compar_op(stack* s, const char* d);
bool _c_diff_op(stacktype* g, stacktype* g1, stacktype* g2, const char* d);
bool _c_same(stacktype* g, stacktype* g1, stacktype* g2);
bool _c_diff(stacktype* g, stacktype* g1, stacktype* g2);

bool _b_sub(stacktype* g, stacktype* g1, stacktype* g2);
bool _b_div(stacktype* g, stacktype* g1, stacktype* g2);
bool _b_modulus(stacktype* g, stacktype* g1, stacktype* g2);
