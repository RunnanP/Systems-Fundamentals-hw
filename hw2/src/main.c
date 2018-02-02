#include "debug.h"

#include "utf.h"
#include "wrappers.h"
#include <stdlib.h>


int
main(int argc, char *argv[])
{
  int infile, outfile, in_flags, out_flags;
  parse_args(argc, argv);       //args.c
 // printf("ddddd");
  check_bom();                  //utf.c
  print_state();               //args.c
  in_flags = O_RDONLY;          //

  out_flags = O_WRONLY | O_CREAT;
  infile = Open(program_state->in_file, in_flags);
  outfile = Open(program_state->out_file, out_flags);
 // lseek(SEEK_SET, program_state->bom_length, infile); /* Discard BOM */
  lseek(infile, program_state->bom_length, SEEK_SET);
  get_encoding_function()(infile, outfile);  //utf.c
  //if(program_state != NULL) {
    //close((int)program_state);
  //  close((int)(long)program_state);

    free(program_state);

  //}

    close(outfile);
    close(infile);

  return EXIT_SUCCESS;
}
