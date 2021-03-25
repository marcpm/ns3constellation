#include "orbital-coords.h"
#include "ns3/vector.h"
#include "ns3/double.h"
#include "ns3/log.h"
#include "ns3/integer.h"

#include "ns3/geographic-positions.h" // geo converter


#define _USE_MATH_DEFINES
#include <cmath>

namespace ns3 {



/**
    * Default constructor
    */
Topos::Topos()
    : azimuth(0.0)
    , elevation(0.0)
    , range(0.0)
    , range_rate(0.0)
{
}

/**
    * Constructor
    * @param[in] az azimuth in radians
    * @param[in] el elevation in radians
    * @param[in] rnge range in kilometers
    * @param[in] rnge_rate range rate in kilometers per second
    */
Topos::Topos(
        double az,
        double el,
        double rnge,
        double rnge_rate)
    : azimuth(az)
    , elevation(el)
    , range(rnge)
    , range_rate(rnge_rate)
{
    m_azRad = azimuth * M_PI / 180;
    m_elRad = elevation * M_PI / 180;
}

Topos::Topos(
        double az,
        double el,
        double rnge,
        )
    : azimuth(az)
    , elevation(el)
    , range(rnge)
    , range_rate(0.0)

{
    m_azRad = azimuth * M_PI / 180;
    m_elRad = elevation * M_PI / 180;
}


/**
    * Copy constructor
    * @param[in] topo object to copy from
    */
Topos::Topos(const Topos& topo)
{
    azimuth = topo.azimuth;
    elevation = topo.elevation;
    range = topo.range;
    range_rate = topo.range_rate;

    m_azRad = azimuth * M_PI / 180;
    m_elRad = elevation * M_PI / 180;
}

/**
    * Assignment operator
    * @param[in] topo object to copy from
    */

Topos 
operator=(const Topos& topo)
{
    if (this != &topo)
    {
        azimuth = topo.azimuth;
        elevation = topo.elevation;
        range = topo.range;
        range_rate = topo.range_rate;
        m_azRad = azimuth * M_PI / 180;
        m_elRad = elevation * M_PI / 180;
    }
    return *this;
}

/**
    * Dump this object to a string
    * @returns string
    */

std::string 
Topos::ToString() const
{
    std::stringstream ss;
    ss << std::right << std::fixed << std::setprecision(3);
    ss << "Az: " << std::setw(8) << Util::RadiansToDegrees(azimuth);
    ss << ", El: " << std::setw(8) << Util::RadiansToDegrees(elevation);
    ss << ", Rng: " << std::setw(10) << range;
    ss << ", Rng Rt: " << std::setw(7) << range_rate;
    return ss.str();
}

    











/* -----------------------------------------------------------------------------
*
*                           function gstime
*
*  this function finds the greenwich sidereal time.
*
*  author        : david vallado                  719-573-2600    1 mar 2001
*
*  inputs          description                    range / units
*    jdut1       - julian date in ut1             days from 4713 bc
*
*  outputs       :
*    gstime      - greenwich sidereal time        0 to 2pi rad
*
*  locals        :
*    temp        - temporary variable for doubles   rad
*    tut1        - julian centuries from the
*                  jan 1, 2000 12 h epoch (ut1)
*
*  coupling      :
*    none
*
*  references    :
*    vallado       2004, 191, eq 3-45
* --------------------------------------------------------------------------- 
*/

double  gstime
        (
          double jdut1
        )
   {
     const double twopi = 2.0 * PI;
     const double deg2rad = PI / 180.0;
     double       temp, tut1;

     tut1 = (jdut1 - 2451545.0) / 36525.0;
     temp = -6.2e-6* tut1 * tut1 * tut1 + 0.093104 * tut1 * tut1 +
             (876600.0*3600 + 8640184.812866) * tut1 + 67310.54841;  // sec
     temp = fmod(temp * deg2rad / 240.0, twopi); //360/86400 = 1/240, to deg, to rad

     // ------------------------ check quadrants ---------------------
     if (temp < 0.0)
         temp += twopi;

     return temp;
   }  // end gstime




/*
polarm
This function calulates the transformation matrix that accounts for polar
motion. Polar motion coordinates are estimated using IERS Bulletin
rather than directly input for simplicity.
Author: David Vallado, 2007
Ported to C++ by Grady Hillhouse with some modifications, July 2015.
INPUTS          DESCRIPTION                     RANGE/UNITS
jdut1           Julian date                     days
OUTPUTS         DESCRIPTION
pm              Transformation matrix for ECEF - PEF
*/

void 
polarm(double jdut1, double pm[3][3])
{
    double MJD; //Julian Date - 2,400,000.5 days
    double A;
    double C;
    double xp; //Polar motion coefficient in radians
    double yp; //Polar motion coefficient in radians

    double pm[3][3];

    
    //Predict polar motion coefficients using IERS Bulletin - A (Vol. XXVIII No. 030)
    MJD = jdut1 - 2400000.5;
    A = 2 * PI * (MJD - 57226) / 365.25;
    C = 2 * PI * (MJD - 57226) / 435;
    
    xp = (0.1033 + 0.0494*cos(A) + 0.0482*sin(A) + 0.0297*cos(C) + 0.0307*sin(C)) * 4.84813681e-6;
    yp = (0.3498 + 0.0441*cos(A) - 0.0393*sin(A) + 0.0307*cos(C) - 0.0297*sin(C)) * 4.84813681e-6;
    
    // pm = (rot1(yp)*rot2(xp))
    // with rot1(a) = 1 ,     0  ,     0  ;
    //                0 , cos(a) , sin(a) ;
    //                0 , -sin(a) , cos(a) ;
    //  and rot2(a) = cos(a) , 0 , -sin(a) ;
    //                    0  , 1 ,      0  ;
    //                sin(a) , 0 ,  cos(a) ;
    //
    // transponiert:
    // pm_T =  cos(xp) , sin(xp)sin(yp) ,  cos(yp)sin(xp)  ;
    //        0        ,        cos(yp) , -sin(yp)         ;
    //        -sin(xp) , cos(xp)sin(yp) ,  cos(yp)*cos(xp) ;

    // row major
    pm[0][0] = cos(xp);
    pm[0][1] = 0.0;
    pm[0][2] = -sin(xp);
    pm[1][0] = sin(xp) * sin(yp);
    pm[1][1] = cos(yp);
    pm[1][2] = cos(xp) * sin(yp);
    pm[2][0] = sin(xp) * cos(yp);
    pm[2][1] = -sin(yp);
    pm[2][2] = cos(xp) * cos(yp);
}




/*
teme2ecef
This function transforms a vector from a true equator mean equinox (TEME)
frame to an earth-centered, earth-fixed (ECEF) frame.
Author: David Vallado, 2007
Ported to C++ by Grady Hillhouse with some modifications, July 2015.
INPUTS          DESCRIPTION                     RANGE/UNITS
rteme           Position vector (TEME)          km
vteme           Velocity vector (TEME)          km/s
jdut1           Julian date                     days
OUTPUTS         DESCRIPTION                     RANGE/UNITS
recef           Position vector (ECEF)          km  
vecef           Velocity vector (ECEF)          km/s
*/
PVCoords
teme2ecef(const double rteme[3], const double vteme[3], double jdut1)
{
    double gmst;
    double st[3][3];
    double rpef[3];
    double vpef[3];
    double pm[3][3];
    double omegaearth[3];
    
    //Get Greenwich mean sidereal time
    gmst = gstime(jdut1);
    
    //st is the pef - tod matrix
    st[0][0] = cos(gmst);
    st[0][1] = -sin(gmst);
    st[0][2] = 0.0;
    st[1][0] = sin(gmst);
    st[1][1] = cos(gmst);
    st[1][2] = 0.0;
    st[2][0] = 0.0;
    st[2][1] = 0.0;
    st[2][2] = 1.0;
    
    //Get pseudo earth fixed position vector by multiplying the inverse pef-tod matrix by rteme
    rpef[0] = st[0][0] * rteme[0] + st[1][0] * rteme[1] + st[2][0] * rteme[2];
    rpef[1] = st[0][1] * rteme[0] + st[1][1] * rteme[1] + st[2][1] * rteme[2];
    rpef[2] = st[0][2] * rteme[0] + st[1][2] * rteme[1] + st[2][2] * rteme[2];
    
    //Get polar motion vector
    polarm(jdut1, pm);
    
    //ECEF postion vector is the inverse of the polar motion vector multiplied by rpef
    recef[0] = pm[0][0] * rpef[0] + pm[1][0] * rpef[1] + pm[2][0] * rpef[2];
    recef[1] = pm[0][1] * rpef[0] + pm[1][1] * rpef[1] + pm[2][1] * rpef[2];
    recef[2] = pm[0][2] * rpef[0] + pm[1][2] * rpef[1] + pm[2][2] * rpef[2];
    
    //Earth's angular rotation vector (omega)
    //Note: I don't have a good source for LOD. Historically it has been on the order of 2 ms so I'm just using that as a constant. The effect is very small.
    omegaearth[0] = 0.0;
    omegaearth[1] = 0.0;
    omegaearth[2] = 7.29211514670698e-05 * (1.0  - 0.002/86400.0);
    
    //Pseudo Earth Fixed velocity vector is st'*vteme - omegaearth X rpef
    vpef[0] = st[0][0] * vteme[0] + st[1][0] * vteme[1] + st[2][0] * vteme[2] - (omegaearth[1]*rpef[2] - omegaearth[2]*rpef[1]);
    vpef[1] = st[0][1] * vteme[0] + st[1][1] * vteme[1] + st[2][1] * vteme[2] - (omegaearth[2]*rpef[0] - omegaearth[0]*rpef[2]);
    vpef[2] = st[0][2] * vteme[0] + st[1][2] * vteme[1] + st[2][2] * vteme[2] - (omegaearth[0]*rpef[1] - omegaearth[1]*rpef[0]);
    
    //ECEF velocty vector is the inverse of the polar motion vector multiplied by vpef
    vecef[0] = pm[0][0] * vpef[0] + pm[1][0] * vpef[1] + pm[2][0] * vpef[2];
    vecef[1] = pm[0][1] * vpef[0] + pm[1][1] * vpef[1] + pm[2][1] * vpef[2];
    vecef[2] = pm[0][2] * vpef[0] + pm[1][2] * vpef[1] + pm[2][2] * vpef[2];

    PVCoords pv_ecef;
    pv_ecef.position = Vector(recef[0], recef[1], recef[2])
    pv_ecef.velocity = Vector(vecef[0], vecef[1], vecef[2])
    
    return pv_ecef;
}




} // namespace ns3




















