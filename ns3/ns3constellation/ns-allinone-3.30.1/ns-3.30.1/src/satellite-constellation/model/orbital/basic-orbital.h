#ifndef ORBITAL_BASIC_H
#define ORBITAL_BASIC_H


#include <iomanip> 
#include "ns3/object.h"
#include "ns3/mobility-model.h"
#include "ns3/vector.h"

namespace ns3 {




// in degrees and kms.
struct Topos
{
    double azimuth;
    double elevation;
    double range;
    double azdot;
    double eldot;
    double rangedot;
    bool visible;

    std::string ToString();
};

/// in Degrees and Kms.
struct CoordGeodetic
{
    double latitude;
    double longitude;
    double altitude;

    std::string ToString();
};



double  sgn (double x);

double  mag (const double x[3]);

double  dot (double x[3], double y[3]);



void site(double latgd, double lon, double alt, double rs[3], double vs[3]);

void TEMErv2azel(const double ro[3], const double vo[3], double latgd, double lon, double alt, double jdut1, double razel[3], double razelrates[3]);

void ECEF2azel(const double recef[3], const double vecef[3], double latgd, double lon, double alt, double razel[3], double razelrates[3]);

void rot3(double invec[3], double xval, double outvec[3]);
void rot2(double invec[3], double xval, double outvec[3]);

// std::list < Vector > ECEFtoTEME (const Vector p_ecef, Vector v_ecef);
// std::list < Vector > TEMEtoECEF (const Vector p_teme, Vector v_teme);






} // namespace ns3

#endif /* BASIC_ORBITAL_H */


