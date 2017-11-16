#include <string>
#include "base/CommandLineParser.h"
#include "base/FileParser.h"



int main( int argc, char** argv )
{

  commandArg<string> fileCmmd("-i","input file");
  commandArg<int> nCmmd("-n","how many");
  commandLineParser P(argc,argv);
  P.SetDescription("Testing the file parser.");
  P.registerArg(fileCmmd);
  P.registerArg(nCmmd);
  
  P.parse();
  
  string fileName = P.GetStringValueFor(fileCmmd);
  int n = P.GetIntValueFor(nCmmd);
  

  //comment. ???
  FlatFileParser parser;
  
  parser.Open(fileName);

  int i;
 
  svec<string> all;
  

 
  int k = 0;
  FILE * p = NULL;

  while (parser.ParseLine()) {
    if (k >= all.isize())
      all.resize(k + 500000);

    all[k] = parser.Line();
    k++;
  }
  all.resize(k);

  int chunk = (k+n) / n;
  int count = 0;
  cout << "Total: " << k << " chunk=" << chunk << " c*n=" << chunk * n << endl;
  for (i=0; i<k; i++) {
    if (i % chunk == 0) {
      cout << "Create: " << count << " i=" << i << endl;
      if (p != NULL)
	fclose(p);
      string name = fileName;
      char tmp[128];
      sprintf(tmp, ".%d", count);
      count++;
      name += tmp;
      p = fopen(name.c_str(), "w");
    }
    fprintf(p, "%s\n", all[i].c_str());
  }
  fclose(p);
  return 0;
}
