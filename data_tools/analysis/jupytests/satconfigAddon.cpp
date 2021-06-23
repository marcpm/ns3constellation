


//setting up all intraplane links
  NS_LOG_INFO("Setting up intra-plane links with distance of "<<distance<<" km and delay of "<<delay<<" seconds."<<std::endl);


  for (uint32_t i=0; i<totalNumSatellites; i++)
  {
    std::vector<float> = {};
    for (uint32_t j=0; j<totalNumSatellites; j++)
    {

        Vector nodeAPosition = this->plane[0].Get(0)->GetObject<MobilityModel>()->GetPosition();
        Vector nodeBPosition = this->plane[0].Get(1)->GetObject<MobilityModel>()->GetPosition();
        double distance = CalculateDistance(nodeAPosition, nodeBPosition);
        double delay = (distance * 1000)/speed_of_light; //should get delay in seconds
        
        
        PointToPointHelper intraplane_link_helper;
        intraplane_link_helper.SetDeviceAttribute ("DataRate", StringValue ("5.36Gbps"));
        intraplane_link_helper.SetChannelAttribute ("Delay", TimeValue(Seconds (delay)));




      m_islDevices.push_back()  
      this->intra_plane_devices.push_back(intraplane_link_helper.Install(plane[i].Get(j), plane[i].Get((j+1)%num_satellites_per_plane)));
      std::cout<<"Plane "<<i<<": channel between node "<<j<<" and node "<<(j+1)%num_satellites_per_plane<<std::endl;
    }
  }