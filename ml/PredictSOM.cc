#include <string>
#include "ml/NNet.h"
#include "base/RandomStuff.h"
#include "base/CommandLineParser.h"
#include "base/FileParser.h"

void Func(svec<double> & val, double x) {
  int i;
  for (i=1; i<val.isize(); i++)
    val[i-1] = val[i];
  val[val.isize()-1] = sin(x);  
} 



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
  
  int size = 8;
  int train = 2000;
  double inc = 0.2;
  int eval = 100;
  

  NeuralNetwork nn;
  int neurons = 60;
  nn.Setup(neurons, size, 1);
  //nn.SetTimeShift(1);
  nn.ReSetup(-1., 1);
  
  svec<double> data;
  data.resize(size+1, 0.);

  for (int k=0; k<20; k++) {
    for (i=0; i<train; i++) {
      Func(data, inc*(double)i);
      if (i < size+1)
	continue;
      
      NPCIO io;
      io.resize(size);
      for (j=0; j<size; j++)
	io[j] = data[j];
      
      io.SetAllValid(true);
      nn.Learn(io);    
    }
  }
  
  // nn.Train(1000, 0.1);
  nn.Print();
  double x = inc*(double)train;
  x -= 0.24;
  for (i=0; i<eval; i++) {
    
    double y = sin(x);
    x += 0.785*inc;
    NPCIO io;
    io.resize(size);

    for (j=0; j<size; j++)
      io[j] = data[j];
    
    io.SetAllValid(true);
    io.SetValid(size-1, false);
    io[size-1] = 0;
    nn.RetrieveWeighted(io);

    for (j=1; j<size; j++)
      data[j-1] = data[j];
    data[size-1] = io[size-1];
    
    cout << "RESULT " << i << " " << io[size-1] << " -> " << y << endl;

    
  }
  //cout << "Error: " << err << endl;
  
  return 0;
}
