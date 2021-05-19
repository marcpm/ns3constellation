/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */


#include "satellite-network-config.h"

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (SatelliteNetworkConfig);
NS_LOG_COMPONENT_DEFINE ("SatelliteNetworkConfig");

extern double CalculateDistanceGroundToSat (const Vector &a, const Vector &b);

double speed_of_light = 299792458; //in m/s

//typeid
TypeId SatelliteNetworkConfig::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::SatelliteNetworkConfig")
  .SetParent<Object> ()
  .SetGroupName("LeoSatellite")
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
   if (satFile.is_open()){   //checking whether the file is open
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
            MobilityHelper mobility;
            
            mobility.SetMobilityModel(
                        "ns3::SatellitePositionMobilityModel",
                        "SatellitePositionHelper",
                        SatellitePositionHelperValue(SatellitePositionHelper(satellite))
                );
            mobility.Install(m_satellitesNodes.Get(idxSat)); // install mobility in new node created

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

// extra util split string with chosen delimiter
std::vector<std::string> 
SatelliteNetworkConfig::splitString (std::string s, std::string delimiter) 
{
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    std::string token;
    std::vector<std::string> res;

    while ((pos_end = s.find (delimiter, pos_start)) != std::string::npos) {
        token = s.substr (pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back (token);
    }

    res.push_back (s.substr (pos_start));
    return res;
}



void SatelliteNetworkConfig::ReadGSConfigFile (std::string GSfilepath)
{/////// editttt
   fstream gsFile;
   gsFile.open(satConfigFilepath, ios::in); //open a file to perform read operation using file object
   if (gsFile.is_open()){   //checking whether the file is open
      string GSname;
      string tmp;
      
      uint32_t idxGs = 0;
      while(getline(gsFile, tmp))
      { //read data from file object and put it into string.

        std::vector <string> gsStrLine = splitString(tmp, ",");
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

                        )
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

//constructor
SatelliteNetworkConfig::SatelliteNetworkConfig (std::string TLEfilepath, std::string GSfilepath)

{
  ReadSatConfigFile();
  ReadGSConfigFile();

  uint32_t totalNumSatellites = m_satellitesNodes.GetN();
  // // NodeContainer temp;
  // // m_satellitesNodes.Create(totalNumSatellites); // 

  Vector nodeAPosition = this->plane[0].Get(0)->GetObject<SateliteMobilityModel>()->GetPosition();
  Vector nodeBPosition = this->plane[0].Get(1)->GetObject<FullGroundStationMobilityModel>()->GetPosition();
  double distance = CalculateDistance(nodeAPosition, nodeBPosition);
  double delay = (distance * 1000)/speed_of_light; //should get delay in seconds
  PointToPointHelper isl_link_helper;
  isl_link_helper.SetDeviceAttribute ("DataRate", StringValue ("5.36Gbps"));
  isl_link_helper.SetChannelAttribute ("Delay", TimeValue(Seconds (delay)));

  NS_LOG_INFO("Setting up intra-plane links with distance of "<<distance<<" km and delay of "<<delay<<" seconds."<<std::endl);

 for (uint32_t i=0; i<num_planes; i++)
  {
    for (uint32_t j=0; j<num_satellites_per_plane; j++)
    {
      this->intra_plane_devices.push_back(intraplane_link_helper.Install(plane[i].Get(j), plane[i].Get((j+1)%num_satellites_per_plane)));
      NS_LOG_INFO("Plane "<<i<<": channel between node "<<j<<" and node "<<(j+1)%num_satellites_per_plane<<std::endl);
    }
  }

  // //setting up interplane links
  // NS_LOG_INFO("Setting up inter-plane links"<<std::endl);
  // for (uint32_t i=0; i<num_planes; i++)
  // {
  //   for (uint32_t j=0; j<num_satellites_per_plane; j++)
  //   {
  //     uint32_t nodeBIndex;
  //     (i == num_planes - 1) ? nodeBIndex = num_satellites_per_plane - j - 1: nodeBIndex = j;
  //     Vector nodeAPos = this->plane[i].Get(j)->GetObject<MobilityModel>()->GetPosition();
  //     Vector nodeBPos = this->plane[(i+1)%num_planes].Get(nodeBIndex)->GetObject<MobilityModel>()->GetPosition();
  //     double distance = CalculateDistance(nodeAPos, nodeBPos);
  //     double delay = (distance*1000)/speed_of_light;
  //     CsmaHelper interplane_link_helper;
  //     interplane_link_helper.SetChannelAttribute("DataRate", StringValue ("5.36Gbps"));
  //     interplane_link_helper.SetChannelAttribute("Delay", TimeValue(Seconds(delay)));

  //     NS_LOG_INFO("Channel open between plane "<<i<<" satellite "<<j<<" and plane "<<(i+1)%num_planes<<" satellite "<<nodeBIndex<< " with distance "<<distance<< "km and delay of "<<delay<<" seconds"<<std::endl);

  //     NodeContainer temp_node_container;
  //     temp_node_container.Add(this->plane[i].Get(j));
  //     temp_node_container.Add(this->plane[(i+1)%num_planes]);
  //     NetDeviceContainer temp_netdevice_container;
  //     temp_netdevice_container = interplane_link_helper.Install(temp_node_container);
  //     Ptr<CsmaChannel> csma_channel;
  //     Ptr<Channel> channel;
  //     channel = temp_netdevice_container.Get(0)->GetChannel();
  //     csma_channel = channel->GetObject<CsmaChannel> ();
      
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
  //   }
  // }

//////////////////////////////////////////////////////
///////  GROND-SAT LINKS     ////////////////////
//////////////////////////////////////////////////////

  totalNumGroundstations = m_groundStationsNodes.GetN();


  //Install IP stack
  InternetStackHelper stack;
  stack.Install(m_groundStationsNodes);
  

  // for (std::vector<GroundStation>::iterator gs = m_groundStations.begin(); gs != m_groundStations.end(); )
  // for (GroundStation& const gs: m_groundStations)
  // for (Node& gsNode: m_groundStationsNodes)
  // {
  //   Vector gsPos = gsNode->GetObject<MobilityModel> () ->GetPosition();
  //   // Vector gsPos = gsNode->GetObject<GroundStation> () ->GetLatLonAlt();

  //   // NS_LOG_INFO("" << Simulator::Now().GetSeconds() << ": ground station # " << gs.GetId() << ": lat = " << gs.GetLat() 
  //   //           << ", lon = " << gs.GetLon() << 
  //   //           "alt = "<< gs.GetAlt()<< std::endl);

  //   NS_LOG_INFO("" << Simulator::Now().GetSeconds() << ": ground station # " << j << ": x = " << temp.x << ", y = " << temp.y << std::endl);
  // }


  for (int j = 0; j<totalNumGroundstations; j++)
  {
    Vector temp = m_groundStationsNodes.Get(j)->GetObject<MobilityModel> ()->GetPosition();
    // Vector LatLonAlt = ground)stations.Get(j)
    NS_LOG_INFO("" << Simulator::Now().GetSeconds() << ": ground station # " << j << ": x = " << temp.x << ", y = " << temp.y << std::endl);
  }


  //setting up links between ground stations and their closest satellites
  NS_LOG_INFO("Setting links between ground stations and satellites"<<std::endl);
  for (uint32_t idxGs=0; idxGs<totalNumGroundstations; idxGs++)
  {
    Vector gsPos = m_groundStationsNodes.Get(idxGs)->GetObject<FullGroundStationMobilityModel> ()->GetPosition();
    std::string dataRate = m_groundStationsNodes.Get(idxGs)->GetObject<FullGroundStationMobilityModel> ()->GetDataRate();
    uint32_t closestSatid = 0;
    double closestSatDist = 0;

    //find closest adjacent satellite for ground station
    for (uint32_t idxSat=0; idxSat<totalNumSatellites; idxSat++)
    {
      Vector3D satPos = m_constellationSatsNodes.Get(idxSat)->GetObject<MobilityModel>()->GetPosition();
      Vector3D satVel = m_constellationSatsNodes.Get(idxSat)->GetObject<MobilityModel>()->GetVelocity();
      PVCoords satPVecef(satPos, satVel, FrameType::ECEF);

      Topos gsSatTopos = m_groundStationsNodes.Get(idxGs)->GetObject<FullGroundStationMobilityModel> ()->GetVisibilityGroundToSat(satPVecef);
      
      
      double temp_dist = gsSatTopos.range;
      double visible = gsSatTopos.visibility;

      if( ( (temp_dist < closestAdjSatDist) || (idxSat==0)) && visible )
      {
        closestSatDist = temp_dist;
        closestSatId = idxSat;
      }
    }

    double delay = (closestSatDist*1000)/speed_of_light;
    CsmaHelper ground_station_link_helper;
    ground_station_link_helper.SetChannelAttribute("DataRate", StringValue (dataRate));
    ground_station_link_helper.SetChannelAttribute("Delay", TimeValue(Seconds(delay)));

    NS_LOG_INFO("Channel open between ground station " << idxGs << 
            " and satellite " << closestSatId << "with distance "<< 
            closestSatDist<< "km and delay of "<<delay<<" seconds"<<std::endl);


    NodeContainer temp_node_container;
    temp_node_container.Add(ground_stations.Get(idxGs));
    temp_node_container.Add(this->plane[planeIndex]);
    NetDeviceContainer temp_netdevice_container;
    temp_netdevice_container = ground_station_link_helper.Install(temp_node_container);
    Ptr<CsmaChannel> csma_channel;
    Ptr<Channel> channel;
    channel = temp_netdevice_container.Get(0)->GetChannel();
    csma_channel = channel->GetObject<CsmaChannel> ();

    for (uint32_t k=0; k<num_satellites_per_plane; k++)
    {
      if (closestAdjSat != k)
      {
        csma_channel->Detach(temp_netdevice_container.Get(k+1)->GetObject<CsmaNetDevice> ());
      }
    }
        
    this->ground_station_devices.push_back(temp_netdevice_container);
    this->ground_station_channels.push_back(csma_channel);
    this->ground_station_channel_tracker.push_back(closestAdjSat);
  }

  //Configure IP Addresses for all NetDevices
  Ipv4AddressHelper address;
  address.SetBase ("10.1.0.0", "255.255.255.0");

  //configuring IP Addresses for IntraPlane devices
  for(uint32_t i=0; i< this->intra_plane_devices.size(); i++)
  {
    address.NewNetwork();
    this->intra_plane_interfaces.push_back(address.Assign(this->intra_plane_devices[i]));
  }
  
  //configuring IP Addresses for InterPlane devices
  for(uint32_t i=0; i< this->inter_plane_devices.size(); i++)
  {
    address.NewNetwork();
    this->inter_plane_interfaces.push_back(address.Assign(this->inter_plane_devices[i]));
    for(uint32_t j=1; j<= this->num_satellites_per_plane; j++)
    {
      if(j != this->inter_plane_channel_tracker[i] + 1)
      {
        std::pair< Ptr< Ipv4 >, uint32_t > interface = this->inter_plane_interfaces[i].Get(j);
        interface.first->SetDown(interface.second);
      }
    }
  }

  //configuring IP Addresses for Ground devices
  for(uint32_t i=0; i< this->ground_station_devices.size(); i++)
  {
    address.NewNetwork();
    this->ground_station_interfaces.push_back(address.Assign(this->ground_station_devices[i]));
    for(uint32_t j=1; j<= this->num_satellites_per_plane; j++)
    {
      if(j != this->ground_station_channel_tracker[i] + 1)
      {
        std::pair< Ptr< Ipv4 >, uint32_t > interface = this->ground_station_interfaces[i].Get(j);
        interface.first->SetDown(interface.second);
      }
    }
  }

  //Populate Routing Tables
  NS_LOG_INFO("Populating Routing Tables"<<std::endl);
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
  NS_LOG_INFO("Finished Populating Routing Tables"<<std::endl);

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

/*
******************************************************
******************************************************
******************************************************
******************************************************
******************************************************
******************************************************
*/


void SatelliteNetworkConfig::UpdateLinks()
{
  NS_LOG_INFO(std::endl<<std::endl<<std::endl<<"Updating Links"<<std::endl);

  std::vector<NodeContainer> update_links_plane = this->plane;
  NodeContainer final_plane;
  for(uint32_t j=0; j<num_satellites_per_plane; j++)
  {
    final_plane.Add(this->plane[0].Get(num_satellites_per_plane - j - 1));
  }
  update_links_plane.push_back(final_plane);
 
  for (uint32_t i=0; i<this->num_planes; i++)
  {
    Vector refSatPos;
    uint32_t refSat = 0;
    //find reference satellite (closest to equator)
    for (uint32_t j=0; j<this->num_satellites_per_plane; j++)
    {
      Vector pos = update_links_plane[i].Get(j)->GetObject<MobilityModel>()->GetPosition();
      if ((std::abs(pos.x) < std::abs(refSatPos.x)) || j == 0)
      {
        refSatPos = pos;
        refSat = j;
      }
    }

    //find the closest adjacent satellite to the reference satellite
    uint32_t closestAdjSat = 0;
    double closestAdjSatDist = 0;
    Vector adjPos; //debug
    for (uint32_t j=0; j<this->num_satellites_per_plane; j++)
    {
      Vector pos = update_links_plane[i+1].Get(j)->GetObject<MobilityModel>()->GetPosition();
      double temp_dist = CalculateDistance(refSatPos,pos);
      if((temp_dist < closestAdjSatDist) || (j==0))
      {
        closestAdjSatDist = temp_dist;
        closestAdjSat = j;
        adjPos = pos; //debug
      }
    }

    //calculate the reference increment factor for adjacent satellites in a plane
    uint32_t ref_incr;
    (refSat <= closestAdjSat) ? (ref_incr = closestAdjSat - refSat) : (ref_incr = this->num_satellites_per_plane - refSat + closestAdjSat);

    //update all adjacent satellites for this plane
    for (uint32_t j=0; j<this->num_satellites_per_plane; j++)
    {
      uint32_t access_idx = i*(this->num_satellites_per_plane) + j;
      uint32_t currAdjNodeID = this->inter_plane_channel_tracker[access_idx];
      uint32_t nextAdjNodeID = (j + ref_incr)%(this->num_satellites_per_plane);
      double nextAdjNodeDist;

      Vector constNodePos = update_links_plane[i].Get(j)->GetObject<MobilityModel>()->GetPosition();
      Vector nextAdjNodePos = update_links_plane[(i+1)/*%(this->num_planes)*/].Get(nextAdjNodeID)->GetObject<MobilityModel>()->GetPosition();

      nextAdjNodeDist = CalculateDistance(constNodePos, nextAdjNodePos);

      if (i == this->num_planes - 1)
        nextAdjNodeID = num_satellites_per_plane - nextAdjNodeID - 1;

      if(currAdjNodeID == nextAdjNodeID)
      {
        double new_delay = (nextAdjNodeDist*1000)/speed_of_light;
        this->inter_plane_channels[access_idx]->SetAttribute("Delay", TimeValue(Seconds(new_delay)));
        NS_LOG_INFO("Channel updated between plane "<<i<<" satellite "<<j<<" and plane "<<(i+1)%num_planes<<" satellite "<<nextAdjNodeID<< " with distance "<<nextAdjNodeDist<< "km and delay of "<<new_delay<<" seconds"<<std::endl);
      }
      else
      {
        this->inter_plane_channels[access_idx]->Detach(this->inter_plane_devices[access_idx].Get(currAdjNodeID+1)->GetObject<CsmaNetDevice> ());
        std::pair< Ptr< Ipv4 >, uint32_t> interface = this->inter_plane_interfaces[access_idx].Get(currAdjNodeID+1);
        interface.first->SetDown(interface.second);
        this->inter_plane_channels[access_idx]->Reattach(this->inter_plane_devices[access_idx].Get(nextAdjNodeID+1)->GetObject<CsmaNetDevice> ());
        interface = this->inter_plane_interfaces[access_idx].Get(nextAdjNodeID+1);
        interface.first->SetUp(interface.second);
        this->inter_plane_channel_tracker[access_idx] = nextAdjNodeID;
        double new_delay = (nextAdjNodeDist*1000)/speed_of_light;
        this->inter_plane_channels[access_idx]->SetAttribute("Delay", TimeValue(Seconds(new_delay)));
        NS_LOG_INFO("New channel between plane "<<i<<" satellite "<<j<<" and plane "<<(i+1)%num_planes<<" satellite "<<nextAdjNodeID<< " with distance "<<nextAdjNodeDist<< "km and delay of "<<new_delay<<" seconds"<<std::endl);
      }
    }
  }

  //updating links between ground stations and their closest satellites
  for (uint32_t i=0; i<2; i++)
  {
    Vector gndPos = ground_stations.Get(i)->GetObject<MobilityModel> ()->GetPosition();
    uint32_t closestAdjSat = 0;
    uint32_t closestAdjSatDist = 0;
    uint32_t planeIndex;
    if (i == 0)
      planeIndex = 0;
    else
      planeIndex = floor(3*num_planes/7);
    //find closest adjacent satellite for ground station
    for (uint32_t j=0; j<this->num_satellites_per_plane; j++)
    {
      Vector pos = this->plane[planeIndex].Get(j)->GetObject<MobilityModel>()->GetPosition();
      double temp_dist = CalculateDistanceGroundToSat(gndPos,pos);
      if((temp_dist < closestAdjSatDist) || (j==0))
      {
        closestAdjSatDist = temp_dist;
        closestAdjSat = j;
      }
    }

    uint32_t currAdjNodeID = this->ground_station_channel_tracker[i];
    if(currAdjNodeID == closestAdjSat)
    {
      double new_delay = (closestAdjSatDist*1000)/speed_of_light;
      this->ground_station_channels[i]->SetAttribute("Delay", TimeValue(Seconds(new_delay)));
      NS_LOG_INFO("Channel updated between ground station "<<i<<" and plane "<<planeIndex<<" satellite "<<closestAdjSat<< " with distance "<<closestAdjSatDist<< "km and delay of "<<new_delay<<" seconds"<<std::endl);
      }
      else
      {
        this->ground_station_channels[i]->Detach(this->ground_station_devices[i].Get(currAdjNodeID+1)->GetObject<CsmaNetDevice> ());
        std::pair< Ptr< Ipv4 >, uint32_t> interface = this->ground_station_interfaces[i].Get(currAdjNodeID+1);
        interface.first->SetDown(interface.second);
        this->ground_station_channels[i]->Reattach(this->ground_station_devices[i].Get(closestAdjSat+1)->GetObject<CsmaNetDevice> ());
        interface = this->ground_station_interfaces[i].Get(closestAdjSat+1);
        interface.first->SetUp(interface.second);
        this->ground_station_channel_tracker[i] = closestAdjSat;
        double new_delay = (closestAdjSatDist*1000)/speed_of_light;
        this->ground_station_channels[i]->SetAttribute("Delay", TimeValue(Seconds(new_delay)));
        NS_LOG_INFO("New channel between ground station "<<i<<" and plane "<<planeIndex<<" satellite "<<closestAdjSat<< " with distance "<<closestAdjSatDist<< "km and delay of "<<new_delay<<" seconds"<<std::endl);
      }
  }
  
  //Recompute Routing Tables
  NS_LOG_INFO("Recomputing Routing Tables"<<std::endl);
  Ipv4GlobalRoutingHelper::RecomputeRoutingTables ();
  NS_LOG_INFO("Finished Recomputing Routing Tables"<<std::endl);
}

}

