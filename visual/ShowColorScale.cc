#include "visual/Whiteboard.h"

#include "base/CommandLineParser.h"
#include "base/FileParser.h"
#include "base/SVector.h"
#include "visual/Color.h"

#include "visual/Axes.h"

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

 
  double x_max = 0.;
  double y_max = 0.;

  double x = 0.;
  double y = 0.;
  int n = 100;
  double w = 10.;
  
  for (i=-n; i<=n; i++) {
    double v = (double)i/(double)n;

    color one = color(0.78, 0.1, 0.1);
    color two = color(0.1, 0.1, 0.78);
    color col = GradientMult(v, one, two);
    x = n + i;
    
    board.Add( new ns_whiteboard::rect( ns_whiteboard::xy_coords(x + x_offset, y + y_offset + w), 
					ns_whiteboard::xy_coords((x+1) + x_offset, y + y_offset),
					col) );

    if (x > x_max)
      x_max = x;
  }
  if (y + w > y_max)
    y_max = y + w;

  y += 1.2*w;

  board.Add( new ns_whiteboard::text( ns_whiteboard::xy_coords(x_offset, y + y_offset),
				      "Colour scale -1 to +1 (white)", black, 6., "Times-Roman", 0, true));

  y += w;
  for (i=-n; i<=n; i++) {
    double v = (double)i/(double)n;

    color one = color(0., 0.99, 0.2);
    color two = color(0.99, 0.1, 0.1);
    color col = GradientMult(v, one, two, color(0, 0, 0));
    x = n + i;
    
    board.Add( new ns_whiteboard::rect( ns_whiteboard::xy_coords(x + x_offset, y + y_offset + w), 
					ns_whiteboard::xy_coords((x+1) + x_offset, y + y_offset),
					col) );

    if (x > x_max)
      x_max = x;
  }
  if (y + w > y_max)
    y_max = y + w;

  y += 1.2*w;

  board.Add( new ns_whiteboard::text( ns_whiteboard::xy_coords(x_offset, y + y_offset),
				      "Colour scale -1 to +1 (black)", black, 6., "Times-Roman", 0, true));

  y += w;
  //====================================================================

  /*
  for (i=-n; i<=n; i++) {
    double v = (double)i/(double)n;

    color one = MakeUpColor(1);
    color two = MakeUpColor(17);
    color three = MakeUpColor(5);

    color col = GradientMult(v, one, two, three);
    x = n + i;
    
    board.Add( new ns_whiteboard::rect( ns_whiteboard::xy_coords(x + x_offset, y + y_offset + w), 
					ns_whiteboard::xy_coords((x+1) + x_offset, y + y_offset),
					col) );

    if (x > x_max)
      x_max = x;
  }
  if (y + w > y_max)
    y_max = y + w;


  y += 1.2*w;

  board.Add( new ns_whiteboard::text( ns_whiteboard::xy_coords(x_offset, y + y_offset),
				      "Colour scale -1 to +1 (anything)", black, 6., "Times-Roman", 0, true));
  y += w;
  */
  //====================================================================


  n *=2;
  for (i=0; i<=n; i++) {
    double v = (double)i/(double)n;

    color one = color(0.5, 0., 0.1);
    color two = color(0.99, 0.99, 0.);
    color col = Gradient(v, two, one);
    x = i;
    
    board.Add( new ns_whiteboard::rect( ns_whiteboard::xy_coords(x + x_offset, y + y_offset + w), 
					ns_whiteboard::xy_coords((x+1) + x_offset, y + y_offset),
					col) );

    if (x > x_max)
      x_max = x;
  }
  if (y + w > y_max)
    y_max = y + w;

  y += 1.2*w;

  board.Add( new ns_whiteboard::text( ns_whiteboard::xy_coords(x_offset, y + y_offset),
				      "Colour gradient", black, 6., "Times-Roman", 0, true));



  ofstream out(o.c_str());
  
  ns_whiteboard::ps_display display(out, x_max + 2 * x_offset, y_max + 2 * y_offset);
  board.DisplayOn(&display);
 

  return 0;
}
