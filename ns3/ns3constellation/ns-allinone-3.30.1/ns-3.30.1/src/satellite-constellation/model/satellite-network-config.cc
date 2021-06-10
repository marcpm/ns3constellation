/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */


#include "satellite-network-config.h"

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (SatelliteNetworkConfig);
NS_LOG_COMPONENT_DEFINE ("SatelliteNetworkConfig");

extern double CalculateDistanceGroundToSat (const Vector &a, const Vector &b);

double SPEED_OF_LIGHT = 299792458; //in m/s

//typeid
TypeId SatelliteNetworkConfig::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::SatelliteNetworkConfig")
  .SetParent<Object> ()
  .SetGroupName("SatelliteNetworkConfig")
  ;
  return tid;
}

// SatelliteNetworkConfig::SatelliteNetworkConfig (std::string TLEfilepath, std::string GSfilepath, uint32_t islPerSat): 
//     m_TLEfilepath(TLEfilepath), m_GSfilepath(GSfilepath), m_nISLsPerSat(islPerSat) // num of ISLs per sat
  
// {
//   BuildNetwork ();
// }


// Constructors GS - SAT network
SatelliteNetworkConfig::SatelliteNetworkConfig (std::string TLEfilepath, std::string GSfilepath)
: m_TLEfilepath(TLEfilepath), m_GSfilepath(GSfilepath),  m_nISLsPerSat(2), m_islDataRate("300Mbps"), m_AIRfilepath("")
{
  BuildNetwork();
}

SatelliteNetworkConfig::SatelliteNetworkConfig (std::string TLEfilepath, std::string GSfilepath, uint32_t islPerSat)
: m_TLEfilepath(TLEfilepath), m_GSfilepath(GSfilepath),  m_nISLsPerSat(islPerSat), m_islDataRate("300Mbps"), m_AIRfilepath("")
{
  BuildNetwork();
}

SatelliteNetworkConfig::SatelliteNetworkConfig (std::string TLEfilepath, std::string GSfilepath, uint32_t islPerSat, std::string islDataRate)
: m_TLEfilepath(TLEfilepath), m_GSfilepath(GSfilepath),  m_nISLsPerSat(islPerSat), m_islDataRate(islDataRate), m_AIRfilepath("")
{
  BuildNetwork();
}

// Constructors GS - AIR - SAT network
SatelliteNetworkConfig::SatelliteNetworkConfig (std::string TLEfilepath, std::string GSfilepath, uint32_t islPerSat,  std::string islDataRate, std::string airFilepath)
: m_TLEfilepath(TLEfilepath), m_GSfilepath(GSfilepath),  m_nISLsPerSat(islPerSat), m_islDataRate(islDataRate), m_AIRfilepath(airFilepath)
{
  BuildNetwork();
}





SatelliteNetworkConfig::~SatelliteNetworkConfig ()
{
}

TypeId SatelliteNetworkConfig::GetInstanceTypeId (void) const
{
  TypeId tid = this->GetTypeId();
  return tid;
}

void SatelliteNetworkConfig::ReadSatConfigFile (std::string satConfigFilepath)
{
   std::cout << "Reading Sat Config File." << std::endl;
   std::fstream satFile;
   satFile.open(satConfigFilepath, std::ios::in); //open a file to perform read operation using file object
   if (satFile.is_open()) //checking whether the file is open
   {   
      
     /*
     ***************
     filename === simulation start
     */
      // std::string::size_type n;
      // n = satConfigFilepath.find("___"); // (e.g. oneweb__2021-05-12 00:00:00.txt)
      // std::string timestampplus = satConfigFilepath.substr(n+3); // "2021-05-12 00:00:00.txt"
      // std::string utcSimulationStart = timestampplus.substr( timestampplus.length() - 4);
      
           /*
     ***************
    first line TLE txt === simulation start
     */
      std::cout << "Inside file open" << std::endl;
      std::string utcSimulationStart;
      getline(satFile, utcSimulationStart); //first line utc timestamp of simulation start. (multi TLE coherence)
      JulianDate  simulationStartTime(utcSimulationStart, DateTime::TimeSystem::UTC); // satellite module julian date start
      std::cout <<"Starting Simulation at "<< utcSimulationStart <<std::endl;


      std::cout <<"Reading Satellite Data ...." << std::endl;
      std::string satname;
      std::string line1;
      std::string line2;
      std::string tmp;
      
      int lineNum = 0;
      int idxSat = 0;
      while(getline(satFile, tmp))
      { //read data from file object and put it into string.
        if (lineNum % 3 == 0)
        {
          if (lineNum > 0) // construct + append satellite object every 3 lines read.
          {
            // std::cout << "Pre Satellite Ptr" << std::endl;
            Ptr<Satellite> satellite = CreateObject<Satellite>();
            satellite->SetName(satname);
            

            satellite->SetTleInfo(line1, line2);

            std::cout << satellite->GetName() << std::endl;
            // m_constellationSats.push_back(satellite); // add satellites to member variable: vector of satellites
            m_satellitesNodes.Create(1); // add new satellite node 
            MobilityHelper satMobility;
            
            satMobility.SetMobilityModel(
                        "ns3::SatellitePositionMobilityModel",
                        "SatellitePositionHelper",
                        SatellitePositionHelperValue(SatellitePositionHelper(satellite, simulationStartTime) )
                );
            // satMobility.SetSatellite(satellite);
            // satMobility.SetStartTime(simulationStartTime);
            satMobility.Install(m_satellitesNodes.Get(idxSat)); // install mobility in new node created

            idxSat++;

          }

          satname = tmp; // read satellite name line
          
        }else if ( (lineNum-1) % 3 == 0)
        {
          if (tmp.size() > 69)
          {
            line1 = tmp.substr(0, 69);

          }else
          {
            line1 = tmp;  // read TLE - line 1
          }
        }else if ( (lineNum-2) % 3 == 0)
        {
          if (tmp.size() > 69)
          {
            line2 = tmp.substr(0, 69);

          }else
          {
            line2 = tmp;  // read TLE - line 1
          }
        }

        lineNum++;
      }
      satFile.close(); //close the file object.

   
    }
    else
    {
      std::cout << "Error Reading  Config File" << std::endl;
    }
}

// extra util split string with chosen delimiter
std::vector<std::string> 
SatelliteNetworkConfig::splitString (std::string s, std::string delimiter) 
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



