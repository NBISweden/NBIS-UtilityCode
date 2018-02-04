#include <string>
#include "ml/DLNet.h"
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

  int size = 8;
  int train = 2000;
  double inc = 0.2;
  int eval = 100;
  
  int i, j;
  
  DLNet nn;
  nn.AddForwardLayer(size);  
  nn.AddForwardLayer(size);
  nn.AddForwardLayer(1);

  svec<double> data;
  data.resize(size+1, 0.);
  
  for (i=0; i<train; i++) {
    Func(data, inc*(double)i);
    if (i < size+1)
      continue;
    
    DLIOSingle s;
    s.In().resize(size);
    s.Out().resize(1);

    for (j=0; j<size; j++)
      s.In()[j] = data[j];
    
    s.Out()[0] = data[size];
    
    nn.AddInput(s);
  }
   

  //nn.SetDropOutRate(0.1);
  //nn.SetTrainByLayer(true);
  
  for (i=0; i<100; i++) {   
    double err = nn.TrainOne(0.5);
    cout << i << " " <<  err << endl;
  }
 

  for (i=0; i<eval; i++) {
    DLIOSingle s;
    s.In().resize(size);
    s.Out().resize(1);
    s.Out()[0] = 0;
    for (j=0; j<size; j++)
      s.In()[j] = data[j];

    svec<DLIOSingle> in;
    in.push_back(s);
    nn.SupplyInput(in);
    nn.Evaluate();

    const DLNeuron & neuron = nn.GetNeuron(nn.Neurons()-1);
    cout << "RESULT " << i << " " << neuron.GetOutput() << endl;

    for (j=1; j<size; j++)
      data[j-1] = data[j];
    data[size-1] = neuron.GetOutput();
    
  }
  //cout << "Error: " << err << endl;
  
  return 0;
}
