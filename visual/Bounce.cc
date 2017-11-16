#include "visual/Whiteboard.h"

#include "base/CommandLineParser.h"
#include "base/FileParser.h"
#include "base/SVector.h"
#include "visual/Color.h"

#include "visual/Axes.h"

#include <iostream>




void OneFrame(const string & o, double x, double y)
{
  double x_offset = 20;
  double y_offset = 20;


  int i, j;
  ns_whiteboard::whiteboard board;

 
  double x_max = 300.;
  double y_max = 300.;

  //double x = 0.;
  //double y = 0.;
  board.Add( new ns_whiteboard::rect( ns_whiteboard::xy_coords(0, 0), 
				      ns_whiteboard::xy_coords(x_max + 2*x_offset, y_max + 2*y_offset),
				      color(0.99,0.99,0.99)));

  board.Add( new ns_whiteboard::arc( ns_whiteboard::xy_coords(x+x_offset, y+y_offset), 
				     15., 0, 360, 1.0,
				     color(0,0,0)) );
   
 


  ofstream out(o.c_str());
  
  ns_whiteboard::ps_display display(out, x_max + 2 * x_offset, y_max + 2 * y_offset);
  board.DisplayOn(&display);
 
}

int main( int argc, char** argv )
{
 
  commandArg<string> aStringO("-o","outfile (post-script)");
 
  
  commandLineParser P(argc,argv);
  P.SetDescription("Animation example");

  P.registerArg(aStringO);

  P.parse();

  string o = P.GetStringValueFor(aStringO);
   
  
  // OneFrame(o, 0, 0);

  double x = 0.;
  double y = 270.;

  int i;
  double g;
  double v = 0.;
  int k = 1000;
  double inc = 2.;
  for (i=0; i<150; i++) {
    x += 2.;
    v -= inc;
    
    y += v;
    if (y < 0) {
      v = -v;
      inc *= 1.2;
    }
    char name[256];
    sprintf(name, "%s%d.ps", o.c_str(), k);
    OneFrame(name, x, y);
    k++;
  }
 
  return 0;
}
