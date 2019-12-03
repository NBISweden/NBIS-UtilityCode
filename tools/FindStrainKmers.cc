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

  /*commandArg<string> aStringCmmd("-i","fasta file");
  commandArg<string> oStringCmmd("-o","fasta output");
  commandArg<int> kCmmd("-k","kmer size", 12);
 
  commandLineParser P(argc,argv);
  P.SetDescription("Eliminates redundant sequences.");
  P.registerArg(aStringCmmd);
  P.registerArg(oStringCmmd);
  P.registerArg(kCmmd);


  P.parse();

  string aString = P.GetStringValueFor(aStringCmmd);
  string oString = P.GetStringValueFor(oStringCmmd);
  int k = P.GetIntValueFor(kCmmd);*/

  int k = 12;
 
  int i, j;

  vecDNAVector lo1, lo2, lo3;
  vecDNAVector hi1, hi2, hi3;
  
  lo1.Read("SMI416_hgap.fasta");
  lo2.Read("SMI798_hgap.fasta");
  lo3.Read("SMI706_hgap.fasta");

  hi1.Read("SMI588_hgap.fasta");
  hi2.Read("SMI596_hgap.fasta");
  hi3.Read("SMI828_hgap.fasta");

  
  vecDNAVector out_lo, out_hi;

  


  KmerAlignCore<KmerAlignCoreRecordWithScore> low;
  KmerAlignCore<KmerAlignCoreRecordWithScore> high;
  
  int num12 = 4;
  low.SetNumTables(num12);
  high.SetNumTables(num12);
  
  TranslateBasesToNumberExact trans_lo;
  trans_lo.SetSize(k); // Use 15-mers
  TranslateBasesToNumberExact trans_hi;
  trans_hi.SetSize(k); // Use 15-mers
  low.SetTranslator(&trans_lo);
  high.SetTranslator(&trans_hi);
  
  cout << "Adding targets" << endl;
  /* low.AddData(lo1, false, 1);
  low.AddData(lo2, false, 1);
  low.AddData(lo3, false, 1);
  high.AddData(hi1, false, 1);
  high.AddData(hi2, false, 1);
  high.AddData(hi3, false, 1);
  */
  
  low.AddData(hi1, false, 1);
  low.AddData(hi2, false, 1);
  low.AddData(hi3, false, 1);
  high.AddData(lo1, false, 1);
  high.AddData(lo2, false, 1);
  high.AddData(lo3, false, 1);
 
  low.SortAll();
  high.SortAll();
  cout << "done" << endl;
  int nn = 0;
  for (i=0; i<lo1.lsize(); i++) {
 

    DNAVector d = lo1[i];
    DNAVector r = d;
    r.ReverseComplement();

    int nF = 0;
    int nR = 0;

    cout << "Test seq " << lo1.Name(i) << endl;
    int last = -1;
    for (j=0; j<=d.lsize()-k*num12; j++) {
      DNAVector subF, subR;
      subF.SetToSubOf(d, j, k*num12);
      subR.SetToSubOf(r, j, k*num12);
      svec<KmerAlignCoreRecordWithScore> matchesF_lo;
      svec<KmerAlignCoreRecordWithScore> matchesR_lo;
      svec<KmerAlignCoreRecordWithScore> matchesF_hi;
      svec<KmerAlignCoreRecordWithScore> matchesR_hi;

      low.GetMatches(matchesF_lo, subF);
      low.GetMatches(matchesR_lo, subR);
      
      high.GetMatches(matchesF_hi, subF);
      high.GetMatches(matchesR_hi, subR);

      if (matchesF_lo.isize() + matchesR_lo.isize() == 3 &&
	  matchesF_hi.isize() + matchesR_hi.isize() == 0) {

	if (j == last+1) {
	  DNAVector & dd = out_lo[out_lo.isize()-1];
	  dd.resize(dd.isize()+1);
	  dd[dd.isize()-1] = subF[subF.isize()-1];
	} else {
	  nn++;
	  string name = ">kmer_" + Stringify(nn) + "_" + Stringify(out_lo.isize());
	  out_lo.push_back(subF, name);
	}
  	last = j;

     }
      
    }

    
     
  }
  cout << "Found: " << out_lo.isize() << endl;
  out_lo.Write("low_exclusive.fasta");

  //==============================


  nn = 0;
  for (i=0; i<hi1.lsize(); i++) {
 

    DNAVector d = hi1[i];
    DNAVector r = d;
    r.ReverseComplement();

    int nF = 0;
    int nR = 0;

    cout << "Test seq " << hi1.Name(i) << endl;
    int last = -1;
    for (j=0; j<=d.lsize()-k*num12; j++) {
      DNAVector subF, subR;
      subF.SetToSubOf(d, j, k*num12);
      subR.SetToSubOf(r, j, k*num12);
      svec<KmerAlignCoreRecordWithScore> matchesF_lo;
      svec<KmerAlignCoreRecordWithScore> matchesR_lo;
      svec<KmerAlignCoreRecordWithScore> matchesF_hi;
      svec<KmerAlignCoreRecordWithScore> matchesR_hi;

      low.GetMatches(matchesF_lo, subF);
      low.GetMatches(matchesR_lo, subR);
      
      high.GetMatches(matchesF_hi, subF);
      high.GetMatches(matchesR_hi, subR);

      if (matchesF_lo.isize() + matchesR_lo.isize() == 0 &&
	  matchesF_hi.isize() + matchesR_hi.isize() == 3) {
	//cout << "Found high exclusive!" << endl;
	if (j == last+1) {

	  DNAVector & dd = out_hi[out_hi.isize()-1];
	  dd.resize(dd.isize()+1);
	  dd[dd.isize()-1] = subF[subF.isize()-1];

	} else {
	  nn++;
	  string name = ">kmer_" + Stringify(nn) + "_" + Stringify(out_lo.isize());
	  out_hi.push_back(subF, name);
	}
  	last = j;
	
      }

    }
     
  } 

  cout << "Found: " << out_hi.isize() << endl;

  
  out_hi.Write("high_exclusive.fasta");
  return 0;

}
