#include <string>
#include "base/CommandLineParser.h"
#include "base/FileParser.h"



int main( int argc, char** argv )
{

  commandArg<string> fileCmmd("-i","input file");
  commandLineParser P(argc,argv);
  P.SetDescription("Concatenates broken up scaffolds.");
  P.registerArg(fileCmmd);
  
  P.parse();
  
  string fileName = P.GetStringValueFor(fileCmmd);
  

  //comment. ???
  FlatFileParser parser;
  
  parser.Open(fileName);

  while (parser.ParseLine()) {
    StringParser p;
    p.SetLine(parser.Line(), ".");
    if (p.GetItemCount() <= 1) {
      cout << parser.Line() << endl;
      continue;
    }
    const string & s = p.AsString(1);
   
    if (s[0] == '1' && s[1] == '-')
      cout << p.AsString(0) << endl;

  }
  return 0;
}
