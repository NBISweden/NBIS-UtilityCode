#include <string>
#include "base/CommandLineParser.h"
#include "base/FileParser.h"



int main( int argc, char** argv )
{

  /*
  commandArg<string> fileCmmd("-i","input file");
  commandLineParser P(argc,argv);
  P.SetDescription("Testing the file parser.");
  P.registerArg(fileCmmd);
 
  P.parse();
  
  string fileName = P.GetStringValueFor(fileCmmd);
  */

  string cmd = "rm ";
  cmd += argv[1];

  cout << "Try removing " << cmd << endl;
  int r = system(cmd.c_str());
  if (r == 0)
    cout << "Yepp!" << endl;
  else
    cout << "Nope..." << endl;
  
  return 0;
}
