#include <string>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include "base/CommandLineParser.h"
#include "base/FileParser.h"
#include <api/BamReader.h>

class MiRNA
{
public:
  MiRNA()
    : ctg(""),
      strd(""),
      start(0),
      stop(0),
      cnts()
  {};

  MiRNA(string& contig, string& strand, int& start, int& stop, vector<string>& bams)
    : ctg(contig),
      strd(strand),
      start(start),
      stop(stop),
      cnts()
  {
    for(unsigned i = 0; i < bams.size(); i++)
      {
	cnts[bams[i]] = 0;
      }
  };

  ~MiRNA()
  {};
  
  MiRNA(const MiRNA& m)
    : ctg(m.ctg),
      strd(m.strd),
      start(m.start),
      stop(m.stop),
      cnts(m.cnts)
  {};

  MiRNA& operator=(const MiRNA& m)
  {
    if(&m != this)
      {
	ctg = m.ctg;
	strd = m.strd;
	start = m.start;
	stop = m.stop;
	cnts = m.cnts;
      }
    return(*this);
  };
  
  // bool operator<(const MiRNA& m) const
  // {
  //   return(
  // 	   ctg < m.ctg ||
  // 	   strd < m.strd ||
  // 	   (strd == m.strd  &&
  // 	    (start < m.start ||
  // 	     stop < m.stop)
  // 	    )
  // 	   );
  // };
  
  void addCount(const string& file, const double& n)
  {
    cnts[file] += n;
  };

  bool passCoverage(const double& coverageCutoff)
  {
    for(map<string, double>::const_iterator i = cnts.begin();
	i!= cnts.end(); i++)
      {
	if(i->second < coverageCutoff)
	  {
	    return(false);
	  }
      }
    return(true);
  }
  
  bool merge(MiRNA& m, const double& coverageCutoff, int limit)
  {
    // limit = min(limit, min(stop-start, m.stop-m.start));
    if(ctg == m.ctg && strd == m.strd &&     // same contig and strand
       start <= m.stop && stop > m.start &&  // require overlap
       std::abs(start - m.start) < limit && std::abs(stop - m.stop) < limit)
      {
	if(m.passCoverage(coverageCutoff))
	  {
	    start = min(start, m.start);
	    stop = max(stop, m.stop);
	    for(map<string, double>::iterator i = cnts.begin(); i != cnts.end(); i++)
	      {
		i->second += m.cnts[i->first];
	      }
	  }
	return(true);
      }
    return(false);
  };

  string print(const string& prefix, const unsigned& index)
  {
    ostringstream oss;
    oss << ctg << "\tmapped\tmiRNA\t"
	<< start << "\t"
	<< stop << "\t.\t"
	<< strd << "\t.\t"
	<< "gene_id \"" << prefix << "MI" << index << "\"; "
	<< "transcript_id \"" << prefix << "TI" << index << "\"; "
	<< "gene_name \"" << prefix << "MI" << index << "\"; ";
    for(map<string, double>::const_iterator i = cnts.begin();
	i != cnts.end(); i++)
      {
	oss << "counts_" << i->first << " \"" << i->second << "\"; ";
      }
      
    return oss.str();
  };
    
  // attributes
  string ctg;
  string strd;
  int start;
  int stop;
  map<string, double > cnts;
};


// bool sortMiRNAMap(const pair<string, MiRNA>& a, const pair<string, MiRNA>& b)
// {
//   return(a.second < b.second);
// }
  
