#include <string>
#include "base/CommandLineParser.h"
#include "base/FileParser.h"

#include "src/DNAVector.h"


class SNP
{
  
public:
  int pos;
  string name;

  bool operator < (const SNP & s) const {
    if (name != s.name)
      return name < s.name;
    return pos < s.pos;
  }

};


void LoadSNPs(svec<SNP> & snps, const string fileName)
{
  FlatFileParser parser;
  
  parser.Open(fileName);

  while (parser.ParseLine()) {
    if (parser.GetItemCount() == 0)
      continue;
    SNP s;
    s.name = parser.AsString(2);
    s.pos = parser.AsInt(3);
    snps.push_back(s);
  }
  Sort(snps);
}


int main( int argc, char** argv )
{

  commandArg<string> fileCmmd("-i","input alignment file");
  commandArg<string> snpCmmd("-s","list of SNP positions", "");
  commandLineParser P(argc,argv);
  P.SetDescription("Parses Cola/Satsuma alignments.");
  P.registerArg(fileCmmd);
  P.registerArg(snpCmmd);
  
  P.parse();
  
  string fileName = P.GetStringValueFor(fileCmmd);  
  string snpName = P.GetStringValueFor(snpCmmd);

  svec<SNP> snps;
  LoadSNPs(snps, snpName);
  
  //vecDNAVector dna;
  //dna.Read(fastaName);

  FlatFileParser parser;
  
  parser.Open(fileName);
  int i, j;


  while (parser.ParseLine()) {
    if (parser.GetItemCount() < 5)
      continue;
    
    string refChr = parser.AsString(0);
    string ref = parser.AsString(3);
    int refStart = parser.AsInt(2);
    parser.ParseLine();
    parser.ParseLine();
    string seq = parser.AsString(3);
    const string & chr = parser.AsString(0);
    const string & ori = parser.AsString(1);
    int start = parser.AsInt(2);
    int stop = parser.AsInt(4);
    //cout << parser.Line() << endl;
    if (stop < start) {
      int tmp = start;
      start = stop;
      stop = tmp;
    }

 
    //const DNAVector & d = dna(chr);
    int k = 0;
    int len = ref.size();
    i = start;
    for (j=0; j<len; j++) {
      //for (i=start; i<=stop; i++) {
      if (seq[k] == 'N') {
	k++;
	continue;
      }
      //int t = d.Qual(i);


      SNP test;
      test.pos = i;
      test.name = refChr;

      int idx = BinSearch(snps, test);

      if (idx >= 0) {
	cout << refChr << "\t" << i << "\t" << ref[k] << "\t" << seq[k] << endl;
      }
      
      if (seq[k] != '-')
	i++;
      

      if (ref[k] == 'N') {
	k++;
 	continue;
      }

      k++;
    }
  }

  return 0;
}
