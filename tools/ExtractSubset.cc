#include <string>
#include "base/CommandLineParser.h"
#include "base/FileParser.h"
#include "base/RandomStuff.h"

void LoadList(svec<string> & l)
{

 FlatFileParser parser;
  
  parser.Open("p_values.txt");

  while (parser.ParseLine()) {
    if (parser.GetItemCount() == 0)
      continue;
    l.push_back(parser.AsString(0));
  }
}


int main( int argc, char** argv )
{

  commandArg<string> fileCmmd("-i","input file");
  //commandArg<int> bCmmd("-from","from column");
  //commandArg<int> eCmmd("-to","to column");
  //commandArg<bool> nCmmd("-newline","add newline", false);
  commandLineParser P(argc,argv);
  P.SetDescription("Testing the file parser.");
  P.registerArg(fileCmmd);
  //P.registerArg(bCmmd);
  //P.registerArg(eCmmd);
  //P.registerArg(nCmmd);
 
  P.parse();
  
  string fileName = P.GetStringValueFor(fileCmmd);
  //int from = P.GetIntValueFor(bCmmd);
  // int to = P.GetIntValueFor(eCmmd);
  //bool bN = P.GetBoolValueFor(nCmmd);
 

  svec<string> l;
  LoadList(l);

  int i;
  
  //comment. ???
  FlatFileParser parser;
  
  parser.Open(fileName);
  parser.ParseLine();

  svec<int> yes;
  yes.resize(parser.GetItemCount(), 0);

  cout << "Lipid";
  for (i=0; i<parser.GetItemCount(); i++) {
    for (int j=0; j<l.isize(); j++) {
      if (l[j] == parser.AsString(i)) {
	yes[i] = 1;
	cout << "\t" << l[j];
	break;
      }
	
    }
  }
  cout << endl;

  parser.ParseLine();
  parser.ParseLine();
  
  while (parser.ParseLine()) {
    if (parser.GetItemCount() == 0)
      continue;
    cout << parser.AsString(3);
    for (i=0; i<parser.GetItemCount(); i++) {
      if (yes[i]) {
	if (parser.AsString(i) != ".")
	  cout << "\t" << parser.AsString(i);
	else
	  cout << "\t" << RandomFloat(0.05);
      }
    }
    cout << endl;

  }
  return 0;
}
