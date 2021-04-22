#include <stdio.h>
#include <assert.h>
#include <ROSE_ABORT.h>

void __ci_writeResultsToFile (char* input_file_name, char* output_file_name, int* loop_iter_counters, int loop_count)
{
  FILE * input_file = fopen(input_file_name, "r");
  if (input_file == NULL)
  {
    printf ("Error: cannot open input program information file!");
    ROSE_ABORT ();
  }

  // This happens only once for the main() of an instrumented application.
  // There should be reentry. 
  // append _w_counters
  FILE * output_file = fopen(output_file_name, "w");
  if (output_file == NULL)
  {
    printf ("Error: cannot open input program information file!");
    ROSE_ABORT ();
  }

  int __lc_init_i; 
  for (__lc_init_i = 0; __lc_init_i < loop_count; __lc_init_i++) {
    { 
      int input_int_id;
      char* input_file_path[4028];
      assert (input_file != NULL);
      fscanf (input_file, "%d\t%s\n",&input_int_id, input_file_path);
      //  while (fgetc(input_file) != '\n'); // bad idea, infinite looping 
//      printf ("%d\t%s\t%d\n", __lc_init_i, input_file_path, 0);
      fprintf (output_file, "%d\t%s\t%d\n", __lc_init_i, input_file_path, loop_iter_counters[__lc_init_i]);
    }
  }
  fclose(input_file); 
  fclose(output_file);
} 

