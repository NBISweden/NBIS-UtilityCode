#include <string>
#include "base/CommandLineParser.h"
#include "base/FileParser.h"

#include "workflow/ScriptParser.h"



int main( int argc, char** argv )
{

  commandArg<string> fileCmmd("-i","script file");
  commandArg<string> outCmmd("-o","output script file");
  //commandArg<bool> helpCmmd("-h","help", false);
  commandLineParser P(argc,argv);
  P.SetDescription("Natural language-like workflow manager.");
  P.registerArg(fileCmmd);
  P.registerArg(outCmmd);
  // P.registerArg(helpCmmd);
 
  P.parse();
  
  string fileName = P.GetStringValueFor(fileCmmd);
  string outName = P.GetStringValueFor(outCmmd);
  // bool bHelp = P.GetBoolValueFor(helpCmmd);

  int i, j;
  
  ScriptParser p;
  p.Read(fileName);

  for (j=0; j<p.GetCount(); j++) {
    p.Process(j);


    string name = outName;
    if (p.GetCount() > 1) {
      name += ".";
      name += Stringify(j);
    }
    
    FILE * pOut = fopen(name.c_str(), "w");

    cout << "*****************************************************" << endl;
    cout << "***************** Generating script " << j << endl;
    for (i=0; i<p.isize(); i++) {
      fprintf(pOut, "%s\n", p[i].c_str());
      cout << p[i] << endl;
    }
    
    fclose(pOut);
    cout << "***************** DONE " << endl;
    cout << "*****************************************************" << endl;
  }
  
  string me = "chmod +x " + outName;
  int r = system(me.c_str());
  
  return 0;
}
