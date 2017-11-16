#include <string>

#include "base/CommandLineParser.h"
#include "src/DNAVector.h"


int Frame(DNAVector & d, int shift, bool bAnnot, int min) 
{
  int i;
  int longest = 0;
  int count = 0;

  int longestStart = 0;
  int longestEnd = 0;
  //external start
  bool bOpen = true;
  for (i=shift; i<d.isize()-3; i+=3) {
    if (d[i] == 'A' && d[i+1] == 'T' && d[i+2] == 'G')
      bOpen = true;

    bool bClose = false;
    if (d[i] == 'T' && d[i+1] == 'A' && d[i+2] == 'A')
      bClose = true;
    if (d[i] == 'T' && d[i+1] == 'G' && d[i+2] == 'A')
      bClose = true;
    if (d[i] == 'T' && d[i+1] == 'A' && d[i+2] == 'G')
      bClose = true;

    
    if (bClose) {
      bOpen = false;
      if (count > longest) {
	longestEnd = i;
	longestStart = i-count;
	longest = count;
      }
      count = 0;

    }
    
    if (bOpen) 
      count += 3;

    /* if (bOpen && bAnnot) {
      d[i] = (char)tolower(d[i]);
      d[i+1] = (char)tolower(d[i+1]);
      d[i+2] = (char)tolower(d[i+2]);
      }*/
  }
  // external stop
  if (count > longest) {
    longestEnd = i;
    longestStart = i-count;
    longest = count;
  }

  if (longest >= min && bAnnot) {
    for (i=longestStart; i<longestEnd; i++)
      d[i] = (char)tolower(d[i]);
  }

  return longest;
}


int main(int argc,char** argv)
{

  
  commandArg<string> aStringCmmd("-i","fasta input file");
  commandArg<string> bStringCmmd("-o","output file");
  commandArg<int> minCmmd("-min","minimum ORF, discard all else", 0);
  commandArg<bool> nCmmd("-n","print out NON ORFs", false);
  commandArg<bool> fwCmmd("-fw","Forward only", false);
  commandLineParser P(argc,argv);
  P.SetDescription("Finds the longest open reading frame.");
  P.registerArg(aStringCmmd);
  P.registerArg(bStringCmmd);
  P.registerArg(minCmmd);
  P.registerArg(nCmmd);
  P.registerArg(fwCmmd);

  P.parse();

  string aString = P.GetStringValueFor(aStringCmmd);
  string bString = P.GetStringValueFor(bStringCmmd);
  int min = P.GetIntValueFor(minCmmd);
  bool bNon = P.GetBoolValueFor(nCmmd);
  bool bFW = P.GetBoolValueFor(fwCmmd);
  
  vecDNAVector dna;
  vecDNAVector out;
  
  //cout << "Reading file..." << endl;
  dna.Read(aString);
  //cout << "done!" << endl;
  //test.ReverseComplement();

  int i, j;

  int minFrame = 50;

  

  for (i=0; i<dna.isize(); i++) {
    DNAVector & d = dna[i];
    int shiftFW = -1;
    int maxLenFW = 0;
    int shiftRC = -1;
    int maxLenRC = 0;
    for (j=0; j<3; j++) {
      int l = Frame(d, j, false, min);
      if (l > maxLenFW) {
	maxLenFW = l;
	shiftFW = j;
      }
    }

    d.ReverseComplement();
    if (!bFW) {
      for (j=0; j<3; j++) {
	int l = Frame(d, j, false, min);
	if (l > maxLenRC) {
	  maxLenRC = l;
	  shiftRC = j;
	}
      }
    }
    if (maxLenFW < minFrame && maxLenRC < minFrame) {
      d.ReverseComplement();
      if (maxLenFW >= min ||  maxLenRC >= min)
	out.push_back(dna[i], dna.Name(i));
      continue;
    }
    if (maxLenFW > maxLenRC) {
      d.ReverseComplement();
      Frame(d, shiftFW, true, min);
      dna.SetName(i, dna.Name(i)+"+ORF");
    } else {
      Frame(d, shiftRC, true, min);      
      dna.SetName(i, dna.Name(i)+"-ORF");
    }
    
    if (bNon) {
      if (!(maxLenFW >= min ||  maxLenRC >= min))
	out.push_back(dna[i], dna.Name(i));
    } else {
      if (maxLenFW >= min ||  maxLenRC >= min)
	out.push_back(dna[i], dna.Name(i));
    }

  }


  out.Write(bString);
  for (i=0; i<out.isize(); i++) {
    const DNAVector & d = out[i];
    int k = 0;
    cout << out.Name(i);
    for (j=0; j<d.isize(); j++) {
      if (d[j] > 'Z') {
	if (k % 80 == 0)
	  cout << endl;
	cout << d[j];
	k++;
      }
    }
    cout << endl;
  }


  

  return 0;

}
  
