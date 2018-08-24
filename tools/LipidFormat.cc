#include <string>
#include "base/CommandLineParser.h"
#include "base/FileParser.h"
#include "math/Spearman.h"
#include "base/RandomStuff.h"

class Lipid
{
public:
  string name;
  string cat;
  svec<double> data;
};

class Sample
{
public:
  string name;
  string cat;
  svec<double> data;
};

class Pair
{
public:
  double val;
  string cat;
  bool operator < (const Pair & r) const {
    return val < r.val;
  }
};

void TestEnrich(const Lipid & l, const svec<Sample> & s)
{
  svec<Pair> pair;

  int i, j;
  for (i=0; i<l.data.isize(); i++) {
    Pair p;
    if (s[i].cat == "CTL" || s[i].cat == "DM") {
      p.cat = s[i].cat;
      p.val = l.data[i];
      pair.push_back(p);
    }
  }
  Sort(pair);
  double dm = 0.;
  double ctl = 0;
  double dm_c = 0.;
  double ctl_c = 0.;

  for (i=0; i<pair.isize(); i++) {
    if (pair[i].cat == "CTL") {
      ctl_c += 1.;
      ctl += (double)i/(double)pair.isize();
    }
    if (pair[i].cat == "DM") {
      dm_c += 1.;
      dm += (double)i/(double)pair.isize();
    }
      
  }

  double lo = 0.5 * dm_c/(double)pair.isize();
  double hi = 1.-lo;
  double range = hi-lo;
  
  cout << "DM: " << dm/dm_c << " CTL: " << ctl/ctl_c << "\tscore: " << (dm/dm_c-lo)/range << endl;

}


int main( int argc, char** argv )
{

  commandArg<string> fileCmmd("-i","input file");
  commandLineParser P(argc,argv);
  P.SetDescription("Testing the file parser.");
  P.registerArg(fileCmmd);
 
  P.parse();
  
  string fileName = P.GetStringValueFor(fileCmmd);

  svec<Lipid> lipids;
  svec<Sample> samples;

  //comment. ???
  FlatFileParser parser;
  
  parser.Open(fileName);

  int i, j;
  int off = 7;
  parser.ParseLine();
  for (i=off; i<parser.GetItemCount(); i++) {
    Lipid l;
    l.name = parser.AsString(i);
    lipids.push_back(l);
  }
  
  parser.ParseLine();
  string last;
  for (i=off; i<parser.GetItemCount(); i++) {
    if (parser.AsString(i) != ".")
      last = parser.AsString(i);
    lipids[i-off].cat = last;
    
  }

  
  while (parser.ParseLine()) {
    if (parser.GetItemCount() == 0)
      continue;
    Sample s;
    s.name = parser.AsString(3);
    if (strstr(s.name.c_str(), "CTL") != NULL)
      s.cat = "CTL";
    if (strstr(s.name.c_str(), "DM") != NULL)
      s.cat = "DM";

    for (i=off; i<parser.GetItemCount(); i++) {
      if (parser.AsString(i) == ".") {
	double v = RandomFloat(0.05);
	s.data.push_back(v);
	lipids[i-off].data.push_back(v);
      } else {
	s.data.push_back(parser.AsFloat(i));
	lipids[i-off].data.push_back(parser.AsFloat(i));
      }
    }
    samples.push_back(s);
  }

  FILE * pTrain = fopen("train.txt", "w");
  FILE * pTest = fopen("test.txt", "w");
  FILE * pTruth = fopen("truth.txt", "w");
  FILE * pTruthT = fopen("truth_test.txt", "w");

  int a = 0;
  int b = 0;
  for (i=0; i<lipids.isize(); i++) {
    if (lipids[i].name == "PCO@18.2.0#16.0.0")
      a = i;
    if (lipids[i].name == "TAG50.1")
      b = i;    
  }
  for (i=0; i<lipids[a].data.isize(); i++) {
    bool good = false;
    int yesno = 0;
    if (samples[i].cat == "DM") {
      good = true;
      yesno = 1;
      //fprintf(pTruth, "1\n");
    }
    if (samples[i].cat == "CTL") {
      good = true;
      yesno = 0;
      //fprintf(pTruth, "0\n");
    }
    if (good) {
      if (RandomFloat(1.) > 0.2) {
	fprintf(pTrain, "%f\t%f\n", 0.6*log(1+lipids[a].data[i]),  10*log(1+lipids[b].data[i]));
	fprintf(pTruth, "%d\n", yesno);
      } else {
	fprintf(pTest, "%f\t%f\n", 0.6*log(1+lipids[a].data[i]),  10*log(1+lipids[b].data[i]));
 	fprintf(pTruthT, "%d\n", yesno);
     }
    }
  }


  
  fclose(pTrain);
  fclose(pTest);
  fclose(pTruth);
  fclose(pTruthT);
  return 0;
}
