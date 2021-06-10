/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <utility>

#include "ns3/object.h"
#include "ns3/ptr.h"
#include "ns3/core-module.h"
#include "ns3/node.h"
#include "ns3/node-container.h"


// #include "ns3/satellite.h"
#include "ns3/mobility-model.h"
#include "ns3/mobility-helper.h"
#include "ns3/aircraft-mobility.h"


using namespace ns3;

NS_LOG_COMPONENT_DEFINE("AircraftExample");



std::vector<std::string> 
splitString (std::string s, std::string delimiter) 
{
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    std::string token;
    std::vector<std::string> res;

    while ((pos_end = s.find (delimiter, pos_start)) != std::string::npos) 
    {
        token = s.substr (pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back (token);
    }

    res.push_back (s.substr (pos_start));
    return res;
}








int 
main (int argc, char *argv[])
{
  std::cout << "*******************" << std::endl << "Running example Aircraft read" << std::endl;
  
  bool verbose = true;
  std::string filepath = "/home/tieg/plathon/ns3-constellations/ns3constellation/ns3/ns3constellation/ns-allinone-3.30.1/ns-3.30.1/src/satellite-constellation/examples/air-LHR_MAD-tracklog-sheet.AIR";

  CommandLine cmd;
  cmd.AddValue ("verbose", "Tell application to log if true", verbose);

  cmd.AddValue ("Airpath", "Set TLE filepath", filepath);
  cmd.Parse (argc,argv);

// / //  // // /

  std::vector <std::string>  airStrLine;
  std::string aircraftName;
  double angleIncidence;
  std::string dataRate;
  uint32_t numASLs;
  
  std::vector<std::pair<double, Vector3D>> m_airTrackLog;

   std::fstream airFile;
   airFile.open(filepath, std::ios::in); //open a file to perform read operation using file object
   if (airFile.is_open()) //checking whether the file is open
   {   
      std::string tmp;
      std::cout << "Reading Aircraft Logs" << std::endl;
    
      // Read Info 1st line
      getline(airFile, tmp);
      airStrLine = splitString(tmp, ",");
      aircraftName = airStrLine[0];
      angleIncidence = std::stof(airStrLine[1]);
      dataRate = airStrLine[2];
      numASLs = std::stoi(airStrLine[3]);

      // Position interpolation lines
      while(getline(airFile, tmp))
      { //read data from file object and put it into string.

        std::vector <std::string> airStrLine = splitString(tmp, ",");
        // std::string gsId = airStrLine[0];
        double airTime = std::stof(airStrLine[0]);
        double airLat = std::stof(airStrLine[1]);
        double airLon = std::stof(airStrLine[2]);
        double airAlt = std::stof(airStrLine[3]);
      
        std::pair < double, Vector3D> parsedData = {airTime, Vector3D(airLat, airLon, airAlt)};
        m_airTrackLog.push_back(parsedData);

      }
      airFile.close(); //close the file object.
   
    }
    else
    {
      std::runtime_error( "Error Reading  Config File");
    }


    

  NodeContainer testAircraftNodes;
  testAircraftNodes.Create(1);

  MobilityHelper mobility;
  mobility.SetMobilityModel(
                  "ns3::AircraftMobilityModel",
                  "Name", StringValue(aircraftName),
                  "DataRate", StringValue(dataRate),
                  "AngleIncidence", DoubleValue(angleIncidence),
                  "NumAsl", IntegerValue(numASLs)
                  );
  
  mobility.Install(testAircraftNodes.Get(0));


  for (NodeContainer::Iterator j = testAircraftNodes.Begin ();
       j != testAircraftNodes.End (); ++j)
    {
      Ptr<Node> node = *j;
      Ptr<AircraftMobilityModel> mobModel = node->GetObject<AircraftMobilityModel> ();    
      mobModel->SetAircraftTrackLog(m_airTrackLog); // needs to be done to initialize

      
    }


  
  


  double now = 22.0; //seconds
  Vector3D latlonalt = testAircraftNodes.Get(0) -> GetObject <AircraftMobilityModel>()->PropagateLatLonAlt(now);
  std::cout << "\n Geo position at " << now << "seconds" << std::endl;
  std::cout << latlonalt.x << ", " << latlonalt.y << ", " << latlonalt.z << std::endl;
  std::cout << "*******************" << std::endl; 
  return 0;
   }






