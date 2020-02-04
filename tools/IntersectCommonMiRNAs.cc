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
  };
  
  int start;
  int stop;
  string chr;
  string ori;
  string name;
  string source;
  bool byName;
  string gene;
  map<string, string> annot;
  string feature; 
  
  bool operator < (const Annot & a) const {
    if (byName) {
      return name <= a.name;
    }
    if (chr != a.chr)
      return chr < a.chr;
    //if (ori != a.ori)
    //return ori < a.ori;
    return start < a.start;
  };

  bool merge(Annot& m, int limit)
  {
    // limit = min(limit, min(stop-start, m.stop-m.start));
    if(chr == m.chr && ori == m.ori &&     // same contig and strand
       feature == m.feature &&             // same feature!
       start <= m.stop && stop > m.start &&  // require overlap
       std::abs(start - m.start) < limit && std::abs(stop - m.stop) < limit)
      {
	start = min(start, m.start);
	stop = max(stop, m.stop);
	return(true);
      }
    return(false);
  };
  
  string print(const string& prefix, unsigned index, const bool& keepAnnot) const {
    ostringstream oss;
    oss << chr << "\tmapped\t"
	<< feature << "\t"
	<< start << "\t"
	<< stop << "\t.\t"
	<< ori << "\t.\t"
	<< "gene_id \"" << prefix << "MI" << index << "\"; "
	<< "transcript_id \"" << prefix << "TI" << index << "\"; ";
    if(keepAnnot)
      {
	oss  << "gene_name \"" << annot.find("gene_name")->second << "\"";
      // 	cerr << "\n\n" << name << endl;
      // 	StringParser s;
      // 	s.SetLine(annot, ";");
      // 	for (unsigned i=0; i<s.GetItemCount(); i++)
      // 	  {
      // 	    cerr << s.AsString(i) << endl;
      // 	    StringParser t;
      // 	    t.SetLine(s.AsString(i), " ");
      // 	    if(t.AsString(0) != "gene_id" &&
      // 	       t.AsString(0) != "transcript_id")
      // 	      {
      // 	    cerr << s.AsString(i) << endl;
      // 		oss << s.AsString(i) << "; ";
      // 	      }
      // 	  }
      }
    else
      {
	oss << "gene_name \"" << prefix << "MI" << index << "\"; ";
      }
    return(oss.str());
  };
};

void Load(svec<Annot> & annot, const string fileName,
	  const string & filter, const set<string>* features)
{
  FlatFileParser parser;
  
  parser.Open(fileName);

  while (parser.ParseLine()) {
    if (parser.GetItemCount() == 0)
      continue;
    if(features != NULL && features->find(parser.AsString(2)) == features->end())
      {
	continue;
      }
    if (filter != "" && parser.AsString(1) != filter)
      continue;
    Annot a;
    a.chr = parser.AsString(0);
    a.feature = parser.AsString(2);
    a.start = parser.AsInt(3);
    a.stop = parser.AsInt(4);
    a.source = fileName;
    a.ori = parser.AsString(6);
    a.name = parser.AsString(9);
    for(int i = 10; i < parser.GetItemCount(); i += 2)
      {
	a.annot[parser.AsString(i)] = parser.AsString(i+1);
      }
    // StringParser nn;
    // if (parser.GetItemCount() > 13) {
    //   nn.SetLine(parser.AsString(13), "Name=");
    //   if (nn.GetItemCount() == 3)
    // 	a.gene = nn.AsString(1);
    // }
    
    annot.push_back(a);
  }

}


int main( int argc, char** argv )
{
  commandArg<string> qCmmd("-q","query GTF file");
  commandArg<string> fCmmd("-f","gene call filter", "");
  commandArg<int> diffCmmd("-d","allowed diff limit for start and stop positions when merging adjacent miRNAs", 25);
  commandArg<string> prefixCmmd("-p","gene name prefix", "miRNA");
  commandArg<string> feStringCmmd("-F", "Only include these features in out ut gtf (comma-separated list)","");
  commandArg<bool> keepAnnotCmmd("-a", "Keep annotations (except gene_id and transcript_id)", false);
  commandLineParser P(argc,argv);
  P.SetDescription("Intersects two GTF files.");
  P.registerArg(qCmmd);
  P.registerArg(fCmmd);
  P.registerArg(diffCmmd);
  P.registerArg(prefixCmmd);
  P.registerArg(feStringCmmd);
  P.registerArg(keepAnnotCmmd);
 
  P.parse();
  
  string fileNameQ = P.GetStringValueFor(qCmmd);
  string filter = P.GetStringValueFor(fCmmd);
  int diffLimit = P.GetIntValueFor(diffCmmd);
  string prefix = P.GetStringValueFor(prefixCmmd);
  bool keepAnnot = P.GetBoolValueFor(keepAnnotCmmd);

  set<string> features;
  string featurestring  = P.GetStringValueFor(feStringCmmd);
  StringParser s;
  s.SetLine(featurestring, ",");
  for (unsigned i=0; i<s.GetItemCount(); i++) {
    features.insert(s.AsString(i));
  }
  
  svec<Annot> input;
  Load(input, fileNameQ, filter, &features);
  Sort(input);

  unsigned index = 0;  
  for(svec<Annot>::iterator i = input.begin(); i != input.end();)
    {
      svec<Annot>::iterator j = i;
      do
	{
	  j++;
	}
      while(j != input.end() && i->merge(*j, diffLimit));
      cout << i->print(prefix, index++, keepAnnot) <<endl;
      i = j;
    }

  return 0;
}