void SatelliteNetworkConfig::ReadGSConfigFile (std::string GSfilepath)
{
   std::fstream gsFile;
   gsFile.open(GSfilepath, std::ios::in); //open a file to perform read operation using file object
   if (gsFile.is_open()) //checking whether the file is open
   {   
      std::string GSname;
      std::string tmp;
      
      uint32_t idxGs = 0;
      while(getline(gsFile, tmp))
      { //read data from file object and put it into string.

        std::cout << "Reading GSs" << std::endl;
        std::vector <std::string> gsStrLine = splitString(tmp, ",");
        // std::string gsId = gsStrLine[0];
        std::string gsName = gsStrLine[1];
        double gsLat = std::stof(gsStrLine[2]);
        double gsLon = std::stof(gsStrLine[3]);
        double gsAlt = std::stof(gsStrLine[4]);
        double angleIncidence = std::stof(gsStrLine[5]);
        std::string dataRate = gsStrLine[6];
        uint32_t numGSLs = std::stoi(gsStrLine[7]);

        m_groundStationsNodes.Create(1);

        MobilityHelper mobility;
        mobility.SetMobilityModel(
                        "ns3::FullGroundStationMobilityModel",
                        "Latitude", DoubleValue(gsLat),
                        "Longitude", DoubleValue(gsLon),
                        "Altitude", DoubleValue(gsAlt),
                        "Name", StringValue(gsName),
                        "DataRate", StringValue(dataRate),
                        "AngleIncidence", DoubleValue(angleIncidence),
                        "NumGsl", IntegerValue(numGSLs)
                        );
                
        mobility.Install(m_groundStationsNodes.Get(idxGs));
    
        // Ptr<MobilityModel> mobilityModel = m_groundStationsNodes.Get(idxGs)->GetObject<FullGroundStationMobilityModel>();
        // mobilityModel->SetPosition();     

        // GroundStation groundStation(gsLat, gsLon, gsAlt, gsName);
        // m_groundStations.push_back(groundStation);

        idxGs++;
      }
      gsFile.close(); //close the file object.
   
    }
    else
    {
      std::runtime_error( "Error Reading  Config File");
    }
}


void SatelliteNetworkConfig::ReadAirConfigFile (std::string Airfilepath)
{

  std::vector <std::pair <double, Vector3D>> airTrackLog;
  std::vector <std::string>  airStrLine;
  std::string aircraftName;
  double angleIncidence;
  std::string dataRate;
  uint32_t numASLs;
  

   std::fstream airFile;
   airFile.open(Airfilepath, std::ios::in); //open a file to perform read operation using file object
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
        airTrackLog.push_back(parsedData);

      }
      airFile.close(); //close the file object.
   
    }
    else
    {
      std::runtime_error( "Error Reading  Air Config File");
    }
  
  m_aircraftNodes.Create(1);

  MobilityHelper mobility;
  mobility.SetMobilityModel(
                  "ns3::AircraftMobilityModel",
                  "Name", StringValue(aircraftName),
                  "DataRate", StringValue(dataRate),
                  "AngleIncidence", DoubleValue(angleIncidence),
                  "NumAsl", IntegerValue(numASLs)
                  );
  
  mobility.Install(m_aircraftNodes.Get(0));


  for (NodeContainer::Iterator j = m_aircraftNodes.Begin ();
       j != m_aircraftNodes.End (); ++j)
    {
      Ptr<Node> node = *j;
      Ptr<AircraftMobilityModel> mobModel = node->GetObject<AircraftMobilityModel> ();    
      mobModel->SetAircraftTrackLog(airTrackLog); // Cleaner way to pass std::vector in ns3.9.
      
    }






}


void SatelliteNetworkConfig::BuildISLsP2PFixed()
{

  /* 
  ******* ******* ******* ******* ******* ******* ******* ******* ******* 
  ******* ******* ******* ******* ISL Links ***** ******* ******* ******* 
  ******* ******* ******* ******* ******* ******* ******* ******* ******* 
  */
  //Install IP stack
  InternetStackHelper stack;
  stack.Install(m_satellitesNodes);
  // std::vector < std::vector<int> >  m_islChannelTracker; // keep track of already linked satellites to skip iterations.


  for (uint32_t idxSat1=0; idxSat1 < m_satellitesNodes.GetN(); idxSat1++)
  {
    


    // count all links using idxSat1, skip iteration if equal to ISLs per sat.
    uint32_t nidxSat1LinksRemaining = m_nISLsPerSat - std::count_if( m_islChannelTracker.begin(),  m_islChannelTracker.end(),
                                                                      [&idxSat1](std::pair<uint32_t, uint32_t> const& v) 
                                                                      { return (v.first == idxSat1) || (v.second == idxSat1);}
                                                                  ); 



    if( nidxSat1LinksRemaining <= 0 )
    {
      std::cout <<"Skipping satIdx1 = #" << idxSat1 << ". All links full." << std::endl;
      
      continue;
    } 

    Vector sat1Pos = m_satellitesNodes.Get(idxSat1)->GetObject<MobilityModel>()->GetPosition();
    // PVCoords satPVecef(satPos, satVel, FrameType::ECEF);
    // uint32_t closestSatId = 0;
    // double closestSatDist = 0;
    std::vector < std::pair< uint32_t, double > > idxDistVec;

    // find closest adjacent satellite 
    for (uint32_t idxSat2=0; idxSat2 < m_satellitesNodes.GetN(); idxSat2++)
    {

      // check if (idx2Sat already has  max ISLs defined) 
      if( (std::count_if( m_islChannelTracker.begin(),  m_islChannelTracker.end(),
                              [&idxSat2](std::pair<uint32_t, uint32_t> const& v) 
                              { return (v.first == idxSat2) || (v.second == idxSat2);}
                        ) >= m_nISLsPerSat
          )   
                    || //or ( idx1Sat--idx2Sat ISL is already defined)
          (std::count_if( m_islChannelTracker.begin(),  m_islChannelTracker.end(),
                                        [idxSat1, idxSat2](std::pair<uint32_t, uint32_t>  const& v) 
                                        { return (v.first == idxSat1  && v.second == idxSat2) ||
                                            (v.first == idxSat2  && v.second == idxSat1);
                                        }
                        ) > 0
          )                   
        )
      {
        std::cout <<"idxSat2 full or ISL already defined -- Skipping ISL Link between satIdx1 = " << idxSat1 << " and  satIdx2 = " << idxSat2 << std::endl;
        continue;
      } 

      if (idxSat2 != idxSat1)
      {
        Vector3D sat2Pos = m_satellitesNodes.Get(idxSat2)->GetObject<MobilityModel>()->GetPosition();
        // Vector3D sat2Vel = m_constellationSatsNodes.Get(idxSat2)->GetObject<MobilityModel>()->GetVelocity();
        // PVCoords satPVecef(satPos, satVel, FrameType::ECEF);

        double temp_dist = CalculateDistance(sat1Pos, sat2Pos);

        std::pair< uint32_t, double > idxDist = {idxSat2, temp_dist};
        idxDistVec.push_back(idxDist);

        // if (temp_dist < 2*6000000.0)

        // {
        //   std::pair< uint32_t, double > idxDist = {idxSat2, temp_dist};
        //   idxDistVec.push_back(idxDist);
        // }
        // else 
        // {
        //   std::cout << "ISL not valid, no contact." << std::endl;
        //   continue;
        // }
      }
      else
      {
        std::cout << "Skipping same satellite ID ISL." << std::endl;
      }
      
      

    }

    if (idxDistVec.size() == 0 ) // no available ISLs
    {
      continue;
    }

    // sort  distance pair vectors from min to max, (keeping track of indexes) 
    std::sort(idxDistVec.begin(), idxDistVec.end(), [](std::pair <uint32_t, double> a, std::pair <uint32_t, double> b) 
                                    {return a.second < b.second;} 
                                    );
    


    // Iterating over links remaining for satIdx1: and adding links with idxSat2's starting from the one at min distance.

    uint32_t nLinksToSetup =  (nidxSat1LinksRemaining  < idxDistVec.size()) ?  nidxSat1LinksRemaining : idxDistVec.size();
    for (uint32_t islId=0; islId < nLinksToSetup  ; islId++ )
    {

      int idxSat2 = idxDistVec.at(islId).first;
      
      m_islChannelTracker.push_back({idxSat1, idxSat2}); // add satellite link to tracker.
      double distance =  idxDistVec.at(islId).second;      // in meters
      double delay = distance /  SPEED_OF_LIGHT; // in seconds
    
      PointToPointHelper isl_link_helper;
      isl_link_helper.SetDeviceAttribute ("DataRate", StringValue (m_islDataRate));
      isl_link_helper.SetChannelAttribute ("Delay", TimeValue(Seconds (delay)));

      // NetDeviceContainer temp_isl_netdevice_container;
      // temp_isl_netdevice_container = gsl_link_helper.Install(m_satellitesNodes.Get(idxSat1), m_satellitesNodes.Get(idxSat2)); // 

      m_islDevices.push_back( isl_link_helper.Install(m_satellitesNodes.Get(idxSat1), m_satellitesNodes.Get(idxSat2))  );

      std::cout <<"Setting up #"<<islId << "ISL link with distance of "<<distance/1000<<" km and delay of "<<delay<<" seconds."<< std::endl;

    }

  } //end for idxSat1

  std::cout << "ISLs successfully setup!" << std::endl;
}  // end of BuildISLsP2PFixed

