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



class SeqBySize
{
public:
  SeqBySize() {
    m_index = m_size = -1;
  }

  void Set(int index, int size) {
    m_index = index;
    m_size = size;
  }

  int GetIndex() const {return m_index;}
  int GetSize() const {return m_size;}

  bool operator < (const SeqBySize & s) const {
    return m_size < s.m_size;
  }

private:
  int m_index;
  int m_size;
};


//========================================================================
//========================================================================
//========================================================================



int main(int argc,char** argv)
{

  commandArg<string> iStringCmmd("-i","fasta files (sequence unique to those)");
  commandArg<string> aStringCmmd("-a","outgroup (sequence no present in these)");
  commandArg<string> oStringCmmd("-o","fasta output");
  commandArg<int> kCmmd("-k","kmer size", 12);
 
  commandLineParser P(argc,argv);
  P.SetDescription("Eliminates redundant sequences.");
  P.registerArg(iStringCmmd);
  P.registerArg(aStringCmmd);
  P.registerArg(oStringCmmd);
  P.registerArg(kCmmd);


  P.parse();

  string iString = P.GetStringValueFor(iStringCmmd);
  string aString = P.GetStringValueFor(aStringCmmd);
  string oString = P.GetStringValueFor(oStringCmmd);
  int k = P.GetIntValueFor(kCmmd);

  
 
  int i, j;

  svec<vecDNAVector> in, ref;

  StringParser ip;
  ip.SetLine(iString, ",");
  StringParser ap;
  ap.SetLine(aString, ",");

  in.resize(ip.GetItemCount());
  ref.resize(ap.GetItemCount());
  
  for (i=0; i<ip.GetItemCount(); i++)
    in[i].Read(ip.AsString(i));
  
  for (i=0; i<ap.GetItemCount(); i++) {
    ref[i].Read(ap.AsString(i));
  }
  
 
  vecDNAVector out;

  


  KmerAlignCore<KmerAlignCoreRecordWithScore> core;
  KmerAlignCore<KmerAlignCoreRecordWithScore> self;
 
  
  int num12 = 4;
  core.SetNumTables(num12);
  self.SetNumTables(num12);
  TranslateBasesToNumberExact trans_c;
  trans_c.SetSize(k); // Use 15-mers
  core.SetTranslator(&trans_c);

  TranslateBasesToNumberExact trans_s;
  trans_s.SetSize(k); // Use 15-mers
  self.SetTranslator(&trans_s);

  
  cout << "Adding targets" << endl;
  /* low.AddData(lo1, false, 1);
  low.AddData(lo2, false, 1);
  low.AddData(lo3, false, 1);
  high.AddData(hi1, false, 1);
  high.AddData(hi2, false, 1);
  high.AddData(hi3, false, 1);
  */

  for (i=0; i<ref.isize(); i++)
    core.AddData(ref[i], false, 1);
  for (i=0; i<in.isize(); i++)
    self.AddData(in[i], false, 1);

  core.SortAll();
  cout << "done" << endl;
  int nn = 0;
  for (i=0; i<in[0].lsize(); i++) {
 

    const DNAVector & d = (in[0])[i];

    int nF = 0;
    int nR = 0;

    cout << "Test seq " << in[0].Name(i) << endl;
    int last = -1;
    for (j=0; j<=d.lsize()-k*num12; j++) {
      DNAVector subF, subR;
      subF.SetToSubOf(d, j, k*num12);
      subR.SetToSubOf(d, j, k*num12);
      subR.ReverseComplement();
      
      svec<KmerAlignCoreRecordWithScore> matchesF_lo;
      svec<KmerAlignCoreRecordWithScore> matchesR_lo;
      svec<KmerAlignCoreRecordWithScore> matchesF_hi;
      svec<KmerAlignCoreRecordWithScore> matchesR_hi;

      self.GetMatches(matchesF_lo, subF);
      self.GetMatches(matchesR_lo, subR);
      
      core.GetMatches(matchesF_hi, subF);
      core.GetMatches(matchesR_hi, subR);

      if (matchesF_lo.isize() + matchesR_lo.isize() == in.isize() &&
	  matchesF_hi.isize() + matchesR_hi.isize() == 0) {

	if (j == last+1) {
	  DNAVector & dd = out[out.isize()-1];
	  dd.resize(dd.isize()+1);
	  dd[dd.isize()-1] = subF[subF.isize()-1];
	} else {
	  nn++;
	  string name = ">kmer_" + Stringify(nn) + "_" + Stringify(out.isize());
	  out.push_back(subF, name);
	}
  	last = j;

     }
      
    }

    
     
  }
  cout << "Found: " << out.isize() << endl;
  out.Write(oString);

   return 0;

}
