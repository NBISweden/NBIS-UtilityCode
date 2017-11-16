#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "visual/Whiteboard.h"
#include <math.h>

class Geometry3D
{
 public:
  Geometry3D() {
    m_dist = 500.;
    m_phi = 0.;
    m_theta = 0.;
    m_offset = 20;
  }

  void SetDist(double d) {
    m_dist = d;
  }

  void SetRotation(double phi, double theta) {
    m_phi = phi;
    m_theta = theta;
  }

  void SetOffset(double d) {
    m_offset = d;
  }

  // Note: the z axis is vertical by default
  ns_whiteboard::xy_coords Coords(double xr, double yr, double zr) {
    //cout << "x=" << xr << " y=" << yr << " z=" << zr << " -> " ;
    double x = xr*cos(m_theta)*cos(m_phi) + yr*(cos(m_theta)*sin(m_phi)+sin(m_phi)*sin(m_theta)*cos(m_phi)) + zr*(sin(m_phi)*sin(m_theta) - cos(m_phi)*sin(m_theta)*cos(m_phi));

    double y = xr*cos(m_theta)*sin(m_phi) + yr*(cos(m_phi)*cos(m_theta)-sin(m_phi)*sin(m_theta)*sin(m_phi)) + zr*(sin(m_phi)*cos(m_theta)+cos(m_phi)*sin(m_theta)*sin(m_phi));

    double z = xr*sin(m_theta) - yr*sin(m_phi)*cos(m_theta) + zr*cos(m_phi)*cos(m_theta);

    //cout << "x=" << x << " y=" << y << " z=" << z << endl;

    double d = sqrt(x*x + z*z + m_dist*m_dist);
    cout << "d=" << d << endl;
    double x1 = x*m_dist/(y+d);
    double y1 = z*m_dist/(y+d);

    //cout << "x=" << x << " y=" << y << " z=" << z << " -> " ;
    //cout << "Project x=" << x1 + m_offset << " y=" << y1 + m_offset << endl;
    return ns_whiteboard::xy_coords(x1+m_offset, y1+m_offset);
  } 

 private:
  double m_phi;
  double m_theta;
  double m_dist;
  double m_offset;

};





#endif //GEOMETRY_H