void SatelliteNetworkConfig::BuildISLsDynamic() // non working 
{

  /* 
  ******* ******* ******* ******* ******* ******* ******* ******* ******* 
  ******* ******* ******* ******* ISL Links ***** ******* ******* ******* 
  ******* ******* ******* ******* ******* ******* ******* ******* ******* 
  */

  // std::vector < std::vector<int> >  m_islChannelTracker; // keep track of already linked satellites to skip iterations.


  // for (uint32_t idxSat1=0; idxSat1 < m_satellitesNodes.GetN(); idxSat1++)
  // {
    
  //   // count all links using idxSat1, skip iteration if equal to ISLs per sat.
  //   int nidxSat1LinksRemaining = m_nISLsPerSat - std::count_if( m_islChannelTracker.begin(),  m_islChannelTracker.end(),
  //                                                                     [&idxSat1](std::pair<uint32_t, uint32_t> const& v) 
  //                                                                     { return v.first == idxSat1 || v.second == idxSat1;}
  //                                                                 ); 

  //   if( nidxSat1LinksRemaining <= 0 )
  //   {
  //     std::cout << "Skipping satIdx1 = #" << idxSat1 << ". All links full." << std::endl;
      
  //     continue;
  //   } 

  //   Vector sat1Pos = m_satellitesNodes.Get(idxSat1)->GetObject<MobilityModel>()->GetPosition();
  //   // PVCoords satPVecef(satPos, satVel, FrameType::ECEF);
  //   // uint32_t closestSatId = 0;
  //   // double closestSatDist = 0;
  //   std::vector < std::pair< uint32_t, double > > idxDistVec;
  //   NodeContainer temp_node_container;
  //   temp_node_container.Add(m_satellitesNodes.Get(idxSat1));


  //   //find closest adjacent satellite for ground station
  //   for (uint32_t idxSat2=0; idxSat2 < m_satellitesNodes.GetN(); idxSat2++)
  //   {

  //     // check if (idx2Sat already has  max ISLs defined) 
  //     if( (std::count_if( m_islChannelTracker.begin(),  m_islChannelTracker.end(),
  //                             [&idxSat2](std::pair<uint32_t, uint32_t> const& v) 
  //                             { return v.first == idxSat2 || v.second == idxSat2;}
  //                       ) >= m_nISLsPerSat
  //         )   
  //                   || //or ( idx1Sat--idx2Sat ISL is already defined)
  //         (std::count_if( m_islChannelTracker.begin(),  m_islChannelTracker.end(),
  //                                       [idxSat1, idxSat2](std::pair<uint32_t, uint32_t>  const& v) 
  //                                       { return (v.first == idxSat1  && v.second == idxSat2) ||
  //                                           (v.first == idxSat2  && v.second == idxSat1);
  //                                       }
  //                       ) > 0
  //         )                   
  //       )
  //     {
  //       std::cout <<"Skipping ISL Link between satIdx1 = " << idxSat1 << " and  satIdx2 = " << idxSat2 << std::endl;
  //       continue;
  //     } 


      
  //     Vector3D sat2Pos = m_satellitesNodes.Get(idxSat2)->GetObject<MobilityModel>()->GetPosition();
  //     // Vector3D sat2Vel = m_constellationSatsNodes.Get(idxSat2)->GetObject<MobilityModel>()->GetVelocity();
  //     // PVCoords satPVecef(satPos, satVel, FrameType::ECEF);

  //     double temp_dist = CalculateDistance(sat1Pos, sat2Pos);

  //     std::pair< uint32_t, double > idxDist = {idxSat2, temp_dist};
  //     idxDistVec.push_back(idxDist);
  //     double delay  = temp_dist / SPEED_OF_LIGHT;

  //     temp_node_container.Add(m_satellitesNodes.Get(idxSat2));
  //     CsmaHelper isl_link_helper;
  //     isl_link_helper.SetDeviceAttribute ("DataRate", StringValue (m_islDataRate));
  //     isl_link_helper.SetChannelAttribute ("Delay", TimeValue(Seconds (delay)));

  //   }

      
  //     NetDeviceContainer temp_netdevice_container;
  //     temp_netdevice_container = interplane_link_helper.Install(temp_node_container);
  //     Ptr<CsmaChannel> csma_channel;
  //     Ptr<Channel> channel;
  //     channel = temp_netdevice_container.Get(0)->GetChannel();
  //     csma_channel = channel->GetObject<CsmaChannel> ();
      

    

  //   // sort  distance pair vectors from min to max, (keeping track of indexes) 
  //   std::sort(idxDistVec.begin(), idxDistVec.end(), [](std::pair <uint32_t, double> a, std::pair <uint32_t, double> b) 
  //                                   {return a.second < b.second;} 
  //                                   );
    
  //   // Iterating over links remaining for satIdx1: and adding links with idxSat2's starting from the one at min distance.
  //   for (uint32_t islId=0; islId <  nidxSat1LinksRemaining; islId++ )
  //   {

  //     int idxSat2 = idxDistVec.at(islId).first
  //     m_islChannelTracker.push_back({idxSat1, idxSat2}); // add satellite link to tracker.
  //     double distance =  idxDistVec.at(islId).second      // in meters
  //     double delay = closestSatDist /  SPEED_OF_LIGHT; // in seconds
    
  //     CsmaHelper isl_link_helper;
  //     isl_link_helper.SetDeviceAttribute ("DataRate", StringValue (m_islDataRate));
  //     isl_link_helper.SetChannelAttribute ("Delay", TimeValue(Seconds (delay)));

  //     // NetDeviceContainer temp_isl_netdevice_container;
  //     // temp_isl_netdevice_container = gsl_link_helper.Install(m_satellitesNodes.Get(idxSat1), m_satellitesNodes.Get(idxSat2)); // 



  //     for (uint32_t k=0; k<num_satellites_per_plane; k++)
  //     {
  //       if (j != k)
  //       {
  //         csma_channel->Detach(temp_netdevice_container.Get(k+1)->GetObject<CsmaNetDevice> ());
  //       }
  //     }

  //     this->inter_plane_devices.push_back(temp_netdevice_container);
  //     this->inter_plane_channels.push_back(csma_channel);
  //     this->inter_plane_channel_tracker.push_back(nodeBIndex);

  //     m_islDevices.push_back( isl_link_helper.Install(m_satellitesNodes.Get(idxSat1), m_satellitesNodes.Get(idxSat2))  );

  //     std::cout <<"Setting up #"<<islId << "ISL link with distance of "<<distance/1000<<" km and delay of "<<delay<<" seconds."<< std::endl;

  //   }


  // }

}

