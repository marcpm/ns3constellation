#include "orbital-coords.h"
#include "ns3/vector.h"
#include "ns3/double.h"
#include "ns3/log.h"
#include "ns3/integer.h"

#include "ns3/geographic-positions.h" // geo converter


#define _USE_MATH_DEFINES
#include <cmath>

#include "sgp4math.h"

namespace ns3 {


        PVCoords::PVCoords();

        // Constructor for pos-vel unpackedvectors
        PVCoords::PVCoords(double px, double py, double pz, double vx, double vy, double vz, FrameType frame): 
                m_pos(px, py, pz), m_vel(vx, vy, vz), m_frame(frame)
                {
                    NS_LOG_FUNCTION (this);

                };

        // constructor for pos-vel vectors
        PVCoords::PVCoords(Vector pos, Vector vel, FrameType frame): 
                m_pos(pos), m_vel(vel), m_frame(frame)
                {
                    NS_LOG_FUNCTION (this);
                };

        PVCoords::PVCoords(Vector3D pos, Vector3D vel, FrameType frame): 
                m_pos(pos), m_vel(vel), m_frame(frame)
                {
                    NS_LOG_FUNCTION (this);
                };


        std::string PVCoords::PrintFrameType()
        {
            switch(m_frame)
            {
                case FrameType::ECI:
                    std::cout << "ECI" << std::endl;    
                    break;
                case FrameType::ECEF:
                    std::cout << "ECEF" << std::endl;
                    break;
                case FrameType::TEME:
                    std::cout << "TEME" << std::endl;
                    break;
                case FrameType::GEO:
                    std::cout << "GEO" << std::endl;
                    break;
            }
        };

        Vector PVCoords::GetPos()
        {
            return m_pos;
        };
        
        Vector PVCoords::GetVel()
        {
            return m_vel;
        };

        Vector PVCoords::GetLatLonAlt()
        {
            if (m_frame == FrameType::GEO)
            {
                return m_pos; // x,y,z == lat,lon,alt
            }
            else
            {   // create new geodetic frame for instant conversion to lat lon alt.
                PVCoords geotemp = this->TransformTo(FrameType::GEO);
                return geotemp.GetPos();
            }
        }

        Vector PVCoords::GetLat()
        {
            return GetLatLonAlt().x
        };
        Vector PVCoords::GetLon()
        {
            return GetLatLonAlt().y
        };
        Vector PVCoords::GetAlt()
        {
            return GetLatLonAlt().z
        };


