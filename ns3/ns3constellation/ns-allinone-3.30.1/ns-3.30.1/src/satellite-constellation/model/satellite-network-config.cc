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
   fstream satFile;
   satFile.open(satConfigFilepath, ios::in); //open a file to perform read operation using file object
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
      std::string utcSimulationStart = getline(satFile); //first line utc timestamp of simulation start. (multi TLE coherence)
      JulianDate  simulationStartTime(utcSimulationStart, TimeSystem::UTC); // satellite module julian date start
      std::cout <<"Starting Simulation at "<< utcSimulationStart <<std::endl;


      std::cout <<"Reading Satellite Data ...." << std::endl;
      string satname;
      string line1;
      string line2;
      string tmp;
      
      int lineNum = 0;
      int idxSat = 0;
      while(getline(satFile, tmp))
      { //read data from file object and put it into string.
        if (lineNum % 3 == 0)
        {
          if (lineNum > 0) // construct + append satellite object every 3 lines read.
          {
            Satellite satellite;
            satellite.SetName(satname);
            satellite.SetTleInfo(line1, line2);
            m_constellationSats.push_back(satellite); // add satellites to member variable: vector of satellites
            m_satellitesNodes.Create(1) // add new satellite node 
            MobilityHelper satMobility;
            
            satMobility.SetMobilityModel(
                        "ns3::SatellitePositionMobilityModel",
                        "SatellitePositionHelper",
                        SatellitePositionHelperValue(SatellitePositionHelper(satellite))
                );
            satMobility.SetStartTime(simulationStartTime);
            satMobility.Install(m_satellitesNodes.Get(idxSat)); // install mobility in new node created

            idxSat++;

          }

          satname = tmp; // read satellite name line
          
        }else if ( (lineNum-1) % 3 == 0)
        {
          line1 = tmp;  // read TLE - line 1
        }else if ( (lineNum-2) % 3 == 0)
        {
          line2 = tmp;   // read TLE - line 2
        }

        lineNum++;
      }
      satFile.close(); //close the file object.

   
    }
    else
    {
      std::runtime_error( "Error Reading  Config File");
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
   fstream gsFile;
   gsFile.open(satConfigFilepath, ios::in); //open a file to perform read operation using file object
   if (gsFile.is_open()) //checking whether the file is open
   {   
      string GSname;
      string tmp;
      
      uint32_t idxGs = 0;
      while(getline(gsFile, tmp))
      { //read data from file object and put it into string.

        std::vector <std::string> gsStrLine = splitString(tmp, ",");
        string gsName = gsStrLine[0];
        double gsLat = std::stof(gsStrLine[1]);
        double gsLon = std::stof(gsStrLine[2]);
        double gsAlt = std::stof(gsStrLine[3]);


        m_groundStationsNodes.Create(1);

        MobilityHelper mobility;
        mobility.SetMobilityModel(
                        "ns3::FullGroundStationMobilityModel",
                        "Latitude", DoubleValue(gsLat),
                        "Longitude", DoubleValue(gsLon),
                        "Altitude", DoubleValue(gsAlt),
                        "Name", StringValue(gsName),

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

// Constructors GS - SAT network
SatelliteNetworkConfig::SatelliteNetworkConfig (std::string &TLEfilepath, std::string &GSfilepath)
: m_TLEfilepath(TLEfilepath), m_GSfilepath(GSfilepath),  m_nISLsPerSat(1), m_islDataRate("300Mbps"), m_airFilepath("")
{
  BuildNetwork(TLEfilepath, GSfilepath);
}

SatelliteNetworkConfig::SatelliteNetworkConfig (std::string TLEfilepath, std::string GSfilepath, uint32_t islPerSat)
: m_TLEfilepath(TLEfilepath), m_GSfilepath(GSfilepath),  m_nISLsPerSat(islPerSat), m_islDataRate("300Mbps"), m_airFilepath("")
{
  BuildNetwork(TLEfilepath, GSfilepath);
}

SatelliteNetworkConfig::SatelliteNetworkConfig (std::string TLEfilepath, std::string GSfilepath, uint32_t islPerSat, std::string islDataRate)
: m_TLEfilepath(TLEfilepath), m_GSfilepath(GSfilepath),  m_nISLsPerSat(islPerSat), m_islDataRate(islDataRate), m_airFilepath("")
{
  BuildNetwork(TLEfilepath, GSfilepath);
}

// Constructors GS - AIR - SAT network
SatelliteNetworkConfig::SatelliteNetworkConfig (std::string TLEfilepath, std::string GSfilepath, std::string airFilepath, uint32_t islPerSat, std::string islDataRate)
: m_TLEfilepath(TLEfilepath), m_GSfilepath(GSfilepath),  m_nISLsPerSat(islPerSat), m_islDataRate(islDataRate), m_airFilepath(airFilepath)
{
  BuildNetwork(TLEfilepath, GSfilepath);
}







void SatelliteNetworkConfig::BuildISLsP2PFixed()
{

  /* 
  ******* ******* ******* ******* ******* ******* ******* ******* ******* 
  ******* ******* ******* ******* ISL Links ***** ******* ******* ******* 
  ******* ******* ******* ******* ******* ******* ******* ******* ******* 
  */

  // std::vector < std::vector<int> >  m_islChannelTracker; // keep track of already linked satellites to skip iterations.


  for (uint32_t idxSat1=0; idxSat1 < m_satellitesNodes.GetN(); idxSat1++)
  {
    
    // count all links using idxSat1, skip iteration if equal to ISLs per sat.
    int nidxSat1LinksRemaining = m_nISLsPerSat - std::count_if( m_islChannelTracker.begin(),  m_islChannelTracker.end(),
                                                                      [&idxSat1](std::pair<uint32_t, uint32_t> const& v) 
                                                                      { return v.first == idxSat1 || v.second == idxSat1;}
                                                                  ); 

    if( nidxSat1LinksRemaining <= 0 )
    {
      std::cout <<"Skipping satIdx1 = #" << idxSat1 << ". All links full.");
      
      continue;
    } 

    Vector sat1Pos = m_satellitesNodes.Get(idxSat1)->GetObject<MobilityModel>()->GetPosition();
    // PVCoords satPVecef(satPos, satVel, FrameType::ECEF);
    // uint32_t closestSatId = 0;
    // double closestSatDist = 0;
    std::vector < std::pair< uint32_t, double > > idxDistVec;

    //find closest adjacent satellite for ground station
    for (uint32_t idxSat2=0; idxSat2 < m_satellitesNodes.GetN(); idxSat2++)
    {

      // check if (idx2Sat already has  max ISLs defined) 
      if( (std::count_if( m_islChannelTracker.begin(),  m_islChannelTracker.end(),
                              [&idxSat2](std::pair<int, int> const& v) 
                              { return v.first == idxSat2 || v.second == idxSat2;}
                        ) >= m_nISLsPerSat
          )   
                    || //or ( idx1Sat--idx2Sat ISL is already defined)
          (std::count_if( m_islChannelTracker.begin(),  m_islChannelTracker.end(),
                                        [idxSat1, idxSat2](std::pair<int, int>  const& v) 
                                        { return (v.first == idxSat1  && v.second == idxSat2) ||
                                            (v.first == idxSat2  && v.second == idxSat1);
                                        }
                        ) > 0
          )                   
        )
      {
        std::cout <<"Skipping ISL Link between satIdx1 = " << idxSat1 << " and  satIdx2 = " << idx2Sat);
        continue;
      } 


      
      Vector3D sat2Pos = m_satellitesNodes.Get(idxSat2)->GetObject<MobilityModel>()->GetPosition();
      // Vector3D sat2Vel = m_constellationSatsNodes.Get(idxSat2)->GetObject<MobilityModel>()->GetVelocity();
      // PVCoords satPVecef(satPos, satVel, FrameType::ECEF);

      double temp_dist = CalculateDistance(sat1Pos, sat2Pos);

      std::pair< uint32_t, double > idxDist = {idxSat2, temp_dist};
      idxDistVec.push_back(idxDist);

    }

    // sort  distance pair vectors from min to max, (keeping track of indexes) 
    std::sort(idxDistVec.begin(), idxDistVec.end(), [](std::pair <uint32_t, double> a, std::pair <uint32_t, double> b) 
                                    {return a.second < b.second;} 
                                    );
    
    // Iterating over links remaining for satIdx1: and adding links with idxSat2's starting from the one at min distance.
    for (uint32_t islId=0; islId <  nidxSat1LinksRemaining; islId++ )
    {

      int idxSat2 = idxDistVec.at(islId).first
      m_islChannelTracker.push_back({idxSat1, idxSat2}); // add satellite link to tracker.
      double distance =  idxDistVec.at(islId).second      // in meters
      double delay = closestSatDist /  SPEED_OF_LIGHT; // in seconds
    
      PointToPointHelper isl_link_helper;
      isl_link_helper.SetDeviceAttribute ("DataRate", StringValue (m_islDataRate));
      isl_link_helper.SetChannelAttribute ("Delay", TimeValue(Seconds (delay)));

      // NetDeviceContainer temp_isl_netdevice_container;
      // temp_isl_netdevice_container = gsl_link_helper.Install(m_satellitesNodes.Get(idxSat1), m_satellitesNodes.Get(idxSat2)); // 

      m_islDevices.push_back( isl_link_helper.Install(m_satellitesNodes.Get(idxSat1), m_satellitesNodes.Get(idxSat2))  );

      std::cout <<"Setting up #"<<islId << "ISL link with distance of "<<distance/1000<<" km and delay of "<<delay<<" seconds."<< std::endl;

    }

  } //end for idxSat1
}  

void SatelliteNetworkConfig::BuildISLsDynamic() // non working 
{

  /* 
  ******* ******* ******* ******* ******* ******* ******* ******* ******* 
  ******* ******* ******* ******* ISL Links ***** ******* ******* ******* 
  ******* ******* ******* ******* ******* ******* ******* ******* ******* 
  */

  // std::vector < std::vector<int> >  m_islChannelTracker; // keep track of already linked satellites to skip iterations.


  for (uint32_t idxSat1=0; idxSat1 < m_satellitesNodes.GetN(); idxSat1++)
  {
    
    // count all links using idxSat1, skip iteration if equal to ISLs per sat.
    int nidxSat1LinksRemaining = m_nISLsPerSat - std::count_if( m_islChannelTracker.begin(),  m_islChannelTracker.end(),
                                                                      [&idxSat1](std::pair<uint32_t, uint32_t> const& v) 
                                                                      { return v.first == idxSat1 || v.second == idxSat1;}
                                                                  ); 

    if( nidxSat1LinksRemaining <= 0 )
    {
      std::cout << "Skipping satIdx1 = #" << idxSat1 << ". All links full." << std::endl;
      
      continue;
    } 

    Vector sat1Pos = m_satellitesNodes.Get(idxSat1)->GetObject<MobilityModel>()->GetPosition();
    // PVCoords satPVecef(satPos, satVel, FrameType::ECEF);
    // uint32_t closestSatId = 0;
    // double closestSatDist = 0;
    std::vector < std::pair< uint32_t, double > > idxDistVec;
    NodeContainer temp_node_container;
    temp_node_container.Add(m_satellitesNodes.Get(idxSat1));


    //find closest adjacent satellite for ground station
    for (uint32_t idxSat2=0; idxSat2 < m_satellitesNodes.GetN(); idxSat2++)
    {

      // check if (idx2Sat already has  max ISLs defined) 
      if( (std::count_if( m_islChannelTracker.begin(),  m_islChannelTracker.end(),
                              [&idxSat2](std::pair<int, int> const& v) 
                              { return v.first == idxSat2 || v.second == idxSat2;}
                        ) >= m_nISLsPerSat
          )   
                    || //or ( idx1Sat--idx2Sat ISL is already defined)
          (std::count_if( m_islChannelTracker.begin(),  m_islChannelTracker.end(),
                                        [idxSat1, idxSat2](std::pair<int, int>  const& v) 
                                        { return (v.first == idxSat1  && v.second == idxSat2) ||
                                            (v.first == idxSat2  && v.second == idxSat1);
                                        }
                        ) > 0
          )                   
        )
      {
        std::cout <<"Skipping ISL Link between satIdx1 = " << idxSat1 << " and  satIdx2 = " << idx2Sat << std::endl;
        continue;
      } 


      
      Vector3D sat2Pos = m_satellitesNodes.Get(idxSat2)->GetObject<MobilityModel>()->GetPosition();
      // Vector3D sat2Vel = m_constellationSatsNodes.Get(idxSat2)->GetObject<MobilityModel>()->GetVelocity();
      // PVCoords satPVecef(satPos, satVel, FrameType::ECEF);

      double temp_dist = CalculateDistance(sat1Pos, sat2Pos);

      std::pair< uint32_t, double > idxDist = {idxSat2, temp_dist};
      idxDistVec.push_back(idxDist);


      temp_node_container.Add(m_satellitesNodes.Get(idxSat2));
      CsmaHelper isl_link_helper;
      isl_link_helper.SetDeviceAttribute ("DataRate", StringValue (m_islDataRate));
      isl_link_helper.SetChannelAttribute ("Delay", TimeValue(Seconds (delay)));

    }

      
      NetDeviceContainer temp_netdevice_container;
      temp_netdevice_container = interplane_link_helper.Install(temp_node_container);
      Ptr<CsmaChannel> csma_channel;
      Ptr<Channel> channel;
      channel = temp_netdevice_container.Get(0)->GetChannel();
      csma_channel = channel->GetObject<CsmaChannel> ();
      

    

    // sort  distance pair vectors from min to max, (keeping track of indexes) 
    std::sort(idxDistVec.begin(), idxDistVec.end(), [](std::pair <uint32_t, double> a, std::pair <uint32_t, double> b) 
                                    {return a.second < b.second;} 
                                    );
    
    // Iterating over links remaining for satIdx1: and adding links with idxSat2's starting from the one at min distance.
    for (uint32_t islId=0; islId <  nidxSat1LinksRemaining; islId++ )
    {

      int idxSat2 = idxDistVec.at(islId).first
      m_islChannelTracker.push_back({idxSat1, idxSat2}); // add satellite link to tracker.
      double distance =  idxDistVec.at(islId).second      // in meters
      double delay = closestSatDist /  SPEED_OF_LIGHT; // in seconds
    
      CsmaHelper isl_link_helper;
      isl_link_helper.SetDeviceAttribute ("DataRate", StringValue (m_islDataRate));
      isl_link_helper.SetChannelAttribute ("Delay", TimeValue(Seconds (delay)));

      // NetDeviceContainer temp_isl_netdevice_container;
      // temp_isl_netdevice_container = gsl_link_helper.Install(m_satellitesNodes.Get(idxSat1), m_satellitesNodes.Get(idxSat2)); // 



      for (uint32_t k=0; k<num_satellites_per_plane; k++)
      {
        if (j != k)
        {
          csma_channel->Detach(temp_netdevice_container.Get(k+1)->GetObject<CsmaNetDevice> ());
        }
      }

      this->inter_plane_devices.push_back(temp_netdevice_container);
      this->inter_plane_channels.push_back(csma_channel);
      this->inter_plane_channel_tracker.push_back(nodeBIndex);

      m_islDevices.push_back( isl_link_helper.Install(m_satellitesNodes.Get(idxSat1), m_satellitesNodes.Get(idxSat2))  );

      std::cout <<"Setting up #"<<islId << "ISL link with distance of "<<distance/1000<<" km and delay of "<<delay<<" seconds."<< std::endl;

    }


  }

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
    Vector gsPos = m_groundStationsNodes.Get(idxGs)->GetObject<FullGroundStationMobilityModel> ()->GetPosition();
    std::string dataRate = m_groundStationsNodes.Get(idxGs)->GetObject<FullGroundStationMobilityModel> ()->GetDataRate();
    //*** simultaneous sat links
    // uint32_t numGsl = m_groundStationsNodes.Get(idxGs)->GetObject<FullGroundStationMobilityModel> ()->GetNumGsl();


    // std::vector < std::pair< uint32_t, double > > idxDistVec;

    uint32_t closestSatId = 0;
    double closestSatDist = 0;

    /*
    ***********************************************************************************************
    //************* #TODO:  ADD simultaneous multi satellite links (based on ISL setup) *************
    ***********************************************************************************************
    */
    //find closest adjacent satellite for ground station
    for (uint32_t idxSat=0; idxSat<totalNumSatellites; idxSat++)
    {
      // <SatellitePositionMobilityModel> ?
      // JulianDate currentJdut = m_satellitesNodes.Get(idxSat)->GetObject<SatellitePositionMobilityModel>()->GetStartTime() + Simulator::Now();

      Vector3D satPos = m_satellitesNodes.Get(idxSat)->GetObject<MobilityModel>()->GetPosition();
      Vector3D satVel = m_satellitesNodes.Get(idxSat)->GetObject<MobilityModel>()->GetVelocity();
      PVCoords satPVecef(satPos, satVel, FrameType::ECEF);

      
      Topos gsSatTopos = m_groundStationsNodes.Get(idxGs)->GetObject<FullGroundStationMobilityModel> ()->GetVisibilityGroundToSat(satPVecef);
      
      double temp_dist = gsSatTopos.range;
      bool visible = gsSatTopos.visibility;

      if( ( (temp_dist < closestSatDist) || (idxSat==0)) && visible )
      {
        closestSatDist = temp_dist;
        closestSatId = idxSat;
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


    double delay = (closestSatDist)/SPEED_OF_LIGHT;
    
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




    Ptr<CsmaChannel> csma_channel;
    Ptr<Channel> channel;
    gs_channel = temp_netdevice_container.Get(0)->GetChannel();
    gs_csma_channel = gs_channel->GetObject<CsmaChannel> ();
    
    // num_satellites_per_plane = m_planes[idxPlane].size();
    for (uint32_t k = 1; k <= temp_node_container.GetN(); k++)
    // for (uint32_t k = 0; k < m_satellitesNodes.GetN(); k++)
    {
      if (k != closestSatId)
      {
        gs_csma_channel->Detach(temp_netdevice_container.Get(k)->GetObject<CsmaNetDevice> ());
      }
    }    

    std::cout <<"Channel open between ground station " << idxGs << 
            " and satellite " << closestSatId << "with distance "<< 
            closestSatDist/1000 << "km and delay of "<< delay <<" seconds"<< std::endl;    

    m_groundStationsDevices.push_back(temp_netdevice_container);
    m_groundStationsChannels.push_back(csma_channel);
    m_groundStationsChannelTracker.push_back(closestSatId);
  
  }

}

void SatelliteNetworkConfig::BuildGSLsMultiSat()
{
  return -1;
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
  address.SetBase ("10.1.0.0", "255.255.255.0");


   //configuring IP Addresses for ISL devices
  for(uint32_t i=0; i< this->m_islDevices.size(); i++)
  {
    address.NewNetwork();
    m_islInterfaces.push_back(address.Assign(m_islDevices[i]));
  }

  //configuring IP Addresses for Ground devices
  for(uint32_t i=0; i < m_groundStationsDevices.size(); i++)
  {
    address.NewNetwork();
    m_groundStationsInterfaces.push_back(address.Assign(m_groundStationsDevices[i]));
    for(uint32_t j=1; j<= m_groundStationsDevices[i].GetN(); j++)
    {
      if(j != m_groundStationsChannelTracker[i] + 1)
      {
        std::pair< Ptr< Ipv4 >, uint32_t > interface = m_groundStationsInterfaces[i].Get(j);
        interface.first->SetDown(interface.second);
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
SatelliteNetworkConfig::BuildNetwork (std::string &TLEfilepath, std::string &GSfilepath)

{
  ReadSatConfigFile(TLEfilepath);
  ReadGSConfigFile(GSfilepath);
  // ReadAirConfigFile();
  
  
  BuildISLsP2PFixed();
  // BuildISLsDynamic();
  BuildGSLsSingleSat();
  // BuildGSLsMultiSat();

  SetupIPConfig();

 }


void SatelliteNetworkConfig::UpdateISLsP2PFixed()
{
return -1;
}


void SatelliteNetworkConfig::UpdateLinks()
{
  std::cout <<std::endl<<std::endl<<std::endl<<"Updating Links ... "<< std::endl;


  /*
  //******************************************************************************************
  //*************************** ISL UPDATES **************************************************
  //******************************************************************************************
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
//******************************************************************************************
//*************************** GSL UPDATES **************************************************
//******************************************************************************************
*/


  //setting up links between ground stations and their closest satellites
  std::cout <<"Setting links between ground stations and satellites"<< std::endl;
  for (uint32_t idxGs=0; idxGs<m_groundStationsNodes.GetN(); idxGs++)
  {
    Vector gsPos = m_groundStationsNodes.Get(idxGs)->GetObject<FullGroundStationMobilityModel> ()->GetPosition();
    std::string dataRate = m_groundStationsNodes.Get(idxGs)->GetObject<FullGroundStationMobilityModel> ()->GetDataRate();

    uint32_t closestSatId = 0;
    double closestSatDist = 0;

/*
***********************************************************************************************
//************* #TODO:  ADD simultaneous multi satellite links (based on ISL setup) *************
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
      bool visible = gsSatTopos.visibility;

      if( ( (temp_dist < closestSatDist) || (idxSat==0)) && visible )
      {
        closestSatDist = temp_dist;
        closestSatId = idxSat;
      }
    }

    double newDelay = (closestSatDist)/SPEED_OF_LIGHT;
    
    
    uint32_t prevClosestSatId = m_groundStationsChannelTracker[idxGs];
    
    if(prevClosestSatId == closestSatId) // no change of closest
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

}



double 
SatelliteNetworkConfig::CalculateDistance(Vector &pos1, Vector &pos2)
{
  return (pos1 - pos2).GetLength();
}
double 
SatelliteNetworkConfig::CalculateDistance(PVCoords &pv1, PVCoords &pv2)
{
  return (pv1.GetPos() - pv2.GetPos()).GetLength();
}



} // end ns3 namespace