void SatelliteNetworkConfig::BuildGSLsSingleSat()
{

  /* 
  ******* ******* ******* ******* ******* ******* ******* ******* ******* ******* ******* ******* 
  ******* ******* ******* ******* *******  GSL Links ***** ******* ******* ******* ******* ******
  ******* ******* ******* ******* ******* ******* ******* ******* ******* ******* ******* ******* 
  */

  // totalNumGroundstations = m_groundStationsNodes.GetN();


  //Install IP stack
  InternetStackHelper stack;
  stack.Install(m_groundStationsNodes);

  //setting up links between ground stations and their closest satellites
  std::cout <<"Setting links between ground stations and satellites"<< std::endl;
  for (uint32_t idxGs=0; idxGs<m_groundStationsNodes.GetN(); idxGs++)
  {
    // Vector gsPos = m_groundStationsNodes.Get(idxGs)->GetObject<FullGroundStationMobilityModel> ()->GetPosition();
    std::string dataRate = m_groundStationsNodes.Get(idxGs)->GetObject<FullGroundStationMobilityModel> ()->GetDataRate();
    // * ** simultaneous sat links
    // uint32_t numGsl = m_groundStationsNodes.Get(idxGs)->GetObject<FullGroundStationMobilityModel> ()->GetNumGsl();

    Vector3D gsPos = m_groundStationsNodes.Get(idxGs)->GetObject<FullGroundStationMobilityModel> ()->GetPosition();
    Vector3D gsLatlonAlt = m_groundStationsNodes.Get(idxGs)->GetObject<FullGroundStationMobilityModel> ()->GetLatLonAlt();
    std::cout << "GSid: "<< idxGs << " at ECEFxyz = " << gsPos.x <<", "  << gsPos.y << ", "<< gsPos.z <<"\n " <<
             "Lat: "<<gsLatlonAlt.x << " Lon: " << gsLatlonAlt.y << " Alt: "<<gsLatlonAlt.z << std::endl;
    
    // std::vector < std::pair< uint32_t, double > > idxDistVec;

    // uint32_t closestSatId = NULL;
    int closestSatId = -1;
    double closestSatDist = 9e9;

    /*
    ***********************************************************************************************
    ************* #TODO:  ADD simultaneous multi satellite links (based on ISL setup) *************
    ***********************************************************************************************
    */
    //find closest adjacent satellite for ground station
    for (uint32_t idxSat=0; idxSat<m_satellitesNodes.GetN(); idxSat++)
    {
      // <SatellitePositionMobilityModel> ?
      // JulianDate currentJdut = m_satellitesNodes.Get(idxSat)->GetObject<SatellitePositionMobilityModel>()->GetStartTime() + Simulator::Now();

      Vector3D satPos = m_satellitesNodes.Get(idxSat)->GetObject<MobilityModel>()->GetPosition();
      Vector3D satVel = m_satellitesNodes.Get(idxSat)->GetObject<MobilityModel>()->GetVelocity();
      // PVCoords satPVecef(satPos, satVel, FrameType::ECEF);

      
      Topos gsSatTopos = m_groundStationsNodes.Get(idxGs)->GetObject<FullGroundStationMobilityModel> ()->GetVisibilityGroundToSat(satPos, satVel);
      
      double temp_dist = gsSatTopos.range;
      bool visible = gsSatTopos.visible;
      if (visible == true ) std::cout << "satId: " << idxSat << " Visible! from GS: " << idxGs << std::endl;

      if( ( (temp_dist < closestSatDist) ) && visible )
      {
        closestSatDist = temp_dist;
        closestSatId = (int) idxSat;
      }


      // ********* MULTI LINKS *********  
      // std::pair< uint32_t, double > idxDist = {idxSat, temp_dist};
      // idxDistVec.push_back(idxDist); 
      
    }

    // ********* MULTI LINKS *********
    // // sort  distance pair vectors from min to max, (keeping track of indexes) 
    // std::sort(idxDistVec.begin(), idxDistVec.end(), [](std::pair <uint32_t, double> a, std::pair <uint32_t, double> b) 
    //                                 {return a.second < b.second;} 
    //                                 );
    // for (uint32_t gslId=0; gslId < numGsl; gslId++)
    // {

    //   int idxSat = idxDistVec.at(gslId).first
    //   m_groundStationsChannelTracker.push_back(idxSat);// add satellite link to tracker.
    //   double distance =  idxDistVec.at(gslId).second      // in meters
    //   double delay = closestSatDist /  SPEED_OF_LIGHT; // in seconds

    
    // }

 
    double delay = (closestSatId != -1) ? (closestSatDist)/SPEED_OF_LIGHT : 0.0 ;
    
    // GSL Links
    CsmaHelper gsl_link_helper;
    gsl_link_helper.SetChannelAttribute("DataRate", StringValue (dataRate));
    gsl_link_helper.SetChannelAttribute("Delay", TimeValue(Seconds(delay)));

    NodeContainer temp_node_container;
    temp_node_container.Add(m_groundStationsNodes.Get(idxGs));
    // temp_node_container.Add(m_satellitesNodes.Get(idxSat));
    temp_node_container.Add(m_satellitesNodes);
    NetDeviceContainer temp_netdevice_container;
    temp_netdevice_container = gsl_link_helper.Install(temp_node_container); 

    Ptr<CsmaChannel> gs_csma_channel;
    Ptr<Channel> channel;
    channel = temp_netdevice_container.Get(0)->GetChannel();
    gs_csma_channel = channel->GetObject<CsmaChannel> ();
    
    // num_satellites_per_plane = m_planes[idxPlane].size();
    // iterate over all sats and detach all except the one visible and closest.
    for (int k = 0; k < (int) m_satellitesNodes.GetN(); k++)
    // for (uint32_t k = 0; k < m_satellitesNodes.GetN(); k++)
    {
        if (k != closestSatId) // skips if closestSatId is null -> no visible sat for ith GS.
        {
          gs_csma_channel->Detach(temp_netdevice_container.Get(k+1)->GetObject<CsmaNetDevice> ());
        }
      

    }    

    if (closestSatId != -1)
    {
      std::cout <<"Channel open between ground station " << idxGs << 
            " and satellite " << closestSatId << " with distance "<< 
            closestSatDist/1000 << "km and delay of "<< delay <<" seconds"<< std::endl;    
    }
    
    m_groundStationsChannelTracker.push_back(closestSatId); // will add -1 if no channel was attached.
    // (closestSatId.size() > 0) ? m_groundStationsChannelTracker.push_back(closestSatId.at(0))   :   m_groundStationsChannelTracker.push_back(-1) ;
    m_groundStationsDevices.push_back(temp_netdevice_container);
    m_groundStationsChannels.push_back(gs_csma_channel);


  
  }

}

