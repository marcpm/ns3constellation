/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#ifndef GROUND_STATION_MOBILITY_H
#define GROUND_STATION_MOBILITY_H

#include "ns3/object.h"
#include "ns3/mobility-model.h"
#include "ns3/vector.h"

namespace ns3 {




/**
 * \ingroup leo-satellite
 * \brief ground station mobility model.
 *
 * For a simplified simulation, ground stations will be placed along the 
 * longitude of satellites orbiting above and at varying latitudes
 * Currently supports two ground stations
 */
class GroundStationMobilityModel : public MobilityModel
{
public:
  /**
   * Register this type with the TypeId system.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);
  GroundStationMobilityModel();

  Vector GetLatLonAlt (void);
  Vector GetPosEcef (void);
  std::string ToString();

private:
  virtual void DoSetPosition (void);
  virtual Vector DoGetPosition (void) const;
  virtual Vector DoGetVelocity (void) const;
  friend double CalculateDistanceGroundToSat (const Vector &a, const Vector &b); // Vectors must correspond to a ground station and a LEO satellite (deprecated)
  friend Vector GetVisibilityGroundToSat (const &position);

  // virtual Vector GetLatLonAlt (void) const;
  virtual Vector GetZenithDirection(void) const
  
  double m_latitude; // latitude of ground station
                     // negative value indicates southern latitude, positive value indicates northern latitude
  double m_longitude; // longitude of ground station
                      // negative value indicates western longitude, positive value indicates eastern longitude
  double m_altitude; // Ground station altitude wrt to sea level

  // double m_antennaSpecs;  Data rate, incidence angle, etc.

  std::string m_name;

  Vector m_ecefPosition;
  Vector m_ecefVelocity;
};

} // namespace ns3

#endif /* GROUND_STATION_MOBILITY_H */


