#include <string>
#include <sstream>
#include "base/CommandLineParser.h"
#include "base/FileParser.h"


class Connect
{
public:
  svec<string> c;
};

class Index
{
public:
  svec<int> link;
};


void Load(svec<Connect> & a, svec<string> & all, svec<string> & gene,  const string & fileName)
{
  FlatFileParser parser;
  
  parser.Open(fileName);

  Connect conn;

  svec<string> gene_tmp;
  svec<string> keep;
  int i;
  
  while (parser.ParseLine()) {
    if (parser.GetItemCount() == 0)
      continue;
    if (parser.AsString(0) == "SINGLE" || parser.AsString(0) == "MATCH:" || parser.AsString(0) == "MULT:") {
      a.push_back(conn);
      conn.c.clear();
      continue;
    }
    conn.c.push_back(parser.AsString(0));
        
    
    gene_tmp.push_back(parser.AsString(5));
    all.push_back(parser.AsString(0));
    keep.push_back(parser.AsString(0));
  }
  
  a.push_back(conn);
  
  //keep = all;

  //cout << "Before: " << all.isize() << endl;
  
  UniqueSort(all);
  gene.resize(all.isize());

  //cout << keep.isize() << " " << gene_tmp.isize() << " " << all.isize() << endl;
  
  for (i=0; i<keep.isize(); i++) {
    //cout << i << endl;
    int index = BinSearch(all, keep[i]);
    
    if (gene_tmp[i] != "<unk>" && strstr(gene[index].c_str(), gene_tmp[i].c_str()) == NULL) {
      if (gene[index] != "")
	gene[index] += "|";
      gene[index] += gene_tmp[i];
    }
  }
  
}

string Scrub(const string & in)
{
  StringParser p;
  p.SetLine(in, "\"");
  string s = p.AsString(0);
  return s;
}

int main( int argc, char** argv )
{

  commandArg<string> iCmmd("-i","comma-separated list of input files");
  commandArg<string> pCmmd("-p","gene prefix", "CANPMUL");
  commandLineParser P(argc,argv);
  P.SetDescription("Intersects two GTF files.");
  P.registerArg(iCmmd);
  P.registerArg(pCmmd);
 
  P.parse();
  
  string in = P.GetStringValueFor(iCmmd);
  string gene_prefix = P.GetStringValueFor(pCmmd);

  int i, j;

  svec<Connect> conn;
  svec<string> all;
  svec<string> gene;

  StringParser s;
  s.SetLine(in, ",");

  for (i=0; i<s.GetItemCount(); i++) {
    Load(conn, all, gene, s.AsString(i));
  }

  
  svec<Index> idx;
  idx.resize(all.isize());

  for (i=0; i<conn.isize(); i++) {
    svec<string> one;
    if (conn[i].c.isize() == 0)
      continue;
    //if (i % 100 == 0)
    //cout << i << endl;
    int last = BinSearch(all, conn[i].c[0]);
    for (j=1; j<conn[i].c.isize(); j++) {
      int index = BinSearch(all, conn[i].c[j]);
      //cout << last << " " << index << " -> " << conn[i].c[j] << endl;
      if (last == index)
	{
	  cerr << "ERROR!!! repeated entry: " << last << " " << index << conn[i].c[j] << endl;
	  // exit(-1);
	}
      
      idx[last].link.push_back(index);
      last = index;
    }
  }

  svec<string> prefix;
  for (i=0; i<all.isize(); i++) {
    char tmp[256];
    strcpy(tmp, all[i].c_str());
    tmp[10] = 0;
    prefix.push_back(tmp);
  }
  UniqueSort(prefix);

  cout << "Unique";
  for (i=0; i<prefix.isize(); i++)
    cout << "\t" << Scrub(prefix[i]);
  cout << "\tSymbol" << endl;
  
  //cout << "Expanding..." << endl;
  int k = 1;
  for (i=0; i<all.isize(); i++) {
    //cout << i << endl;
    svec<string> line;
    svec<string> matrix;
    matrix.resize(prefix.isize());
    
    string gene_name = "<unknown>";
    if (all[i] == "")
      continue;
    
    int next = i;
    line.push_back(all[i]);
    gene_name = gene[i];
    all[i] = "";
    int ctr = 0;
    do {      
      for (j=0; j<idx[next].link.isize(); j++) {
	if (all[idx[next].link[j]] != "") {
	  line.push_back(all[idx[next].link[j]]);
	}
	all[idx[next].link[j]] = "";
      }

      //cout << "Next " << next << endl;
      //cout << "  " << idx[next].link.isize() << endl;
	
      if (idx[next].link.isize() == 0)
	next = -1;
      else
	next = idx[next].link[0];
      
      //cout << "? " << next << endl;
      //ctr++;
    } while (next != -1);
    //cout << "Sort" << endl;
    UniqueSort(line);
    //cout << "Printing" << endl;

    string u = gene_prefix;
    string num = Stringify(k);
    k++;
    for (j=0; j<10-(int)num.length(); j++)
      u += "0";
    u += num;
    if (gene_name == "")
      gene_name = "<unknown>";
    
    cout << u;

    //int cnt = 0;

    //cout << "Line " << line.isize() << " Matrix: " << matrix.isize() << endl;
    for (j=0; j<line.isize(); j++) {
      for (int x=0; x<prefix.isize(); x++) {
       
	if (strstr(line[j].c_str(), prefix[x].c_str()) != NULL) {
	  if (matrix[x] != "")
	    matrix[x] += ",";
	  matrix[x] += Scrub(line[j]);
	}
      }
    }
    for (int y=0; y<matrix.isize(); y++) {
      if (matrix[y] == "")
	cout << "\t.";
      else
	cout << "\t" << matrix[y];
    }
        
    cout << "\t" << gene_name << endl;
  }

  
  return 0;
}