void SatelliteNetworkConfig::BuildGSLsMultiSat() // not fully implemented
{
  
}


void SatelliteNetworkConfig::BuildASLsSingleSat()
{

  /* 
  ******* ******* ******* ******* ******* ******* ******* ******* ******* ******* ******* ******* 
  ******* ******* ******* ******* *******  ASL Links ***** ******* ******* ******* ******* ******
  ******* ******* ******* ******* ******* ******* ******* ******* ******* ******* ******* ******* 
  */

  


  //Install IP stack
  InternetStackHelper stack;
  stack.Install(m_aircraftNodes);

  //setting up links between ground stations and their closest satellites
  std::cout <<"Setting up links between Aircraft and satellites"<< std::endl;
  for (uint32_t idxGs=0; idxGs<m_aircraftNodes.GetN(); idxGs++)
  {
    // Vector gsPos = m_groundStationsNodes.Get(idxGs)->GetObject<FullGroundStationMobilityModel> ()->GetPosition();
    std::string dataRate = m_aircraftNodes.Get(idxGs)->GetObject<AircraftMobilityModel> ()->GetDataRate();
    // * ** simultaneous sat links
    // uint32_t numGsl = m_groundStationsNodes.Get(idxGs)->GetObject<FullGroundStationMobilityModel> ()->GetNumGsl();

    Vector3D gsPos = m_aircraftNodes.Get(idxGs)->GetObject<AircraftMobilityModel> ()->GetPosition();
    Vector3D gsLatlonAlt = m_aircraftNodes.Get(idxGs)->GetObject<AircraftMobilityModel> ()->PropagateLatLonAlt();
    std::cout << "Aircraft id: "<< idxGs << " at ECEFxyz = " << gsPos.x <<", "  << gsPos.y << ", "<< gsPos.z <<"\n " <<
             "Lat: "<<gsLatlonAlt.x << " Lon: " << gsLatlonAlt.y << " Alt: "<<gsLatlonAlt.z << std::endl;
    
    // std::vector < std::pair< uint32_t, double > > idxDistVec;

    // uint32_t closestSatId = NULL;
    int closestSatId = -1;
    double closestSatDist = 9e9;

    /*
    ***********************************************************************************************
    ************* #TODO:  ADD simultaneous multi satellite links (based on ISL setup) *************
    ***********************************************************************************************
    */
    //find closest adjacent satellite for ground station
    for (uint32_t idxSat=0; idxSat<m_satellitesNodes.GetN(); idxSat++)
    {
      // <SatellitePositionMobilityModel> ?
      // JulianDate currentJdut = m_satellitesNodes.Get(idxSat)->GetObject<SatellitePositionMobilityModel>()->GetStartTime() + Simulator::Now();

      Vector3D satPos = m_satellitesNodes.Get(idxSat)->GetObject<MobilityModel>()->GetPosition();
      Vector3D satVel = m_satellitesNodes.Get(idxSat)->GetObject<MobilityModel>()->GetVelocity();
      // PVCoords satPVecef(satPos, satVel, FrameType::ECEF);

      
      Topos gsSatTopos = m_aircraftNodes.Get(idxGs)->GetObject<AircraftMobilityModel> ()->GetVisibilityAirToSat(satPos, satVel);
      
      double temp_dist = gsSatTopos.range;
      bool visible = gsSatTopos.visible;
      if (visible == true ) std::cout << "satId: " << idxSat << " Visible! from Aircraft: " << idxGs << std::endl;

      if( ( (temp_dist < closestSatDist) ) && visible )
      {
        closestSatDist = temp_dist;
        closestSatId = (int) idxSat;
      }


      // ********* MULTI LINKS *********  
      // std::pair< uint32_t, double > idxDist = {idxSat, temp_dist};
      // idxDistVec.push_back(idxDist); 
      
    }

    // ********* MULTI LINKS *********
    // // sort  distance pair vectors from min to max, (keeping track of indexes) 
    // std::sort(idxDistVec.begin(), idxDistVec.end(), [](std::pair <uint32_t, double> a, std::pair <uint32_t, double> b) 
    //                                 {return a.second < b.second;} 
    //                                 );
    // for (uint32_t gslId=0; gslId < numGsl; gslId++)
    // {

    //   int idxSat = idxDistVec.at(gslId).first
    //   m_groundStationsChannelTracker.push_back(idxSat);// add satellite link to tracker.
    //   double distance =  idxDistVec.at(gslId).second      // in meters
    //   double delay = closestSatDist /  SPEED_OF_LIGHT; // in seconds

    
    // }

 
    double delay = (closestSatId != -1) ? (closestSatDist)/SPEED_OF_LIGHT : 0.0 ;
    
    // GSL Links
    CsmaHelper gsl_link_helper;
    gsl_link_helper.SetChannelAttribute("DataRate", StringValue (dataRate));
    gsl_link_helper.SetChannelAttribute("Delay", TimeValue(Seconds(delay)));

    NodeContainer temp_node_container;
    temp_node_container.Add(m_aircraftNodes.Get(idxGs));
    // temp_node_container.Add(m_satellitesNodes.Get(idxSat));
    temp_node_container.Add(m_satellitesNodes);
    NetDeviceContainer temp_netdevice_container;
    temp_netdevice_container = gsl_link_helper.Install(temp_node_container); 

    Ptr<CsmaChannel> gs_csma_channel;
    Ptr<Channel> channel;
    channel = temp_netdevice_container.Get(0)->GetChannel();
    gs_csma_channel = channel->GetObject<CsmaChannel> ();
    
    // num_satellites_per_plane = m_planes[idxPlane].size();
    // iterate over all sats and detach all except the one visible and closest.
    for (int k = 0; k < (int) m_satellitesNodes.GetN(); k++)
    // for (uint32_t k = 0; k < m_satellitesNodes.GetN(); k++)
    {
        if (k != closestSatId) // skips if closestSatId is null -> no visible sat for ith GS.
        {
          gs_csma_channel->Detach(temp_netdevice_container.Get(k+1)->GetObject<CsmaNetDevice> ());
        }
      

    }    

    if (closestSatId != -1)
    {
      std::cout <<"Channel open between aircraft " << idxGs << 
            " and satellite " << closestSatId << " with distance "<< 
            closestSatDist/1000 << "km and delay of "<< delay <<" seconds"<< std::endl;    
    }
    
    m_aircraftChannelTracker.push_back(closestSatId); // will add -1 if no channel was attached.
    // (closestSatId.size() > 0) ? m_groundStationsChannelTracker.push_back(closestSatId.at(0))   :   m_groundStationsChannelTracker.push_back(-1) ;
    m_aircraftDevices.push_back(temp_netdevice_container);
    m_aircraftChannels.push_back(gs_csma_channel);


  
  }

}



