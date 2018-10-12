#include <string>
#include "base/CommandLineParser.h"
#include "base/FileParser.h"



int main( int argc, char** argv )
{

  commandArg<string> fileCmmd("-i","input file");
  commandLineParser P(argc,argv);
  P.SetDescription("Converts GFF3 to GTF.");
  P.registerArg(fileCmmd);
 
  P.parse();
  
  string fileName = P.GetStringValueFor(fileCmmd);
 

  //comment. ???
  FlatFileParser parser;
  
  parser.Open(fileName);
  int i;
  while (parser.ParseLine()) {
    if (parser.GetItemCount() == 0)
      continue;
    if (parser.AsString(0)[0] == '#')
      continue;
    StringParser p;
    p.SetLine(parser.AsString(8), ";");
   
    cout << parser.AsString(0);
    for (i=1; i<8; i++)
      cout << "\t" << parser.AsString(i);

    StringParser pp;
    if (parser.AsString(2) == "gene") {
      pp.SetLine(p.AsString(0), "=");
      cout << "\tgene_id \"" << pp.AsString(1) << "\"; ";
    } else {
      pp.SetLine(p.AsString(1), "=");
      cout << "\tgene_id \"" << pp.AsString(1) << "\"; ";     
      pp.SetLine(p.AsString(0), "=");
      cout << " transcript_id \"" << pp.AsString(1) << "\"; ";
    }
    cout << "gene_name \"" << parser.AsString(8) << "\"; " << endl;
  }
  return 0;
}
