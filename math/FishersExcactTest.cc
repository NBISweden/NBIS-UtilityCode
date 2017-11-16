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

  commandArg<string> tCmmd("-t","test input file");
  commandArg<string> rCmmd("-r","reference input file");
  commandLineParser P(argc,argv);
  P.SetDescription("Testing the file parser.");
  P.registerArg(tCmmd);
  P.registerArg(rCmmd);
  
  P.parse();
  
  svec<string> ref, test;

  Read(P.GetStringValueFor(rCmmd), ref);
  Read(P.GetStringValueFor(tCmmd), test);

  //cout << FishersExact(1, 9, 11, 3) << endl;
  //return 0;


  int i, j;
  int n = 0;
  int count = 0;
  string last;
 
  last = test[0];
  for (i=1; i<test.isize(); i++) {
    if (test[i] == last) {
      count++;
    } else {
      int refCount = 0;
      for (j=0; j<ref.isize(); j++) {
	if (ref[j] == last)
	  refCount++;
      }

      int a = count;
      int b = refCount;
      int c = test.isize() - a;
      int d = ref.isize() - b;
      
      double p = FishersExact(a, b, c, d);
      //if (p < 0.0000000000000000001)
      cout << last << "\t" << p << endl;

      count = 1;
      last = test[i];
    }
  }


  //comment. ???
  return 0;
}
