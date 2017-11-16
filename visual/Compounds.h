#ifndef COMPOUNDS_H
#define COMPOUNDS_H

#include "visual/Whiteboard.h"
#include <math.h>

class Box
{
 public:
  Box(double width = 1., const color & color = color(0, 0, 0)) {
    m_width = width;
    m_col = color;
  }

  void Draw(ns_whiteboard::whiteboard & board,
	    const ns_whiteboard::xy_coords & from, 
	    const ns_whiteboard::xy_coords & to) {
    
    board.Add( new ns_whiteboard::line( ns_whiteboard::xy_coords(from.first, from.second), 
					ns_whiteboard::xy_coords(from.first, to.second), 
					m_width,
					m_col) );
    board.Add( new ns_whiteboard::line( ns_whiteboard::xy_coords(from.first, to.second), 
					ns_whiteboard::xy_coords(to.first, to.second), 
					m_width,
					m_col) );
    board.Add( new ns_whiteboard::line( ns_whiteboard::xy_coords(to.first, to.second), 
					ns_whiteboard::xy_coords(to.first, from.second), 
					m_width,
					m_col) );
    board.Add( new ns_whiteboard::line( ns_whiteboard::xy_coords(to.first, from.second), 
					ns_whiteboard::xy_coords(from.first, from.second), 
					m_width,
					m_col) );
       
  }

  void Draw(ns_whiteboard::whiteboard & board,
	    const ns_whiteboard::xy_coords & a, 
	    const ns_whiteboard::xy_coords & b, 
	    const ns_whiteboard::xy_coords & c, 
	    const ns_whiteboard::xy_coords & d) {

    board.Add( new ns_whiteboard::line( ns_whiteboard::xy_coords(a.first, a.second), 
					ns_whiteboard::xy_coords(b.first, b.second), 
					m_width,
					m_col) );
    board.Add( new ns_whiteboard::line( ns_whiteboard::xy_coords(b.first, b.second), 
					ns_whiteboard::xy_coords(c.first, c.second), 
					m_width,
					m_col) );
    board.Add( new ns_whiteboard::line( ns_whiteboard::xy_coords(c.first, c.second), 
					ns_whiteboard::xy_coords(d.first, d.second), 
					m_width,
					m_col) );
    board.Add( new ns_whiteboard::line( ns_whiteboard::xy_coords(d.first, d.second), 
					ns_whiteboard::xy_coords(a.first, a.second), 
					m_width,
					m_col) );
  }



 private:
  double m_width;
  color m_col;
};


class Arrow
{
 public:
  Arrow(double size_off = 8.,
	double size_back = 10.,
	double width = 1., 
	const color & color = color(0, 0, 0)) {
    m_width = width;
    m_col = color;
    m_size_off = size_off;
    m_size_back = size_back;
  }

  void Draw(ns_whiteboard::whiteboard & board,
	    const ns_whiteboard::xy_coords & from, 
	    const ns_whiteboard::xy_coords & to) {

    board.Add( new ns_whiteboard::line( from, 
					to, 
					m_width,
					m_col) );
    double x = to.first - from.first;
    double y = to.second - from.second;

    //double phi = 0.;
    //if (x != 0.)
    //phi = atan(y/x);
    double phi = acos(x/sqrt(x*x+y*y));
    //cout << phi << endl;

    double size = 10 * m_width;
    double r = sqrt(x*x+y*y);

    double x1 = r - m_size_back;
    double y1 = m_size_off;
    double x2 = r - m_size_back;
    double y2 = -m_size_off;
    //cout << "x1=" << x1 << " y1=" << y1 << endl;
   
    double rx1 = from.first + x1 * cos(phi) - y1 * sin(phi);
    double ry1 = from.second + x1 * sin(phi) + y1 * cos(phi);

    double rx2 = from.first + x2 * cos(phi) - y2 * sin(phi);
    double ry2 = from.second + x2 * sin(phi) + y2 * cos(phi);

    board.Add( new ns_whiteboard::line( to, 
					ns_whiteboard::xy_coords(rx1, ry1), 
					m_width,
					m_col) );

    board.Add( new ns_whiteboard::line( to, 
					ns_whiteboard::xy_coords(rx2, ry2), 
					m_width,
					m_col) );
    }


 private:
  double m_width;
  double m_size_off;
  double m_size_back;
  
  color m_col;
};




#endif


