/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "ns3/core-module.h"
#include "ns3/constellation-helper.h"
#include "ns3/satelite.h"

using namespace ns3;


int 
main (int argc, char *argv[])
{
  bool verbose = true;

  CommandLine cmd (__FILE__);
  cmd.AddValue ("verbose", "Tell application to log if true", verbose);

  cmd.Parse (argc,argv);


//  READING and Storing TLEs as Satellite objects

   std::vector<Satellite> constellation;
   fstream newfile;
   newfile.open("celestrak-iridium.txt",ios::in); //open a file to perform read operation using file object
   if (newfile.is_open()){   //checking whether the file is open
      string satname;
      string line1;
      string line2;
      string tp;
      int i = 0;
      while(getline(newfile, tp))
      { //read data from file object and put it into string.
        if (i % 3 == 0)
        {
          if (satname != "") // construct + append satellite object if not first line of file
          {
            Satellite satellite;
            satellite.SetName(satname);
            satellite.SetTleInfo(line1, line2);
            constellation.push_back(satellite);
          }

          satname = tp;
          cout << satname << endl;
        }else if ( (i-1) % 3 == 0)
        {
          line1 = tp;
        }else if ( (i-2) % 3 == 0)
        {
          line2 = tp;
        }

        i++;
          }
      newfile.close(); //close the file object.

      for (auto sat: constellation)
      {
        std::cout << sat << std::endl;
      }
    
    return 0;
   }










  /* ... */

  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}


