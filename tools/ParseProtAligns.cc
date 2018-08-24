#include <string>
#include "base/CommandLineParser.h"
#include "base/FileParser.h"



class OneAlign
{
public:
  OneAlign() {
    m_ident = 0;
    m_pval = 1.;
  }

  OneAlign(const string & seq,
	   const string & match,
	   double ident,
	   double pval) {
    m_seq = seq;
    m_match = match;
    m_ident = ident;
    m_pval = pval;
  }

  OneAlign(const string & line) {
    m_all = line;
    StringParser s;
    s.SetLine(line, "\t");
    m_seq = s.AsString(0);
    m_match = s.AsString(1);
    m_ident = s.AsFloat(2);
    m_pval = s.AsFloat(10);
  }



  const string & GetSeq() const {return m_seq;}
  const string & GetMatch() const {return m_match;}
  double GetIdent() const {return m_ident;}
  double GetPVal() const {return m_pval;}
  const string & GetAll() const {return m_all;}

private:
  string m_seq;
  string m_match;
  double m_ident;
  double m_pval;
  string m_all;
};


class HitList
{
public:
  HitList() {}

  void clear() {
    m_hits.clear();
  }

  bool Add(const string & line) {
    OneAlign one(line);
    if (m_hits.isize() == 0 || m_hits[0].GetSeq() == one.GetSeq()) {
      m_hits.push_back(one);
      return true;
    } 
    return false;
  }

  void PrintBest() const;

private:
  svec<OneAlign> m_hits;
};

// ==============================================================
void HitList::PrintBest() const
{
  // Print first match
  //cout << m_hits[0].GetMatch() << endl;
  cout << m_hits[0].GetAll() << endl;

}
// ==============================================================

int main( int argc, char** argv )
{

  commandArg<string> fileCmmd("-i","input file");
  commandLineParser P(argc,argv);
  P.SetDescription("Extracts information from blastp/blastx style alignments.");
  P.registerArg(fileCmmd);
 
  P.parse();
  
  string fileName = P.GetStringValueFor(fileCmmd);
 

  //comment. ???
  FlatFileParser parser;
  
  parser.Open(fileName);
  HitList hits;
  while (parser.ParseLine()) {
    if (parser.GetItemCount() == 0)
      continue;
    
    bool b = hits.Add(parser.Line());
    
    if (!b) { 
      hits.PrintBest();
      hits.clear();
      hits.Add(parser.Line());
    }
    
    
  }

  hits.PrintBest();
 

  return 0;
}
