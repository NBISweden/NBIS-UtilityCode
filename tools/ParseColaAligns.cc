#include <string>
#include "base/CommandLineParser.h"
#include "base/FileParser.h"

#include "src/DNAVector.h"

#define EXON 1
#define UTR3 2
#define UTR5 4
#define CDS 8
#define REG_DOWN 16
#define REG_UP 32
#define INTRON 64

#define STRAND_MASK 128

class Annot
{
public:
  Annot() {
    start = stop = -1;
  }

  int start;
  int stop;
  string chr;
  string gene;

};

void MarkDNA(svec<Annot> & annot, vecDNAVector & dna, const string & fileName)
{
  int i, j;
  for (i=0; i<dna.isize(); i++) {
    DNAVector & dd = dna[i];
    for (j=0; j<dd.isize(); j++) {
      if (dd.Qual(j) != 0) {
	cout << "ERROR!!!" << endl;
      } 
    }
  }

  
  FlatFileParser parser;
  
  parser.Open(fileName);
  int total = 0;
  while (parser.ParseLine()) {
    if (parser.GetItemCount() < 3)
      continue;
    
    const string & t = parser.AsString(2);

    if (t == "gene") {
      Annot a;
      //cout << parser.Line() << endl;
      a.start = parser.AsInt(3);
      a.stop = parser.AsInt(4);
      a.chr = parser.AsString(0);
      StringParser pp;
      pp.SetLine(parser.AsString(8), ";");
      a.gene = pp.AsString(1);
      annot.push_back(a);
    }
    
    int n = 0;
    if (t == "exon")
      n = EXON;
    if (t == "CDS") {
      n = CDS;
      //cout << "FOUND CDS: " << parser.Line() << endl;
    }
    if (t == "five_prime_UTR")
      n = UTR5;
    if (t == "three_prime_UTR")
      n = UTR3;
    if (t == "upstream")
      n = REG_UP;
    if (t == "downstream")
      n = REG_DOWN;
    if (t == "intron")
      n = INTRON;

    //if (parser.AsString(5) == "-")
    //n = STRAND_MASK;
    int start = parser.AsInt(3)-1;
    int end = parser.AsInt(4)-1;
    if (t == "exon")
      total += end - start;
    DNAVector & d = dna(parser.AsString(0));
    for (i=start; i<=end; i++) {
      int x = (n | d.Qual(i));
      //if (x & CDS) {
      //cout << "ERROR: " << x << " " << n << " " << d.Qual(i) << endl;
      //}
      if (i >= 0 && i < d.isize())
	d.SetQual(i, x); 
      //if (d.Qual(i) & CDS) {
      //	cout << "ERROR: " << x << " " << n << " " << d.Qual(i) << endl;
      //}
    }
  }
  //cout << total << endl;
}

