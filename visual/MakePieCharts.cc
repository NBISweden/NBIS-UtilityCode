#include <string>
#include "base/CommandLineParser.h"
#include "base/FileParser.h"
#include <math.h>
#include "visual/Whiteboard.h"
#include "visual/Color.h"

#include "visual/Axes.h"
#include <iostream>
#include "base/RandomStuff.h"

#include <math.h>

int main( int argc, char** argv )
{

  
  string outName = "pie.ps";

  //comment. ???
  //FlatFileParser parser;
  
  //parser.Open(fileName);
 

  double x, y;
  double offset = 10.;

  x = offset;
  y = offset;
  double dot = 3.;
  double x_max = 0;
  double y_max = 0;
  
  ns_whiteboard::whiteboard board;

  
  int i, j;
  double rad = 50.;

  FlatFileParser parser;
  
  parser.Open("data/numbers3");
  double s = 0.1;

  while (parser.ParseLine()) {
    if (parser.GetItemCount() == 0)
      continue;

    double scale = 0.01;
    double a = parser.AsFloat(0) + parser.AsFloat(1);
    cout << a << endl;
    a *= scale;
    rad = sqrt(a);
    double c = parser.AsFloat(1);
    c *= scale;

    //a = a + c;
    //b = b/a;
    a = (a - c) / a;

    //b *= 360;
    double alpha = 2 * 3.1415962 * a;
    a *= 360;

    //for (int i=0; i<1; i++) {

    double r = 0.;
    double g = 0.;
    double b = 0.;
    
    color col(r, g, b); 
    color col2(0.99, 0., b); 
    
    x = offset + rad;
    y += 3*rad + 2;

    if (x + rad > x_max)
      x_max = x + rad;
    if (y + rad > y_max)
      y_max = y + rad;

    //board.Add( new ns_whiteboard::arc( ns_whiteboard::xy_coords(x, y), 
    //				       rad, 0, 360, s,
    //				       col) );
   
    board.Add( new ns_whiteboard::arc( ns_whiteboard::xy_coords(x, y), 
				       rad, 0, a, s,
				       col) );
    board.Add( new ns_whiteboard::arc( ns_whiteboard::xy_coords(x, y), 
				       rad, a, 360, s,
				       col2) );

    double y1 = rad*sin(alpha);
    double x1 = rad*cos(alpha);

    board.Add( new ns_whiteboard::line( ns_whiteboard::xy_coords(x, y), 
					ns_whiteboard::xy_coords(x + x1, y + y1),
					s, col) );




    board.Add( new ns_whiteboard::line( ns_whiteboard::xy_coords(x, y), 
					ns_whiteboard::xy_coords(x + rad, y),
					s, col) );
    
    
  }

  ofstream out(outName.c_str());
  
  ns_whiteboard::ps_display display(out, x_max+2*offset, y_max + 2*offset);
  board.DisplayOn(&display);
 

  return 0;
}
