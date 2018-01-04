#include <string>
#include "ml/NNet.h"
#include "base/RandomStuff.h"
#include "base/CommandLineParser.h"
#include "base/FileParser.h"


void Read(svec<NPCIO> & data, const string & fileName, int extra)
{
  FlatFileParser parser;
  parser.Open(fileName);

  int k = 0;
  int i;
  
  while (parser.ParseLine()) {
    if (parser.GetItemCount() == 0)
      continue;
    if (k == 0 && !parser.IsFloat(0)) {
      cout << "INPUT: Remove header!" << endl;
      continue;
    }
    NPCIO tmp;
    if (k < data.isize())
      tmp = data[k];
    int floor = tmp.isize();
    tmp.resize(floor + parser.GetItemCount() + extra);
    //cout << tmp.isize() << endl;
    for (i=0; i<extra; i++)
      tmp.SetValid(floor + parser.GetItemCount() + i, false);

    //cout << "Valid" << endl;
    for (i=0; i<parser.GetItemCount(); i++) {
      //cout << tmp.isize() << " " << i+floor << endl;
      if (floor == 0)
	tmp.SetValid(i+floor, true);
      else
	tmp.SetValid(i+floor, false);
 	
      //cout << "OK" << endl;
      double v = parser.AsFloat(i);

      /*
      if (i+floor == 9) {
	//cout << v << endl;
	v /= 4;
      }
     
      if (i+floor == 0)
	v /= 40;
      if (i == 1)
	v /= 140;
      if (i == 2)
	v /= 320;
      if (i == 5)
	v /= 160;
      */
      
      tmp[i+floor] = v;
      //cout << "Next" << endl;
    }
    //cout << "Assign" << endl;
    if (k < data.isize())
      data[k] = tmp;
    else
      data.push_back(tmp);
    k++;
  }
}

void Normalize(svec<NPCIO> & data, svec<double> & fac)
{
  int i, j;

  for (i=0; i<data[0].isize(); i++) {
    double max = 0.;
    for (j=0; j<data.isize(); j++) {
      if ((data[j])[i] > max) {
	max = (data[j])[i];
      }
    }
    double scale = 1./max;
    fac.push_back(scale);
    for (j=0; j<data.isize(); j++) {
      (data[j])[i] *= scale;      
    }
    
  }
  
}

void ReNormalize(svec<NPCIO> & data, const svec<double> & fac)
{
 
  int i, j;

  for (i=0; i<data[0].isize(); i++) {
   double scale = fac[i];
   for (j=0; j<data.isize(); j++) {
      (data[j])[i] *= scale;      
    }
    
  }

}

int main( int argc, char** argv )
{
  commandArg<string> fileCmmd("-i","input file (feed)");
  commandArg<string> outCmmd("-o","output file (truth)");
  commandArg<string> testCmmd("-t","test input file");
  commandArg<string> resCmmd("-r","results (guesses)");
  //commandArg<string> saveCmmd("-s","save the network", "");
  commandArg<int> nCmmd("-n","iterations", 2000);
  commandLineParser P(argc,argv);
  P.SetDescription("Run the DLN on matched in/out data and test by leaving one out.");
  P.registerArg(fileCmmd);
  P.registerArg(outCmmd);
  P.registerArg(testCmmd);
  P.registerArg(resCmmd);
  //P.registerArg(saveCmmd);
  P.registerArg(nCmmd);
  
  P.parse();
  
  string fileName = P.GetStringValueFor(fileCmmd);
  string outName = P.GetStringValueFor(outCmmd);
  string testName = P.GetStringValueFor(testCmmd);
  string resName = P.GetStringValueFor(resCmmd);
  //string confName = P.GetStringValueFor(hconfCmmd);
  //string saveName = P.GetStringValueFor(saveCmmd);
  int iter = P.GetIntValueFor(nCmmd);

  int i, j;


  svec<NPCIO> train, test;
  cout << "Read " << fileName << endl;
  Read(train, fileName, 0);
  int in = train[0].isize();
  cout << "Read " << outName << endl;
  Read(train, outName, 0);
  int out = train[0].isize();

  cout << "Read " << testName << endl;
  Read(test, testName, out-in);

  svec<double> fac;
  Normalize(train, fac);
  ReNormalize(test, fac);
  
  
  NeuralNetwork nn;
  int neurons = 40;
  cout << "Neurons: " << train[0].isize() << " times " << neurons << endl;
  nn.Setup(neurons, train[0].isize(), 1);
  nn.ReSetup(-1, 1);

  svec<double> tx, ty;
  
  cout << "NN set up, train" << endl;
  for (i=0; i<iter; i++) {
    int index = RandomInt(train.isize());
    nn.Learn(train[index]);
  }

  
  nn.Print();
  FILE * pOut = fopen(resName.c_str(), "w");

  for (i=0; i<test.isize(); i++) {
    NPCIO io = test[i];
    nn.RetrieveWeighted(io);

    //   cout << "RESULT " << x1 << " " << y << "  " << io[1] << endl;
    for (j=in; j<out; j++) {
      fprintf(pOut, "%f ", io[j]);
    }
    fprintf(pOut, "\n");
    fflush(pOut);  

    
  }
  fclose(pOut);

  return 0;
}
