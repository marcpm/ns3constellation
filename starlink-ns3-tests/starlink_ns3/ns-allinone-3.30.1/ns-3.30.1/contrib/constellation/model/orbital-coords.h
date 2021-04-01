ifndef ORBITAL_UTILS_H
#define ORBITAL_UTILS_H

#include "ns3/object.h"
#include "ns3/mobility-model.h"
#include "ns3/vector.h"

namespace ns3 {



struct PassDetails
{
    DateTime aos;
    DateTime los;
    double range;
    double max_elevation;
};

// struct CoordTopoCentric
// {
//     float azimuth;
//     float elevation;
//     float range;
//     float range_rate;
// }

struct CoordGeodetic
{
    float latitude;
    float longitude;
    float altitude;
};

struct PVCoords
{
    Vector position;
    Vector velocity;
};


// Topos point in a TopoCentric Coordinate frame. (on earth's surface)
class Topos
{
 public:

    /** azimuth in degrees */
    double azimuth;
    /** elevations in degrees */
    double elevation;
    /** range in meters */
    double range;
    /** range rate in meters per second */
    double range_rate;

    /**
     * Default constructor
     */
    Topos();
    /**
     * Constructor
     * @param[in] az azimuth in radians
     * @param[in] el elevation in radians
     * @param[in] rnge range in kilometers
     * @param[in] rnge_rate range rate in kilometers per second
     */
    Topos( double az, double el, double rnge, double rnge_rate);


    Topos( double az, double el, double rnge);


    /**
     * Copy constructor
     * @param[in] topo object to copy from
     */
    Topos(const Topos& topo);

    Topos& operator=(const Topos& topo);


    /**
     * Dump this object to a string
     * @returns string
     */
    std::string ToString() const;

 private:

    m_azRad = azimuth * M_PI / 180;
    m_elRad = elevation * M_PI / 180;
    
};



double gstime (double jdut1);
void polarm(double jdut1, double pm[3][3]);

PVCoords teme2ecef(const double rteme[3], const double vteme[3], double jdut1);




void TEMErv2azel(const double ro[3], const double vo[3], double latgd, double lon, double alt, double jdut1, double razel[3], double razelrates[3]);



// std::list < Vector > ECEFtoTEME (const Vector p_ecef, Vector v_ecef);
// std::list < Vector > TEMEtoECEF (const Vector p_teme, Vector v_teme);




/*
************************************************************
************************************************************
*/
class ElevationMask : 
{
public:
//   /**
//    * Register this type with the TypeId system.
//    * \return the object TypeId
//    */
//   static TypeId GetTypeId (void);
//   GroundStationMobilityModel();

private:

  friend double CalculateDistanceGroundToSat (const Vector &satPosition); 
  friend double GetVisibilityGroundToSat (const Vector &satPosition);
  friend Vector GetAzElRange (const &satPosition)

  virtual Vector GetGeodeticPosition (void) const;
  virtual Vector GetZenithDirection(void) const
  
  double m_latitude; // latitude of ground station
                     // negative value indicates southern latitude, positive value indicates northern latitude
  double m_longitude; // longitude of ground station
                      // negative value indicates western longitude, positive value indicates eastern longitude
  double m_altitude; // Ground station altitude wrt to sea level


};



} // namespace ns3

#endif /* ORBITAL_UTILS_H */


