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

  parser.ParseLine();
  //cout << parser.Line() << endl;

 
  int idx1 = -1;
  int idx2 = -1;
  int i, j;

  for (i=0; i<parser.GetItemCount(); i++) {
    if (parser.AsString(i) == "PCO@18.2.0#16.0.0") {
      idx1 = i;
    }
    if (parser.AsString(i) == "TAG50.1.0") {
      idx2 = i;
    }
  }
  cout << "Sample\t" << parser.AsString(idx1) << "\t" << parser.AsString(idx2) << endl;
  
  while (parser.ParseLine()) {
    if (parser.GetItemCount() == 0)
      continue;
    if (parser.AsString(idx1) == "." || parser.AsString(idx2) == ".")
      continue;
    string pre;
    if (strstr(parser.AsString(3).c_str(), "CTL") != NULL) {
      pre = parser.AsString(3);
    }
    if (strstr(parser.AsString(3).c_str(), "DM") != NULL) {
      pre = parser.AsString(3);
    }
    if (pre == "")
      continue;
    cout << pre << "\t" << parser.AsString(idx1) << "\t" << parser.AsString(idx2) << endl;
    
  }


  
  return 0;
}
