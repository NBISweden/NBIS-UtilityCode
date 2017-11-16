

#ifndef COLOR_H
#define COLOR_H

#include <iostream>
using namespace std;


class color {
    
  public:
    
    color( float r, float g, float b ) : r_(r), g_(g), b_(b) { }
    
    color( ) :
      r_(0.0),
      g_(0.0),
      b_(0.0)
    { }

    float R( ) const { return r_; }
    float G( ) const { return g_; }
    float B( ) const { return b_; }
    
    friend bool operator==( const color& lhs, const color& rhs )
      { return ( lhs.r_ == rhs.r_ && lhs.g_ == rhs.g_ && lhs.b_ == rhs.b_); }
    
    friend bool operator!=( const color& lhs, const color& rhs )
      { return !( lhs == rhs ); }
    
    friend std::ostream& operator<<( std::ostream& o, const color& c);
    
  private:
    
    float r_, g_, b_;
  };
  
  const color white(1, 1, 1), black(0, 0, 0);
  const color red(1, 0, 0), green(0, 1, 0), blue(0, 0, 1);

  const color pink(1, 0.5, 0.5);
  const color lighterpink(1, 0.8, 0.8);

  const color magenta(1, 0, 1), yellow(1, 1, 0), cyan(0, 1, 1);

  const color gray(0.5, 0.5, 0.5);
  const color darkergray(0.3, 0.3, 0.3);
  const color darkgray(0.3, 0.3, 0.3);
  const color lightgray(0.6, 0.6, 0.6);
  const color lightergray(0.8, 0.8, 0.8);

  const color azure(0, 0.5, 1);
  const color orangered(1, 0.27, 0);
  const color seashell(1, 0.96, 0.93);
  const color limegreen(0.2, 0.8, 0.2);
  const color wheat(0.96, 0.87, 0.7);



color MakeUpColor(int num);

// Returns the gradient color in between base and back
// Use this for e.g. density plots
inline color Gradient(double val, const color & base, const color & back = color(0.99, 0.99, 0.99))
{
  if (val > 0.999)
    val = 0.999;
  double r = base.R() * val + back.R() * (1.-val);
  double g = base.G() * val + back.G() * (1.-val);
  double b = base.B() * val + back.B() * (1.-val);
  return color(r, g, b);
}

// Returns the gradient via an intermediate on a scale between -1 and +1, e.g. for heat maps
inline color GradientMult(double val, const color & neg, const color & pos, const color & back = color(0.99, 0.99, 0.99))
{
  if (val >= 0) {
    if (val > 0.999)
      val = 0.999;
    double r = pos.R() * val + back.R() * (1.-val);
    double g = pos.G() * val + back.G() * (1.-val);
    double b = pos.B() * val + back.B() * (1.-val);
    return color(r, g, b);
  } else {
    val = -val;
    if (val > 0.999)
      val = 0.999;
    double r = neg.R() * val + back.R() * (1.-val);
    double g = neg.G() * val + back.G() * (1.-val);
    double b = neg.B() * val + back.B() * (1.-val);
    return color(r, g, b);

  }
}

#endif