void SatelliteNetworkConfig::SetupIPConfig()
{

  /*
  ******************************************************************************************
  *************************** IP CONFIG ****************************************************
  ******************************************************************************************
  */



  //Configure IP Addresses for all NetDevices
  Ipv4AddressHelper address;
  address.SetBase ("10.0.0.0", "255.255.240.0"); //255.255.255.0 -> 200+ nodes 


   //configuring IP Addresses for ISL devices
  for(uint32_t i=0; i< m_islDevices.size(); i++)
  {
    address.NewNetwork();
    m_islInterfaces.push_back(address.Assign(m_islDevices[i]));
  }

  //configuring IP Addresses for Ground devices
  for(int i=0; i < (int) m_groundStationsDevices.size(); i++)
  {
    address.NewNetwork();
    m_groundStationsInterfaces.push_back(address.Assign(m_groundStationsDevices[i]));
    // for(uint32_t j=1; j<= m_groundStationsDevices[i].GetN(); j++)
    for(int j=1; j<= (int) m_satellitesNodes.GetN(); j++)
    {

      if (m_groundStationsChannelTracker[i] == -1)
      {
          std::pair< Ptr< Ipv4 >, uint32_t > interface = m_groundStationsInterfaces[i].Get(j);
          interface.first->SetDown(interface.second); // 
      }
      else
      {
        if(j != (m_groundStationsChannelTracker[i] + 1))
        {
          std::pair< Ptr< Ipv4 >, uint32_t > interface = m_groundStationsInterfaces[i].Get(j);
          interface.first->SetDown(interface.second); // 
        }
      
      }
    }
  }

  if (m_AIRfilepath.empty() == false )
  {
    //configuring IP Addresses for Aicraft devices
    for(int i=0; i < (int) m_aircraftDevices.size(); i++)
    {
      address.NewNetwork();
      m_aircraftInterfaces.push_back(address.Assign(m_aircraftDevices[i]));
      // for(uint32_t j=1; j<= m_groundStationsDevices[i].GetN(); j++)
      for(int j=1; j<= (int) m_satellitesNodes.GetN(); j++)
      {

        if (m_aircraftChannelTracker[i] == -1)
        {
            std::pair< Ptr< Ipv4 >, uint32_t > interface = m_aircraftInterfaces[i].Get(j);
            interface.first->SetDown(interface.second); // 
        }
        else
        {
          if(j != (m_aircraftChannelTracker[i] + 1))
          {
            std::pair< Ptr< Ipv4 >, uint32_t > interface = m_aircraftInterfaces[i].Get(j);
            interface.first->SetDown(interface.second); // 
          }
        
        }
      }
    }


  }



  //Populate Routing Tables
  std::cout <<"Populating Routing Tables"<< std::endl;
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
  std::cout <<"Finished Populating Routing Tables"<< std::endl;

  // Set up packet sniffing for entire network
  /*CsmaHelper csma;
  for (uint32_t i=0; i< this->inter_plane_devices.size(); i++)
  {
    csma.EnablePcap("inter-sniff", this->inter_plane_devices[i].Get(1), true);
  }
  PointToPointHelper p2p;
  for(uint32_t i=0; i< this->intra_plane_devices.size(); i++)
  {
    p2p.EnablePcap("intra-sniff", this->intra_plane_devices[i].Get(1), true);
  }
  for(uint32_t i=0; i< this->ground_station_devices.size(); i++)
  {
    p2p.EnablePcap("ground-sniff", this->ground_station_devices[i].Get(1), true);
  }*/
}



// Build all
void
SatelliteNetworkConfig::BuildNetwork ()

{
  ReadSatConfigFile(m_TLEfilepath);
  ReadGSConfigFile(m_GSfilepath);

  BuildISLsP2PFixed();
  BuildGSLsSingleSat();


  if (m_AIRfilepath.empty() == false )
  {
    ReadAirConfigFile(m_AIRfilepath);
    BuildASLsSingleSat();
  }
  
  
  
  // BuildISLsDynamic();
  // BuildGSLsMultiSat();

  SetupIPConfig();

 }


void SatelliteNetworkConfig::UpdateISLsP2PFixed()
{
// return -1;
}


