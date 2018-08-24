
#include <string>
#include "base/CommandLineParser.h"
#include "base/FileParser.h"

bool HasNumber(const string & s)
{
  int i;
  if (s.length() == 1)
    return true;
  
  for (i=0; i<(int)s.length(); i++) {
    if (s[i] >= '0' && s[i] <= '9')
      return true;
  }
  return false;
}

int main( int argc, char** argv )
{

  commandArg<string> fileCmmd("-i","input file");
  //commandArg<int> bCmmd("-from","from column");
  //commandArg<int> eCmmd("-to","to column");
  commandArg<bool> nCmmd("-c","count", false);
  commandLineParser P(argc,argv);
  P.SetDescription("Testing the file parser.");
  P.registerArg(fileCmmd);
  //P.registerArg(bCmmd);
  //P.registerArg(eCmmd);
  P.registerArg(nCmmd);
 
  P.parse();
  
  string fileName = P.GetStringValueFor(fileCmmd);
  //int from = P.GetIntValueFor(bCmmd);
  //int to = P.GetIntValueFor(eCmmd);
  bool bCount = P.GetBoolValueFor(nCmmd);
 

  //comment. ???
  FlatFileParser parser;
  
  parser.Open(fileName);
  string last;
  int i;
  int n = 0;

  int off = 0;
  if (bCount)
    off = 1;
  while (parser.ParseLine()) {
    if (parser.GetItemCount() == 0)
      continue;

    
    string s = parser.AsString(off);
    for (i=off+1; i<parser.GetItemCount(); i++) {
      string g = parser.AsString(i);
      if (g == "(Fragment)")
	continue;
      if (i>=2+off && HasNumber(g))
	continue;
      s += " " + g;
    }
    if (s != last) {
      if (n > 0)
	cout << n << "\t" << last << endl;
      last = s;
      n = 0;
    }
    if (bCount)
      n += parser.AsInt(0);
    else
      n++;
  }
  return 0;
}
