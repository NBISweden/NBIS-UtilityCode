#define FORCE_DEBUG

#include <string>
#include "base/CommandLineParser.h"
#include "base/FileParser.h"
#include "src/DNAVector.h"
#include "base/StringUtil.h"

class Coords
{
public:
  Coords() {pos = -1;}
  
  string scaff;
  int pos;

  bool operator < (const Coords & c) const {
    return pos > c.pos;
  }
};

void LoadCoords(svec<Coords> & coords, const string & fileName)
{
  FlatFileParser parser;
  
  parser.Open(fileName);

  while (parser.ParseLine()) {
    //cout << "Line " << parser.Line() << endl;
    if (parser.GetItemCount() == 0)
      continue;
  
    Coords c;
    c.scaff = parser.AsString(0);
    c.pos = parser.AsInt(1);
    //cout << "Push" << endl;
    coords.push_back(c);
    //cout << "Coords: " << coords.isize() << endl;
  }
}

class Annot
{
public:
  Annot() {
    m_start = -1;
    m_stop = -1;
  }

  void Read(FlatFileParser & f) {
    if (f.Line()[0] == '#')
      return;
    //cout << f.Line() << endl;
    m_start = f.AsInt(3);
    m_stop = f.AsInt(4);
    m_scaff = f.AsString(0);

    StringParser p;
    p.SetLine(f.AsString(8), ";");
    if (p.GetItemCount() > 1)
      m_id = p.AsString(1);
    else
      m_id = p.AsString(0);
       
    for (int i=0; i<f.GetItemCount(); i++)
      m_all.push_back(f.AsString(i));
    //cout << "done" << endl;
  }

  const int & Start() const {return m_start;}
  const int & Stop() const {return  m_stop;}
  const string & Scaff() const {return m_scaff;}
  const string & ID() const {return m_id;}

  int & Start() {return m_start;}
  int & Stop()  {return  m_stop;}
  string & Scaff()  {return m_scaff;}

  void Print() const {
    if (m_scaff == "")
      return;
    for (int i=0; i<m_all.isize(); i++) {
      if (i == 0) {
	cout << m_scaff;
	continue;
      }
      if (i == 3) {
	cout << "\t" << m_start;
	continue;
      }
      if (i == 4) {
	cout << "\t" << m_stop;
	continue;
      }
      if (i > 8)
	cout << " " << m_all[i];
      else
	cout << "\t" << m_all[i];
    }
    cout << endl;
  }
   
private:
  int m_start;
  int m_stop;
  string m_scaff;
  svec<string> m_all;
  string m_id;
};


int main( int argc, char** argv )
{

  commandArg<string> fileCmmd("-i","input file");
  commandArg<string> cCmmd("-c","coordinates");
  commandArg<string> aCmmd("-a","annotation GFF file");
  commandLineParser P(argc,argv);
  P.SetDescription("Splits a fasta file at given coordinates and translates an annotation.");
  P.registerArg(fileCmmd);
  P.registerArg(cCmmd);
  P.registerArg(aCmmd);

  P.parse();
  
  string fileName = P.GetStringValueFor(fileCmmd);
  string coordsName = P.GetStringValueFor(cCmmd);
  string gff = P.GetStringValueFor(aCmmd);

  //svec<string> scaff;
  //svec<int> pos;

  svec<Coords> coords;
  LoadCoords(coords, coordsName);

  Sort(coords);
  //cout << coords[0].pos << endl;
  //cout << coords[1].pos << endl;

  vecDNAVector dna;
  dna.Read(fileName);
  
  //comment. ???
  FlatFileParser parser;
  
  parser.Open(gff);
  
  svec<Annot> annot;
  //cout << "Start" << endl;
  while (parser.ParseLine()) {
    //cout << parser.Line() << endl;
    if (parser.GetItemCount() == 0)
      continue;
    Annot a;
    a.Read(parser);
    annot.push_back(a);
  }
  //cout << "Done loading gff" << endl;
  int i, j;

  //cout << "Starting" << endl;

  svec<string> remove;
  for (i=0; i<coords.isize(); i++) {
    int index = dna.NameIndex(coords[i].scaff);
    if (index < 0) {
      cout << "Sequence not found: " << coords[i].scaff << endl;
      return -1;
    }
    DNAVector d, a;
    if (coords[i].pos >= dna[index].isize()) {
      cout << "ERROR, position=" << coords[i].pos << ", length=" << dna[index].isize() << endl;
      return -1;
    }
    d.SetToSubOf(dna[index], coords[i].pos, dna[index].isize()-coords[i].pos);
    //a.SetToSubOf(dna[index], 0, coords[i].pos);

    dna[index].resize(coords[i].pos);
    //dna[index] = a;
    
    string name = dna.NameClean(index);
    name += "_split_" + Stringify(coords[i].pos);
    string nameX = ">" + name;
    dna.push_back(d, nameX);
    //cout << "Split " << name << endl;

    for (j=0; j<annot.isize(); j++) {
      if (annot[j].Scaff() != coords[i].scaff)
	continue;

      if (annot[j].Start() < coords[i].pos && annot[j].Start() > coords[i].pos) {
	remove.push_back(annot[j].ID());
      }
      if (annot[j].Start() >= coords[i].pos) {	
	annot[j].Start() -= coords[i].pos;
 	annot[j].Stop() -= coords[i].pos;
	annot[j].Scaff() = name;
      }
    }
    
  }

  UniqueSort(remove);

  cout << "##gff-version 3" << endl;

  
  for (j=0; j<annot.isize(); j++) {
    bool b = false;
    for (i=0; i<remove.isize(); i++) {
      if (annot[j].ID() == remove[i])
	b = true;
    }
    if (!b) {
      annot[j].Print();
    }
  }

  dna.Write(fileName + ".split.fasta");

  
  
  return 0;
}
