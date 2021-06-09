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


  double linkTimeStep = 100.0;
  double timespan = 2000.0;
  uint32_t gsServerId = 0;
  uint32_t gsClientId = 0;
  
  


  CommandLine cmd;
  
  std::string tleFilepath = "/home/tieg/plathon/ns3-constellations/ns3constellation/case-studies/testcase1/IRIDIUM-1622772974_519141.TLE";
  std::string gsFilepath = "/home/tieg/plathon/ns3-constellations/ns3constellation/case-studies/testcase1/OneWebDemoGStations.GS";
  
  cmd.AddValue ("satPath", "Info file path.", tleFilepath);
  cmd.AddValue ("gsPath", "Ground Stations file Path.", gsFilepath);
  
  cmd.AddValue ("gsClientId", "Client Ground Station id (row in file) .", gsClientId); // options
  cmd.AddValue ("gsServerId", "Server Ground Station id (row in file) .", gsServerId);


  cmd.AddValue ("linkTimeStep", "Link Update Time in seconds", linkTimeStep);
  cmd.AddValue ("timespan", "Simulation timespan in seconds", timespan);


  cmd.Parse (argc,argv);

  LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
  LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);

  std::cout << "Pre satnetwork build;" << std::endl;

  SatelliteNetworkConfig sat_network(tleFilepath, gsFilepath);
  
  std::cout << "Post satnetwork build;" << std::endl;

  UdpEchoServerHelper echoServer (9);

  ApplicationContainer serverApps = echoServer.Install(sat_network.m_groundStationsNodes.Get(gsServerId));
  serverApps.Start (Seconds (1.0));
  serverApps.Stop (Seconds (500.0));

  UdpEchoClientHelper echoClient (sat_network.m_groundStationsInterfaces[gsServerId].GetAddress(1), 9);
  echoClient.SetAttribute("MaxPackets", UintegerValue (20));
  echoClient.SetAttribute("Interval", TimeValue(Seconds(100.0)));
  echoClient.SetAttribute("PacketSize", UintegerValue (1024));

  ApplicationContainer clientApps = echoClient.Install (sat_network.m_groundStationsNodes.Get(gsClientId));
  clientApps.Start (Seconds (2.0));
  clientApps.Stop (Seconds (500.0));

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
  std::string simulationInfo = base_filename.substr(0, p);


  std::stringstream ss;
  std::string file_title;
  ss << "basic-satellite-constellation-example-"<< simulationInfo<<".flowmon";
  ss >> file_title;

  flowmonHelper.SerializeToXmlFile (file_title, false, false);
  std::cout << "Flowmon file written successfully. ";
  return 0; 
}


