#include <string>
#include "base/CommandLineParser.h"
#include "base/FileParser.h"
#include <math.h>

class Point
{
public:
  Point() {}

  void resize(int n) {m_data.resize(n, 0.);}
  void push_back(double d) {m_data.push_back(d);}
  int isize() const {return m_data.isize();}
  double & operator[](int i) {return m_data[i];}
  const double & operator[](int i) const {return m_data[i];}
private:
  svec<double> m_data;
};


Point Middle(svec<Point> & d)
{
  int i, j;
  Point m;
  m.resize(d[0].isize());
  for (i=0; i<d.isize(); i++) {
    for (j=0; j<m.isize(); j++)
      m[j] += (d[i])[j]/(double)d.isize();

  }
  return m;
}

double Distance(const Point & a, const Point & b)
{
  int i;
  double d = 0.;
  for (i=0; i<a.isize(); i++)
    d += (a[i]-b[i])*(a[i]-b[i]);
  return sqrt(d);
}


int main( int argc, char** argv )
{

  commandArg<int> nCmmd("-n","dim");
  commandLineParser P(argc,argv);
  P.SetDescription("Testing the file parser.");
  P.registerArg(nCmmd);
 
  P.parse();
  
  int n = P.GetIntValueFor(nCmmd);

  double d = 0.5;
  
  svec<Point> points;

  int i, j;

  // Let's set up the first two manually
  Point a, b;
  a.push_back(d);
  b.push_back(-d);

  points.push_back(a);
  points.push_back(b);
  
  for (i=1; i<n; i++) {
    Point m = Middle(points); // Compute the middle
    
    double tmp = Distance(points[0], m); // Any point will do, let's take 0
    double u = sqrt(4*d*d-tmp*tmp);
    m.push_back(u);
    
    for (j=0; j<points.isize(); j++)
      points[j].push_back(0.);

    points.push_back(m);
  }


  cout << "Printing coordinates: " << endl;

  for (i=0; i<points.isize(); i++) {
    for (j=0; j<points[i].isize(); j++)
      cout << " " << (points[i])[j];
    cout << endl;
  }

  cout << "Computing distances:" << endl;
  for (i=0; i<points.isize(); i++) {
    for (j=i+1; j<points.isize(); j++) {
      cout << i << " <-> " << j << ": " << Distance(points[i], points[j]) << endl;
    }
  }
  
  
  return 0;
}
