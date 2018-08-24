#include <string>
#include "base/CommandLineParser.h"
#include "base/FileParser.h"
#include "math/Spearman.h"
#include "base/RandomStuff.h"
#include "math/FishersExact.h"

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

void TestEnrich(const Lipid & l, const svec<Sample> & s, const string &name, double corr)
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
  

  double p = 1.;
  double a=0;
  double b=0;
  double c=0;
  double d=0;
  
  for (i=0; i<pair.isize(); i++) {
    if (pair[i].cat == "DM") {
      c++;
    }
    if (pair[i].cat == "CTL") {
      d++;
    }
  }

  for (i=0; i<pair.isize(); i++) {    
    if (pair[i].cat == "DM") {
      a++;
    }
    if (pair[i].cat == "CTL") {
      b++;
    }
        
    double t = FishersExact(a, b, c, d);
    //cout << a << " " << b << " " << c << " " << d << " -> " << t << endl;
    if (t < p)
      p = t;
  } 
  p *= pair.isize();
  p *= corr;
  if (p > 1.)
    p = 1.;

  if (p < 0.01) {
    cout << name << "\tDM: " << dm/dm_c << " CTL: " << ctl/ctl_c << "\tscore: " << (dm/dm_c-lo)/range;
   
    cout << "\t" << p << endl;
  }
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

  for (i=0; i<lipids.isize(); i++) {
    //cout << lipids[i].name << "\t";
    TestEnrich(lipids[i], samples, lipids[i].name, (double)lipids.isize());
  }
  return 0;
  cout << "Correlations" << endl;
  
  for (i=0; i<lipids.isize(); i++) {
    for (j=0; j<lipids.isize(); j++) {
      if (strstr(lipids[i].name.c_str(), "PCO") == NULL ||
	  strstr(lipids[j].name.c_str(), "TAG") == NULL)
	continue;
      
      SpearmansRho r;
      double rho = r.Compute(lipids[i].data, lipids[j].data);
      double p = r.Significance();
      //if (rho > 0.1) {
	cout << lipids[i].name << " <-> " << lipids[j].name << "\t";
	cout << rho << "\t" << p << endl;
	// }
      //for (int x=0; x<lipids[i].data.isize(); x++) {
      //cout << lipids[i].data[x] << "\t" << lipids[j].data[x] << endl;
      //}
    }
  }


  
  return 0;
}
