#include <string>
#include "base/CommandLineParser.h"
#include "base/FileParser.h"
#include "src/DNAVector.h"


void Tokenize(svec<string> & out, const string & in)
{
  int i;

  string b;

  b += in[0];
  
  for (i=1; i<(int)in.size(); i++) {
    if (in[i] == 'N' && in[i-1] != 'N') {
      out.push_back(b);
      b = "";
    }
    if (in[i] != 'N' && in[i-1] == 'N') {
      out.push_back(b);
      b = "";
    }

    b += in[i];
  }

  out.push_back(b);
}


int main( int argc, char** argv )
{

  commandArg<string> fileCmmd("-i","fasta input file");
  commandArg<string> outCmmd("-o","fasta output file");
  commandLineParser P(argc,argv);
  P.SetDescription("Testing the file parser.");
  P.registerArg(fileCmmd);
  P.registerArg(outCmmd);
 
  P.parse();
  
  string fileName = P.GetStringValueFor(fileCmmd);
  string outName = P.GetStringValueFor(outCmmd);

  vecDNAVector dna;

  dna.Read(fileName);

  CMTokenizer t;
  t.AddDelimiter("N");
  t.AddDelimiter("ANNNNNNNNNNC");
  
  int i, j, k=0;
  for (j=0; j<dna.isize(); j++) {
    string name = dna.Name(j);
    for (i=1; i<(int)name.size(); i++) {
      if (name[i] == '>')
	name[i] = '_';
    }
    dna.SetName(j, name);
    const DNAVector & d = dna[j];

    DNAVector n;
    
    string s = d.AsString();
    svec<string> split;
    Tokenize(split, s);
	     
    string b;
    
    for (i=0; i<split.isize(); i++) {
      
      if ((split[i])[0] == 'N' && split[i].size() == 10) {
	if (split[i-1].size() == 0)
	  continue;
	
	split[i] = "";
	cout << "Remove " << split[i-1].size() << " " <<  split[i+1].size() << endl;
	if (split[i-1].size() > split[i+1].size()) {
	  split[i+1] = "";
	  i++;
	} else {
	  split[i-1] = "";
	}
      } 
    }

    for (i=0; i<split.isize(); i++)
      b += split[i];
    
    
    n.SetFromBases(b);
    n.SetName(name);
    dna[j] = n;
    
    
  
  }

  dna.Write(outName);
  
  return 0;
}
