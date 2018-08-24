#include <string>

#include "base/CommandLineParser.h"
#include "src/DNAVector.h"
#include <string.h>
#include "base/FileParser.h"



int main(int argc,char** argv)
{

  
  commandArg<string> aStringCmmd("-f","fasta input file");
  commandArg<string> bStringCmmd("-i","list of sequence names");
  commandArg<bool> keepCmmd("-keep","KEEP sequences", false);
  commandLineParser P(argc,argv);
  P.SetDescription("Removes sequences from a fasta file.");
  P.registerArg(aStringCmmd);
  P.registerArg(bStringCmmd);
  P.registerArg(keepCmmd);

  P.parse();

  string fasta = P.GetStringValueFor(aStringCmmd);  
  string fileName = P.GetStringValueFor(bStringCmmd);  
  bool bKeep = P.GetBoolValueFor(keepCmmd);  


  vecDNAVector test;
  
  //cout << "Reading file..." << endl;
  test.Read(fasta, false, true);
 
  int i;

  FlatFileParser parser;
  
  parser.Open(fileName);

  svec<string> names;

  while (parser.ParseLine()) {
    if (parser.GetItemCount() == 0)
      continue;
    if (parser.AsString(0)[0] == '>') {
      names.push_back(parser.AsString(0));
    } else{
      string name = ">";
      name += parser.AsString(0);
      names.push_back(name);
    }
    //cout << names[names.isize()-1] << endl;
  }

  Sort(names);

  for (i=0; i<test.isize(); i++) {
    string n = test.Name(i);
    //cout << "Search for " << n << endl;
    int index = BinSearch(names, n);
    if ((!bKeep && index < 0) || (bKeep && index >=0)) {

      cout << n << endl;
      for (int j=0; j<test[i].isize(); j++)
	cout << (test[i])[j];
      cout << endl;

    }
  }

  return 0;

}
  
