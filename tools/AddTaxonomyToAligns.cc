#include <string>
#include "base/CommandLineParser.h"
#include "base/FileParser.h"
#include "src/DNAVector.h"

void Parse(string & spec, const string & in) 
{
  StringParser a, b;
  a.SetLine(in, "_[");
  //a.SetLine(in, "Tax=");
  if (a.GetItemCount() < 2)
    return;
  //cout << a.AsString(0) << endl;
  string c = a.AsString(1);
  b.SetLine(c, "_");
  spec = b.AsString(0);
}

class Species
{
public:
  Species() {}

  void Read(FlatFileParser & parser) {
    //cout << parser.Line() << endl;
    m_name = parser.AsString(1);
    int i = 3;
    while (strstr(parser.AsString(i).c_str(), ";") == NULL && i < parser.GetItemCount()) {
      //cout << parser.AsString(i) << endl;
      i++;
    }
    if (i >= parser.GetItemCount()-1) {
      m_name = "";
      return;
    }
    m_tax = parser.AsString(i);
    m_tax += " ";
    m_tax += parser.AsString(i+1);

    for (int j=i+2; j<parser.GetItemCount(); j++) {
      m_tax += " ";
      m_tax += parser.AsString(j);
     
      if (j > i+4)
	break;
    }
    /*
    if (parser.GetItemCount() > 2) {
      m_tax += " ";
      m_tax += parser.AsString(i+2);    
      }*/
    /*
    if (parser.GetItemCount()+i > 3) {
      m_tax += " ";
      m_tax += parser.AsString(i+3);    
    }
    if (parser.GetItemCount()+i > 4) {
      m_tax += " ";
      m_tax += parser.AsString(i+4);    
      }*/


    //cout << m_tax << endl;
    
  }

  const string & Name() {return m_name;}
  const string & Tax()  {return m_tax;}

private:
  string m_name;
  string m_tax;
};

int main( int argc, char** argv )
{

  commandArg<string> fileCmmd("-i","input file");
  commandArg<string> taxCmmd("-t","taxonomy file");
  commandLineParser P(argc,argv);
  P.SetDescription("Adds taxonomic info to alignments.");
  P.registerArg(fileCmmd);
  P.registerArg(taxCmmd);
  
  P.parse();
  
  string fileName = P.GetStringValueFor(fileCmmd);
  string taxName = P.GetStringValueFor(taxCmmd);
  

  svec<Species> spec;
  FlatFileParser parser1;
  parser1.Open(taxName);

  while (parser1.ParseLine()) {
    if (parser1.GetItemCount() == 0)
      continue;
    Species s;
    if (parser1.GetItemCount() > 9) {
      s.Read(parser1);
      spec.push_back(s);
    }
  }
  int i;

  FlatFileParser parser;
  parser.Open(fileName);

  while (parser.ParseLine()) {
    if (parser.GetItemCount() == 0)
      continue;
  
    for (i=0; i<parser.GetItemCount(); i++) {
      const char * p = parser.AsString(i).c_str();
      if (p[0] == '[') {
	char tmp[256];
	strcpy(tmp, &p[1]);
	string tax;
	for (int j=0; j<spec.isize(); j++) {
	  if (spec[j].Name() == tmp) {
	    tax = spec[j].Tax();
	    break;
	  }
	}
	cout << parser.Line() << "\t" << tax << endl;
      }
    }
  }


  return 0;
}
