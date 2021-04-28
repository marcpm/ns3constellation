/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */


#include "ground-station-mobility.h"
#include "orbital-coords.h"

#include "ns3/vector.h"
#include "ns3/double.h"
#include "ns3/log.h"
#include "ns3/integer.h"

#include "ns3/geographic-positions.h" // geo converter
#include "orbital-coords.h"

#define _USE_MATH_DEFINES
#include <cmath>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("GroundStationMobility");

NS_OBJECT_ENSURE_REGISTERED (GroundStationMobilityModel);

uint32_t current = 0; // to know if we are setting up first or second ground station

TypeId
GroundStationMobilityModel::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::GroundStationMobilityModel")
    .SetParent<MobilityModel> ()
    .SetGroupName ("Mobility")
    .AddConstructor<GroundStationMobilityModel> ()
    .AddAttribute ("Latitude",
                   "Latitude of ground station.",
                   DoubleValue(1.0),
                   MakeDoubleAccessor (&GroundStationMobilityModel::m_latitude),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("Longitude",
                   "Longitude of ground station.",
                   DoubleValue(1.0),
                   MakeDoubleAccessor (&GroundStationMobilityModel::m_longitude),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("Altitude", "The altitude of ground station.",
                   DoubleValue (0.0),
                   MakeDoubleAccessor (&GroundStationMobilityModel::m_altitude),
                   MakeDoubleChecker<double> ())
  ;

  return tid;
}

GroundStationMobilityModel::GroundStationMobilityModel(double latitude, double longitude, double altitude)
{
    m_latitude = latitude;
    m_longitude = longitude;
    m_altitude = altitude;
    m_name = "";
}

GroundStationMobilityModel::GroundStationMobilityModel(Vector latLonAlt)
{
    m_latitude = latitude;
    m_longitude = longitude;
    m_altitude = altitude;
    m_name = "";
}

GroundStationMobilityModel::GroundStationMobilityModel(Vector latLonAlt, std::string name)
{
    m_latitude = latitude;
    m_longitude = longitude;
    m_altitude = altitude;
    m_name = name;
    // m_id
}



GroundStationMobilityModel::GroundStationMobilityModel()
{
}





Vector 
GroundStationMobilityModel::GetLatLonAlt (void) const
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
    info << "elevation=" << m_elevation << ", ";
    info << "ecef_position=" << m_ecefPosition;
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
GroundStationMobilityModel::DoSetPosition ()
{
  m_ecefPosition = GeographicPositions::GeographicToCartesianCoordinates(m_latitude, m_longitude, m_altitude);
  m_ecefVelocity = Vector(0.0, 0.0, 0.0);
}

Vector
GroundStationMobilityModel::DoGetPosition (void) const
{
 
  return m_ecefPosition;
}




double 
CalculateDistanceGroundToSat (const Vector &ecefSat) // (const Vector &a, const Vector &b)
{

  distance = (m_ecefPosition - ecefSat).GetLength()
  // distance = sqrt( pow( b.x - a.x, 2) +  pow( b.y - a.y, 2)  + pow( b.z - a.z, 2)  )

  return distance;
}

Vector 
GetVisibilityGroundToSat (const &satPVCoords)
{
  
  
  ECEF2azel(const PVCoords satEcefCoords, double latgd, double lon, double alt, double jdut1, double razel[3], double razelrates[3]);

  



    return Topos(az,
            el,
            rangew,
            rate);

  return false 
}



Vector
GroundStationMobilityModel::DoGetVelocity (void) const
{
  
 // ecef velocity = 0; fixed (to earths rotating axis) ref frame
  return m_ecefVelocity;
}








Vector 
GroundStationMobility::GetZenithDirection(void) const
{
  zenith = Vector(cos(m_longitude) * cos(m_latitude), sin(m_longitude) * cos(m_latitude), sin(m_latitude) )
  return zenith;
}


} // namespace ns3
