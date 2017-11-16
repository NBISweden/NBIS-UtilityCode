#include <string>

#include "base/CommandLineParser.h"
#include "src/DNAVector.h"
#include <string.h>

int main(int argc,char** argv)
{

  
  commandArg<string> aStringCmmd("-i","fasta input file");
  commandArg<string> bStringCmmd("-n","name", "");
  commandArg<bool> tCmmd("-total","total length only", false);
  commandLineParser P(argc,argv);
  P.SetDescription("Prints the sizes of a fasta file.");
  P.registerArg(aStringCmmd);
  P.registerArg(bStringCmmd);
  P.registerArg(tCmmd);

  P.parse();

  string aString = P.GetStringValueFor(aStringCmmd);  
  string bString = P.GetStringValueFor(bStringCmmd);  
  bool bTotal = P.GetBoolValueFor(tCmmd);  


  vecDNAVector test;
  
  //cout << "Reading file..." << endl;
  test.Read(aString);
  //cout << "done!" << endl;
  //test.ReverseComplement();

  //test.Write(bString);
  int tot(0);
  int ll = 0;
  int i;
  int longest = 0;
  string lName;
  if (bString != "")
    cout << bString << endl;
  unsigned int total = 0;
  for (i=0; i<test.isize(); i++) {
    total += test[i].isize();
    if (!bTotal) {
      char tmp[2048];
      strcpy(tmp, &((const char*)test.Name(i).c_str())[1]);
      cout << tmp << "\t" << test[i].isize() << endl;
    }
    tot+=test[i].isize();
    if (test[i].isize() > longest) {
      longest = test[i].isize();
      ll = i;
      lName = test.Name(i);
    }
  }

  cout << endl;

  cout << "total: " << tot << endl;
  cout << "longest: " << longest <<  " (" << ll << " " << lName << " )" << endl;
  return 0;

}
  
