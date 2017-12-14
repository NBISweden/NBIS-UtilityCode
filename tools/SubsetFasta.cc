#include <string>
#include "base/CommandLineParser.h"
#include "base/FileParser.h"
#include "base/RandomStuff.h"


int main( int argc, char** argv )
{

  commandArg<string> fileCmmd1("-i1","input file");
  commandArg<string> fileCmmd2("-i2","input file");
  commandArg<string> outCmmd1("-o1","output file");
  commandArg<string> outCmmd2("-o2","output file");
  commandArg<double> fracCmmd("-f","fraction of reads to be kept");
  commandLineParser P(argc,argv);
  P.SetDescription("Subsets a reads fasta/fastq file.");
  P.registerArg(fileCmmd1);
  P.registerArg(fileCmmd2);
  P.registerArg(outCmmd1);
  P.registerArg(outCmmd2);
  P.registerArg(fracCmmd);

  P.parse();
  
  string fileName1 = P.GetStringValueFor(fileCmmd1);
  string fileName2 = P.GetStringValueFor(fileCmmd2);
  string outName1 = P.GetStringValueFor(outCmmd1);
  string outName2 = P.GetStringValueFor(outCmmd2);
  double frac = P.GetDoubleValueFor(fracCmmd);

  //comment. ???
  FlatFileParser parser1, parser2;
  
  parser1.Open(fileName1);
  parser2.Open(fileName2);

  FILE * pOut1 = fopen(outName1.c_str(), "w");
  FILE * pOut2 = fopen(outName2.c_str(), "w");

  int lines = 2;
  
  while (parser1.ParseLine()) {
    parser2.ParseLine();
    if (parser1.GetItemCount() == 0)
      continue;
    if (parser1.AsString(0)[0] != '>')
      lines = 4;

    bool bPrint = true;
    if (RandomFloat(1) > frac)
      bPrint = false;

    //cout << parser1.Line() << endl;

    if (bPrint) {
      fprintf(pOut1, "%s\n", parser1.Line().c_str());
      fprintf(pOut2, "%s\n", parser2.Line().c_str());
    }
    
    for (int i=0; i<lines-1; i++) {
      parser1.ParseLine();
      parser2.ParseLine();
      //cout << " " << parser1.Line() << endl;
      if (bPrint) {
	fprintf(pOut1, "%s\n", parser1.Line().c_str());
	fprintf(pOut2, "%s\n", parser2.Line().c_str());
      }
    }

    
  }

  fclose(pOut1);
  fclose(pOut2);
  
  return 0;
}
