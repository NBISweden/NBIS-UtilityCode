#include "visual/Whiteboard.h"

#include "base/CommandLineParser.h"
#include "base/FileParser.h"
#include "base/SVector.h"
#include "visual/Color.h"

#include "visual/Axes.h"

#include <iostream>

int main( int argc, char** argv )
{
  //No call to RunTime() made in order to allow clean ctrl-C exit.
 
  commandArg<string> aStringI1("-i","Matrix file");
  commandArg<string> aStringO("-o","outfile (post-script)");
  commandArg<bool> bwCmd("-bw","grey-scale only", 0);

  
  commandLineParser P(argc,argv);
  P.SetDescription("Heat map plotter");

  P.registerArg(aStringI1);
  P.registerArg(aStringO);
  P.registerArg(bwCmd);

  P.parse();

  string in = P.GetStringValueFor(aStringI1);
  string o = P.GetStringValueFor(aStringO);
  bool bBW = P.GetBoolValueFor(bwCmd);
  
  double x_offset = 20;
  double y_offset = 20;


  int i, j;
  ns_whiteboard::whiteboard board;

  FlatFileParser parser;
  
  parser.Open(in);
  

  double x_max = 0;
  double y_max = 0;

  double space = 50;
  double dot = 5;

  double x = 0;
  double y = 0;

  while(parser.ParseLine()) {
    if (parser.GetItemCount() < 1)
      continue;
    
    //if (parser.AsString(0) != "Frame")
    //continue;

    cout << "Adding plot " << parser.Line() << endl;
    string caption = parser.Line();
    //parser.ParseLine();
    // Get the fish names
    parser.ParseLine();
    svec<string> names;
    names.resize(parser.GetItemCount());
    for (i=0; i<parser.GetItemCount(); i++)
      names[i] = parser.AsString(i);

    cout << "Read names: " << names.isize() << endl;
    
    int n = -1;
    j = 0;

  

    while(parser.ParseLine()) {
      if (n == -1)
	n = parser.GetItemCount()-1;

      double localX = 0;
      double localY = 0;
      
      for (i=0; i<parser.GetItemCount()-1; i++) {
	localX = x + i * dot;
	localY = y + j * dot;

	double r = 0.;
	double g = 0.;
	double b = 0.;

	double val = parser.AsFloat(i+1);
	
	/*
	val /= 3;
	if (val > 1.)
	  val = 1.;
	if (val < -1.)
	  val = -1;
	*/

	if (bBW) {
	  r = g = b = 1. - val;


	} else {

	  val = 1. - 2*val;
	  
	  if (val > 0)
	    g = val;
	  else
	    r = -val;
	}
	
	color black_c(r, g, b);
	//cout << "r=" << r << endl;
	//cout << "Adding at " << localX << endl;
	board.Add( new ns_whiteboard::rect( ns_whiteboard::xy_coords(localX + x_offset, localY + y_offset + dot), 
					    ns_whiteboard::xy_coords(localX + x_offset + dot, localY + y_offset),
					    black_c) );

	if (n == 1) {
	  board.Add( new ns_whiteboard::text( ns_whiteboard::xy_coords(localX + x_offset + dot/2, localY + y_offset + 2 * dot),
					      names[i], black, 3., "Times-Roman", 70, true));
	}
 
	if (localX + x_offset + dot > x_max)
	  x_max = localX + x_offset + dot;
	if (localY + y_offset + dot > y_max)
	  y_max = localY + y_offset + dot;
      }

      board.Add( new ns_whiteboard::text( ns_whiteboard::xy_coords(localX + x_offset + 2 * dot, localY + y_offset + dot / 2),
					  names[j], black, 3., "Times-Roman", 0, true));

      n--;
      j++;

      
      if (n == 0) {
	break;
      }
    }
    board.Add( new ns_whiteboard::text( ns_whiteboard::xy_coords(x_offset + x, y_offset - 2*dot + y),
					caption, black, 8., "Times-Roman", 0, true));

    x += parser.GetItemCount() * dot + space;
    if (x > 400) {
      x = 0;
      y +=  parser.GetItemCount() * dot + space;
    }
  }

  cout << "xmax=" << x_max << "  ymax=" << y_max << endl;
  ofstream out(o.c_str());
  
  ns_whiteboard::ps_display display(out, x_max + 2 * x_offset + space, y_max + 2 * y_offset + space);
  board.DisplayOn(&display);
 

  return 0;
}
