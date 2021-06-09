/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "full-ground-station-mobility.h"

#define _USE_MATH_DEFINES
#include <cmath>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("FullGroundStationMobility");

NS_OBJECT_ENSURE_REGISTERED (FullGroundStationMobilityModel);


TypeId
FullGroundStationMobilityModel::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::FullGroundStationMobilityModel")
    .SetParent<MobilityModel> ()
    .SetGroupName ("Mobility")
    .AddConstructor<FullGroundStationMobilityModel> ()
    .AddAttribute ("Latitude",
                   "Latitude of ground station [deg].",
                   DoubleValue(1.0),
                   MakeDoubleAccessor (&FullGroundStationMobilityModel::m_latitude),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("Longitude",
                   "Longitude of ground station [deg].",
                   DoubleValue(1.0),
                   MakeDoubleAccessor (&FullGroundStationMobilityModel::m_longitude),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("Altitude", "The altitude of ground station [m].",
                   DoubleValue (0.0),
                   MakeDoubleAccessor (&FullGroundStationMobilityModel::m_altitude),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("Name", "The ground station Name.",
                   StringValue (""),
                   MakeStringAccessor (&FullGroundStationMobilityModel::m_name),
                   MakeStringChecker ())
    .AddAttribute ("DataRate", "The ground station Data Rate.",
                   StringValue ("5.36Gbps"),
                   MakeStringAccessor (&FullGroundStationMobilityModel::m_dataRate),
                   MakeStringChecker ())
    .AddAttribute ("AngleIncidence", "The minimum Angle of incidence of ground station [deg].",
                   DoubleValue (25.0),
                   MakeDoubleAccessor (&FullGroundStationMobilityModel::m_angleIncidence),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("NumGsl", "Number of simultaneous GSL links.",
                   IntegerValue (1),
                   MakeIntegerAccessor (&FullGroundStationMobilityModel::m_numGsl),
                   MakeIntegerChecker<uint32_t> ())
                   
  ;

  return tid;
}

FullGroundStationMobilityModel::FullGroundStationMobilityModel()
{
  
}


FullGroundStationMobilityModel::FullGroundStationMobilityModel(double latitude, double longitude, double altitude, 
                                                      std::string name, double angleIncidence, std::string dataRate,
                                                      uint32_t numGSLs)
{
    m_latitude = latitude;
    m_longitude = longitude;
    m_altitude = altitude;
    m_name = name;
    m_angleIncidence = angleIncidence;
    m_dataRate = dataRate;
    m_numGsl = numGSLs;
    m_ecefPosition = GeographicPositions::GeographicToCartesianCoordinates(m_latitude, m_longitude, m_altitude, GeographicPositions::EarthSpheroidType::WGS84);
    m_ecefVelocity = Vector(0.0, 0.0, 0.0);
}

FullGroundStationMobilityModel::FullGroundStationMobilityModel(double latitude, double longitude, double altitude, 
                                                      std::string name, double angleIncidence, std::string dataRate)
{
    m_latitude = latitude;
    m_longitude = longitude;
    m_altitude = altitude;
    m_name = name;
    m_angleIncidence = angleIncidence;
    m_dataRate = dataRate;
    m_numGsl = 1;
    m_ecefPosition = GeographicPositions::GeographicToCartesianCoordinates(m_latitude, m_longitude, m_altitude, GeographicPositions::EarthSpheroidType::WGS84);
    m_ecefVelocity = Vector(0.0, 0.0, 0.0);
}

FullGroundStationMobilityModel::FullGroundStationMobilityModel(double latitude, double longitude, double altitude)
{
    m_latitude = latitude;
    m_longitude = longitude;
    m_altitude = altitude;
    m_name = "";
    m_angleIncidence = 25.0;
    m_dataRate = "5.36Gbps";
    m_numGsl = 1;

    m_ecefPosition = GeographicPositions::GeographicToCartesianCoordinates(m_latitude, m_longitude, m_altitude, GeographicPositions::EarthSpheroidType::WGS84);
    m_ecefVelocity = Vector(0.0, 0.0, 0.0);
}

FullGroundStationMobilityModel::FullGroundStationMobilityModel(Vector latLonAlt)
{
    m_latitude = latLonAlt.x;
    m_longitude = latLonAlt.y;
    m_altitude = latLonAlt.z;
    m_name = "";
    m_angleIncidence = 25.0;
    m_dataRate = "5.36Gbps";
    m_numGsl = 1;
    m_ecefPosition = GeographicPositions::GeographicToCartesianCoordinates(m_latitude, m_longitude, m_altitude, GeographicPositions::EarthSpheroidType::WGS84);
    m_ecefVelocity = Vector(0.0, 0.0, 0.0);
}

FullGroundStationMobilityModel::FullGroundStationMobilityModel(Vector latLonAlt, std::string name)
{
    m_latitude = latLonAlt.x;
    m_longitude = latLonAlt.y;
    m_altitude = latLonAlt.z;
    m_name = name;
    m_angleIncidence = 25.0;
    m_dataRate = "5.36Gbps";
    m_numGsl = 1;
    
    m_ecefPosition = GeographicPositions::GeographicToCartesianCoordinates(m_latitude, m_longitude, m_altitude, GeographicPositions::EarthSpheroidType::WGS84);
    m_ecefVelocity = Vector(0.0, 0.0, 0.0);
}


FullGroundStationMobilityModel::~FullGroundStationMobilityModel(void)
{
}


Vector 
FullGroundStationMobilityModel::GetLatLonAlt (void) const
{
  return Vector(m_latitude, m_longitude, m_altitude);
}

double
FullGroundStationMobilityModel::GetLatitude() const
{
    return m_latitude;
}

double
FullGroundStationMobilityModel::GetLongitude() const
{
    return m_longitude;
}

double
FullGroundStationMobilityModel::GetElevation() const
{
    return m_altitude;
}


 double 
 FullGroundStationMobilityModel::GetAngleOfIncidence(void) const
 {
   return m_angleIncidence;
 }

std::string  
FullGroundStationMobilityModel::GetDataRate(void) const
{
  return m_dataRate;
}

double FullGroundStationMobilityModel::GetNumGsl(void) const
{
  return m_numGsl;
}


Vector 
FullGroundStationMobilityModel::GetZenithDirection(void) const
{
  Vector zenith(cos(m_longitude) * cos(m_latitude), sin(m_longitude) * cos(m_latitude), sin(m_latitude) );
  return zenith;
}



std::string 
FullGroundStationMobilityModel::GetName(void) const
{
  return m_name;
}

std::string 
FullGroundStationMobilityModel::ToString() const
  {
    std::stringstream info;
    info << "Ground station[";
    info << "name=" << m_name << ", ";
    info << "latitude=" << m_latitude << ", ";
    info << "longitude=" << m_longitude << ", ";
    info << "elevation=" << m_altitude << ", ";
    // info << "ecef_position=" << m_ecefPosition;
    info << "]";
    return info.str();
  };


// Topos
// FullGroundStationMobilityModel::GetVisibilityGroundToSat (const PVCoords &satPVCoords) const
Topos
FullGroundStationMobilityModel::GetVisibilityGroundToSat (const Vector satPos, const Vector satVel)
{
  
  // PVCoords ecefSat = satPVCoords->TransformTo(FrameType::ECEF);
  
  // bool jdut1 = false;

  double razel[3];
  double razelrates[3];

  double recef[3] = {satPos.x/1000.0, satPos.y/1000.0, satPos.z/1000.0}; // in  km
  double vecef[3] = {satVel.x/1000.0, satVel.y/1000.0, satVel.z/1000.0};
  // ecef2azel needs lat,lon in rad, alt in km.



  ECEF2azel(recef, vecef, m_latitude * M_PI / 180.0 , m_longitude * M_PI / 180.0 , m_altitude / 1000.0, 
            razel, razelrates);

  bool visible = (razel[2] / M_PI * 180.0 ) > m_angleIncidence;
  
  // toposAzelRa Coords in az[deg] el[deg]  ra[m] : idem rates [/s]
  Topos topoFrame = {razel[1] / M_PI * 180.0, razel[2] / M_PI * 180.0, razel[0] * 1000.0, 
                    razelrates[1] / M_PI * 180.0, razelrates[2] / M_PI * 180.0, razelrates[0] * 1000.0, visible 
                  };

  return topoFrame;
  
}




// privs


void 
FullGroundStationMobilityModel::DoSetPosition (const Vector &position)
{

}

Vector
FullGroundStationMobilityModel::DoGetPosition (void) const
{
  double rs[3];
  double vs[3];
  site(m_latitude * M_PI / 180.0 , m_longitude * M_PI / 180.0 , m_altitude / 1000.0, 
        rs, vs);
 
  return Vector3D(rs[0], rs[1], rs[2]);
}

Vector
FullGroundStationMobilityModel::DoGetVelocity (void) const
{
  
 // ecef velocity = 0; fixed (to earths rotating axis) ref frame
  return Vector3D(0,0,0);
}



double 
FullGroundStationMobilityModel::CalculateDistanceGroundToSat (const Vector &ecefPosSat) // (const Vector &a, const Vector &b)
{

  double distance = (m_ecefPosition - ecefPosSat).GetLength();
  // distance = sqrt( pow( b.x - a.x, 2) +  pow( b.y - a.y, 2)  + pow( b.z - a.z, 2)  )

  return distance;
}








} // namespace ns3
