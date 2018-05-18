#include <string>

#include "base/CommandLineParser.h"
#include "src/DNAVector.h"


int main(int argc,char** argv)
{

  
  commandArg<string> aStringCmmd("-i","input file");
  commandArg<string> bStringCmmd("-o","output file");
  commandLineParser P(argc,argv);
  P.SetDescription("Tool to test the VecDNAVector.");
  P.registerArg(aStringCmmd);
  P.registerArg(bStringCmmd);

  P.parse();

  string aString = P.GetStringValueFor(aStringCmmd);
  string bString = P.GetStringValueFor(bStringCmmd);
  
  vecDNAVector test;
  
  cout << "Reading file..." << endl;
  test.Read(aString);
  cout << "done!" << endl;

  
  for (int i=0; i<test.isize(); i++) {
    char tmp[512];
    sprintf(tmp, ">Seq_%d", i);
    test.SetName(i, tmp);
  }
  

  test.Write(bString);

  return 0;

}
  
