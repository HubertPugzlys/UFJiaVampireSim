//------------------------------------------------------------------------------
//
//   This file is part of the VAMPIRE open source package under the
//   Free BSD licence (see licence file for details).
//
//   (c) Richard F L Evans 2017. All rights reserved.
//
//   Email: richard.evans@york.ac.uk
//
//------------------------------------------------------------------------------
//

// C++ standard library headers
#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>
#include <cstdlib>

// program header
#include "vdc.hpp"

namespace vdc{

// forward function declarations
double scale( double scale_min, double scale_max, double start_min, double start_max, double x );
void rgb2hsl( double& red, double& green, double& blue, double& hue, double& light, double& saturation );
void rgb2hsi( double& red, double& green, double& blue, double& h2, double& intensity, double& saturation );
void hsl2rgb( double& red, double& green, double& blue, double& hue, double& light, double& saturation );
void hsi2rgb( double& red, double& green, double& blue, double& hue, double& intensity, double& saturation );
void interpolate(double xy_angle, double& red, double& green, double& blue );


void rgb( const double& sx, const double& sy, const double& sz, double& red, double& green, double& blue){
   // parameters
   const double pi  = 3.14159265358979323846;

   // variables
   double xy_angle, z_angle, hue, light, saturation;
   double sx2, sy2, sz2; // spin coordinates after change of axes
   double sx1, sy1, sz1; ///
   sx1 = 0.0; ///
   sy1 = 0.0; ///
   sz1 = 0.5; ///
   // perform change of coordinates
   sx2 = vdc::vector_x[0]*sx + vdc::vector_x[1]*sy + vdc::vector_x[2]*sz;
   sy2 = vdc::vector_y[0]*sx + vdc::vector_y[1]*sy + vdc::vector_y[2]*sz;
   sz2 = vdc::vector_z[0]*sx + vdc::vector_z[1]*sy + vdc::vector_z[2]*sz;
   //std::cout << "sx2 = " << sx2 << "\tsy2 = " << sy2 << "\tsz2 = " << sz2 << std::endl;
   // in x,y plane, angle = the spin direction
   xy_angle = std::atan2(sy2,sx2);
   //std::cout << "xy_angle = " << xy_angle << std::endl;
   xy_angle = std::fmod(xy_angle + 2*pi, 2*pi);  // range [0-2pi]
   //std::cout << "xy_angle = " << xy_angle << std::endl;

   // to apply colourmap, need value between 0-255
   xy_angle = scale(0.0, 255.0, 0.0, 2.0*pi, xy_angle);
   //std::cout << "xy_angle = " << xy_angle << std::endl;
   // find rgb values
   interpolate( xy_angle, red, green, blue );
   //std::cout << "red = " << red << std::endl << "green = " << green << std::endl << "blue = " << blue << std::endl;
   // temp values for hue, saturation and light
   hue = 0.0;
   saturation = 0.0;
   light = 0.0;

   // we have resolved xy plane colours. to adjust bightness (x-axis) we
   // convert to HSL
   rgb2hsl(red, green, blue, hue, light, saturation);
   //std::cout << "hue = " << hue << std::endl << "light = " << light << std::endl << "sat = " << saturation << std::endl;
   // adjust lightness to reflect z-axis orientation
   // find angle w.r.t. z-axis [value -pi/2 to pi/2]
   z_angle = std::atan(sz2/std::sqrt(sx2*sx2 + sy2*sy2));
   if ( z_angle >= 0.0 ){
      light = light + (1.0 - light)*(2.0*z_angle/pi);
   }
   else {
      light = light + light*(2.0*z_angle/pi);
   }
   //std::cout << "z_angle = " << z_angle << std::endl;
   //std::cout << "hue = " << hue << std::endl << "light = " << light << std::endl << "sat = " << saturation << std::endl;
   // convert back to rgb to get final colour
   hsl2rgb(red, green, blue, hue, light, saturation);

   if ( red < 0.0 || red > 1.0 ){
      std::cout << "Error red = " << red << std::endl;
   }
   if ( green < 0.0 || green > 1.0 ){
      std::cout << "Error green = " << green << std::endl;
   }
   if ( blue < 0.0 || blue > 1.0 ){
      std::cout << "Error blue = " << blue << std::endl;
   }
   //std::exit(EXIT_FAILURE);
   return;
}

//------------------------------------------------------------------------------
// Scale a data set from range [start_min - start_max]
// to [scale_min - scale_max] with even bias
//------------------------------------------------------------------------------
double scale( double scale_min, double scale_max, double start_min, double start_max, double x ){

   return ((scale_max - scale_min)*(x - start_min)/(start_max - start_min)) + scale_min;
}

//------------------------------------------------------------------------------
// Convert RGB colour to HSL
//------------------------------------------------------------------------------
void rgb2hsl( double& red, double& green, double& blue, double& hue, double& light, double& saturation ){

   // function specific variables
   double maximum, minimum, chroma;

   // variables used to work out hue
   maximum = std::max(std::max(red,green),blue);
   minimum = std::min(std::min(red,green),blue);
   chroma  = maximum - minimum;

   // hue, depends on maximum value
   if ( (chroma <= 0.000001) && (chroma >= -0.000001) ){
      hue = 0.0;
   }
   else if ( (maximum <= (red + 0.000001)) && (maximum >=  (red - 0.000001)) ){
      hue = std::fmod(std::fmod( (green - blue)/chroma, 6.0 ) + 6.0, 6.0);
   }
   else if ( (maximum <= (green + 0.000001)) && (maximum >=  (green - 0.000001)) ){
      hue = (blue - red)/chroma + 2.0;
   }
   else if ( (maximum <= (blue + 0.000001)) && (maximum >=  (blue - 0.000001)) ){
      hue = (red - green)/chroma + 4.0;
   }
   hue = hue*60; // should be unnecessary as undone later

   // lightness
   light = 0.5*(maximum + minimum);

   // saturation
   if ( (light <= 1.000001) && (light >= (1.0 - 0.000001)) ){
      saturation = 0.0;
   }
   else {
      saturation = chroma/(1.0 - std::abs(2.0*light - 1.0));
   }

   return;
}

//------------------------------------------------------------------------------
// Convert RGB to HSI
//------------------------------------------------------------------------------
void rgb2hsi( double& red, double& green, double& blue, double& h2, double& intensity, double& saturation ){

   const double pi  = 3.14159265358979323846;
   double alpha, beta, c2;

   // calculate hue (h2) and chroma (c2)
   alpha = 0.5*( 2.0*red - green - blue );
   beta  = 0.5*std::sqrt(3.0)*( green - blue );

   h2 = std::atan2(beta,alpha);
   h2 = std::fmod(h2 + 2*pi, 2*pi);
   h2 = h2*180.0/pi;

   c2 = std::sqrt( alpha*alpha + beta*beta );

   // calculate intensity
   intensity = ( red + green + blue )/3.0;

   // calculate saturation
   if ( (intensity <= 0.000001) && (intensity >= -0.000001) ){
      saturation = 0.0;
   }
   else {
      saturation = 1.0 - std::min(std::min(red,green),blue)/intensity;
   }

   return;
}

//------------------------------------------------------------------------------
// Convert HSL colour to RGB
//------------------------------------------------------------------------------
void hsl2rgb( double& red, double& green, double& blue, double& hue, double& light, double& saturation ){

   // function specific variables
   double chroma, temp_x, modifier;

   // first find new chroma
   chroma = (1.0 - std::abs(2.0*light - 1.0))*saturation;

   // hue
   hue    = hue/60;
   temp_x = chroma*(1.0 - std::abs( std::fmod(hue, 2.0) - 1.0));
   if ( (hue < 0.0) || (hue > 6.0) ){
      red   = 0.0;
      green = 0.0;
      blue  = 0.0;
   }
   else if ( (0.0 <= hue) && (hue <= 1.0) ){
      red   = chroma;
      green = temp_x;
      blue  = 0.0;
   }
   else if ( (1.0 <= hue) && (hue <= 2.0) ){
      red   = temp_x;
      green = chroma;
      blue  = 0.0;
   }
   else if ( (2.0 <= hue) && (hue <= 3.0) ){
      red   = 0.0;
      green = chroma;
      blue  = temp_x;
   }
   else if ( (3.0 <= hue) && (hue <= 4.0) ){
      red   = 0.0;
      green = temp_x;
      blue  = chroma;
   }
   else if ( (4.0 <= hue) && (hue <= 5.0) ){
      red   = temp_x;
      green = 0.0;
      blue  = chroma;
   }
   else if ( (5.0 <= hue) && (hue <= 6.0) ){
      red   = chroma;
      green = 0.0;
      blue  = temp_x;
   }

   // finally we adjust the values by a constant
   modifier = light - 0.5*chroma;
   red      = red   + modifier;
   green    = green + modifier;
   blue     = blue  + modifier;

   return;
}

//------------------------------------------------------------------------------
// Convert HSL colour to RGB
//------------------------------------------------------------------------------
void hsi2rgb( double& red, double& green, double& blue, double& hue, double& intensity, double& saturation ){

   double z, chroma, temp_x, modifier;

   hue    = hue/60;
   z      = 1 - std::abs( std::fmod(hue, 2.0) - 1.0 );
   chroma = (intensity*saturation)/(1.0 + z);
   temp_x = chroma*z;

   if ( (hue < 0.0) || (hue > 6.0) ){
      red   = 0.0;
      green = 0.0;
      blue  = 0.0;
   }
   else if ( (0.0 <= hue) && (hue <= 1.0) ){
      red   = chroma;
      green = temp_x;
      blue  = 0.0;
   }
   else if ( (1.0 <= hue) && (hue <= 2.0) ){
      red   = temp_x;
      green = chroma;
      blue  = 0.0;
   }
   else if ( (2.0 <= hue) && (hue <= 3.0) ){
      red   = 0.0;
      green = chroma;
      blue  = temp_x;
   }
   else if ( (3.0 <= hue) && (hue <= 4.0) ){
      red   = 0.0;
      green = temp_x;
      blue  = chroma;
   }
   else if ( (4.0 <= hue) && (hue <= 5.0) ){
      red   = temp_x;
      green = 0.0;
      blue  = chroma;
   }
   else if ( (5.0 <= hue) && (hue <= 6.0) ){
      red   = chroma;
      green = 0.0;
      blue  = temp_x;
   }

   modifier = intensity*(1.0 - saturation)/3.0;
   red      = 3*(red   + modifier);
   green    = 3*(green + modifier);
   blue     = 3*(blue  + modifier);

   return;
}

//------------------------------------------------------------------------------
// Find point between integer colorvals using y=mx+c
// x-values [0-255], y-values rgb [0-1]
//------------------------------------------------------------------------------
void interpolate( double xy_angle, double& red, double& green, double& blue ){

   std::vector<std::vector<double>> colourmap(256, std::vector<double>(3));
   std::vector<double> m(3), c(3), ymin(3), ymax(3);
   int xmin, xmax;

   // values between which we interpolate
   xmin = int(std::floor(xy_angle));
   xmax = int(std::ceil(xy_angle));
   //std::cout << "xmin = " << xmin << std::endl;
   //std::cout << "xmax = " << xmax << std::endl;
   // initialise colourwheel
   vdc::colourwheel( colourmap );

   // find colorval associated with min and max
   ymin = colourmap[xmin];
   ymax = colourmap[xmax];
   //std::cout << "ymin = " << ymin[0] << " " << ymin[1] << " " << ymin[2] << std::endl;
   //std::cout << "ymax = " << ymax[0] << " " << ymin[1] << " " << ymin[2] << std::endl;
   // if different: work out gradients and intersects
   if ( ymin == ymax ) {
      red   = ymin[0];
      green = ymin[1];
      blue  = ymin[2];
   }
   else {
      for ( int i=0; i < 3; i++){
         m[i] = (ymax[i] - ymin[i])/(xmax - xmin);
         c[i] = ymin[i] - m[i]*xmin;
      }
      //std::cout << "m = " << m[0] << " " << m[1] << " " << m[2] << std::endl;
      //std::cout << "c = " << c[0] << " " << c[1] << " " << c[2] << std::endl;
      // input angle into y=mx+c
      red   = m[0]*xy_angle + c[0];
      green = m[1]*xy_angle + c[1];
      blue  = m[2]*xy_angle + c[2];
   }

   return;
}

} // end of namespace vdc
