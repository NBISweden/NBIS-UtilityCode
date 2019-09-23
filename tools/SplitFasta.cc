#include <string>

#include "base/CommandLineParser.h"
#include "src/DNAVector.h"


int main(int argc,char** argv)
{

  
  commandArg<string> aStringCmmd("-i","input fasta");
  commandArg<int> nCmmd("-n","how many");
  //commandArg<string> cStringCmmd("-o2","output file");
  commandLineParser P(argc,argv);
  P.SetDescription("Tool to test the VecDNAVector.");
  P.registerArg(aStringCmmd);
  P.registerArg(nCmmd);
  //P.registerArg(cStringCmmd);

  P.parse();

  string aString = P.GetStringValueFor(aStringCmmd);
  int howMany = P.GetIntValueFor(nCmmd);
  //string cString = P.GetStringValueFor(cStringCmmd);
  
  vecDNAVector test;
  
  cout << "Reading file..." << endl;
  test.Read(aString);

  int n = (test.isize() + howMany)/howMany;

  //out.resize(n);

  int i, j;
  for (j=0; j<howMany; j++) {
    vecDNAVector out;
    for (i=j; i<test.isize(); i+= howMany) {
      out.push_back(test[i], test.Name(i));     
    }
    char tmp[256];
    sprintf(tmp, ".%d", j);

    out.Write(aString + tmp);
  }

  cout << "done!" << endl;

  return 0;

}
  
