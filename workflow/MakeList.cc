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
  
  for (int i=3; i<argc; i++) {
    StringParser pp;
    pp.SetLine(argv[i], ",");
    for (int j=0; j<pp.GetItemCount(); j++)
      fprintf(p, "%s\n", pp.AsString(j).c_str());
  }

  fclose(p);

 
  
  return 0;
}
