#include "basic-orbital.h"
#include "ns3/vector.h"
#include "ns3/double.h"
#include "ns3/log.h"
#include "ns3/integer.h"

#include "ns3/geographic-positions.h" // geo converter


#define _USE_MATH_DEFINES
#include <cmath>

// #include "sgp4math.h"

namespace ns3 {



std::string 
Topos::ToString()
{
    std::stringstream ss;
    ss << std::right << std::fixed << std::setprecision(3);
    ss << "Az: " << std::setw(8) << azimuth << "[deg]";
    ss << ", El: " << std::setw(8) << elevation << "[deg]";
    ss << ", Rng: " << std::setw(10) << range << "[km]";;
    // ss << ", Rng Rt: " << std::setw(7) << rangedot;
    return ss.str();
}

std::string 
CoordGeodetic::ToString()
{
    std::stringstream ss;
    ss << std::right << std::fixed << std::setprecision(3);
    ss << "Lat: " << std::setw(8) << latitude << "[deg]";
    ss << ", Lon: " << std::setw(8) << longitude << "[deg]";
    ss << ", Alt: " << std::setw(10) << altitude << "[km]";
    // ss << ", Rng Rt: " << std::setw(7) << rangedot;
    return ss.str();
}


// ******************************************************
// ******************************************************
// SGPEXT + C based funcs . Not clean


double  sgn
        (
          double x
        )
   {
     if (x < 0.0)
       {
          return -1.0;
       }
       else
       {
          return 1.0;
       }

   }  // end sgn

double  mag
        (
          const double x[3]
        )
   {
     return sqrt(x[0]*x[0] + x[1]*x[1] + x[2]*x[2]);
   }

double  dot
        (
          double x[3], double y[3]
        )
   {
     return (x[0]*y[0] + x[1]*y[1] + x[2]*y[2]);
   } 






/*

*****************************************************************************
*/



/*
site
This function finds the position and velocity vectors for a site. The
outputs are in the ECEF coordinate system. Note that the velocity vector
is zero because the coordinate system rotates with the earth.
Author: David Vallado, 2007
Ported to C++ by Grady Hillhouse with some modifications, July 2015.
INPUTS          DESCRIPTION                     RANGE/UNITS
latgd           Site geodetic latitude          -PI/2 to PI/2 in radians
lon             Longitude                       -2PI to 2PI in radians
alt             Site altitude                   m
OUTPUTS         DESCRIPTION
rs              Site position vector            m
vs              Site velocity vector            m/s
*/

void site(double latgd, double lon, double alt, double rs[3], double vs[3])
{
    double sinlat, re, eesqrd, cearth, rdel, rk;
    re = 6378137;              //radius of earth in m
    eesqrd = 0.006694385000;    //eccentricity of earth sqrd wgs84
    
    //Find rdel and rk components of site vector
    sinlat = sin(latgd);
    cearth = re / sqrt( 1.0 - (eesqrd*sinlat*sinlat) );
    rdel = (cearth + alt) * cos(latgd);
    rk = ((1.0 - eesqrd) * cearth + alt ) * sinlat;
    
    //Find site position vector (ECEF)
    rs[0] = rdel * cos( lon );
    rs[1] = rdel * sin( lon );
    rs[2] = rk;
    
    //Velocity of site is zero because the coordinate system is rotating with the earth
    vs[0] = 0.0;
    vs[1] = 0.0;
    vs[2] = 0.0;
}



/*
rv2azel
This function calculates the range, elevation, and azimuth (and their rates)
from the TEME vectors output by the SGP4 function.
Author: David Vallado, 2007.

Ported to C++ by Grady Hillhouse with some modifications, July 2015.
Repurposed by Marc PM with modifications for PVCoords input, April 2021.

INPUTS          DESCRIPTION                     RANGE/UNITS
satCoords       Position velocity (any Frame)   
latgd           Site geodetic latitude          -pi/2 to pi/2 in radians
lon             Site longitude                  -2pi to 2pi in radians
alt             Site altitude                   km
jdut1           Julian date                     days
OUTPUTS         DESCRIPTION
razel           Range, azimuth, and elevation matrix
razelrates      Range rate, azimuth rate, and elevation rate matrix
*/
// void ECEF2azel(const double recef[3], const double vecef[3], double latgd, double lon, double alt, double jdut1, double razel[3], double razelrates[3])
void ECEF2azel(const double recef[3], const double vecef[3], double latgd, double lon, double alt, double razel[3], double razelrates[3])
{
    //Locals
    double halfPI = M_PI * 0.5;
    double small  = 0.00000001;
    double temp;
    double rs[3];
    double vs[3];
    double rhoecef[3];
    double drhoecef[3];
    double tempvec[3];
    double rhosez[3];
    double drhosez[3];
    double magrhosez;
    double rho, az, el;
    double drho, daz, del;

    // for (uint32_t i = 0; i < 3; i++)
    // {
    //     recef[i] = recef[i] / 1000; // convert to kms
    //     vecef[i] = vecef[i] / 1000;
    // }
    

    // PVCoords satCoordsEcef =  satCoords.TransformTo(FrameType::ECEF);
    // Vector posSat = satCoordsEcef.GetPos(); // in m
    // Vector velSat = satCoordsEcef.GetVel(); // in m/s
    // double recef[3] = {posSat.x/1000, posSat.y/1000, posSat.z/1000};
    // double vecef[3] = {velSat.x/1000, velSat.y/1000, velSat.z/1000};


    // original implementation  of rv input in TEME
    // teme2ecef(ro, vo, jdut1, recef, vecef);

    
    //Get site vector in ECEF coordinate system
    site(latgd, lon, alt, rs, vs);
    
    

     
    //Find ECEF range vectors
    for (int i = 0; i < 3; i++)
    {
        rhoecef[i] = recef[i] - rs[i];
        drhoecef[i] = vecef[i];
    }
    rho = mag(rhoecef); //Range in km
    
    //Convert to SEZ (topocentric horizon coordinate system)
    rot3(rhoecef, lon, tempvec);
    rot2(tempvec, (halfPI-latgd), rhosez);
    
    rot3(drhoecef, lon, tempvec);
    rot2(tempvec, (halfPI-latgd), drhosez);
    
    //Calculate azimuth, and elevation
    temp = sqrt(rhosez[0]*rhosez[0] + rhosez[1]*rhosez[1]);
    if (temp < small)
    {
        el = sgn(rhosez[2]) * halfPI;
        az = atan2(drhosez[1], -drhosez[0]);
    }
    else
    {
        magrhosez = mag(rhosez);
        el = asin(rhosez[2]/magrhosez);
        az = atan2(rhosez[1]/temp, -rhosez[0]/temp);
    }
    
    //Calculate rates for range, azimuth, and elevation
    drho = dot(rhosez,drhosez) / rho;
    
    if(abs(temp*temp) > small)
    {
        daz = (drhosez[0]*rhosez[1] - drhosez[1]*rhosez[0]) / (temp * temp);
    }
    else
    {
        daz = 0.0;
    }
    
    if(abs(temp) > small)
    {
        del = (drhosez[2] - drho*sin(el)) / temp;
    }
    else
    {
        del = 0.0;
    }
    
    //Move values to output vectors
    razel[0] = rho;             //Range (km)
    razel[1] = az;              //Azimuth (radians)
    razel[2] = el;              //Elevation (radians)
    
    razelrates[0] = drho;       //Range rate (km/s)
    razelrates[1] = daz;        //Azimuth rate (rad/s)
    razelrates[2] = del;        //Elevation rate (rad/s)
}






void rot3(double invec[3], double xval, double outvec[3])
{
    double temp = invec[1];
    double c = cos(xval);
    double s = sin(xval);
    
    outvec[1] = c*invec[1] - s*invec[0];
    outvec[0] = c*invec[0] + s*temp;
    outvec[2] = invec[2];
    /* rot3 =
      c s 0
     -s c 0
      0 0 1
    */
}

void rot2(double invec[3], double xval, double outvec[3])
{
    double temp = invec[2];
    double c = cos(xval);
    double s = sin(xval);
    
    outvec[2] = c*invec[2] + s*invec[0];
    outvec[0] = c*invec[0] - s*temp;
    outvec[1] = invec[1];

    /* rot2 =
      c  0 -s
      0  1  0
      s  0  c
    */
}










} // namespace ns3