int main( int argc, char** argv )
{

  commandArg<string> fileCmmd("-i","input alignment file");
  commandArg<string> gffCmmd("-g","gff3 file", "");
  commandArg<string> fastaCmmd("-f","genome fasta file");
  commandLineParser P(argc,argv);
  P.SetDescription("Parses Cola/Satsuma alignments.");
  P.registerArg(fileCmmd);
  P.registerArg(gffCmmd);
  P.registerArg(fastaCmmd);
  
  P.parse();
  
  string fileName = P.GetStringValueFor(fileCmmd);  
  string gffName = P.GetStringValueFor(gffCmmd);
  string fastaName = P.GetStringValueFor(fastaCmmd);

  vecDNAVector dna;
  dna.Read(fastaName);

  svec<Annot> annot;
  
  if (gffName != "")
    MarkDNA(annot, dna, gffName);
  //cout << "Done" << endl;
  //comment. ???
  FlatFileParser parser;
  
  parser.Open(fileName);
  int i, j;

  svec<int> diffcounts;
  svec<int> indelcounts;

  diffcounts.resize(256, 0);
  indelcounts.resize(256, 0);

  double totalCDS = 0;
  double totalExon = 0;
  double totalUTR3 = 0;
  double totalUTR5 = 0;
  double totalNothing = 0;
  double totalUp = 0;
  double totalDown = 0;
  double totalIntron = 0;

  double diffCDS = 0;
  double diffExon = 0;
  double diffUTR3 = 0;
  double diffUTR5 = 0;
  double diffNothing = 0;
  double diffUp = 0;
  double diffDown = 0;
  double diffIntron = 0;
  
  double indelCDS = 0;
  double indelExon = 0;
  double indelUTR3 = 0;
  double indelUTR5 = 0;
  double indelNothing = 0;
  double indelUp = 0;
  double indelDown = 0;
  double indelIntron = 0;

  /*  for (i=0; i<dna.isize(); i++) {
    const DNAVector & d = dna[i];
    for (j=0; j<d.isize(); j++) {
      if (d.Qual(j) & EXON)
	totalExon += 1.;
      if (d.Qual(j) & CDS)
	totalCDS += 1.;
      if (d.Qual(j) & UTR3)
	totalUTR3 += 1.;
      if (d.Qual(j) & UTR5)
	totalUTR5 += 1.;
      if (d.Qual(j) == 0)
	totalNothing += 1.;
    }
    }*/
  double totalAnything = 0.;


  string lastChr;
  int lastPos = -1;
  bool bDelim = true;
  
  string lastR, lastS, lastOri;
  int inversions = 0;
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

    if (refChr == lastR && chr == lastS) {
      if (ori != lastOri) {
	inversions++;
	cout << "Inversion at " << refChr << "\t" << refStart << endl;
      }
    }

    lastS = chr;
    lastR = refChr;
    lastOri = ori;

    //const DNAVector & d = dna(chr);
    const DNAVector & d = dna(chr);
    int k = 0;
    int len = ref.size();
    i = start;
    //j = refStart;
    for (j=0; j<len; j++) {
      //for (i=start; i<=stop; i++) {
      if (seq[k] == 'N') {
	k++;
	continue;
      }
      int t = d.Qual(i);

      if (i >= d.isize()) {
	cout << "ERROR: " << i << " " << d.isize() << endl;
	cout << parser.Line() << endl;
      }

      if (seq[k] != '-')
	i++;
      //if (ref[k] != '-')
      //j++;
       

      if (ref[k] == 'N') {
	k++;
	totalAnything += 1.;
 	continue;
      }
      //cout << ref[k] << " " << seq[k] << endl;
      //IMPORTANT: Switch this when calculating stats for the ref!!!
      if (seq[k] != '-' && seq[k] != 'N') {
	//cout << "Good." << endl;
	if (t & EXON)
	  totalExon += 1.;
	if (t & CDS) {
	  //cout << "FOUND CDS: " << t << " " << d.isize() << " " << i << endl;
	  //cout << parser.Line() << endl;
	  totalCDS += 1.;
	}
	if (t & UTR3)
	  totalUTR3 += 1.;
	if (t & UTR5)
	  totalUTR5 += 1.;
	if (t & REG_UP)
	  totalUp += 1.;
	if (t & REG_DOWN)
	  totalDown += 1.;
	if (t & INTRON)
	  totalIntron += 1.;
	if (t == 0)
	  totalNothing += 1.;
	totalAnything += 1.;
      }

      if (ref[k] == '-' || seq[k] == '-') {
	indelcounts[t]++;
	if (t & EXON)
	  indelExon += 1.;
	if (t & CDS)
	  indelCDS += 1.;
	if (t & UTR3)
	  indelUTR3 += 1.;
	if (t & UTR5)
	  indelUTR5 += 1.;
	if (t & REG_UP)
	  indelUp += 1.;
	if (t & REG_DOWN)
	  indelDown += 1.;
	if (t & INTRON)
	  indelIntron += 1.;
	if (t == 0)
	  indelNothing += 1.;

      } else {
	if (ref[k] != seq[k]) {
	  //cout << "Different: " << ref[k] << " " << seq[k] << endl;
	  diffcounts[t]++;

	  if (i - lastPos < 80 && i > lastPos && chr == lastChr) {
	    if (bDelim)
	      cout << "---------------------------------------------> GENE" << endl;
	    bDelim = false;
	    cout << "SNP in " << chr << " " << i << " " << refChr << " " << j << " ";
	    cout << ref[k] << " " << seq[k];

	    for (int m=0; m<annot.isize(); m++) {
	      //cout << annot[m].gene << endl;
	      if (annot[m].start < i && i < annot[m].stop && annot[m].chr == chr)
		cout << " " << annot[m].gene;
	    }
	    
	    cout << endl;
	  } else {
	    bDelim = true;
	  }

	  lastPos = i;
	  lastChr = chr;
	  
	  if (t & EXON)
	    diffExon += 1.;
	  if (t & CDS) {
	    diffCDS += 1.;
	    
	    
	  }

	  if (t & UTR3)
	    diffUTR3 += 1.;
	  if (t & UTR5)
	    diffUTR5 += 1.;
	  if (t & REG_UP)
	    diffUp += 1.;
	  if (t & REG_DOWN)
	    diffDown += 1.;
	  if (t & INTRON)
	    diffIntron += 1.;
	  if (t == 0)
	    diffNothing += 1.;
	}
      }
      k++;
    }
  }
  //cout << totalNothing << " -> " << diffNothing << endl;
  double all = 0.;
  for (i=0; i<dna.isize(); i++) {
    all += dna[i].isize();
  }
  cout << "Total aligned (of query): " << 100*totalAnything/all << " %" << endl << endl;

  cout << "Inversions: " << inversions << endl;
  cout << "Neutral: " << endl;
  cout << "total: " << totalNothing << " diff: " << diffNothing/totalNothing << " " << diffNothing << " indel: " << indelNothing/totalNothing << " " << indelNothing << endl; 
 
  cout << "Exon: " << endl;
  cout << "total: " << totalExon << " diff: " << diffExon/totalExon << " " << diffExon << " indel: " << indelExon/totalExon << " " << indelExon << endl; 

  cout << "CDS: " << endl;
  cout << "total: " << totalCDS << " diff: " << diffCDS/totalCDS << " " << diffCDS << " indel: " << indelCDS/totalCDS << " " << indelCDS << endl; 
 
  cout << "5' UTR: " << endl;
  cout << "total: " << totalUTR5 << " diff: " << diffUTR5/totalUTR5 << " indel: " << indelUTR5/totalUTR5 << endl; 
 
  cout << "3' UTR: " << endl;
  cout << "total: " << totalUTR3 << " diff: " << diffUTR3/totalUTR3 << " indel: " << indelUTR3/totalUTR3 << endl; 
 
  cout << "upstream: " << endl;
  cout << "total: " << totalUp << " diff: " << diffUp/totalUp << " indel: " << indelUp/totalUp << endl; 

  cout << "downstream: " << endl;
  cout << "total: " << totalDown << " diff: " << diffDown/totalDown << " indel: " << indelDown/totalDown << endl; 
  
  cout << "intron: " << endl;
  cout << "total: " << totalIntron << " diff: " << diffIntron/totalIntron << " indel: " << indelIntron/totalIntron << endl; 
 
    

  return 0;
}