void SatelliteNetworkConfig::UpdateLinks()
{
  std::cout <<std::endl<<std::endl<<std::endl<<"Updating Links ... "<< std::endl;


  /*
  ******************************************************************************************
  *************************** ISL UPDATES **************************************************
  ******************************************************************************************
  */

  // for (uint32_t idxSat1=0; idxSat1 < m_satellitesNodes.GetN(); idxSat1++)
  // {
  //   // if (std::find( m_islChannelTracker.begin(),  m_islChannelTracker.end(), idxSat1) !=  m_islChannelTracker.end()) //check if idxSat1 is already linked
  //   //   {
  //   //   continue; 
  //   //   } 
  //   if((std::count_if( m_islChannelTracker.begin(),  m_islChannelTracker.end(),
  //                   [](std::vector<int> const& v) { return std::find(v.begin(), v.end(), idxSat1) 
  //                                                                     != v.end();}) )
  //        ==  m_nISLsPerSat)
  //   {
  //      m_islChannelTracker.push_back( std::vector <int> {-1})
  //     continue;
  //   } 

  //   Vector sat1Pos = m_satellitesNodes.Get(idxSat1)->GetObject<MobilityModel>()->GetPosition();
  //   // PVCoords satPVecef(satPos, satVel, FrameType::ECEF);
  //   // uint32_t closestSatId = 0;
  //   // double closestSatDist = 0;
  //   std::vector < std::pair< uint32_t, double > > idxDistVec;

  //   //find closest adjacent satellite for ground station
  //   for (uint32_t idxSat2=1; idxSat2 < m_satellitesNodes.GetN(); idxSat2++)
  //   {
      

  //     // if (std::find( m_islChannelTracker.begin(),  m_islChannelTracker.end(), idxSat1) !=  m_islChannelTracker.end()) //check if idxSat1 is already linked
  //     // {

  //     // continue; 
  //     // } 

  //     // check if already all ISLs for this satellite are implemented. and if so, skip.
  //     if((std::count_if( m_islChannelTracker.begin(),  m_islChannelTracker.end(),
  //                   [](std::vector<uint32_t> const& v) { return std::find(v.begin(), v.end(), idxSat2) 
  //                                                                     != v.end();}) )
  //        ==  m_nISLsPerSat)
  //     {
        
  //       continue;
  //     } 


      
  //     Vector3D sat2Pos = m_satellitesNodes.Get(idxSat2)->GetObject<MobilityModel>()->GetPosition();
  //     // Vector3D sat2Vel = m_constellationSatsNodes.Get(idxSat2)->GetObject<MobilityModel>()->GetVelocity();
  //     // PVCoords satPVecef(satPos, satVel, FrameType::ECEF);

  //     double temp_dist = CalculateDistance(sat1Pos, sat2Pos);

  //     std::pair< uint32_t, double > idxDist = {idxSat, temp_dist};
  //     idxDistVec.push_back(idxDist);

  //     // if(  (temp_dist < closestSatDist) || (idxSat==0) )
  //     // {
  //     //   // closestSatDist = temp_dist;
  //     //   // closestSatId = idxSat;
  //     // }
  //   }

  //   // sort from min to max, distance pair vectors (keeping track of indexes) 
  //   std::sort(idxDistVec.begin(), idxDistVec.end(), [](std::pair <uint32_t, double> a, std::pair <uint32_t, double> b) 
  //                                   {return a.second < b.second;} 
  //                                   );
    
    

  //    m_islChannelTracker.push_back(std::vector<int>{}); // add empty vector for idxSat1's isl satlinks.
  //   for (uint32_t islId=0; islId <  m_nISLsPerSat; islId++ )
  //   {

  //     int idxSat2 = idxDistVec.at(islId).first
  //      m_islChannelTracker.at(idxSat1).push_back(idxSat2); // add each satellite link  to first loop sat1
  //     double distance =  idxDistVec.at(islId).second      
  //     double delay = closestSatDist * 1000 /  SPEED_OF_LIGHT; // in seconds
    
  //     PointToPointHelper isl_link_helper;
  //     isl_link_helper.SetDeviceAttribute ("DataRate", StringValue (m_islDataRate));
  //     isl_link_helper.SetChannelAttribute ("Delay", TimeValue(Seconds (delay)));

  //     // NetDeviceContainer temp_isl_netdevice_container;
  //     // temp_isl_netdevice_container = gsl_link_helper.Install(m_satellitesNodes.Get(idxSat1), m_satellitesNodes.Get(idxSat2)); // 

  //     m_islDevices.push_back( isl_link_helper.Install(m_satellitesNodes.Get(idxSat1), m_satellitesNodes.Get(idxSat2))  );

  //     std::cout <<"Setting up #"<<islId << "ISL link with distance of "<<distance/1000<<" km and delay of "<<delay<<" seconds."<< std::endl;

  //   }

  // }





/// ********


      // if(currAdjNodeID == nextAdjNodeID)
      // {
      //   double new_delay = (nextAdjNodeDist*1000)/SPEED_OF_LIGHT;
      //   this->inter_plane_channels[access_idx]->SetAttribute("Delay", TimeValue(Seconds(new_delay)));
      //   std::cout <<"Channel updated between plane "<<i<<" satellite "<<j<<" and plane "<<(i+1)%num_planes<<" satellite "<<nextAdjNodeID<< " with distance "<<nextAdjNodeDist<< "km and delay of "<<new_delay<<" seconds"<< std::endl;
      // }
      // else
      // {
      //   this->inter_plane_channels[access_idx]->Detach(this->inter_plane_devices[access_idx].Get(currAdjNodeID+1)->GetObject<CsmaNetDevice> ());
      //   std::pair< Ptr< Ipv4 >, uint32_t> interface = this->inter_plane_interfaces[access_idx].Get(currAdjNodeID+1);
      //   interface.first->SetDown(interface.second);
      //   this->inter_plane_channels[access_idx]->Reattach(this->inter_plane_devices[access_idx].Get(nextAdjNodeID+1)->GetObject<CsmaNetDevice> ());
      //   interface = this->inter_plane_interfaces[access_idx].Get(nextAdjNodeID+1);
      //   interface.first->SetUp(interface.second);
      //   this->inter_plane_channel_tracker[access_idx] = nextAdjNodeID;
      //   double new_delay = (nextAdjNodeDist*1000)/SPEED_OF_LIGHT;
      //   this->inter_plane_channels[access_idx]->SetAttribute("Delay", TimeValue(Seconds(new_delay)));
      //   std::cout <<"New channel between plane "<<i<<" satellite "<<j<<" and plane "<<(i+1)%num_planes<<" satellite "<<nextAdjNodeID<< " with distance "<<nextAdjNodeDist<< "km and delay of "<<new_delay<<" seconds"<< std::endl;
      // }
    
  


/*
******************************************************************************************
*************************** GSL UPDATES **************************************************
******************************************************************************************
*/

  std::cout <<"Updating links between ground stations and satellites"<< std::endl;

  //setting up links between ground stations and their closest satellites
  
  for (uint32_t idxGs=0; idxGs<m_groundStationsNodes.GetN(); idxGs++)
  {
    // Vector gsPos = m_groundStationsNodes.Get(idxGs)->GetObject<FullGroundStationMobilityModel> ()->GetPosition();
    std::string dataRate = m_groundStationsNodes.Get(idxGs)->GetObject<FullGroundStationMobilityModel> ()->GetDataRate();

    Vector3D gsPos = m_groundStationsNodes.Get(idxGs)->GetObject<FullGroundStationMobilityModel> ()->GetPosition();
    Vector3D gsLatlonAlt = m_groundStationsNodes.Get(idxGs)->GetObject<FullGroundStationMobilityModel> ()->GetLatLonAlt();
    std::cout << "GSid: "<< idxGs << " at ECEFxyz = " << gsPos.x <<", "  << gsPos.y << ", "<< gsPos.z <<"\n " <<
             "Lat: "<<gsLatlonAlt.x << " Lon: " << gsLatlonAlt.y << " Alt: "<<gsLatlonAlt.z << std::endl;
  

    int closestSatId = -1;
    double closestSatDist = 9e9;

/*
***********************************************************************************************
************* #TODO:  ADD simultaneous multi satellite links (based on ISL setup) *************
***********************************************************************************************
*/
    //find closest adjacent satellite for ground station
    for (uint32_t idxSat=0; idxSat<m_satellitesNodes.GetN(); idxSat++)
    {
      // <SatellitePositionMobilityModel> ?
      // JulianDate currentJdut = m_satellitesNodes.Get(idxSat)->GetObject<SatellitePositionMobilityModel>()->GetStartTime() + Simulator::Now();

      Vector3D satPos = m_satellitesNodes.Get(idxSat)->GetObject<MobilityModel>()->GetPosition(); // in ecef [m]
      Vector3D satVel = m_satellitesNodes.Get(idxSat)->GetObject<MobilityModel>()->GetVelocity(); // in ecef [m/s]
      // PVCoords satPVecef(satPos, satVel, FrameType::ECEF);

      
      Topos gsSatTopos = m_groundStationsNodes.Get(idxGs)->GetObject<FullGroundStationMobilityModel> ()->GetVisibilityGroundToSat(satPos, satVel);
      
      double temp_dist = gsSatTopos.range;
      bool visible = gsSatTopos.visible;

      if( ( (temp_dist < closestSatDist) ) && visible )
      {
        closestSatDist = temp_dist;
        closestSatId = (int) idxSat;
      }
    }

    double newDelay = (closestSatId != -1) ? (closestSatDist)/SPEED_OF_LIGHT : 0.0 ;
    
    
    int prevClosestSatId = (int) m_groundStationsChannelTracker[idxGs];
    
    if(prevClosestSatId == closestSatId) // no change of closest // valid also for No visible sat
    {

      m_groundStationsChannels[idxGs]->SetAttribute("Delay", TimeValue(Seconds(newDelay)));
      std::cout<< "Channel updated between ground station "<<idxGs<<" and satellite "<<closestSatId<< " with distance "<<closestSatDist/1000<< "km and delay of "<<newDelay<<" seconds"<<std::endl;
    }
      else
      {
        // Detach previous closest satellite node.
        //prevClosestSatId (( + 1 ))!! due to the first device on m_groundStationsDevices is the ground station itself,
        m_groundStationsChannels[idxGs]->Detach( m_groundStationsDevices[idxGs].Get(prevClosestSatId+1)->GetObject<CsmaNetDevice> ());
        std::pair< Ptr< Ipv4 >, uint32_t> interface =  m_groundStationsInterfaces[idxGs].Get(prevClosestSatId+1);
        interface.first->SetDown(interface.second);

        // Attach new closest sat node.
        m_groundStationsChannels[idxGs]->Reattach( m_groundStationsDevices[idxGs].Get( closestSatId+1)->GetObject<CsmaNetDevice> ());
        interface =  m_groundStationsInterfaces[idxGs].Get( closestSatId+1);
        interface.first->SetUp(interface.second);
        m_groundStationsChannelTracker[idxGs] =  closestSatId;
        
        m_groundStationsChannels[idxGs]->SetAttribute("Delay", TimeValue(Seconds(newDelay)));

        // std::ostringstream oss;
        // oss << 
        // std::cout <<oss);
        std::cout << "New channel between ground station "<< idxGs <<" and satellite "<< closestSatId<< " with distance "<<closestSatDist/1000<< "km and delay of "<<newDelay<<" seconds"<<std::endl;
         
        
      }
  }

  // ASL LINK UPDATES
  // ----------------
  if (m_AIRfilepath.empty() == false )
  {

/*
******************************************************************************************
*************************** ASL UPDATES **************************************************
******************************************************************************************
*/

  std::cout <<"Updating links between aircraft and satellites"<< std::endl;

  //setting up links between ground stations and their closest satellites
  
  for (uint32_t idxGs=0; idxGs<m_aircraftNodes.GetN(); idxGs++)
  {
    // Vector gsPos = m_groundStationsNodes.Get(idxGs)->GetObject<FullGroundStationMobilityModel> ()->GetPosition();
    std::string dataRate = m_aircraftNodes.Get(idxGs)->GetObject<AircraftMobilityModel> ()->GetDataRate();

    Vector3D gsPos = m_aircraftNodes.Get(idxGs)->GetObject<AircraftMobilityModel> ()->GetPosition();
    Vector3D gsLatlonAlt = m_aircraftNodes.Get(idxGs)->GetObject<AircraftMobilityModel> ()->PropagateLatLonAlt();
    std::cout << "Aircraft id: "<< idxGs << " at ECEFxyz = " << gsPos.x <<", "  << gsPos.y << ", "<< gsPos.z <<"\n " <<
             "Lat: "<<gsLatlonAlt.x << " Lon: " << gsLatlonAlt.y << " Alt: "<<gsLatlonAlt.z << std::endl;
  

    int closestSatId = -1;
    double closestSatDist = 9e9;

/*
***********************************************************************************************
************* #TODO:  ADD simultaneous multi satellite links (based on ISL setup) *************
***********************************************************************************************
*/
    //find closest adjacent satellite for ground station
    for (uint32_t idxSat=0; idxSat<m_satellitesNodes.GetN(); idxSat++)
    {
      // <SatellitePositionMobilityModel> ?
      // JulianDate currentJdut = m_satellitesNodes.Get(idxSat)->GetObject<SatellitePositionMobilityModel>()->GetStartTime() + Simulator::Now();

      Vector3D satPos = m_satellitesNodes.Get(idxSat)->GetObject<MobilityModel>()->GetPosition(); // in ecef [m]
      Vector3D satVel = m_satellitesNodes.Get(idxSat)->GetObject<MobilityModel>()->GetVelocity(); // in ecef [m/s]
      // PVCoords satPVecef(satPos, satVel, FrameType::ECEF);

      
      Topos gsSatTopos = m_aircraftNodes.Get(idxGs)->GetObject<AircraftMobilityModel> ()->GetVisibilityAirToSat(satPos, satVel);
      
      double temp_dist = gsSatTopos.range;
      bool visible = gsSatTopos.visible;

      if( ( (temp_dist < closestSatDist) ) && visible )
      {
        closestSatDist = temp_dist;
        closestSatId = (int) idxSat;
      }
    }

    double newDelay = (closestSatId != -1) ? (closestSatDist)/SPEED_OF_LIGHT : 0.0 ;
    
    
    int prevClosestSatId = (int) m_aircraftChannelTracker[idxGs];
    
    if(prevClosestSatId == closestSatId) // no change of closest // valid also for No visible sat
    {

      m_aircraftChannels[idxGs]->SetAttribute("Delay", TimeValue(Seconds(newDelay)));
      std::cout<< "Channel updated between ground station "<<idxGs<<" and satellite "<<closestSatId<< " with distance "<<closestSatDist/1000<< "km and delay of "<<newDelay<<" seconds"<<std::endl;
    }
      else
      {
        // Detach previous closest satellite node.
        //prevClosestSatId (( + 1 ))!! due to the first device on m_groundStationsDevices is the ground station itself,
        m_aircraftChannels[idxGs]->Detach( m_aircraftDevices[idxGs].Get(prevClosestSatId+1)->GetObject<CsmaNetDevice> ());
        std::pair< Ptr< Ipv4 >, uint32_t> interface =  m_aircraftInterfaces[idxGs].Get(prevClosestSatId+1);
        interface.first->SetDown(interface.second);

        // Attach new closest sat node.
        m_aircraftChannels[idxGs]->Reattach( m_aircraftDevices[idxGs].Get( closestSatId+1)->GetObject<CsmaNetDevice> ());
        interface =  m_aircraftInterfaces[idxGs].Get( closestSatId+1);
        interface.first->SetUp(interface.second);
        m_aircraftChannelTracker[idxGs] =  closestSatId;
        
        m_aircraftChannels[idxGs]->SetAttribute("Delay", TimeValue(Seconds(newDelay)));

        // std::ostringstream oss;
        // oss << 
        // std::cout <<oss);
        std::cout << "New channel between aircraft "<< idxGs <<" and satellite "<< closestSatId<< " with distance "<<closestSatDist/1000<< "km and delay of "<<newDelay<<" seconds"<<std::endl;
         
        
      }
  }


  }









  //Recompute Routing Tables
  std::cout<<"Recomputing Routing Tables"<<std::endl;
  Ipv4GlobalRoutingHelper::RecomputeRoutingTables ();
  std::cout<<"Finished Recomputing Routing Tables"<<std::endl;

} // end UPDATELINKS()



double 
SatelliteNetworkConfig::CalculateDistance(Vector &pos1, Vector &pos2)
{
  return (pos1 - pos2).GetLength();
}
// double 
// SatelliteNetworkConfig::CalculateDistance(PVCoords &pv1, PVCoords &pv2)
// {
//   return (pv1.GetPos() - pv2.GetPos()).GetLength();
// }



} // end ns3 namespace