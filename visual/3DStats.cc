
#define FORCE_DEBUG
#include "visual/Whiteboard.h"

#include "base/CommandLineParser.h"
#include "base/FileParser.h"
#include "base/SVector.h"
#include "visual/Color.h"

#include "visual/Axes.h"
#include "visual/Geometry.h"
#include "visual/Compounds.h"

#include <iostream>



class Cube
{
public:
  Cube() {
    m_size = 100;
    int i, j;
    m_data.resize(m_size);
    for (i=0; i<m_size; i++) {
      svec< svec < double > > & m = m_data[i];
      m.resize(m_size);
      for (j=0; j<m_size; j++) {
	svec< double > & h = m[j];
	m[j].resize(m_size, 0.);
      }
    }
  }

  int Size() const {return m_size;}

  void Add(double xx, double yy, double zz) {
    int x = (int)(xx + 0.5);
    int y = (int)(yy + 0.5);
    int z = (int)(zz + 0.5);
    if (x >= m_size || x < 0)
      return;
    //x = m_size-1;
    if (y >= m_size || y < 0)
      return;
    //y = m_size-1;
    if (z >= m_size || z < 0)
      return;
    //z = m_size-1;

    ((m_data[x])[y])[z] += 1.;
  }
  
  double Get(int x, int y, int z) const {
    return ((m_data[x])[y])[z];
  }

private:
  int m_size;
  svec< svec < svec < double > > > m_data;
};


void OneFrame(const string & o, const Cube & c, double phi, double theta)
{
  double x_offset = 20;
  double y_offset = 20;


  int i, j, k;
  ns_whiteboard::whiteboard board;

 
  double x_max = 300.;
  double y_max = 300.;


  //double x = 0.;
  //double y = 0.;
  board.Add( new ns_whiteboard::rect( ns_whiteboard::xy_coords(0, 0), 
  				      ns_whiteboard::xy_coords(x_max + 2*x_offset, y_max + 2*y_offset),
  				      color(0.99,0.99,0.99)));
  
  Geometry3D g;
  g.SetOffset(120.);
  g.SetRotation(phi, theta);

  double d = 200.;

  
  for (k=0; k<c.Size(); k++) {
    for (j=0; j<c.Size(); j++) {
      for (i=0; i<c.Size(); i++) {
 	double v = c.Get(i, j, k);
	if (v < 1.)
	  continue;
	double val = v / 20. - 1.;
	cout << "density: " << v << " " << i << " " << j << " " << k << endl;
	color col = GradientMult(val, color(0.99, 0.99, 0.99), color(0.99, 0.99, 0.), color(0.99, 0., 0.));
	
	double x = 220. * (double)i / (double)c.Size();
	double y = 220. * (double)j / (double)c.Size();
	double z = 220. * (double)k / (double)c.Size();
	
	Box box(1., col);
	double w = 4.;
	box.Draw(board, 
		 g.Coords(x, y, z),
		 g.Coords(x, y, z+w),
		 g.Coords(x, y+w, z+w),
		 g.Coords(x, y+w, z));

	  //board.Add( new ns_whiteboard::rect( g.Coords(x, y, z), 
	  //				    g.Coords(x+3, y+3, z+3),
	  //				    col));
	
      }
    }
  }

  board.Add( new ns_whiteboard::line( g.Coords(0, 0, 0), 
                                      g.Coords(d, 0, 0),
                                      1.,
                                      color(0.5,0.,0.)));

  board.Add( new ns_whiteboard::line( g.Coords(0, 0, 0), 
                                      g.Coords(0, d, 0),
                                      1.,
                                      color(0.,0.5,0.)));

  board.Add( new ns_whiteboard::line( g.Coords(0, 0, 0), 
                                      g.Coords(0, 0, d),
                                      1.,
                                      color(0.,0.,0.5)));

  ofstream out(o.c_str());
  
  ns_whiteboard::ps_display display(out, x_max + 2 * x_offset, y_max + 2 * y_offset);
  board.DisplayOn(&display);
 
}


int main( int argc, char** argv )
{
 
  commandArg<string> iStringO("-i","input");
  commandArg<string> aStringO("-o","outfile (post-script)");
 
  
  commandLineParser P(argc,argv);
  P.SetDescription("Animation example");

  P.registerArg(iStringO);
  P.registerArg(aStringO);

  P.parse();

  string o = P.GetStringValueFor(aStringO);
  string in = P.GetStringValueFor(iStringO);
   
  Cube c;
  FlatFileParser parser;
  
  parser.Open(in);
  
  while (parser.ParseLine()) {
    if (parser.GetItemCount() == 0)
      continue;
    double x = (parser.AsFloat(0) - 200) / 6.;
    double y = parser.AsFloat(1) / 6.;
    double z = (parser.AsFloat(2)-0.3) * 190.;
    cout << "Add: " << x << " " << y << " " << z << endl;
    c.Add(x, y, z);
  }
  
  
  //OneFrame(o, c, 0.3, 0.2);
  
  int i;
  int k = 1000;
 
  double phi = 0.;
  for (i=0; i<90; i++) {
    char name[256];
    sprintf(name, "%s%d.ps", o.c_str(), k);
    
    phi = 2*3.1415/90.*(double)i;
    
    OneFrame(name, c, phi, phi);
    k++;
  }
 
  return 0;
}
