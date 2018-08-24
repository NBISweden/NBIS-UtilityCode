#include <string>
#include "base/CommandLineParser.h"
#include "base/FileParser.h"
#include "base/RandomStuff.h"
#include <math.h>

int main( int argc, char** argv )
{

  commandArg<string> fileCmmd("-i","input file");
  //commandArg<string> iCmmd("-ignore","ignore columns", "");
  //commandArg<int> eCmmd("-to","to column");
  //commandArg<bool> nCmmd("-newline","add newline", false);
  commandLineParser P(argc,argv);
  P.SetDescription("Converts a csv file to a table.");
  P.registerArg(fileCmmd);
  //P.registerArg(iCmmd);
  // P.registerArg(eCmmd);
  //P.registerArg(nCmmd);
 
  P.parse();
  
  string fileName = P.GetStringValueFor(fileCmmd);
  //string ign = P.GetStringValueFor(iCmmd);
  //int to = P.GetIntValueFor(eCmmd);
  //bool bN = P.GetBoolValueFor(nCmmd);
 

  //comment. ???
  FlatFileParser parser;
  
  parser.Open(fileName);

  int i, j;

   
  parser.ParseLine();
  //cout << parser.Line() << endl;

  svec<string> l;
  cout << parser.AsString(0) << "\t" << parser.AsString(1);

  svec<int> good;
  good.resize(parser.GetItemCount(), 1);
  for (i=2; i<parser.GetItemCount(); i++) {
    if (strstr(parser.AsString(i).c_str(), "CTL") != NULL) {
      if (RandomFloat(1.) > 0.208)
	good[i] = 0;
    }

    
    if (good[i] == 0)
      continue;

    cout << "\t" << parser.AsString(i);
  }
  cout << endl;

  
  while (parser.ParseLine()) {
    if (parser.GetItemCount() == 0)
      continue;
    double max = 1;
    int n = 0;
    for (i=2; i<parser.GetItemCount(); i++) {
      if (good[i] == 0)
	continue;
      if (parser.AsFloat(i) > max)
	max = parser.AsFloat(i);
      if (parser.AsFloat(i) > 0.0001)
	n++;
    }
    if (n < 100)
      continue;
    
    cout << parser.AsString(0) << "\t" << parser.AsString(1);
    max *= 0.95;
    for (i=2; i<parser.GetItemCount(); i++) {
      if (good[i] == 0)
	continue;
      double d = parser.AsFloat(i)/max;
      if (d > 1.)
	d = 0.9999;
      cout << "\t" << d;
    }
    cout << endl;
   
    
  }

  
  return 0;
}