        PVCoords PVCoords::TransformTo(FrameType newFrame)
        {
            switch (m_frame)
            {   
                
                case FrameType::ECI:
                    switch (newFrame)
                    {   
                        case FrameType::ECI:
                            PVCoords newFrameFrame =  PVCoords(m_pos, m_vel, m_frame) //copy ECI 2 ECI
                            break;
                        case FrameType::ECEF:
                            // ECI2ECEF 
                            break;
                        case FrameType::TEME:
                            // ECI2TEME
                            break;
                        case FrameType::GEO:
                            // ECI2GEO
                            break;
                    }  
                    
                    break;

                case FrameType::ECEF:
                    switch (newFrame)
                    {   
                        case FrameType::ECI:
                            // ECEF2ECI  
                            break;
                        case FrameType::ECEF:
                            PVCoords newCoords =  PVCoords(m_pos, m_vel, m_frame) //copy ECEF 2 ECEF
                            break;
                        case FrameType::TEME:
                            // ECEF2TEME
                            break;
                        case FrameType::GEO:
                            // ECEF2GEO
                            break;
                    }  

                    break;

                case FrameType::TEME:
                    switch (newFrame)
                    {   
                        case FrameType::ECI:
                            // TEME2ECI  
                            break;
                        case FrameType::ECEF:
                            // TEME2ECEF 
                            break;
                        case FrameType::TEME:
                            PVCoords newCoords =  PVCoords(m_pos, m_vel, m_frame) //copy TEME 2 TEME
                            break;
                        case FrameType::GEO:
                            // TEME2GEO
                            break;
                    }  

                    break;
                case FrameType::GEO:
                    switch (newFrame)
                    {   
                        case FrameType::ECI:
                            // GEO2ECI  
                            break;
                        case FrameType::ECEF:
                            PVCoords newCoords = PVCoords::Geodetic2ECEF (, double longitude, double altitude);
                            break;
                        case FrameType::TEME:
                            // GEO2TEME
                            break;
                        case FrameType::GEO:
                            PVCoords newCoords =  PVCoords(m_pos, m_vel, m_frame) //copy GEO 2 GEO
                            break;
                    }  

                    break;
                
            };
            return newCoords;

        };


        
virtual PVCoords PVCoords::Geodetic2ECEF (double latitude, double longitude, double altitude)
{

    // * "Department of Defense World Geodetic System 1984." National Imagery and 
    //  * Mapping Agency, 1 Jan. 2000. 
    //  * <http://earth-info.nga.mil/GandG/publications/tr8350.2/wgs84fin.pdf>.
    //  */

    NS_LOG_FUNCTION_NOARGS ();
    /// Conversion factor: degrees to radians
    static constexpr double DEG2RAD = M_PI / 180.0;

    /// Conversion factor: radians to degrees
    static constexpr double RAD2DEG = 180.0 * M_1_PI;


    
    double latitudeRadians = DEG2RAD * latitude;
    double longitudeRadians = DEG2RAD * longitude;
    double  a = 6378137; // earths semi major axis
    double  e = 0.0818191908426215; // WGS84 eccentricity

    
    double Rn = a / (sqrt (1 - pow (e, 2) * pow (sin (latitudeRadians), 2))); // radius of
                                                                            // curvature
    double x = (Rn + altitude) * cos (latitudeRadians) * cos (longitudeRadians);
    double y = (Rn + altitude) * cos (latitudeRadians) * sin (longitudeRadians);
    double z = ((1 - pow (e, 2)) * Rn + altitude) * sin (latitudeRadians);
    
    Vector ecefPos = Vector (x, y, z);
    Vector ecefVel = Vector(0.0, 0.0, 0.0);

    PVCoords ecefCoords(ecefPos, ecefVel, FrameType::ECEF);
    
    return ecefCoords;
}




/*
ECEF2GEO
This function calulates the latitude, longitude and altitude
given the ECEF position matrix.
Based on David Vallado, 2007, +  Grady Hillhouse.
INPUTS          DESCRIPTION                     RANGE/UNITS
r               Position matrix (ECEF)          m
OUTPUTS         DESCRIPTION
latlongh        Latitude, longitude, and altitude (deg, deg, and m)
*/
PVCoords PVCoords::ECEF2GEO (PVCoords ecefCoords)
{
    double twopi = 2.0*M_PI;
    double small = 0.00000001;          //small value for tolerances
    double re = 6378137;               //radius of earth in m
    double eesqrd = 0.006694385000;     //eccentricity of earth sqrd
    double magr, temp, rtasc;
    
    Vector ecefPos = ecefCoords.getPos()
    double r[3] = {ecefPos.x, ecefPos.y, ecefPos.z};


    magr = mag(r);
    temp = sqrt(r[0]*r[0] + r[1]*r[1]);
 
    if(abs(temp) < small)
    {
        rtasc = sgn(r[2]) * pi * 0.5;
    }
    else
    {
        rtasc = atan2(r[1], r[0]);
    }
    
    latlongh[1] = rtasc;
    
    if (abs(latlongh[1]) >= pi)
    {
        if (latlongh[1] < 0.0)
        {
            latlongh[1] += twopi;
        }
        else
        {
            latlongh[1] -= twopi;
        }
    }
    
    latlongh[0] = asin(r[2] / magr);
    
    //Iterate to find geodetic latitude
    int i = 1;
    double olddelta = latlongh[0] + 10.0;
    double sintemp, c = 0;
    
    while ( (abs(olddelta - latlongh[0]) >= small) && (i < 10) )
    {
        olddelta = latlongh[0];
        sintemp = sin(latlongh[0]);
        c = re / sqrt(1.0 - eesqrd*sintemp*sintemp);
        latlongh[0] = atan( (r[2] + c*eesqrd*sintemp) / temp );
        i++;
    }
    
    if (0.5*pi - abs(latlongh[0]) > pi/180.0)
    {
        latlongh[2] = (temp/cos(latlongh[0])) - c;
    }
    else
    {
        latlongh[2] = r[2]/sin(latlongh[0]) - c*(1.0 - eesqrd);
    }


    PVCoords newGeoCoords(latlongh[0]*RAD2DEG, latlongh[1]*RAD2DEG , latlongh[2]*RAD2DEG, 
                        FrameType::GEO);

    return newGeoCoords;
};







// //////////////////// //////////////////// //////////////////// //////////////////// //////////////////
// //////////////////// //////////////////// //////////////////// //////////////////

/**
    * Default constructor
    */


