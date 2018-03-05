#include <string>
#include "base/CommandLineParser.h"
#include "base/FileParser.h"

void Tokenize(svec<string> & out, const string & in)
{
  int i;
  string t;
  for (i=0; i<(int)in.length(); i++) {
    if (in[i] == ',') {
      if (t == "")
	t = ".";
      out.push_back(t);
      t = "";
      continue;
    }
    t += in[i];
  }
  if (t == "")
    t = ".";
  out.push_back(t);
}

int main( int argc, char** argv )
{

  commandArg<string> fileCmmd("-i","input file");
  commandArg<string> iCmmd("-ignore","ignore columns", "");
  //commandArg<int> eCmmd("-to","to column");
  //commandArg<bool> nCmmd("-newline","add newline", false);
  commandLineParser P(argc,argv);
  P.SetDescription("Converts a csv file to a table.");
  P.registerArg(fileCmmd);
  P.registerArg(iCmmd);
  // P.registerArg(eCmmd);
  //P.registerArg(nCmmd);
 
  P.parse();
  
  string fileName = P.GetStringValueFor(fileCmmd);
  string ign = P.GetStringValueFor(iCmmd);
  //int to = P.GetIntValueFor(eCmmd);
  //bool bN = P.GetBoolValueFor(nCmmd);
 

  //comment. ???
  FlatFileParser parser;
  
  parser.Open(fileName);

  int i, j;


  StringParser pi;
  pi.SetLine(ign, ",");

  svec<int> no;
  for (i=0; i<pi.GetItemCount(); i++)
    no.push_back(atol(pi.AsString(i).c_str()));
  
  while (parser.ParseLine()) {
    if (parser.GetItemCount() == 0)
      continue;
     string l = parser.Line();

    for (i=0; i<(int)l.length(); i++) {
      if (l[i] == ' ')
	l[i] = '_';
    }
    
    svec<string> tokens;
    Tokenize(tokens, l);

    bool b = false;
    for (i=0; i<tokens.isize(); i++) {
      bool bSkip = false;
      for (j=0; j<no.isize(); j++) {
	if (i == no[j])
	  bSkip = true;
      }
      if (bSkip)
	continue;
      
      if (b)
	cout << "\t";
      b = true;
      cout << tokens[i];
    }
    cout << endl;

    
  }
  return 0;
}
