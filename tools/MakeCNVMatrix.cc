#include <string>
#include "base/CommandLineParser.h"
#include "base/FileParser.h"


void ReadOne(svec<string> & genes, const string & fileName)
{
  FlatFileParser parser;
  
  parser.Open(fileName);

  while (parser.ParseLine()) {
    if (parser.GetItemCount() == 0)
      continue;
    StringParser s;
    s.SetLine(parser.AsString(5), ";");
    StringParser s2;
    s2.SetLine(s.AsString(0), "_");
    genes.push_back(s2.AsString(0));
		   
  }
}

int main( int argc, char** argv )
{

  commandArg<string> fileCmmd("-i","input file");
  commandLineParser P(argc,argv);
  P.SetDescription("Testing the file parser.");
  P.registerArg(fileCmmd);
 
  P.parse();
  
  string fileName = P.GetStringValueFor(fileCmmd);

  StringParser p;
  p.SetLine(fileName, ",");

  int i, j;
  
  svec<string> names;
  for (i=0; i<p.GetItemCount(); i++)
    names.push_back(p.AsString(i));

  svec<string> genes;
  
  for (i=0; i<names.isize(); i++) {
    for (j=0; j<names.isize(); j++) {
      string n = "genes" + names[i] + "_" + names[j];
      FILE * p = fopen(n.c_str(), "r");
      if (p == NULL)
	continue;

      fclose(p);

      ReadOne(genes, n);
      
    }

  }

  Sort(genes);
  
  for (i=0; i<genes.isize(); i++) {
    cout << genes[i] << endl;
  }
  

  return 0;
}
