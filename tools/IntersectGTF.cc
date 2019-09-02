#define FORCE_DEBUG

#include <string>
#include "base/CommandLineParser.h"
#include "base/FileParser.h"


class Annot
{
public:
  Annot() {
    start = stop = 0;
    byName = false;
    gene = "<unk>";
  }
  
  int start;
  int stop;
  string chr;
  string ori;
  string name;
  string source;
  bool byName;
  string gene;
  
  bool operator < (const Annot & a) const {
    if (byName) {
      return name <= a.name;
    }
    if (chr != a.chr)
      return chr < a.chr;
    //if (ori != a.ori)
    //return ori < a.ori;
    return start < a.start;

  }
};

void Load(svec<Annot> & annot, const string fileName, const string & filter)
{
  FlatFileParser parser;
  
  parser.Open(fileName);

  while (parser.ParseLine()) {
    if (parser.GetItemCount() == 0)
      continue;
    //if (parser.AsString(2) != "gene")
    //continue;
    if (filter != "" && parser.AsString(1) != filter)
      continue;
    Annot a;
    a.chr = parser.AsString(0);
    a.start = parser.AsInt(3);
    a.stop = parser.AsInt(4);
    a.source = fileName;
    a.ori = parser.AsString(6);
    a.name = parser.AsString(9);

    StringParser nn;
    //cout << parser.GetItemCount() << endl;

    //cout << parser.AsString(13) << endl;
    if (parser.GetItemCount() > 13) {
      nn.SetLine(parser.AsString(13), "Name=");
      if (nn.GetItemCount() == 3)
	a.gene = nn.AsString(1);
    }
    
    annot.push_back(a);
  }

}

void Print(const svec<Annot> & tmp)
{
  int i;

  svec<string> strain;
  for (i=0; i<tmp.isize(); i++) {
    StringParser p;
    p.SetLine(tmp[i].name, "0");
    strain.push_back(p.AsString(0));
  }
  UniqueSort(strain);
  if (strain.isize() == 1) {
    //cerr << "Skipping." << endl;
    return;
  }
  
  if (tmp.isize() > 0) {
    if (tmp.isize() == 1) {
      cout << "SINGLE" << endl;
    } else {
      if (tmp.isize() == 2)
	cout << "MATCH:" << endl;
      else
	cout << "MULT:" << endl;
    }
    for (i=0; i<tmp.isize(); i++) {
      cout << tmp[i].name << "\t" << tmp[i].chr << "\t" << tmp[i].start << "\t" << tmp[i].stop << "\t" << tmp[i].ori << "\t" << tmp[i].gene << endl;
    }
    cout << endl;
  }
}

bool OK(int a, int b)
{
  int n = a - b;
  if (n < 0)
    n = -n;
  if (n < 20)
    return true;
  return false;
}

bool Check(int start1, int stop1, int start2, int stop2)
{
  if (OK(start1, start2) && OK(stop1, stop2))
    return true;
  return false;
}


int main( int argc, char** argv )
{

  commandArg<string> tCmmd("-t","target GTF file");
  commandArg<string> qCmmd("-q","query GTF file");
  commandArg<string> fCmmd("-f","gene call filter", "");
  commandArg<int> lapCmmd("-l","minimum overlap", 50);
  commandLineParser P(argc,argv);
  P.SetDescription("Intersects two GTF files.");
  P.registerArg(tCmmd);
  P.registerArg(qCmmd);
  P.registerArg(fCmmd);
  P.registerArg(lapCmmd);
 
  P.parse();
  
  string fileNameT = P.GetStringValueFor(tCmmd);
  string fileNameQ = P.GetStringValueFor(qCmmd);
  string filter = P.GetStringValueFor(fCmmd);
  int lap = P.GetIntValueFor(lapCmmd);
 
  int i, j;

  svec<Annot> both;
  Load(both, fileNameT, filter);
  Load(both, fileNameQ, filter);

  
  Sort(both);

  //for (i=0; i<both.isize(); i++)
  // cout << both[i].chr <<  " " << both[i].start << " " << both[i].name << endl;
  //return 0;
  
 
  Annot last;
  svec<Annot> tmp;

  
  for (i=0; i<both.isize(); i++) {
    bool bPrint = false;
    //cout << i << " " << both[i].name << " " << both[i].chr << " " << both[i].start << " " << both[i].stop << " " << last.start << " " << last.stop << endl;
    
    if (both[i].chr == last.chr) {
      if (((last.start <= both[i].start && last.stop > both[i].start + lap)
	   ||(last.start < both[i].stop - lap && last.stop >= both[i].stop)
	   ||(last.start >= both[i].start && last.stop <= both[i].stop && last.stop - last.start > lap))
	  && Check(last.start, last.stop, both[i].start, both[i].stop)) {
	Annot tt = both[i];
	tt.byName = true;
	tmp.push_back(tt);
	//cout << "ADD" << endl;
      } else {
	Sort(tmp);
	Print(tmp);
	tmp.clear();
	Annot tt = both[i];
	tt.byName = true;
	tmp.push_back(tt);

      }
    } else {
      Sort(tmp);
      Print(tmp);
      tmp.clear();
      Annot tt = both[i];
      tt.byName = true;
      tmp.push_back(tt);
    }
    
    last = both[i];
  }
  Sort(tmp);
  Print(tmp);

  
  return 0;
}
