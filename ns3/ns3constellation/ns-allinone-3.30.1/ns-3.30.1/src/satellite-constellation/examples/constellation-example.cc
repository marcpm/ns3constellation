/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include <iostream>
#include <fstream>
#include <string>
#include <vector>


#include "ns3/core-module.h"
#include "ns3/satellite.h"
#include "ns3/julian-date.h"


using namespace ns3;

NS_LOG_COMPONENT_DEFINE("ConstellationExample");





std::vector<Satellite> 
readTLEdata(std::string filepath)
{
  
  std::vector<Satellite> constellation;

   std::fstream newfile;
   newfile.open(filepath, std::ios::in); //open a file to perform read operation using file object
   if (newfile.is_open()){   //checking whether the file is open
      std::string satname;
      std::string line1;
      std::string line2;
      std::string tp;
      int i = 0;
      while(std::getline(newfile, tp))
      { //read data from file object and put it into string.
        if (i % 3 == 0)
        {
          if (i>0) // construct + append satellite object if not first line of file
          {
            Satellite satellite;
            satellite.SetName(satname);
            // std::cout << "line 1 length: "<< line1.size() << std::endl;
            satellite.SetTleInfo(line1, line2);
            constellation.push_back(satellite);
          }

          satname = tp;
          // std::cout << satname << endl;
        }else if ( (i-1) % 3 == 0)
        {
          // std::cout << "Breaking Char: "<< toRawString(tp) << std::endl;
          line1 = tp.substr(0, 69);
        }else if ( (i-2) % 3 == 0)
        {
          line2 = tp.substr(0, 69);
        }

        i++;
          }
      newfile.close(); //close the file object.
   } else
   {
     std::cout << "Error reading TLE file" << std::endl;
   }


  return constellation;
}




int 
main (int argc, char *argv[])
{
  std::cout << "*******************" << std::endl << "Running example TLE read" << std::endl;
  
  bool verbose = true;
  std::string filepath = "/home/tieg/plathon/ns3-constellations/ns3constellation/ns3/ns3constellation/ns-allinone-3.30.1/ns-3.30.1/src/satellite-constellation/examples/celestrak-iridium.txt";

  CommandLine cmd;
  cmd.AddValue ("verbose", "Tell application to log if true", verbose);

  cmd.AddValue ("TLEpath", "Set TLE filepath", filepath);
  cmd.Parse (argc,argv);


  
//  READING and Storing TLEs as Satellite objects
  std::vector<Satellite> constellation = readTLEdata(filepath);

  JulianDate randomDate =  constellation.at(0).GetTleEpoch();
  
    for (uint32_t i = 0; i < constellation.size(); i++)
    {
      Satellite sat = constellation.at(i);
      // JulianDate tleEpoch = sat.GetTleEpoch();
      JulianDate tleEpoch = randomDate;

      std::cout << sat.GetName() << std::endl;
      if (i % 10 == 0)
      {
        std::cout << "ECEF Pos: " << sat.GetPosition(tleEpoch).x <<", "  << sat.GetPosition(tleEpoch).y <<", " << sat.GetPosition(tleEpoch).z << std::endl;
        std::cout << "Vel : " << sat.GetVelocity(tleEpoch).GetLength() / 1000 << " km/s" << std::endl;
        std::cout << "Lat: " << sat.GetGeographicPosition(tleEpoch).x << ", " << "Lon: " << sat.GetGeographicPosition(tleEpoch).y << ", " <<
        "Alt: " << sat.GetGeographicPosition(tleEpoch).z / 1000<< " km" << std::endl;
        std::cout << "Orbital Period: " << sat.GetOrbitalPeriod().GetSeconds() / 60 << " min"<< std::endl;
        std::cout << std::endl; 
      }
    } 
    std::cout << "*******************" << std::endl; 
    return 0;
   }






