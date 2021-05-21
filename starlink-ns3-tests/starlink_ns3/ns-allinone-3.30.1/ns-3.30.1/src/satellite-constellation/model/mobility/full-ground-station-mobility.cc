/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */


#include "full-ground-station-mobility.h"

#define _USE_MATH_DEFINES
#include <cmath>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("FullGroundStationMobility");

NS_OBJECT_ENSURE_REGISTERED (FullGroundStationMobilityModel);

uint32_t current = 0; // to know if we are setting up first or second ground station

TypeId
FullGroundStationMobilityModel::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::FullGroundStationMobilityModel")
    .SetParent<MobilityModel> ()
    .SetGroupName ("Mobility")
    .AddConstructor<FullGroundStationMobilityModel> ()
    .AddAttribute ("Latitude",
                   "Latitude of ground station.",
                   DoubleValue(1.0),
                   MakeDoubleAccessor (&FullGroundStationMobilityModel::m_latitude),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("Longitude",
                   "Longitude of ground station.",
                   DoubleValue(1.0),
                   MakeDoubleAccessor (&FullGroundStationMobilityModel::m_longitude),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("Altitude", "The altitude of ground station.",
                   DoubleValue (0.0),
                   MakeDoubleAccessor (&FullGroundStationMobilityModel::m_altitude),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("Name", "The ground station Name.",
                   StringValue (""),
                   MakeStringAccessor (&FullGroundStationMobilityModel::m_name),
                   MakeStringChecker<string> ())
    .AddAttribute ("DataRate", "The ground station Data Rate.",
                   StringValue ("5.36Gbps"),
                   MakeStringAccessor (&FullGroundStationMobilityModel::m_dataRate),
                   MakeStringChecker<string> ())
    .AddAttribute ("AngleIncidence", "The minimum Angle of incidence of ground station.",
                   DoubleValue (25.0),
                   MakeDoubleAccessor (&FullGroundStationMobilityModel::m_angleIncidence),
                   MakeDoubleChecker<double> ())
                   
  ;

  return tid;
}

FullGroundStationMobilityModel::FullGroundStationMobilityModel(double latitude, double longitude, double altitude, 
                                                      std::string name, double angleIncidence, std::string dataRate,
                                                      uint32_t simultaneous_gsLinks)
{
    m_latitude = latitude;
    m_longitude = longitude;
    m_altitude = altitude;
    m_name = name;
    m_angleIncidence = angleIncidence;
    m_dataRate = dataRate;
    m_simultaneous_gsLinks = simultaneous_gsLinks;
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
    m_simultaneous_gsLinks = 1;
}

FullGroundStationMobilityModel::FullGroundStationMobilityModel(double latitude, double longitude, double altitude)
{
    m_latitude = latitude;
    m_longitude = longitude;
    m_altitude = altitude;
    m_name = "";
    m_angleIncidence = 25.0;
    m_dataRate = "5.36Gbps";
    m_simultaneous_gsLinks = 1;
}

FullGroundStationMobilityModel::FullGroundStationMobilityModel(Vector latLonAlt)
{
    m_latitude = latitude;
    m_longitude = longitude;
    m_altitude = altitude;
    m_name = "";
    m_angleIncidence = 25.0;
    m_dataRate = "5.36Gbps";
    m_simultaneous_gsLinks = 1;
}

FullGroundStationMobilityModel::FullGroundStationMobilityModel(Vector latLonAlt, std::string name)
{
    m_latitude = latitude;
    m_longitude = longitude;
    m_altitude = altitude;
    m_name = name;
    m_angleIncidence = 25.0;
    m_dataRate = "5.36Gbps";
    m_simultaneous_gsLinks = 1;
    // m_id
}



FullGroundStationMobilityModel::FullGroundStationMobilityModel()
{
}





Vector 
FullGroundStationMobilityModel::GetLatLonAlt (void) const
{
  return Vector(m_latitude, m_longitude, m_altitude);
}

  Vector GetPosEcef (void)
  {
    return m_ecefPosition;
  };
  
  std::string ToString()
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







/* To be called after MobilityModel object is created to set position.
   Input should be a NULL vector as position is determined by number of orbital planes and number of satellites per   
   orbital plane
   Both ground stations are set along the longitude of a satellite's orbit (not at the same longitude), and at random 
   latitudes   
 */
void 
FullGroundStationMobilityModel::DoSetPosition ()
{
  m_ecefPosition = GeographicPositions::GeographicToCartesianCoordinates(m_latitude, m_longitude, m_altitude);
  m_ecefVelocity = Vector(0.0, 0.0, 0.0);
}

Vector
FullGroundStationMobilityModel::DoGetPosition (void) const
{
 
  return m_ecefPosition;
}




double 
FullGroundStationMobilityModel::CalculateDistanceGroundToSat (const Vector &ecefSat) // (const Vector &a, const Vector &b)
{

  distance = (m_ecefPosition - ecefSat).GetLength()
  // distance = sqrt( pow( b.x - a.x, 2) +  pow( b.y - a.y, 2)  + pow( b.z - a.z, 2)  )

  return distance;
}

// std::tuple <bool, double >
Topos
FullGroundStationMobilityModel::GetVisibilityGroundToSat (const PVCoords &satPVCoords, double jdut1) const
{
  
  PVCoords ecefSat = satPVCoords->TransformTo(FrameType::ECEF);
  
  
  double razel[3];
  double razelrates[3];
  ECEF2azel(ecefSat, m_latitude, m_longitude, m_altitude, jdut1, razel[3], razelrates[3]);

  bool visible = razel[2] * M_PI / 180.0 > m_angleIncidence;
  

  Topos topoFrame(razel[1] * M_PI / 180.0, razel[2] * M_PI / 180.0, razel[0], 
                  razelrates[1] * M_PI / 180.0, razelrates[2] * M_PI / 180.0, razelrates[0], visible);



  // std::tuple <bool, double > visibDistance = {topoFrame.el > m_angleIncidence, razel[0]};
  


  return topoFrame;
  
}



Vector
FullGroundStationMobilityModel::DoGetVelocity (void) const
{
  
 // ecef velocity = 0; fixed (to earths rotating axis) ref frame
  return m_ecefVelocity;
}




friend double GetDataRate(void) const
{
  return m_dataRate;
}



Vector 
FullGroundStationMobility::GetZenithDirection(void) const
{
  zenith = Vector(cos(m_longitude) * cos(m_latitude), sin(m_longitude) * cos(m_latitude), sin(m_latitude) )
  return zenith;
}



String 
FullGroundStationMobility::GetName(void) const
{
  return m_name;
}



} // namespace ns3
