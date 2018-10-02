//#ifndef FORCE_DEBUG
//#define NDEBUG
//#endif



#include "base/CommandLineParser.h"
#include "base/FileParser.h"



//================================================================
//================================================================
//================================================================
//================================================================
int main( int argc, char** argv )
{
  commandArg<string> aStringCmmd("-i","input fasta sequence");
  //commandArg<string> bStringCmmd("-o","output fasta sequence");
  commandArg<string> sCmmd("-s","sequence name");
  commandArg<int> lCmmd("-l","last sequence", 0);
  commandArg<int> fCmmd("-from","first base", 0);
  commandArg<int> tCmmd("-to","last base", 0);

  commandLineParser P(argc,argv);
  P.SetDescription("Prints out sequence from a multi-fasta file");
  P.registerArg(aStringCmmd);
  //P.registerArg(bStringCmmd);
  P.registerArg(sCmmd);
  P.registerArg(lCmmd);
  P.registerArg(fCmmd);
  P.registerArg(tCmmd);

  P.parse();

  string in = P.GetStringValueFor(aStringCmmd);
  //String out = P.GetStringValueFor(bStringCmmd);
  string seq = P.GetStringValueFor(sCmmd);
  int from = P.GetIntValueFor(fCmmd);
  int to = P.GetIntValueFor(tCmmd);
  int last = P.GetIntValueFor(lCmmd);

  int i, k;
  FlatFileParser parser;
  
  parser.Open(in);

  string line;
  bool bPrint = false;

  k = 0;
  int n = 0;

  if (to == 0)
    to = 0x7FFFFFFF;

  while (parser.GetLine(line)) {
    const char * p = line.c_str();
    int l = strlen(p);
    if (l == 0)
      continue;
    if (p[0] == '>') {
      if (line == ">" + seq /*|| (last > 0 && k >= seq && k <= last)*/)
	bPrint = true;
      else
	bPrint = false;
      n = 0;
      k++;
      if (bPrint) {
	cout << line;
	if (to != 0)
	  cout << ":" << from <<"-"<<to;
	cout << endl;
      }
      continue;
    }


    if (bPrint) {
      bool bDo = false;    
      if (n < to || to == 0)
	bDo = true;
      const char * p = line.c_str();
      if (n + l > from && bDo) {
	if (n > from && (to == 0 || n + l < to)) {
	  cout << p << endl;
	  n += l;
	  continue;
	} 

	int start = from - n;
	int stop = to - n;
	if (stop > l - 1)
	  stop = l - 1;
	if (start < 0)
	  start = 0;

	for (i=start; i<=stop; i++) {
	  cout << p[i];
	  //cout << "  " << i << endl;
	}
	cout << endl;

      }

	//if (to == 0) {
	//cout << line << endl;
	//} else {
	//if (n >= from && n <= to)
	//cout << line << endl;
	//}

    }
    n += l;
  }

  return 0;
}

