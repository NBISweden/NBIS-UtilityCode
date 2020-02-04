#include <string>

#include "base/CommandLineParser.h"
#include "src/DNAVector.h"


int main(int argc,char** argv)
{

  
  commandArg<string> aStringCmmd("-i","fasta file");
  commandArg<string> bStringCmmd("-g","GTF");
  //commandArg<bool> boolCmmd("-nucleotides","input file is in nucleotides", 0);
  commandLineParser P(argc,argv);
  P.SetDescription("Prints out genes as proteins.");
  P.registerArg(aStringCmmd);
  P.registerArg(bStringCmmd);
  //P.registerArg(boolCmmd);

  P.parse();

  string aString = P.GetStringValueFor(aStringCmmd);
  string fileName = P.GetStringValueFor(bStringCmmd);
  //bool bConvert = P.GetBoolValueFor(boolCmmd);
  


  vecDNAVector dna;
  
  cout << "Reading file..." << endl;

  int i, j;
  
  dna.Read(aString);
  //for (int i=0; i<test.isize(); i++)
  FlatFileParser parser;
  
  parser.Open(fileName);

  while (parser.ParseLine()) {
    if (parser.GetItemCount() == 0)
      continue;

    if (parser.AsString(2) != "gene")
      continue;
    
    int start = parser.AsInt(3)-1;
    int stop = parser.AsInt(4);

    int idx = dna.NameIndex(parser.AsString(0));
    if (idx < 0)
      cout << "Not found: " << parser.AsString(0) << endl;
    const DNAVector & d = dna(parser.AsString(0));

    //cout << start << " " << stop << " " << d.isize() << endl;
    DNAVector snip;
    snip.SetToSubOf(d, start, stop-start);
    if (parser.AsString(6) == "-")
      snip.ReverseComplement();

    StringParser p;
    p.SetLine(parser.AsString(11), ";");

    cout << ">" << p.AsString(1) << endl;

    for (j=0; j<snip.isize(); j++)
      cout << snip[j];
    cout << endl;
    
  }

  
  //test.DoProteins(true);
  
  //cout << "done!" << endl;
  //test.ReverseComplement();

  //test.Write(bString);

  return 0;

}
  
