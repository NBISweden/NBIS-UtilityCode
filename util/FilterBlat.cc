#include <string>
#include "base/CommandLineParser.h"
#include "base/FileParser.h"



int main( int argc, char** argv )
{

  commandArg<string> fileCmmd("-i","input file");
  commandLineParser P(argc,argv);
  P.SetDescription("Testing the file parser.");
  P.registerArg(fileCmmd);
  
  P.parse();
  
  string fileName = P.GetStringValueFor(fileCmmd);
  

  //comment. ???
  FlatFileParser parser;
  
  parser.Open(fileName);

  for (int i=0; i<5; i++) {
    parser.ParseLine();
    cout << parser.Line() << endl;
  }
  string last, lastLast;
  while (parser.ParseLine()) {
    const string & s = parser.AsString(9);
    if (s == last && s == lastLast)
      continue;
    cout << parser.Line() << endl;
    lastLast = last;
    last = s;
  }
  return 0;
}
