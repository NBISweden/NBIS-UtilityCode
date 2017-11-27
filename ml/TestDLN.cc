#include <string>
#include "ml/DLNet.h"
#include "base/RandomStuff.h"
#include "base/CommandLineParser.h"
#include "base/FileParser.h"




int main( int argc, char** argv )
{
  /*
  commandArg<string> fileCmmd("-i","input file");
  commandArg<int> bCmmd("-from","from column");
  commandArg<int> eCmmd("-to","to column");
  commandArg<int> cCmmd("-classes","class to train by");
  commandLineParser P(argc,argv);
  P.SetDescription("Testing the file parser.");
  P.registerArg(fileCmmd);
  P.registerArg(bCmmd);
  P.registerArg(eCmmd);
  P.registerArg(cCmmd);
 
  P.parse();
  
  string fileName = P.GetStringValueFor(fileCmmd);
  int from = P.GetIntValueFor(bCmmd);
  int to = P.GetIntValueFor(eCmmd);
  int c = P.GetIntValueFor(cCmmd);
  */
  
  int i, j;
  
  DLNet nn;

  nn.AddForwardLayer(2);
  nn.AddForwardLayer(2);
  nn.AddForwardLayer(2);
  nn.AddForwardLayer(1);
  
  DLIOSingle s;
  s.In().resize(2);
  s.Out().resize(1);

  s.In()[0] = 0.;
  s.In()[1] = 1.;
  s.Out()[0] = 1.;
  nn.AddInput(s);

  s.In()[0] = 1.;
  s.In()[1] = 0.;
  s.Out()[0] = 1.;
  nn.AddInput(s);

  s.In()[0] = 0.;
  s.In()[1] = 0.;
  s.Out()[0] = 0.;
  nn.AddInput(s);

  s.In()[0] = 1.;
  s.In()[1] = 1.;
  s.Out()[0] = 0.;  
  nn.AddInput(s);
  
  
  for (i=0; i<2500; i++) {
    double err = nn.TrainOne(0.5);
    nn.Evaluate();
    cout << "Error: " << err << endl;
  }
  
  return 0;
}
