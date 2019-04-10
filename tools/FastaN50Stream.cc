#include <string>
#include "base/CommandLineParser.h"
#include "base/FileParser.h"
#include "src/DNAVector.h"


int main( int argc, char** argv )
{

  commandArg<string> fileCmmd("-i","input file");
  commandLineParser P(argc,argv);
  P.SetDescription("Testing the file parser.");
  P.registerArg(fileCmmd);
  
  P.parse();
  
  string fileName = P.GetStringValueFor(fileCmmd);
  
  vecDNAVectorStream dna;
  dna.ReadStream(fileName);

  svec<long long> len; 
  long long total = 0;
  int i;
  int n = 0;
  while (dna.Next()) {
    total += dna.Get().isize();
    len.push_back(dna.Get().isize());
    n++;
  }

  Sort(len);
  long long k = 0;
  //cout << "Printing N50" << endl;

  cout << "Sequences:   " << n << endl;
  cout << "Nucleotides: " << total << endl;
  
  for (i=0; i<len.isize(); i++) {
    if (k >= total/2) {
      cout << "N50:         " << len[i] << endl; 
      break;
    }
    k += len[i];
  }

  return 0;
}
