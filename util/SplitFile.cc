#include <string>
#include "base/CommandLineParser.h"
#include "base/FileParser.h"



int main( int argc, char** argv )
{

  commandArg<string> fileCmmd("-i","input file");
  commandArg<int> nCmmd("-n","how many");
  commandArg<bool> numCmmd("-num","numerical numbering", false);
  commandLineParser P(argc,argv);
  P.SetDescription("Testing the file parser.");
  P.registerArg(fileCmmd);
  P.registerArg(nCmmd);
  P.registerArg(numCmmd);
  
  P.parse();
  
  string fileName = P.GetStringValueFor(fileCmmd);
  int n = P.GetIntValueFor(nCmmd);
  bool bNum = P.GetBoolValueFor(numCmmd);
  

  //comment. ???
  FlatFileParser parser;
  
  parser.Open(fileName);

  int i;
  svec<FILE*> files;
  files.resize(n);

  for (i=0; i<n; i++) {
    string name = fileName;
    char tmp[128];
    if (bNum) 
      sprintf(tmp, ".%.3d", i);
    else
      sprintf(tmp, ".%d", i);
    name += tmp;
    files[i] = fopen(name.c_str(), "w");
  }

  int k = 0;
  while (parser.ParseLine()) {
    int index = k % n;
    fprintf(files[index], "%s\n", parser.Line().c_str());

    k++;
  }

  for (i=0; i<n; i++) {
    fclose(files[i]);
  }
  return 0;
}
