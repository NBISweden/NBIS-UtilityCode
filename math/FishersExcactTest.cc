#include <string>
#include "base/CommandLineParser.h"
#include "base/FileParser.h"

void Read(const string & fileName, svec<string> & all)
{
  FlatFileParser parser;
  
  parser.Open(fileName);

  while (parser.ParseLine()) {
    if (parser.GetItemCount() == 0)
      continue;
    all.push_back(parser.AsString(1));
  }
  Sort(all);
}

double FacFrac(int a, int b) 
{
  if (a > b)
    cout << "ERROR!" << endl;
  int i;
  double res = 1.;
  for (i=a+1; i<=b; i++) {
    res *= (double)i;
  }
  //cout << a << " " << b << "! = " << res << endl; 
  return res;

  
}

double Fac(int d) {
  int i;
  double res = 1.;
  for (i=2; i<=d; i++) {
    res *= (double)i;
  }
  //cout << d << "! = " << res << endl; 
  return res;
}

double FishersExact(double a, double b, double c, double d)
{
  //double r = Fac(a+b)*Fac(c+d)*Fac(a+c)*Fac(b+d);
  
  //r /= (Fac(a)*Fac(b)*Fac(c)*Fac(d)*Fac(a+b+c+d));

  //cout << "a=" << a << " b=" << b << " c=" << c << " d=" << d << endl;
  double r = 1/FacFrac(c+d, a+b+c+d);
  r *= FacFrac(c, a+c);
  r *= FacFrac(d, b+d);

  r *=Fac(a+b);
  r /= Fac(a);
  r /= Fac(b);
  

  return r;
}


int main( int argc, char** argv )
{

  commandArg<double> aCmmd("-a","samples A in test set");
  commandArg<double> bCmmd("-b","samples B in test set");
  commandArg<double> cCmmd("-c","samples A in truth set");
  commandArg<double> dCmmd("-d","samples B in truth set");
  commandLineParser P(argc,argv);
  P.SetDescription("Calculates an enrichment test.");
  P.registerArg(aCmmd);
  P.registerArg(bCmmd);
  P.registerArg(cCmmd);
  P.registerArg(dCmmd);
  
  P.parse();

  double a = P.GetDoubleValueFor(aCmmd);
  double b = P.GetDoubleValueFor(bCmmd);
  double c = P.GetDoubleValueFor(cCmmd);
  double d = P.GetDoubleValueFor(dCmmd);

  cout << "Fisher's exact test: " << FishersExact(a, b, c, d) << endl;
  return 0; 
}
