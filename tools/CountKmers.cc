//#define FORCE_DEBUG

#ifndef FORCE_DEBUG
#define NDEBUG
#endif

#include <string.h>

#include "base/CommandLineParser.h"

#include "aligns/KmerAlignCore.h"
#include "src/DNAVector.h"
#include <math.h>

// #include "src/Papaya/KmerTable.h"
#include "aligns/KmerAlignCore.h"
#include "src/SequenceMatch.h"


class Kmer
{
public:
  Kmer() {
    count = 0;
  }

  
  bool Plus() {
    int n = count;

    for (int i=dna.isize()-1; i>=0; i--) {
      int x = n & 0x3;
      if (x == 0)
	dna[i] = 'A';
      if (x == 1)
	dna[i] = 'C';
      if (x == 2)
	dna[i] = 'G';
      if (x == 3)
	dna[i] = 'T';
      n = n >> 2;
    }
    
    count++;
    return true;
  }
  
  DNAVector dna;
  int count;
  
};



int main(int argc,char** argv)
{

  commandArg<string> aStringCmmd("-i","fasta file");
  commandArg<int> kCmmd("-k","kmer size", 12);
 
  commandLineParser P(argc,argv);
  P.SetDescription("Count k-mers up to 15-mers.");
  P.registerArg(aStringCmmd);
  P.registerArg(kCmmd);


  P.parse();

  string aString = P.GetStringValueFor(aStringCmmd);
  int k = P.GetIntValueFor(kCmmd);
 
  int i, j;

  vecDNAVectorStream seq;
  seq.ReadStream(aString);


  TranslateBasesToNumberExact trans;
  trans.SetSize(k); 
  

  
  svec<double> freq;
  freq.resize(trans.GetBoundValue(), 0);

  int n = 0;
  while(seq.Next()) {
    const DNAVector & d = seq.Get();

    for (i=0; i<d.isize()-k; i++) {
      int n = trans.BasesToNumber(d, i);
      if (n >= 0)       
	freq[n] += 1.;
      
    }
    n++;

    // if (n > 20)
    //break;
  }
  
  Kmer kmer;
  kmer.dna.resize(k);
  kmer.Plus();
  
  //cout << kmer.dna.isize() << endl;
  for (i=0; i<freq.isize(); i++) {
    for (int x=0; x<kmer.dna.isize(); x++)
      cout << kmer.dna[x];
    //cout << " Printed" << endl;

    
    int n = trans.BasesToNumber(kmer.dna, 0);
    cout << " ";
    cout << freq[n] << endl;
    kmer.Plus();
  }
  

  return 0;
   
}
