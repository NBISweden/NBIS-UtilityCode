#include <string>
#include "base/CommandLineParser.h"
#include "base/FileParser.h"
#include "math/FishersExact.h"

void Read(const string & fileName, svec<string> & all)
{
  FlatFileParser parser;
  
  parser.Open(fileName);

  while (parser.ParseLine()) {
    if (parser.GetItemCount() == 0)
      continue;
    all.push_back(parser.AsString(1));
  }
  Sort(all);
}




int main( int argc, char** argv )
{

  commandArg<int> aCmmd("-a","samples A in test set");
  commandArg<int> bCmmd("-b","samples B in test set");
  commandArg<int> cCmmd("-c","samples A in truth set");
  commandArg<int> dCmmd("-d","samples B in truth set");
  commandLineParser P(argc,argv);
  P.SetDescription("Calculates an enrichment test.");
  P.registerArg(aCmmd);
  P.registerArg(bCmmd);
  P.registerArg(cCmmd);
  P.registerArg(dCmmd);
  
  P.parse();

  int a = P.GetIntValueFor(aCmmd);
  int b = P.GetIntValueFor(bCmmd);
  int c = P.GetIntValueFor(cCmmd);
  int d = P.GetIntValueFor(dCmmd);

  cout << "Fisher's exact test: " << FishersExact(a, b, c, d) << endl;
  return 0; 
}
