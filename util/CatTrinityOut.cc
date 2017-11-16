#include <string>
#include "base/CommandLineParser.h"
#include "base/FileParser.h"



int main( int argc, char** argv )
{

  commandArg<int> fileCmmd("-n","number of directories");
  commandLineParser P(argc,argv);
  P.SetDescription("Concatenates Trinity output.");
  P.registerArg(fileCmmd);
  
  P.parse();
  
  int n = P.GetIntValueFor(fileCmmd);
  
  int c = system("mkdir all_out");

  for (int i=0; i<n; i++) {
    char tmp[256];
    sprintf(tmp, "cat RawComps.%d/*all* > all_out/part%d.fasta", i, i);
    cout << "Running: " << tmp << endl;
    c = system(tmp);
  }

  
  
 
  return 0;
}
