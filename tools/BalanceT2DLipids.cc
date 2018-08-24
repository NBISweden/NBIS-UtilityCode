#include <string>
#include "base/CommandLineParser.h"
#include "base/FileParser.h"



int main( int argc, char** argv )
{

  commandArg<string> fileCmmd("-i","input file");
  commandLineParser P(argc,argv);
  P.SetDescription("Testing the file parser.");
  P.registerArg(fileCmmd);
 
  P.parse();
  
  string fileName = P.GetStringValueFor(fileCmmd);
 

  //comment. ???
  FlatFileParser parser;
  
  parser.Open(fileName);

  parser.ParseLine();
  cout << parser.Line() << endl;

  svec<string> all_dm;
  svec<double> bmi_dm;
  svec<string> all_c;
  svec<double> bmi_c;
 
  int idx = -1;
  int i, j;

  for (i=0; i<parser.GetItemCount(); i++) {
    if (parser.AsString(i) == "BMI_B") {
      idx = i;
    }
  }
  
  while (parser.ParseLine()) {
    if (parser.GetItemCount() == 0)
      continue;
    if (strstr(parser.AsString(3).c_str(), "CTL") != NULL) {
      all_c.push_back(parser.Line());
      bmi_c.push_back(parser.AsFloat(idx));
    }
    if (strstr(parser.AsString(3).c_str(), "DM") != NULL) {
      all_dm.push_back(parser.Line());
      bmi_dm.push_back(parser.AsFloat(idx));
    }
    
  }


  for (i=0; i<all_dm.isize(); i++) {
    double min = 99999.;
    int best = -1;
    for(j=0; j<all_c.isize(); j++) {
      double d = (bmi_dm[i]-bmi_c[j])*(bmi_dm[i]-bmi_c[j]);
      if (d < min) {
	min = d;
	best = j;
      }
    }
    cout << all_dm[i] << endl;
    cout << all_c[best] << endl;
    //cout << bmi_dm[i] << "\t" << bmi_c[best] << endl;
    bmi_c[best] = -1;
  }
  
  return 0;
}
