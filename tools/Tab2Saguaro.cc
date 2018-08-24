#include <string>
#include "base/CommandLineParser.h"
#include "base/FileParser.h"


int main( int argc, char** argv )
{

  commandArg<string> fileCmmd("-i","input file");
  //commandArg<string> iCmmd("-ignore","ignore columns", "");
  //commandArg<int> eCmmd("-to","to column");
  //commandArg<bool> nCmmd("-newline","add newline", false);
  commandLineParser P(argc,argv);
  P.SetDescription("Converts a csv file to a table.");
  P.registerArg(fileCmmd);
  //P.registerArg(iCmmd);
  // P.registerArg(eCmmd);
  //P.registerArg(nCmmd);
 
  P.parse();
  
  string fileName = P.GetStringValueFor(fileCmmd);
  //string ign = P.GetStringValueFor(iCmmd);
  //int to = P.GetIntValueFor(eCmmd);
  //bool bN = P.GetBoolValueFor(nCmmd);
 

  //comment. ???
  FlatFileParser parser;
  
  parser.Open(fileName);

  int i, j;

  svec<string> header;
  svec<string> type;
  svec<int> pos;
  int max = 0;
  int start = 7;
  
  parser.ParseLine();
  for (i=start; i<parser.GetItemCount(); i++) {
    if (parser.AsString(i) == "Age_B")
      break;
    header.push_back(parser.AsString(i));
  }
  max = i;

  parser.ParseLine();
  string last;
  int k = 0;
  for (i=start; i<max; i++) {
    if (parser.AsString(i) != ".") {
      last = parser.AsString(i);
      k = 0;
    }
    
    type.push_back(last);
    pos.push_back(k);
    k++;
  }

  //Lipid category, ignore for now
  parser.ParseLine();


  svec<string> label;
  svec< svec < double > > data;

  
  while (parser.ParseLine()) {
    if (parser.GetItemCount() == 0)
      continue;
    //string ll = parser.AsString(0);
    label.push_back(parser.AsString(3));
    svec<double> one; 
    for (i=start; i<max; i++) {
      if (parser.AsString(i) == ".")
	one.push_back(0);
      else
	one.push_back(parser.AsFloat(i));
    }
    data.push_back(one);
  }

  cout << "Lipid\tpos";
  for (i=0; i<label.isize(); i++) {
    cout << "\t" << label[i];
  }
  cout << endl;

  for (i=0; i<data[0].isize(); i++) {
    cout << type[i] << "\t" << pos[i];
    for (j=0; j<data.isize(); j++) {
      cout << "\t" << (data[j])[i];
    }
    cout << endl;
  }

  
  return 0;
}
