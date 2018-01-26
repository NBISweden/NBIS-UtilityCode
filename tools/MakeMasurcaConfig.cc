#include <string>
#include "base/CommandLineParser.h"
#include "base/FileParser.h"



int main( int argc, char** argv )
{

  commandArg<string> fileCmmd("-pe","paired end input files");
  commandArg<string> jumpCmmd("-j","jump input files", "");
  commandArg<int> tCmmd("-t","threads", 16);
  commandArg<string> oCmmd("-o","output config");
  commandLineParser P(argc,argv);
  P.SetDescription("Writes out a masurca config file.");
  P.registerArg(fileCmmd);
  P.registerArg(jumpCmmd);
  P.registerArg(tCmmd);
  P.registerArg(oCmmd);
 
  P.parse();
  
  string peName = P.GetStringValueFor(fileCmmd);
  string juName = P.GetStringValueFor(jumpCmmd);
  string oName = P.GetStringValueFor(oCmmd);
  int threads = P.GetIntValueFor(tCmmd);


  FILE * p = fopen(oName.c_str(), "w");

  char c = 'a';

  fprintf(p, "DATA\n");

  int i;
  
  StringParser parser;
  parser.SetLine(peName, ",");

  for (i=0; i<parser.GetItemCount(); i+=2) {
    fprintf(p, "PE= a%c 150 15 ../%s ../%s\n", c, parser.AsString(i).c_str(), parser.AsString(i+1).c_str());
    c++;
  }
  parser.SetLine(juName, ",");
  c = 'a';
  for (i=0; i<parser.GetItemCount(); i+=2) {
    fprintf(p, "JUMP= b%c sh 3600 200 ../%s ../%s\n", c, parser.AsString(i).c_str(), parser.AsString(i+1).c_str());
    c++;
  }

  
  
  fprintf(p, "END\n\n");
 
  fprintf(p, "PARAMETERS\n");
  fprintf(p, "GRAPH_KMER_SIZE=auto\n");
  fprintf(p, "USE_LINKING_MATES=1\n");
  fprintf(p, "NUM_THREADS= %d\n", threads);
  fprintf(p, "JF_SIZE=20000000000\n");
  fprintf(p, "DO_HOMOPOLYMER_TRIM=0\n");

  
  fprintf(p, "END\n");

  fclose(p);

  return 0;
}
