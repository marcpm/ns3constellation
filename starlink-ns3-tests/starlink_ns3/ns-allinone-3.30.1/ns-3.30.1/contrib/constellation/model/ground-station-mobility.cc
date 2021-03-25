/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */


#include "ground-station-mobility.h"
#include "ns3/vector.h"
#include "ns3/double.h"
#include "ns3/log.h"
#include "ns3/integer.h"

#include "ns3/geographic-positions.h" // geo converter
#include "orbital-utils.h"

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

GroundStationMobilityModel::GroundStationMobilityModel(float latitude, float longitude, float altitude)
{
  m_latitude = latitude;
  m_longitude = longitude;
  m_altitude = altitude;
}

GroundStationMobilityModel::GroundStationMobilityModel(Vector r_ecef)
{
  m_ecefPosition = r_ecef;
}


GroundStationMobilityModel::GroundStationMobilityModel()
{
}

/* To be called after MobilityModel object is created to set position.
   Input should be a NULL vector as position is determined by number of orbital planes and number of satellites per   
   orbital plane
   Both ground stations are set along the longitude of a satellite's orbit (not at the same longitude), and at random 
   latitudes   
 */
void 
GroundStationMobilityModel::DoSetPosition (const Vector &position)
{
  m_ecefPosition = GeographicPositions::GeographicToCartesianCoordinates(m_latitude, m_longitude, m_altitude);
}

Vector
GroundStationMobilityModel::DoGetPosition (void) const
{
 
  return m_ecefPosition;
}

Vector 
GroundStationMobilityModel::GetGeodeticPosition (void) const
{
  return Vector(m_latitude, m_longitude, m_altitude);
}


/* Vector a is the position of the ground station
   Vector b is the position of the  satellite */
double 
CalculateDistanceGroundToSat (const Vector &a, const Vector &b)
{

  distance = sqrt( pow( b.x - a.x, 2) +  pow( b.y - a.y, 2)  + pow( b.z - a.z, 2)  )

  return distance;
}

bool 
GetVisibilityGroundToSat (const &position)
{
  
  return false 
}



Vector
GroundStationMobilityModel::DoGetVelocity (void) const
{
  
  
  m_ecefVelocity = Vector(0.0, 0.0, 0.0); // ecef velocity = 0; fixed (to earths spinning axis) ref frame
  return m_ecefVelocity;
}

Vector 
GroundStationMobility::GetZenithDirection(void) const
{
  zenith = Vector(cos(m_longitude) * cos(m_latitude), sin(m_longitude) * cos(m_latitude), sin(m_latitude) )
  return zenith;
}


} // namespace ns3
