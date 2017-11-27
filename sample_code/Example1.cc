// Example for a program that reads lin for line from an input file

#include <string>
#include "base/CommandLineParser.h"
#include "base/FileParser.h"


// Main entry. NOTE: it has to be written *exactly* like this!
int main( int argc, char** argv )
{

  // Declare your command line options
  commandArg<string> fileCmmd("-i","input file");
  commandArg<int> nCmmd("-n","some integer", 0);
  commandLineParser P(argc,argv);

  // Tell the world what your program is doing.
  P.SetDescription("If you see this message, then whoever wrote this program was too lazy to tell you what it does!!");

  // Register the arguments
  P.registerArg(fileCmmd);
  P.registerArg(nCmmd);

  // Parse the command line
  P.parse();

  // Retrieve the values
  string fileName = P.GetStringValueFor(fileCmmd);
  int n = P.GetIntValueFor(nCmmd);

  // Parser to read ASCII files. Note that it interprets all whitespaces
  // as delimiters. If you have other delimiters, use the StringParser class.
  FlatFileParser parser;

  // Open the file
  parser.Open(fileName);

  int i;

  
  while (parser.ParseLine()) {
    // Skip empty lines (this is always useful, even if
    // your data file doesn't have any empty lines!
    if (parser.GetItemCount() == 0)
      continue;

    for (i=0; i<parser.GetItemCount(); i++) {
      
      // Print the whole line
      cout << parser.Line() << endl;
      
      if (parser.IsString(i))
	cout << parser.AsString(i) << endl;
      
      if (parser.IsInt(i))
	cout << parser.AsInt(i) << endl;
      
      if (parser.IsFloat(i))
	cout << parser.AsFloat(i) << endl;
    }
  }
  return 0;
}
