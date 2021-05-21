/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#ifndef FULL_GROUND_STATION_MOBILITY_H
#define FULL_GROUND_STATION_MOBILITY_H

#include "ns3/object.h"
#include "ns3/geographic-positions.h" // geo converter
#include "ns3/mobility-model.h"

#include "ns3/vector.h"
#include "ns3/double.h"
#include "ns3/log.h"
#include "ns3/integer.h"

#include "ns3/orbital-coords.h"



namespace ns3 {




/**
 * \ingroup leo-satellite
 * \brief ground station mobility model.
 *
 * For a simplified simulation, ground stations will be placed along the 
 * longitude of satellites orbiting above and at varying latitudes
 * Currently supports two ground stations
 */
class FullGroundStationMobilityModel : public MobilityModel
{
public:
  /**
   * Register this type with the TypeId system.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);

  FullGroundStationMobilityModel(double latitude, double longitude, double altitude, 
                                                      std::string name, double angleIncidence, 
                                                      std::string dataRate,
                                                      uint32_t simultaneous_gsLinks);
  FullGroundStationMobilityModel(double latitude, double longitude, double altitude, 
                                                      std::string name, double angleIncidence, 
                                                      std::string dataRate);
  FullGroundStationMobilityModel(double latitude, double longitude, double altitude);
  FullGroundStationMobilityModel(Vector latLonAlt);
  FullGroundStationMobilityModel(Vector latLonAlt, std::string name);
  FullGroundStationMobilityModel();
  
  Vector GetLatLonAlt (void);
  Vector GetPosEcef (void);
  std::string ToString();

private:
  virtual void DoSetPosition (void);
  virtual Vector DoGetPosition (void) const;
  virtual Vector DoGetVelocity (void) const;
  friend double CalculateDistanceGroundToSat (const Vector &a, const Vector &b); // Vectors must correspond to a ground station and a LEO satellite (deprecated)
  friend Topos GetVisibilityGroundToSat (const PVCoords &position, double jdut1) const;
  friend double GetAngleofIncidence(void) const;
  friend double GetDataRate(void) const;
  // virtual Vector GetLatLonAlt (void) const;
  virtual Vector GetZenithDirection(void) const;
  virtual String GetName(void) const;
  double m_latitude; // latitude of ground station
                     // negative value indicates southern latitude, positive value indicates northern latitude
  double m_longitude; // longitude of ground station
                      // negative value indicates western longitude, positive value indicates eastern longitude
  double m_altitude; // Ground station altitude wrt to sea level


  // double m_antennaSpecs;  Data rate, incidence angle, etc.
  double m_angleIncidence;
  std::string m_name;
  std::string m_dataRate;
  uint32_t m_simultaneous_gsLinks;


  // Vector m_ecefPosition;
  // Vector m_ecefVelocity;
};

} // namespace ns3

#endif /* FULL_GROUND_STATION_MOBILITY_H */


