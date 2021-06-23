from skyfield.magnitudelib import planetary_magnitude
from skyfield import api
import matplotlib.pyplot as plt
from matplotlib.patches import Rectangle


if __name__ == "__main__":
    DATETIME = (2021,6,22)

    GS_LATLON  = (41.415902, 2.14010)
    INCIDENCE_ANGLE = 25.0 # degrees
    MIN_ANGLE = 9.0
    
    TIMESPAN = 24 # hours
    N_TIMESTEPS = 100
    CONSTELLATION = "starlink"
    CONSTELLATION = "iridium-NEXT"
    # CONSTELLATION = "oneweb"
    print("****************")
    print("Downloading SPICE kernels")
    ss_ephem = api.load("de421.bsp") # solar system ephemerides)
    ts = api.load.timescale()

    t = ts.now()
    starlink = api.load.tle_file(f"https://celestrak.com/NORAD/elements/{CONSTELLATION}.txt")
    # starlink = api.load.tle_file(f"/Users/marc/Downloads/oneweb.txt")
    print(f"{len(starlink)} Satellites loaded")
    print()    
    print(f"Average days away from TLE epoch {sum([ abs(t-sat.epoch) for sat in starlink] ) / len(starlink)}  ")
    total_minutes = TIMESPAN * 60
    timespan = ts.utc(*DATETIME, 0, range(0,int(total_minutes), int(total_minutes / N_TIMESTEPS) ))

    sunlit_pct = sum( [ sum(sat.at(timespan).is_sunlit(ss_ephem) ) / len(timespan)  for sat in starlink     ]   ) / len(starlink)   
    print(f"Average time sunlit: { sunlit_pct}"      )    
    
    groundstation = api.wgs84.latlon(*GS_LATLON)

    topos = [ (sat - groundstation).at(timespan)  for sat in   starlink   ]

    elsazsranges = [ topo.altaz() for topo in topos ]

    els, azs, ranges = zip(*elsazsranges)
    
    #els = [ elazran[0].degrees  for elazran in   elsazsranges ]
    #azs  = [ elazran[1].degrees  for elazran in   elsazsranges]
    #ranges  = [ elazran[2].km  for elazran in   elsazsranges]
#    plt.scatter([az.degrees for az in  azs  ], [el.degrees for el in els]); plt.show()
    
    #plt.scatter([az.degrees for az in  azs  ], [el.degrees for el in els], s=1, c=[r.km for r in ranges]   ); plt.show()
    
    

    # limited_visib_timesteps = {}
   #  for idxsat, ear_sat in enumerate(elsazsranges):
   #      limited_visib_timesteps[idxsat] = []
   #      for idxtime,  el_at_timestep in enumerate(ear_sat[0].degrees):
   #          if el_at_timestep < INCIDENCE_ANGLE:
   #              limited_visib_timesteps[idxsat].append(idxtime)
   #
   #
   #
   #  azs_limited = [ azs_sat.degrees[limited_visib_timesteps[idxsat]]  for idxsat, azs_sat in enumerate(azs) ]
   #
   #  els_limited = [ els_sat.degrees[limited_visib_timesteps[idxsat]]  for idxsat, els_sat in enumerate(els) ]
   #  ranges_limited = [ ranges_sat.km[limited_visib_timesteps[idxsat]]  for idxsat, ranges_sat in enumerate(ranges) ]
   #  print(f"{len(timespan)}")
   #  print()
   #  print(f"{limited_visib_timesteps[0]=} \n {limited_visib_timesteps[100]=} " )
   #  print("****************")
   #
   #  plt.scatter(list(range(len(starlink))), [len(limited) for limited in limited_visib_timesteps.values()])
   #  plt.ylabel("")
   #  plt.xlabel(" Num Satellites")
   #  plt.show()
    #plt.figure()
   # plt.scatter( azs_limited, els_limited, s=1, c=ranges_limited); plt.show()

    #plt.figure()
    #plt.scatter( azs, els, s=1, c=ranges); plt.show()
    visible_pct = sum([sum (el.degrees > MIN_ANGLE) / len(timespan) for el in els]) / len(starlink)
    visible_num = sum([sum (el.degrees > MIN_ANGLE)  for el in els]) 
    
    print(f"")
    plt.scatter([az.degrees for az in  azs  ], [el.degrees for el in els], s=1, c=[r.km for r in ranges] )
    plt.ylim(0, 90)
    plt.xlabel("Azimuth angle [deg]")
    plt.ylabel("Elevation angle [deg]")
    plt.title(f"{CONSTELLATION.title()} Coverage from Reference Ground Station. \nOver a span of {TIMESPAN} hours \nReachable Satellites {visible_num} \nSatellites sunlit {sunlit_pct*100:.2f}% of the time.")
    currentAxis = plt.gca()
    currentAxis.add_patch(Rectangle((0,0), 360, MIN_ANGLE , alpha=0.4))
    plt.show()
