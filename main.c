#include "nlab.h"
#include "stack.h"
#include "extension.h"

int main(int argc, char* argv[])
{
   test_func();
   test_stack();
   test_prog();
   #ifdef EXTENSION
   test_prog_ext();
   #endif
   test_file();
   
   Program* prog = prog_init();

   // open a file
   if(argc != 2){
      fprintf(stderr, "Usage : %s <file.nlb>\n", argv[0]);
      exit(EXIT_FAILURE);
   }

   if(open_file(prog, argv[1])!=1){
      free(prog);
      return -1;
   }

   if(Prog(prog)==false){
      printf("at least 1 error generated in %s\n", prog->wds[0]);
      #ifdef INTERP
      stack_free(prog->s);
      #endif
      free(prog);
      return 1;
   }
   #ifdef INTERP
   stack_free(prog->s);
   #endif
   free(prog);
   return 0;
}
