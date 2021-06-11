/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */


#include "ns3/core-module.h"
#include "ns3/satellite-network-config.h"
#include "ns3/applications-module.h"
#include "ns3/flow-monitor-helper.h"
#include <string>
#include <sstream>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("AirborneSatelliteConstellationExample");

int 
main (int argc, char *argv[])
{


  double linkTimeStep = 200.0;
  double timespan = 1000.0;
  uint32_t nISLsPerSat = 2;
  uint32_t gsServerId;
  uint32_t gsClientId;
  uint32_t airServerId;
  uint32_t airClientId;
  std::string islDataRate = "300Mbps";

  std::string tleFilepath = "/home/tieg/plathon/ns3-constellations/ns3constellation/case-studies/testcase5airborne/STARLINK-21160_89993.TLE";
  std::string gsFilepath = "/home/tieg/plathon/ns3-constellations/ns3constellation/case-studies/testcase5airborne/OneWebDemoGStations2.GS";
  std::string airFilepath = "/home/tieg/plathon/ns3-constellations/ns3constellation/case-studies/testcase5airborne/air-LHR_MAD-tracklog-sheet.AIR";  
  
  CommandLine cmd;
  cmd.AddValue ("tlePath", "Info file path.", tleFilepath);
  cmd.AddValue ("gsPath", "Ground Stations file Path.", gsFilepath);
  cmd.AddValue ("airPath", "Aircrfat track Log file Path.", airFilepath);
  
  cmd.AddValue ("gsClientId", "Client Ground Station id (row in file)", gsClientId); 
  cmd.AddValue ("gsServerId", "Server Ground Station id (row in file)", gsServerId);

  cmd.AddValue ("linkTimeStep", "Link Update Time in seconds", linkTimeStep);
  cmd.AddValue ("timespan", "Simulation timespan in seconds", timespan);

  cmd.AddValue ("airServerId", "Aircraft Id as server ", airServerId);
  cmd.AddValue("airClientId", "Aircraft Id  as client ", airClientId);

  cmd.AddValue("nIslsPerSat", "Set number of ISLs per satellite ", nISLsPerSat);
  cmd.AddValue("islDataRate", "Set Datarate string ", islDataRate);

  cmd.Parse (argc,argv);

  LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
  LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);

  std::cout << "Pre satnetwork build;" << std::endl;

  SatelliteNetworkConfig sat_network(tleFilepath, gsFilepath, nISLsPerSat, islDataRate, airFilepath);
  
  std::cout << "Post satnetwork build;" << std::endl;

  UdpEchoServerHelper echoServer (9);

  ApplicationContainer serverApps = echoServer.Install(sat_network.m_aircraftNodes.Get(airServerId));
  serverApps.Start (Seconds (1.0));
  serverApps.Stop (Seconds (timespan));

  UdpEchoClientHelper echoClient (sat_network.m_aircraftInterfaces[airServerId].GetAddress(0), 9);
  echoClient.SetAttribute("MaxPackets", UintegerValue (20));
  echoClient.SetAttribute("Interval", TimeValue(Seconds(linkTimeStep)));
  echoClient.SetAttribute("PacketSize", UintegerValue (1024));

  NodeContainer clientContainer;
  for (int i = 0; i < ((int) sat_network.m_groundStationsNodes.GetN()) ; i++)
  {
    if (i != (int) gsServerId)
    {
      clientContainer.Add(sat_network.m_groundStationsNodes.Get(i));
    }
  }

  // ApplicationContainer clientApps = echoClient.Install (sat_network.m_groundStationsNodes.Get(gsClientId));
  ApplicationContainer clientApps = echoClient.Install (clientContainer);
  clientApps.Start (Seconds (2.0));
  clientApps.Stop (Seconds (timespan));

  FlowMonitorHelper flowmonHelper;
  flowmonHelper.InstallAll();

  int n_timesteps = timespan / linkTimeStep;

  for(int i=0; i< n_timesteps-1; i++)
  {
    Simulator::Stop(Seconds(linkTimeStep));
    Simulator::Run();
    sat_network.UpdateLinks();
  }
  Simulator::Stop(Seconds(linkTimeStep)); // last iteration w/o updatelinks
  Simulator::Run();
 
  Simulator::Destroy ();



  // parse info File. 
  // simulationInfo = readParse(infoFilepath)
  // std::string simulationInfo = "IRIDIUM";
    
  std::string base_filename = tleFilepath.substr(tleFilepath.find_last_of("/\\") + 1);
  std::string::size_type const p(base_filename.find_last_of('.'));

  std::string parentDirPath = tleFilepath.substr(0, tleFilepath.find_last_of("/\\"));  
  std::string simulationInfo = base_filename.substr(0, p); // get TLE filename (Constellation-EpochJulianday_epoch) w/o extension



  std::stringstream ss;
  std::string resultsFilename;
  ss << "results-basic-airborne-satellite-constellation-example-"<< simulationInfo<<".flowmon";
  ss >> resultsFilename;
  
  std::string resultsFilepath = parentDirPath + "/" + resultsFilename;
  flowmonHelper.SerializeToXmlFile (resultsFilepath, false, false);
  std::cout << "Flowmon file written successfully.  at " << resultsFilepath << std::endl;
  return 0; 
}


