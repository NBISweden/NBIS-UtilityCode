#include <string>
#include <stdio.h>

#include "base/CommandLineParser.h"
#include "base/FileParser.h"



int main( int argc, char** argv )
{

  if (argc < 4) {
    cout << "Usage: MakeList -o <outfile> <in1> <in2> <in3>..." << endl;
    return 0;    
  }
  
  FILE *p = fopen(argv[2], "w");
  
  for (int i=3; i<argc; i++)
    fprintf(p, "%s\n", argv[i]);

  fclose(p);

 
  
  return 0;
}
