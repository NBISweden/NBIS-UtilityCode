#include <string>
#include "ml/DLNet.h"
#include "base/RandomStuff.h"
#include "base/CommandLineParser.h"
#include "base/FileParser.h"


void Normalize(svec<DLIOSingle> & data, svec<double> & fac)
{
  int i, j;
  for (j=0; j<data[0].In().isize(); j++) {
    double max = 0.;
    for (i=0; i<data.isize(); i++) {
      const DLIOSingle & d = data[i];
      if (d.In()[j] > max)
	max = d.In()[j];
    }
  
    fac.push_back(1./max);
    for (i=0; i<data.isize(); i++) {
      DLIOSingle & d = data[i];
      d.In()[j] /= max;
      //cout << d.In()[j] << " " << i << " " << j << endl;
    }
  }
  for (j=0; j<data[0].Out().isize(); j++) {
    double max = 0.;
    for (i=0; i<data.isize(); i++) {
      const DLIOSingle & d = data[i];
      if (d.Out()[j] > max)
	max = d.Out()[j];
    }
    for (i=0; i<data.isize(); i++) {
      DLIOSingle & d = data[i];
      d.Out()[j] /= max;
      //cout << "Divide " << d.Out()[j] << " by " << max << " " << j << " " << i << endl;
    }
  }

}


void ReadInput(svec<DLIOSingle> & data, const string & fileName)
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
    DLIOSingle tmp;
    tmp.In().resize(parser.GetItemCount());
    for (i=0; i<parser.GetItemCount(); i++)
      tmp.In()[i] = parser.AsFloat(i);
    data.push_back(tmp);
    k++;
  }
}

void ReadOutput(svec<DLIOSingle> & data, const string & fileName)
{
  FlatFileParser parser;
  parser.Open(fileName);

  int k = 0;
  int i;
  
  while (parser.ParseLine()) {
    if (parser.GetItemCount() == 0)
      continue;
    if (k == 0 && !parser.IsFloat(0)) {
      cout << "OUTPUT: Remove header!" << endl;
      continue;
    }

    if (k >= data.isize()) {
      cout << "ERROR: inputs and outputs don't match! More output than input!" << endl;
      throw;
    }
    data[k].Out().resize(parser.GetItemCount());
    for (i=0; i<parser.GetItemCount(); i++)
      data[k].Out()[i] = parser.AsFloat(i);
   
    k++;
    
  }
  if (data.isize() != k) {
    cout << "ERROR: inputs and outputs don't match! " << data.isize() << " vs. " << k << endl;
    throw;
  }
}

void ReadConf(svec<int> & hidden, const string & fileName)
{
  FlatFileParser parser;
  parser.Open(fileName);

  while (parser.ParseLine()) {
    if (parser.GetItemCount() == 0)
      continue;
    hidden.push_back(parser.AsInt(0));
  }

}

int main( int argc, char** argv )
{
  
  commandArg<string> fileCmmd("-i","input file (feed)");
  commandArg<string> outCmmd("-o","output file (truth)");
  commandArg<string> testCmmd("-t","test input file");
  commandArg<string> resCmmd("-r","results (guesses)");
  //commandArg<string> saveCmmd("-s","save the network", "");
  commandArg<int> nCmmd("-n","iterations", 8000);
  commandArg<int> hCmmd("-h","hidden neurons", 2);
  commandArg<string> hconfCmmd("-hc","hidden neurons config file", "");
  commandArg<bool> byCmmd("-l","train one layer at a time", false);
  commandArg<double> dropCmmd("-d","dropout rate", 0.);
  commandArg<bool> normCmmd("-norm","normalize data to 1", false);
  commandLineParser P(argc,argv);
  P.SetDescription("Run the DLN on matched in/out data and test by leaving one out.");
  P.registerArg(fileCmmd);
  P.registerArg(outCmmd);
  P.registerArg(testCmmd);
  P.registerArg(resCmmd);
  //P.registerArg(saveCmmd);
  P.registerArg(nCmmd);
  P.registerArg(hCmmd);
  P.registerArg(hconfCmmd);
  P.registerArg(byCmmd);
  P.registerArg(dropCmmd);
  P.registerArg(normCmmd);
  
  P.parse();
  
  string fileName = P.GetStringValueFor(fileCmmd);
  string outName = P.GetStringValueFor(outCmmd);
  string testName = P.GetStringValueFor(testCmmd);
  string resName = P.GetStringValueFor(resCmmd);
  string confName = P.GetStringValueFor(hconfCmmd);
  //string saveName = P.GetStringValueFor(saveCmmd);
  int iter = P.GetIntValueFor(nCmmd);
  int hidden = P.GetIntValueFor(hCmmd);
  bool one = P.GetBoolValueFor(byCmmd);
  double drop = P.GetDoubleValueFor(dropCmmd);
  bool bNorm = P.GetBoolValueFor(normCmmd);
  
  int i, j;


  svec<int> hid;
  if (confName != "")
    ReadConf(hid, confName);

  svec<DLIOSingle> data;
  svec<DLIOSingle> test;

  ReadInput(data, fileName);
  ReadOutput(data, outName);
  ReadInput(test, testName);
  
  if (bNorm) {
    svec<double> fac;
    Normalize(data, fac);
    
    for (i=0; i<test.isize(); i++) {
      for (j=0; j<test[i].In().isize(); j++) {
	test[i].In()[j] *= fac[j];
      }
    }
  }

  FILE * pOut = fopen(resName.c_str(), "w");
  
  DLNet nn;
  nn.SetTrainByLayer(one);
  nn.SetDropOutRate(drop);

    
  nn.AddForwardLayer(data[0].In().isize());

  if (hid.isize() > 0) {
    for (j=0; j<hid.isize(); j++)
      nn.AddForwardLayer(hid[j]);
    
  } else {
    nn.AddForwardLayer(hidden);
  }
  nn.AddForwardLayer(data[0].Out().isize());
    
  nn.SupplyInput(data);    
  double err = nn.Train(iter, 0.5);

  for (i=0; i<test.isize(); i++) {
    svec<DLIOSingle> one;
    one.push_back(test[i]);
    one[0].Out().resize(data[0].Out().isize(), 0.);
    nn.SupplyInput(one);    
    err = nn.Evaluate();

    for (j=nn.GetOutStart(); j<nn.Neurons(); j++) {
      fprintf(pOut, "%f ", nn.GetNeuron(j).GetOutput());
    }
    fprintf(pOut, "\n");
    fflush(pOut);  
  }
  fclose(pOut);
  
  return 0;
}
