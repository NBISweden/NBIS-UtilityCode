#include "visual/Whiteboard.h"

#include "base/CommandLineParser.h"
#include "base/FileParser.h"
#include "base/SVector.h"
#include "visual/Color.h"

#include "visual/Compounds.h"

#include <iostream>
#include <math.h>

int main( int argc, char** argv )
{
 
  commandArg<string> aStringI("-i","input");
  commandArg<string> aStringO("-o","outfile (post-script)");
 
  
  commandLineParser P(argc,argv);
  P.SetDescription("Scatter plot");

  P.registerArg(aStringO);
  P.registerArg(aStringI);

  P.parse();

  string o = P.GetStringValueFor(aStringO);
  string fileName = P.GetStringValueFor(aStringI);
  
  double x_offset = 20;
  double y_offset = 20;


  int i, j;
  ns_whiteboard::whiteboard board;

 
  double x_max = 0.;
  double y_max = 0.;

  FlatFileParser parser;
  
  parser.Open(fileName);

  double rad = 1.;

  double scale = 12.;

  while (parser.ParseLine()) {
    double x = scale*sqrt(parser.AsFloat(0)+1) + x_offset;
    double y = scale*sqrt(parser.AsFloat(1)+1) + y_offset;
    if (x > 600)
      continue;
    if (y > 600)
      continue;
    if (x < 1)
      continue;
    if (y < 1)
      continue;


    if (x > x_max)
      x_max = x;
    if (y > y_max)
      y_max = y;

    board.Add( new ns_whiteboard::arc( ns_whiteboard::xy_coords(x, y), 
                                       rad, 0., 360., 0.5,
                                       color(0., 0., 0.5)));
  }
  board.Add( new ns_whiteboard::line( ns_whiteboard::xy_coords(x_offset, y_offset), 
				      ns_whiteboard::xy_coords(x_offset, y_max), 
				      1.,
				      color(0., 0., 0.)) );
  board.Add( new ns_whiteboard::line( ns_whiteboard::xy_coords(x_offset, y_offset), 
				      ns_whiteboard::xy_coords(x_max, y_offset), 
				      1.,
				      color(0., 0., 0.)) );


  ofstream out(o.c_str());
  
  ns_whiteboard::ps_display display(out, x_max + 2 * x_offset, y_max + 2 * y_offset);
  board.DisplayOn(&display);
 

  return 0;
}
