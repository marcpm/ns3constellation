/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#ifndef AIRCRAFT_MOBILITY
#define AIRCRAFT_MOBILITY

#include "ns3/object.h"
#include "ns3/geographic-positions.h" // geo converter
#include "ns3/mobility-model.h"

#include <string>
#include <utility>
#include <vector>
#include "ns3/string.h"
#include "ns3/vector.h"
#include "ns3/double.h"
#include "ns3/log.h"
#include "ns3/integer.h"
#include "ns3/nstime.h"
#include "ns3/simulator.h"



#include "ns3/basic-orbital.h"



namespace ns3 {




/**
 * \ingroup leo-satellite
 * \brief ground station mobility model.
 *
 * For a simplified simulation, ground stations will be placed along the 
 * longitude of satellites orbiting above and at varying latitudes
 * Currently supports two ground stations
 */
class AircraftMobilityModel : public MobilityModel
{
public:
  /**
   * Register this type with the TypeId system.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);

  AircraftMobilityModel();

  AircraftMobilityModel(
                                                      std::string name, double angleIncidence, 
                                                      std::string dataRate,
                                                      uint32_t numGSLs);
  AircraftMobilityModel(
                                                      std::string name, double angleIncidence, 
                                                      std::string dataRate);
  
  // FullGroundStationMobilityModel();
  virtual ~AircraftMobilityModel();
  
  void SetAircraftTrackLog (std::vector<std::pair< double, Vector3D>> &geomap);

  
  double GetAngleOfIncidence(void) const ;
  std::string GetDataRate(void) const;
  double GetNumAsl(void) const;
  
  std::string GetName(void) const;
  
  
  Vector3D PropagateLatLonAlt (void) const; 
  Vector3D PropagateLatLonAlt (double time) const;

  Topos GetVisibilityAirToSat (const Vector satPos, const Vector satVel) ;

private:
  virtual void DoSetPosition (const Vector &position);
  virtual Vector DoGetPosition (void) const;
  virtual Vector DoGetVelocity (void) const;
  
  

  std::vector<std::pair< double, Vector3D>> m_airTrackLog;
  
  // double m_antennaSpecs;  Data rate, incidence angle, etc.
  double m_angleIncidence;
  std::string m_name;
  std::string m_dataRate;
  uint32_t m_numAsl; // air to satellite links
  


}; // end class

double interp1d (double x, double x1, double y1, double x2, double y2);
Vector3D LatLonAltInterp(double epoch, double epoch1, Vector3D latlonalt1, 
			  double epoch2, Vector3D latlonalt2);
Vector3D AirPropagator(double epoch, std::vector< std::pair <double, Vector3D >> airTrackLog);

} // namespace ns3

#endif /* AIRCRAFT_MOBILITY_H */


