#include <string>

#include "base/CommandLineParser.h"
#include "src/DNAVector.h"


int main(int argc,char** argv)
{

  
  commandArg<string> aStringCmmd("-i","input fasta file");
  commandArg<int> minCmmd("-min","minimum entry size",0);
  commandArg<int> maxCmmd("-max","maximum entry size",0);
  commandArg<string> repCmmd("-r","replace header", "");
  commandLineParser P(argc,argv);
  P.SetDescription("Filters fasta by length.");
  P.registerArg(aStringCmmd);
  P.registerArg(minCmmd);
  P.registerArg(maxCmmd);
  P.registerArg(repCmmd);

  P.parse();

  string aString = P.GetStringValueFor(aStringCmmd);
  int min = P.GetIntValueFor(minCmmd);
  int max = P.GetIntValueFor(maxCmmd);
  string rep = P.GetStringValueFor(repCmmd);
  


  vecDNAVector test;
  
  //cerr << "Reading file..." << endl;

  
  test.Read(aString);

  //cerr << "done." << endl;
  int k = 0;
  for (int i=0; i<test.isize(); i++) {
    const DNAVector & d = test[i];
    if (d.isize() >= min && (max == 0 || d.isize() <= max)) {
      if (rep == "") {
	printf("%s\n", test.Name(i).c_str());
      } else {
	printf(">%s_%d\n", rep.c_str(), k);	
      }
      k++;
      test[i].Write(stdout);
    }
  }

  return 0;

}
  
