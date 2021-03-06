#include <string>
#include "base/CommandLineParser.h"
#include "base/FileParser.h"



int main( int argc, char** argv )
{

  commandArg<string> fileCmmd("-i","input file");
  //commandArg<int> bCmmd("-from","from column");
  //commandArg<int> eCmmd("-to","to column");
  //commandArg<bool> nCmmd("-newline","add newline", false);
  commandLineParser P(argc,argv);
  P.SetDescription("Testing the file parser.");
  P.registerArg(fileCmmd);
  //P.registerArg(bCmmd);
  //P.registerArg(eCmmd);
  //P.registerArg(nCmmd);
 
  P.parse();
  
  string fileName = P.GetStringValueFor(fileCmmd);
  //int from = P.GetIntValueFor(bCmmd);
  //int to = P.GetIntValueFor(eCmmd);
  //bool bN = P.GetBoolValueFor(nCmmd);
 

  //comment. ???
  FlatFileParser parser;
  
  parser.Open(fileName);

  while (parser.ParseLine()) {
    if (parser.GetItemCount() == 0)
      continue;
     StringParser p;
    p.SetLine(parser.AsString(8), "product=");
    cout << p.AsString(1);
    for (int i=9; i<parser.GetItemCount(); i++)
      cout << " " << parser.AsString(i);
    cout << endl;
  }
  return 0;
}
