#include <string>
#include "base/CommandLineParser.h"
#include "base/FileParser.h"



int main( int argc, char** argv )
{

  commandArg<string> fileCmmd("-i","input file");
  commandArg<string> preCmmd("-p","name prefix");
  commandLineParser P(argc,argv);
  P.SetDescription("Converts BED to GTF.");
  P.registerArg(fileCmmd);
  P.registerArg(preCmmd);
 
  P.parse();
  
  string fileName = P.GetStringValueFor(fileCmmd);
  string prefix = P.GetStringValueFor(preCmmd);
 

  //comment. ???
  FlatFileParser parser;
  
  parser.Open(fileName);
  int i;
  parser.ParseLine();
  
  int n = 0;
  while (parser.ParseLine()) {
    if (parser.GetItemCount() == 0)
      continue;
    if (parser.AsString(0)[0] == '#')
      continue;

    cout << parser.AsString(0) << "\tmapped\tmiRNA\t";
    cout << parser.AsString(1) << "\t" << parser.AsString(2);
    cout << "\t.\t.\t.\tgene_id \"CANPAR" << prefix << "MI" << n << "\";";
    cout << " gene_name \"CANPAR" << prefix << "MI" << n << "\";";
    cout << endl;
    
    n++;
  }
  return 0;
}
