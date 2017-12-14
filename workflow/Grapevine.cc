#include <string>
#include "base/CommandLineParser.h"
#include "base/FileParser.h"

#include "workflow/ScriptParser.h"



int main( int argc, char** argv )
{

  commandArg<string> fileCmmd("-i","script file");
  //commandArg<bool> helpCmmd("-h","help", false);
  commandLineParser P(argc,argv);
  P.SetDescription("Natural language-like workflow manager.");
  P.registerArg(fileCmmd);
  // P.registerArg(helpCmmd);
 
  P.parse();
  
  string fileName = P.GetStringValueFor(fileCmmd);
  // bool bHelp = P.GetBoolValueFor(helpCmmd);

  int i;
  
  ScriptParser p;
  p.Read(fileName);

  p.Process();

  for (i=0; i<p.isize(); i++)
    cout << p[i] << endl;

  
  return 0;
}
