#include <string>

#include "base/CommandLineParser.h"
#include "src/DNAVector.h"


int main(int argc,char** argv)
{

  
  commandArg<string> aStringCmmd("-i","protein input file");
  commandArg<string> bStringCmmd("-o","output file");
  //commandArg<bool> boolCmmd("-nucleotides","input file is in nucleotides", 0);
  commandLineParser P(argc,argv);
  P.SetDescription("Converts protein fasta files to nucleotides w/ ambiguity codes.");
  P.registerArg(aStringCmmd);
  P.registerArg(bStringCmmd);
  //P.registerArg(boolCmmd);

  P.parse();

  string aString = P.GetStringValueFor(aStringCmmd);
  string bString = P.GetStringValueFor(bStringCmmd);
  //bool bConvert = P.GetBoolValueFor(boolCmmd);
  


  vecDNAVector test;
  
  cout << "Reading file..." << endl;


  test.Read(aString);
  //for (int i=0; i<test.isize(); i++)
  test.DoProteins(true);
  
  cout << "done!" << endl;
  //test.ReverseComplement();

  test.Write(bString);

  return 0;

}
  
