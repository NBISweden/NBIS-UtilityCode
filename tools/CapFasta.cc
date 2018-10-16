#include <string>
#include "base/CommandLineParser.h"
#include "base/FileParser.h"
#include "src/DNAVector.h"


int main( int argc, char** argv )
{

  commandArg<string> fileCmmd("-i","input file");
  commandArg<int> bCmmd("-m","maximum contig length");
  commandLineParser P(argc,argv);
  P.SetDescription("Enforces a maximum sequence length.");
  P.registerArg(fileCmmd);
  P.registerArg(bCmmd);
  
  P.parse();
  
  string fileName = P.GetStringValueFor(fileCmmd);
  int max = P.GetIntValueFor(bCmmd);
 
  vecDNAVector dna;
  dna.Read(fileName);

  int i, j;
  
  for (i=0; i<dna.isize(); i++) {
    string name = dna.Name(i);
    if (dna[i].isize() <= max) {
      cout << name << endl;
      for (j=0; j<dna[i].isize(); j++)
	cout << (dna[i])[j];
      cout << endl;
      continue;
    }
    int n = 0;
    int last = 0;
    //cout << name << "_0-" << max-1 << endl;
    for (j=0; j<dna[i].isize(); j++) {
      if (j % max == 0) {
	if (j > 0)
	  cout << endl;
	if (max + j < dna[i].isize())
	  cout << name << "_" << j << "-" << j+max-1 << endl;
	else
	  cout << name << "_" << j << "-" << dna[i].isize()-1 << endl;
      }
      cout << (dna[i])[j];

    }
    cout << endl;

  }

  return 0;
}
