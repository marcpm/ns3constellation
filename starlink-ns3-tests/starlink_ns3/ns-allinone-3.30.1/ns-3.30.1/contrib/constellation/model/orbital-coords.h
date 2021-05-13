ifndef ORBITAL_UTILS_H
#define ORBITAL_UTILS_H

#include "ns3/object.h"
#include "ns3/mobility-model.h"
#include "ns3/vector.h"

namespace ns3 {





struct CoordTopoCentric
{
    double azimuth;
    double elevation;
    double range;
    double range_rate;
};

struct CoordGeodetic
{
    double latitude;
    double longitude;
    double altitude;
};

// where ECI= J2000, TEME=(sgp4 vallado defined), ECEF="ITRF", GEO="WGS84/WGS72"
enum class FrameType {ECI="ECI", TEME="TEME", ECEF="ECEF", GEO="GEO"};

class PVCoords
{
    public:
        PVCoords();
        PVCoords(double px, double py, double pz, double vx, double vy, double vz, FrameType frame); //: m_pos(px, py, pz), m_vel(vx, vy, vz), m_frame(frame);
        PVCoords(Vector pos, Vector vel, FrameType frame); //: m_pos(pos), m_vel(vel), m_frame(frame);

        
        // For Debugging Purposes
        std::string PrintFrameType();
        
        Vector GetPos();
        Vector GetVel();
        
        Vector GetLatLonAlt();
        Vector GetLat();
        Vector GetLon();
        Vector GetAlt();

        PVCoords TransformTo(FrameType newFrame);
        


        virtual Vector PVCoords::Geodetic2ECEF (double latitude, double longitude, double altitude)
        
        // PVCoords ToTEME();
        // PVCoords ToECI();
        // PVCoords ToECEF();
        // PVCoords ToGEO();
        
    private: 
        FrameType m_frame;
        Vector m_pos; // x,y,z in nonGeo frames, lat,lon,alt in Geodetic frame
        Vector m_vel;
};
















//  TopoCentric Coordinate frame. (on earth's surface)
class Topos
{
 public:

    /** azimuth in degrees */
    double az;
    /** elevations in degrees */
    double el;
    /** range in meters */
    double range;
    /** range rate in meters per second */
    double range_rate;

    /**
     * Default constructor
     */
    Topos(): az(0.0), el(0.0), range(0.0), range_rate(0.0);
    /**
     * Constructor
     * @param[in] az azimuth in radians
     * @param[in] el elevation in radians
     * @param[in] rnge range in kilometers
     * @param[in] rnge_rate range rate in kilometers per second
     */
    Topos( double azi, double ele, double rnge, double rnge_rate): 
            az(azi), el(ele), range(rnge), range_rate(0.0);


    Topos( double az, double el, double rnge):
        az(azi), el(ele), range(rnge), range_rate(0.0);


    /*
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




double  sgn
        (
          double x
        );

double  mag
        (
          const double x[3]
        );

double  dot
        (
          double x[3], double y[3]
        );

double gstime (double jdut1);
void polarm(double jdut1, double pm[3][3]);

PVCoords teme2ecef(const double rteme[3], const double vteme[3], double jdut1);




void site(double latgd, double lon, double alt, double rs[3], double vs[3]);

void TEMErv2azel(const double ro[3], const double vo[3], double latgd, double lon, double alt, double jdut1, double razel[3], double razelrates[3]);

void ECEF2azel(const PVCoords satEcefCoords, double latgd, double lon, double alt, double jdut1, double razel[3], double razelrates[3]);

void rot3(double invec[3], double xval, double outvec[3]);
void rot2(double invec[3], double xval, double outvec[3]);

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


