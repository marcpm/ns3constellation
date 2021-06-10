/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "aircraft-mobility.h"

#define _USE_MATH_DEFINES
#include <cmath>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("AircraftMobility");

NS_OBJECT_ENSURE_REGISTERED (AircraftMobilityModel);


TypeId
AircraftMobilityModel::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::AircraftMobilityModel")
    .SetParent<MobilityModel> ()
    .SetGroupName ("Mobility")
    .AddConstructor<AircraftMobilityModel> ()
    .AddAttribute ("Name", "The aircraft Name.",
                   StringValue (""),
                   MakeStringAccessor (&AircraftMobilityModel::m_name),
                   MakeStringChecker ())
    .AddAttribute ("DataRate", "The aircraft Data Rate.",
                   StringValue ("5.36Gbps"),
                   MakeStringAccessor (&AircraftMobilityModel::m_dataRate),
                   MakeStringChecker ())
    .AddAttribute ("AngleIncidence", "The minimum Angle of incidence of aircraft [deg].",
                   DoubleValue (25.0),
                   MakeDoubleAccessor (&AircraftMobilityModel::m_angleIncidence),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("NumAsl", "Number of simultaneous Air to Sat  links.",
                   IntegerValue (1),
                   MakeIntegerAccessor (&AircraftMobilityModel::m_numAsl),
                   MakeIntegerChecker<uint32_t> ())
                   
  ;

  return tid;
}

AircraftMobilityModel::AircraftMobilityModel()
{
  
}


AircraftMobilityModel::AircraftMobilityModel(std::string name, double angleIncidence, std::string dataRate,
                                                      uint32_t numASLs)
{

    m_name = name;
    m_angleIncidence = angleIncidence;
    m_dataRate = dataRate;
    m_numAsl = numASLs;

}

AircraftMobilityModel::AircraftMobilityModel(std::string name, double angleIncidence, std::string dataRate)
{

    m_name = name;
    m_angleIncidence = angleIncidence;
    m_dataRate = dataRate;
    m_numAsl = 1;

}







AircraftMobilityModel::~AircraftMobilityModel(void)
{
}




 double 
 AircraftMobilityModel::GetAngleOfIncidence(void) const
 {
   return m_angleIncidence;
 }

std::string  
AircraftMobilityModel::GetDataRate(void) const
{
  return m_dataRate;
}

double AircraftMobilityModel::GetNumAsl(void) const
{
  return m_numAsl;
}






std::string 
AircraftMobilityModel::GetName(void) const
{
  return m_name;
}




// Topos
// AircraftMobilityModel::GetVisibilityGroundToSat (const PVCoords &satPVCoords) const
Topos
AircraftMobilityModel::GetVisibilityAirToSat (const Vector satPos, const Vector satVel)
{
  
  // PVCoords ecefSat = satPVCoords->TransformTo(FrameType::ECEF);
  
  // bool jdut1 = false;

  double razel[3];
  double razelrates[3];

  double recef[3] = {satPos.x/1000.0, satPos.y/1000.0, satPos.z/1000.0}; // in  km
  double vecef[3] = {satVel.x/1000.0, satVel.y/1000.0, satVel.z/1000.0};
  // ecef2azel needs lat,lon in rad, alt in km.

  Vector3D propagatedState = PropagateLatLonAlt();
  double latitude = propagatedState.x;
  double longitude = propagatedState.y;
  double altitude = propagatedState.z;

  ECEF2azel(recef, vecef, latitude * M_PI / 180.0 , longitude * M_PI / 180.0 , altitude / 1000.0, 
            razel, razelrates);

  bool visible = (razel[2] / M_PI * 180.0 ) > m_angleIncidence;
  
  // toposAzelRa Coords in az[deg] el[deg]  ra[m] : idem rates [/s]
  Topos topoFrame = {razel[1] / M_PI * 180.0, razel[2] / M_PI * 180.0, razel[0] * 1000.0, 
                    razelrates[1] / M_PI * 180.0, razelrates[2] / M_PI * 180.0, razelrates[0] * 1000.0, visible 
                  };

  return topoFrame;
  
}





void 
AircraftMobilityModel::SetAircraftTrackLog ( std::vector<std::pair< double, Vector3D>> &geomap)
{
  m_airTrackLog = geomap;
}


// privs



void 
AircraftMobilityModel::DoSetPosition (const Vector &position)
{
  
}

Vector
AircraftMobilityModel::DoGetPosition (void) const
{
  double rs[3];
  double vs[3];

  Vector3D propagatedState = PropagateLatLonAlt();
  double latitude = propagatedState.x;
  double longitude = propagatedState.y;
  double altitude = propagatedState.z;

  site(latitude * M_PI / 180.0 , longitude * M_PI / 180.0 , altitude / 1000.0, 
        rs, vs);
 
  return Vector3D(rs[0], rs[1], rs[2]);
}


Vector
AircraftMobilityModel::DoGetVelocity (void) const
{
  
 // velocity not interpolated yet. might be useful for future doppler effect modeling
  return Vector3D(0,0,0);
}


// Basic Interpolation Propagation
Vector3D
AircraftMobilityModel::PropagateLatLonAlt (double time) const
{
  
  // Time currEpoch = Simulator::Now().GetSeconds();
  Vector3D latlonalt = AirPropagator(time, m_airTrackLog);

  return latlonalt;
}

// Basic Interpolation Propagation
Vector3D
AircraftMobilityModel::PropagateLatLonAlt (void) const
{
  
  double currEpoch = Simulator::Now().GetSeconds();
  Vector3D latlonalt = AirPropagator(currEpoch, m_airTrackLog);

  return latlonalt;
}



double interp1d (double x, double x1, double y1, double x2, double y2)
{
	double y = y1 + (y2 - y1) * (x - x1) / (x2 - x1);
return y;
}

Vector3D
LatLonAltInterp(double epoch, double epoch1, Vector3D latlonalt1, 
			  double epoch2, Vector3D latlonalt2)
{
	// asert epoch 1 < epoch 2;
	double lat = interp1d(epoch, epoch1, latlonalt1.x, epoch2, latlonalt2.x);
	double lon = interp1d(epoch, epoch1, latlonalt1.y, epoch2, latlonalt2.y);
	double alt  = interp1d(epoch, epoch1, latlonalt1.z, epoch2, latlonalt2.z);

	Vector3D latlonalt (lat, lon, alt);
	return latlonalt;
}

Vector3D
AirPropagator(double epoch, std::vector< std::pair <double, Vector3D >> airTrackLog)
// epoch input should be simulator
// add noise, turbulence, 
// airTrackLog normalized to timespan (0 to timesimulation), not juliandates.
{
	for (int i=0; i < (int) airTrackLog.size(); i++)
	{
		double epoch_i = airTrackLog.at(i).first;

	   	if (epoch <  epoch_i  && i>0)
		{
			Vector3D latlonalt_i = airTrackLog.at(i).second;
			
      double epoch_im1 = airTrackLog.at(i-1).first; // m1 == minus 1 := from previous timestep
			Vector3D latlonalt_im1 = airTrackLog.at(i-1).second;
			
			Vector3D geodPosition = LatLonAltInterp(epoch, epoch_im1, latlonalt_im1, epoch_i, latlonalt_i);
			return geodPosition;
		}	
	}
  //else not found::
  return Vector3D(airTrackLog.at(0).second);
}	





} // namespace ns3
