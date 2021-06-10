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


  double linktimestepsize = 100.0;
  double timespan = 2000.0;

  CommandLine cmd;

  
  cmd.AddValue ("satPath", "Info Satellite file path.", tleFilepath);
  
  cmd.AddValue ("gsPath", "Ground Stations file Path.", gsFilepath);
  cmd.AddValue ("airPath", "Air routes file Path.", airFilepath);

  cmd.AddValue ("gsClientId", "Client Ground Station id (row in file) .", gsClientId); // options
  cmd.AddValue ("gsServerId", "Server Ground Station id (row in file) .", gsServerIdf);
  cmd.AddValue ("airServerId", "Server Ground Station id (row in file) .", airServerId);
  cmd.AddValue ("airClientId", "Client Ground Station id (row in file) .", airClientId); 


  cmd.AddValeu ("linkTimeStep", "Link Update Time in seconds", linkTimeStep)
  cmd.AddValeu ("timespan", "Simulation timespan in seconds", timespan)


  cmd.Parse (argc,argv);

  LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
  LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);

  SatelliteNetworkConfig sat_network(tleFilepath, gsFilepath, 1, "300Mbps", airFilepath);
  
  UdpEchoServerHelper echoServer (9);

  ApplicationContainer serverApps = echoServer.Install(sat_network.ground_stations.Get(gsServerId));
  serverApps.Start (Seconds (1.0));
  serverApps.Stop (Seconds (2000.0));

  UdpEchoClientHelper echoClient (sat_network.ground_station_interfaces[0].GetAddress(1), 9);
  echoClient.SetAttribute("MaxPackets", UintegerValue (20));
  echoClient.SetAttribute("Interval", TimeValue(Seconds(100.0)));
  echoClient.SetAttribute("PacketSize", UintegerValue (1024));

  ApplicationContainer clientApps = echoClient.Install (sat_network.ground_stations.Get(gsClientId));
  clientApps.Start (Seconds (2.0));
  clientApps.Stop (Seconds (2000.0));

  FlowMonitorHelper flowmonHelper;
  flowmonHelper.InstallAll();

  uint32_t n_timesteps = timespan / linkTimeStep;

  for(uint32_t i=0; i< n_timesteps-1; i++)
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
  simulationInfo = "testSimulation"
  std::stringstream ss;
  std::string file_title;
  ss << "airborne-satellite-constellation-example-"<< simulationInfo<<".flowmon";
  ss >> file_title;

  flowmonHelper.SerializeToXmlFile (file_title, false, false);
  return 0; 
}


