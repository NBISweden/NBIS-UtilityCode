#include <string>
#include "base/CommandLineParser.h"
#include "base/FileParser.h"
#include "src/DNAVector.h"


int main( int argc, char** argv )
{

  commandArg<string> fileCmmd("-i","input file");
 
  commandLineParser P(argc,argv);
  P.SetDescription("Testing the file parser.");
  P.registerArg(fileCmmd);
 
  P.parse();
  
  string fileName = P.GetStringValueFor(fileCmmd);

  vecDNAVectorStream s;
    

  s.ReadStream(fileName);

  // Returns an empty vector at the end
  double all = 0.;
  double alldiv = 0.;
  double end = 0.;
  double enddiv = 0.;
  int i;
  
  while (s.Next()) {
    const DNAVector & d = s.Get();
    if (d.isize() < 1000)
      continue;

    if (d.isize() == 0)
      break;
    
    for (i=0; i<d.isize(); i++) {
      alldiv += 1.;
      if (i<250 || d.isize()-i < 250)
	enddiv += 1.;
      if (d[i] == 'G' || d[i] == 'C') {
	all += 1.;
	if (i<250 || d.isize()-i < 250)
	  end += 1.;
      }
    }
    if (i % 100 == 0)
      cout << all/alldiv << "\t" << end/enddiv << endl;
  }
 

  //comment. ???

  return 0;
}