    Topos::Topos(): az(0.0), el(0.0), range(0.0), azdot(0.0), eldot(0.0), rangedot(0.0), visibility(false)
    {}
 
    Topos::Topos( double azi, double ele, double rnge, double rnge_rate): 
            az(azi), el(ele), range(rnge), azdot(rnge_rate), eldot(0.0), rangedot(0.0), visibility(false)
    {}

    Topos::Topos( double azi, double ele, double rnge, 
                double az_rate, double el_rate, double range_rate, bool visible): 
            az(azi), el(ele), range(rnge), 
            azdot(az_rate), eldot(el_rate), rangedot(range_rate), visibility(visible)
    {}

    Topos::Topos( double az, double el, double rnge):
        az(azi), el(ele), range(rnge), azdot(0.0), eldot(0.0), rangedot(0.0), visibility(true);
    {}



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

    PVCoords pv_ecef (Vector(recef[0], recef[1], recef[2]),   Vector(vecef[0], vecef[1], vecef[2]), FrameType::ECEF);
    
    
    return pv_ecef;
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
TEMErv2azel
This function calculates the range, elevation, and azimuth (and their rates)
from the TEME vectors output by the SGP4 function.
Author: David Vallado, 2007
Ported to C++ by Grady Hillhouse with some modifications, July 2015.
INPUTS          DESCRIPTION                     RANGE/UNITS
ro              Sat. position vector (TEME)     km
vo              Sat. velocity vector (TEME)     km/s
latgd           Site geodetic latitude          -PI/2 to PI/2 in radians
lon             Site longitude                  -2PI to 2PI in radians
alt             Site altitude                   km
jdut1           Julian date                     days
OUTPUTS         DESCRIPTION
razel           Range, azimuth, and elevation matrix
razelrates      Range rate, azimuth rate, and elevation rate matrix
*/

void TEMErv2azel(const double ro[3], const double vo[3], double latgd, double lon, double alt, double jdut1, double razel[3], double razelrates[3])
{
    //Locals
    double halfPI = PI * 0.5;
    double small  = 0.00000001;
    double temp;
    double rs[3];
    double vs[3];
    double recef[3];
    double vecef[3];
    double rhoecef[3];
    double drhoecef[3];
    double tempvec[3];
    double rhosez[3];
    double drhosez[3];
    double magrhosez;
    double rho, az, el;
    double drho, daz, del;
    
    //Get site vector in ECEF coordinate system
    site(latgd, lon, alt, rs, vs);
    
    //Convert TEME vectors to ECEF coordinate system
    teme2ecef(ro, vo, jdut1, recef, vecef);
    
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



void ECEF2azel(const PVCoords satCoords, double latgd, double lon, double alt, double jdut1, double razel[3], double razelrates[3])
{
    //Locals
    double halfPI = PI * 0.5;
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
    

    satCoords =  satCoords.TransformTo(FrameType::ECEF);
    posSat = satCoords.GetPos();
    velSat = satCoords.GetVel();
    double recef[3] = {posSat.x, posSat.y, posSat.z};
    double vecef[3] = {velSat.x, velSat.y, velSat.z};


    // case of rv input in TEME
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




