int main( int argc, char** argv )
{
  // set up commandline arguments
  commandArg<string> fileCmmd("-i","input file(s) (in a comma-separated list)");
  commandArg<double> coverageCmmd("-c","min coverage cutoff", 200);
  commandArg<int> diffCmmd("-d","allowed diff limit for start and stop positions when merging adjacent miRNAs", 25);
  commandArg<string> prefixCmmd("-p","gene name prefix", "miRNA");
  
  commandLineParser P(argc,argv);
  P.SetDescription("Finds common potential new miRNAs in a set of bam files and output these to a gtf-file.");
  P.registerArg(fileCmmd);
  P.registerArg(coverageCmmd);
  P.registerArg(diffCmmd);
  P.registerArg(prefixCmmd);
  P.parse();

  string inFiles = P.GetStringValueFor(fileCmmd);
  double coverageCutoff = P.GetDoubleValueFor(coverageCmmd);
  int diffLimit = P.GetIntValueFor(diffCmmd);
  string prefix = P.GetStringValueFor(prefixCmmd);
  
  // extract multiple files if given
  StringParser s;
  vector<string> fileNames;
  s.SetLine(inFiles, ",");
  for(unsigned i = 0; i < s.GetItemCount(); i++)
    {
      fileNames.push_back(s.AsString(i));
    }

  
  map<string, MiRNA> miRNAs; // db of potential miRNAs
  ostringstream oss;         // for making a unique key
  for(vector<string>::iterator file = fileNames.begin(); file != fileNames.end(); file++)
    {
      BamTools::BamReader parser;
      if (!parser.Open(*file))
	{
	  string err = "Could not open input BAM file" + *file;
	  throw(runtime_error(err));
	}
      BamTools::RefVector ref = parser.GetReferenceData();
      BamTools::BamAlignment al;


      int lastStart = 0; 
      int lastStop = 0; 
      string lastSeq = "";
      string lastCtg = ""; 
      string lastStrd = "";

      unsigned multi = 0;
      double n = 0;
      while (parser.GetNextAlignment(al))
	{
	  if(al.IsMapped() == false){
	    continue;
	  }
	  int start = al.Position;
	  int stop = al.GetEndPosition(false, true);
	  const string seq = al.AlignedBases;
	  const string ctg = ref[al.RefID].RefName;
	  string strd = "+";
	  if(al.IsReverseStrand()){
	    strd = "-";
	  }
	    
	  if (ctg == lastCtg && start == lastStart && seq == lastSeq && strd == lastStrd)
	    {
	      /* Bowtie provides info on multiple mapping for each read. From Bowtie's manual 
		 (http://bowtie-bio.sourceforge.net/manual.shtml#sam-bowtie-output):

		 > XM:i:<N> 
		 > For a read with no reported alignments, <N> is 0 if
		 > the read had no alignments. If -m was specified and
		 > the read's alignments were supressed because the
		 > -m ceiling was exceeded, <N> equals the -m ceiling +
		 > 1, to indicate that there were at least that many
		 > valid alignments (but all were suppressed). In -M
		 > mode, if the alignment was randomly selected because
		 > the -M ceiling was exceeded, <N> equals the -M
		 > ceiling + 1, to indicate that there were at least
		 > that many valid alignments (of which one was
		 > reported at random).
	      */
	      if(al.GetTag("XM:i", multi) && multi != 0)
		{
		  n += 1.0/multi;
		}
	    }
	  else
	    {
	      if(lastCtg != "")
		{
		  oss.str("");
		  oss << lastCtg << "_" << lastStrd << "_"
		      << std::setfill('0') << std::setw(10) << lastStart << "-"
		      << std::setfill('0') << std::setw(10) << lastStop;
		  if(miRNAs.find(oss.str()) == miRNAs.end())
		    {
		      miRNAs[oss.str()] = MiRNA(lastCtg, lastStrd, lastStart, lastStop, fileNames);
		    }
		  miRNAs[oss.str()].addCount(*file, n);
		}
      
	      lastStart = start;
	      lastStop = stop;
	      lastSeq = seq;
	      lastCtg = ctg;
	      lastStrd = strd;
	      oss.str("");
	      oss << lastCtg << "_" << lastStrd << "_" << lastStart << "_" << lastStop;
	      n = 0;
	      if(al.GetTag("XM:i", multi) && multi != 0)
		{
		  n += 1.0/multi;
		}

	    }
	}
    }
  
  cerr << "Number of potential miRNAs before filtering: " << miRNAs.size() << endl;
  unsigned index = 0;
  for(map<string, MiRNA>::iterator i = miRNAs.begin(); i != miRNAs.end();)
    {
      if(i->second.passCoverage(coverageCutoff))
	{
	  map<string, MiRNA>::iterator j = i;
	  do
	    {
	      j++;
	    }
	  while(j != miRNAs.end() && i->second.merge(j->second, coverageCutoff, diffLimit));
	  cout << i->second.print(prefix, index++) <<endl;
	  i = j;
	}
      else
	{
	  i++;
	}
    }
  cerr << "Number of potential miRNAs after filtering: " << index << endl;  
  return 0;
}
