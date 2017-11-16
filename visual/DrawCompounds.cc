#include "visual/Whiteboard.h"

#include "base/CommandLineParser.h"
#include "base/FileParser.h"
#include "base/SVector.h"
#include "visual/Color.h"

#include "visual/Compounds.h"

#include <iostream>

int main( int argc, char** argv )
{
 
  commandArg<string> aStringO("-o","outfile (post-script)");
 
  
  commandLineParser P(argc,argv);
  P.SetDescription("Color scale example");

  P.registerArg(aStringO);

  P.parse();

  string o = P.GetStringValueFor(aStringO);
   
  double x_offset = 20;
  double y_offset = 20;


  int i, j;
  ns_whiteboard::whiteboard board;

 
  double x_max = 200.;
  double y_max = 200.;

    
  Box b;
  b.Draw(board,
	 ns_whiteboard::xy_coords(20, 20), 
	 ns_whiteboard::xy_coords(60, 60));
 
  b.Draw(board,
	 ns_whiteboard::xy_coords(30, 70), 
	 ns_whiteboard::xy_coords(20, 120),
	 ns_whiteboard::xy_coords(60, 140),
	 ns_whiteboard::xy_coords(70, 75));
 
 
  Arrow a;
  a.Draw(board,  
	 ns_whiteboard::xy_coords(110, 25), 
	 ns_whiteboard::xy_coords(170, 25));

  a.Draw(board,  
	 ns_whiteboard::xy_coords(100, 30), 
	 ns_whiteboard::xy_coords(150, 80));
  
  Arrow af(10., 0.);
  af.Draw(board,  
	  ns_whiteboard::xy_coords(100, 20), 
	  ns_whiteboard::xy_coords(100, 110));
 

  ofstream out(o.c_str());
  
  ns_whiteboard::ps_display display(out, x_max + 2 * x_offset, y_max + 2 * y_offset);
  board.DisplayOn(&display);
 

  return 0;
}
