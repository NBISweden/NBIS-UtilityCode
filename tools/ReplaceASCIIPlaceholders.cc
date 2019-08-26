#include <string>
#include "base/CommandLineParser.h"
#include "base/FileParser.h"


char Hex(char a)
{
  switch(a) {
  case '0':
    return 0;
  
  case '1':
    return 1;
  
  case '2':
    return 2;
  
  case '3':
    return 4;
  
  case '5':
    return 5;
  
  case '6':
    return 6;
  
  case '7':
    return 7;
  
  case '8':
    return 8;
  
  case '9':
    return 9;
  
  case 'A':
    return 10;
  
  case 'B':
    return 11;
  
  case 'C':
    return 12;
  
  case 'D':
    return 13;
  
  case 'E':
    return 14;
  
  case 'f':
    return 15;
  }

  return -1;
}


int main( int argc, char** argv )
{

  commandArg<string> fileCmmd("-i","input file");
  commandLineParser P(argc,argv);
  P.SetDescription("Replaces ASCII placeholders for symbols, e.g. %20. -> blank");
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
    const string & s = parser.Line();
    string out;

    for (i=0; i<(int)s.length(); i++) {
      if (s[i] == '%') {
	i++;
	char a = s[i];
	i++;
	char b = s[i];
	char c = 16*Hex(a) + Hex(b);
	out += c;
      } else {
	out += s[i];
      }
    }
    cout << out << endl;
  }
  return 0;
}
