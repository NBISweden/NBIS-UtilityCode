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
      freq[n] += 1.;
      
    }
    n++;

    if (n > 20)
      break;
  }

  for (i=0; i<freq.isize(); i++) {
    cout << freq[i] << endl;
  }
  

  return 0;
   
}
