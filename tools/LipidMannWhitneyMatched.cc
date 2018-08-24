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
  Sample() {
    bmi = -1.;
  }
  
  string name;
  string cat;
  svec<double> data;
  double bmi;
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
  int r1 = 0;
  int r2 = 0;
  int n1 = 0;
  int n2 = 0;
  
  for (i=0; i<pair.isize(); i++) {
    if (pair[i].cat == "CTL") {
      r1 += i;
      n1++;
    }
    if (pair[i].cat == "DM") {
      r2 += i;
      n2++;
    }
  }
  int u1 = r1 - n1*(n1+1)/2;
  int u2 = r2 - n2*(n2+1)/2;

  double u = u1;
  string ud = "UP";
  if (u2 < u1) {
    u = u2;
    ud = "DOWN";
  }

  double m = 0.5*(double)n1*(double)n2;
  double sigma = sqrt((double)n1*(double)n2*((double)n1 + (double)n2 + 1)/12);

  double z = (u-m)/sigma;

  double p = 0.5 * (erfc((m-u)/sigma/1.414213562));
  p *= corr;

  if (p < 0.01) {  
    cout << l.name <<  " -> U=" << u << " z=" << z << " p=" << p << " " << ud << endl;
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
  svec<Sample> samples_c;

  //comment. ???
  FlatFileParser parser;
  
  parser.Open(fileName);

  int i, j;
  int off = 7;
  parser.ParseLine();
  int bmi_index = -1;
  for (i=off; i<parser.GetItemCount(); i++) {
    Lipid l;
    l.name = parser.AsString(i);
    lipids.push_back(l);
    if (l.name == "BMI_B")
      bmi_index = i;
  }
  
  parser.ParseLine();
  string last;
  for (i=off; i<parser.GetItemCount(); i++) {
    if (parser.AsString(i) != ".")
      last = parser.AsString(i);
    lipids[i-off].cat = last;
    
  }

  svec<double> bmi;

  int n = 0;
  int nc = 0;
  int nd = 0;
  while (parser.ParseLine()) {
    if (parser.GetItemCount() == 0)
      continue;
    Sample s;

    double bb = parser.AsFloat(bmi_index);

    if (bb < 1)
      continue;

    n++;
    s.name = parser.AsString(3);
 
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
    s.bmi = bb;
    if (strstr(s.name.c_str(), "CTL") != NULL) {
      s.cat = "CTL";
      samples_c.push_back(s);
      nc++;
    }
    if (strstr(s.name.c_str(), "DM") != NULL) {
      s.cat = "DM";
      samples.push_back(s);
      nd++;
    }
  }

  int nn = samples.isize();
  for (i=0; i<nn; i++) {
    double bbb = samples[i].bmi;
    double dist = 99999.;
    int index = -1;
    for(j=0; j<samples_c.isize(); j++) {
      if (samples_c[j].bmi < 0)
	continue;
      double d = samples_c[j].bmi - samples[i].bmi;
      if (d < 0)
	d = -d;
      if (d < dist) {
	index = j;
	dist = d;
      }
    }
    samples.push_back(samples_c[index]);
    samples_c[index].bmi = -1;
  }
  nc = samples.isize() - nn;
  
  cerr << "Samples: " << samples.isize() << " ctl: " << nc << " dia: " << nd << endl;
  cerr << "Lipids: " << lipids.isize() << endl;
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
