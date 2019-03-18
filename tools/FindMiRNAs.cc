#include <string>
#include "base/CommandLineParser.h"
#include "base/FileParser.h"



int main( int argc, char** argv )
{

  commandArg<string> fileCmmd("-i","input file");
  commandLineParser P(argc,argv);
  P.SetDescription("Finds miRNAs in a sam file.");
  P.registerArg(fileCmmd);
 
  P.parse();
  
  string fileName = P.GetStringValueFor(fileCmmd);
  

  //comment. ???
  FlatFileParser parser;
  
  parser.Open(fileName);

  int last = 0;

  int lastStart = 0;
  int lastStop = 0;

  int n = 0;

  string lastSeq;
  string lastCtg;
  
  while (parser.ParseLine()) {
    if (parser.GetItemCount() == 0)
      continue;

    int start = parser.AsInt(3);
    const string & seq = parser.AsString(9);
    const string & ctg = parser.AsString(2);
    int stop = start + (int)seq.size();

    if (start == lastStart && seq == lastSeq) {
      n++;
    } else {
      if (n > 0) {
	if (lastStart > last) {
	  if (n > 10 && lastSeq.size() > 17 && lastSeq.size() < 28) {
	    cout << lastCtg << "\t" << lastStart << "\t" << lastSeq << "\t" << n << "\t" << lastSeq.size() << endl;
	    
	  }
	    
	}
      }
      n = 0;
      last = lastStop;
      lastStart = start;
      lastStop = stop;
      lastSeq = seq;
      lastCtg = ctg;
    }
    
    
    
  }
  return 0;
}
