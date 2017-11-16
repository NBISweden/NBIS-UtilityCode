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
  
  vecDNAVector dna;
  dna.Read(fileName);

  svec<long long> len;
  len.resize(dna.isize(), 0);
  long long total = 0;
  int i;
  for (i=0; i<dna.isize(); i++) {
    total += dna[i].isize();
    len[i] = dna[i].isize();
  }

  Sort(len);
  int k = 0;
  for (i=0; i<len.isize(); i++) {
    if (k >= total/2) {
      cout << "N50: " << len[i] << endl; 
      break;
    }
    k += len[i];
  }
  
  return 0;
}
