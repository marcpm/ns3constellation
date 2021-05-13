/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * LEO Satellite Constellation Config
 * Creates and maintains all satellites and links within a satellite communication network
 *
 * ENSC 427: Communication Networks
 * Spring 2020
 * Team 11
 */
#ifndef LEO_SATELLITE_CONFIG_H
#define LEO_SATELLITE_CONFIG_H

#include "ns3/vector.h"
#include "ns3/object.h"
#include "ns3/ptr.h"
#include "ns3/core-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/leo-satellite-mobility.h"
#include "ns3/ground-station-mobility.h"
#include <vector>
#include "ns3/mobility-module.h"
#include "ns3/csma-module.h"
#include <cmath>
#include "ns3/internet-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/applications-module.h"
#include <sstream>
#include <vector>
#include <fstream>
#include <string>

namespace ns3 {

class LeoSatelliteConfig : public Object
{
public:
  /**
   * Register this type with the TypeId system.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);

  LeoSatelliteConfig (std::string TLEfilepath, std::string GSfilepath);

  virtual ~LeoSatelliteConfig ();
  virtual TypeId GetInstanceTypeId (void) const;
  
  void ReadSatConfigFile (std::string TLEfilepath); // reads and parses file containing 3 line TLEs.
  void ReadGSConfigFile (std::string GSfilepath);
  
  void UpdateLinks (); //update the intersatellite links

  
  NodeContainer m_satellitesNodes;
  NodeContainer m_groundStationsNodes; //node container to hold ground stations

private:
  std::vector<Satellite> m_constellationSats;
  std::vector<GroundStation> m_groundStations;
  
  NodeContainer m_groundStationsNodes;
  std::vector<NetDeviceContainer> m_groundStationsdevices; 
  std::vector<Ptr<CsmaChannel>> m_groundStationschannels;
  std::vector<Ipv4InterfaceContainer> m_groundStationsinterfaces;
  std::vector<uint32_t> m_groundStationschannel_tracker;


  NodeContainer m_constellationSatsNodes;
  NetDeviceContainer m_islDevices;
  std::vector<Ptr<CsmaChannel>> m_oislChannels;
  std::vector<Ptr<CsmaChannel>> m_islChannels;
  std::vector<Ipv4InterfaceContainer> m_islInterfaces;
  std::vector<uint32_t> m_islTracker; //this will have the node from the adjacent plane that is currently connected

  
};
  
}

#endif /* LEO_SATELLITE_CONFIG_H */
