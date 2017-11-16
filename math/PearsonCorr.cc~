#include <string>
#include "base/CommandLineParser.h"
#include "base/FileParser.h"
#include "math/Spearman.h"


int main( int argc, char** argv )
{

  commandArg<string> fileCmmd("-i","input file");
  commandArg<string> file2Cmmd("-i2","input file 2", "");
  commandLineParser P(argc,argv);
  P.SetDescription("Testing Spearman's rho.");
  P.registerArg(fileCmmd);
  P.registerArg(file2Cmmd);
  
  P.parse();
  
  string fileName = P.GetStringValueFor(fileCmmd);
  string fileName2 = P.GetStringValueFor(file2Cmmd);
  

  //comment. ???
  FlatFileParser parser, parser2;
  
  parser.Open(fileName);

  if (fileName2 != "")
    parser2.Open(fileName2);

  svec<double> x, y;

  while (parser.ParseLine()) {
    x.push_back(parser.AsFloat(0));
    if (fileName2 == "") {
      y.push_back(parser.AsFloat(1));
    } else {
      parser2.ParseLine();
      y.push_back(parser2.AsFloat(0));      
    }
  }

  SpearmansRho r;
  double rho = r.Compute(x, y);
  cout << "Rho=" << rho << " Significance=" << r.Significance() << endl;

  return 0;
}
