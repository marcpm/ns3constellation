/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#ifndef SATELLITE_NETWORK_CONFIG_H
#define SATELLITE_NETWORK_CONFIG_H

#include "ns3/vector.h"
#include "ns3/object.h"
#include "ns3/ptr.h"
#include "ns3/core-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/log.h"

// #include "ns3/orbital-coords.h"
#include "ns3/basic-orbital.h"
#include "ns3/satellite-position-helper.h"
#include "ns3/satellite-position-mobility-model.h"
#include "ns3/full-ground-station-mobility.h" 
#include "ns3/mobility-module.h"

#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/applications-module.h"

#include <iostream>
#include <sstream>
#include <vector>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include <stdexcept>
#include <algorithm>
#include <utility>

namespace ns3 {

class SatelliteNetworkConfig : public Object
{
public:
  /**
   * Register this type with the TypeId system.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);

  // SatelliteNetworkConfig (std::string TLEfilepath, std::string GSfilepath);
  SatelliteNetworkConfig (std::string &TLEfilepath, std::string &GSfilepath)
  
  SatelliteNetworkConfig (std::string TLEfilepath, std::string GSfilepath, uint32_t islPerSat)

  SatelliteNetworkConfig (std::string TLEfilepath, std::string GSfilepath, uint32_t islPerSat, std::string islDataRate)

  SatelliteNetworkConfig (std::string TLEfilepath, std::string GSfilepath, std::string airFilepath, uint32_t islPerSat, std::string islDataRate)


  virtual ~SatelliteNetworkConfig ();
  virtual TypeId GetInstanceTypeId (void) const;
  
  void ReadSatConfigFile (std::string TLEfilepath); // reads and parses file containing 3 line TLEs.
  std::vector<std::string> splitString (std::string s, std::string delimiter);
  void ReadGSConfigFile (std::string GSfilepath);
  

  void BuildISLsP2PFixed();

  void BuildISLsDynamic(); // non working 

  void BuildGSLsSingleSat();

  void BuildGSLsMultiSat();

  void SetupIPConfig();

  void BuildNetwork (std::string &TLEfilepath, std::string &GSfilepath);

  void UpdateLinks (); //update the intersatellite links

  uint32_t m_nISLsPerSat; // num of ISLs per sat
  std::string m_islDataRate;
  NodeContainer m_satellitesNodes;
  NodeContainer m_groundStationsNodes; //node container to hold ground stations


  std:string m_TLEfilepath;
  std:string m_GSfilepath;
  std:string m_airFilepath;

private:
  std::vector<Satellite> m_constellationSats;
  // std::vector<GroundStation> m_groundStations;


  
  NodeContainer m_groundStationsNodes;
  std::vector<NetDeviceContainer> m_groundStationsDevices; 
  std::vector<Ptr<CsmaChannel>> m_groundStationsChannels;
  std::vector<Ipv4InterfaceContainer> m_groundStationsInterfaces;
  std::vector<uint32_t> m_groundStationsChannelTracker;


  // NodeContainer m_constellationSatsNodes;
  NetDeviceContainer m_islDevices;
  std::vector<Ptr<CsmaChannel>> m_islChannels;
  std::vector<Ptr<CsmaChannel>> m_islChannels;
  std::vector<Ipv4InterfaceContainer> m_islInterfaces;
  std::vector < std::pair < uint32_t, uint32_t > >  m_islChannelTracker; // keep track of already linked satellites to skip iterations.
  double CalculateDistance(Vector &pos1, Vector &pos2);
  double CalculateDistance(PVCoords &pv1, PVCoords &pv2);
};
  
}

#endif /* SATELLITE_NETWORK_CONFIG_H */
