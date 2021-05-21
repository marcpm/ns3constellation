from skyfield.magnitudelib import planetary_magnitude
from skyfield import api
import matplotlib.pyplot as plt


if __name__ == "__main__":
    DATETIME = (2021,5,19)

    GS_LATLON  = (41.415902, 2.14010)
    INCIDENCE_ANGLE = 25.0 # degrees
    TIMESPAN = 24 # hours
    N_TIMESTEPS = 100
    print("****************")
    print("Downloading SPICE kernels")
    ss_ephem = api.load("de421.bsp") # solar system ephemerides)
    ts = api.load.timescale()

    t = ts.now()
    starlink = api.load.tle_file("http://celestrak.com/NORAD/elements/starlink.txt")
    print(f"{len(starlink)} Satellites loaded")
    print()    
    print(f"Average days away from TLE epoch {sum([ abs(t-sat.epoch) for sat in starlink] ) / len(starlink)}  ")
    total_minutes = TIMESPAN * 60
    timespan = ts.utc(*DATETIME, 0, range(0,int(total_minutes), int(total_minutes / N_TIMESTEPS) ))

    print(f"Average time sunlit: { sum( [ sum(sat.at(timespan).is_sunlit(ss_ephem) ) / len(timespan)  for sat in starlink     ]   ) / len(starlink)   }"      )    
    
    groundstation = api.wgs84.latlon(*GS_LATLON)

    topos = [ (sat - groundstation).at(timespan)  for sat in   starlink   ]

    elsazsranges = [ topo.altaz() for topo in topos ]

    els, azs, ranges = zip(*elsazsranges)
    
    #els = [ elazran[0].degrees  for elazran in   elsazsranges ]
    #azs  = [ elazran[1].degrees  for elazran in   elsazsranges]
    #ranges  = [ elazran[2].km  for elazran in   elsazsranges]
#    plt.scatter([az.degrees for az in  azs  ], [el.degrees for el in els]); plt.show()
    
    #plt.scatter([az.degrees for az in  azs  ], [el.degrees for el in els], s=1, c=[r.km for r in ranges]   ); plt.show()
    
    

    limited_visib_timesteps = {}
    for idxsat, ear_sat in enumerate(elsazsranges):
        limited_visib_timesteps[idxsat] = []
        for idxtime,  el_at_timestep in enumerate(ear_sat[0].degrees):
            if el_at_timestep < INCIDENCE_ANGLE:
                limited_visib_timesteps[idxsat].append(idxtime)



    azs_limited = [ azs_sat.degrees[limited_visib_timesteps[idxsat]]  for idxsat, azs_sat in enumerate(azs) ]

    els_limited = [ els_sat.degrees[limited_visib_timesteps[idxsat]]  for idxsat, els_sat in enumerate(els) ]
    ranges_limited = [ ranges_sat.km[limited_visib_timesteps[idxsat]]  for idxsat, ranges_sat in enumerate(ranges) ]
    print(f"{len(timespan)}")
    print()
    print(f"{limited_visib_timesteps[0]=} \n {limited_visib_timesteps[100]=} " )    
    print("****************")

    plt.scatter(list(range(len(starlink))), [len(limited) for limited in limited_visib_timesteps.values()])
    plt.show()
    #plt.figure()
   # plt.scatter( azs_limited, els_limited, s=1, c=ranges_limited); plt.show()

    #plt.figure()
    #plt.scatter( azs, els, s=1, c=ranges); plt.show()


    plt.scatter([az.degrees for az in  azs  ], [el.degrees for el in els], s=1, c=[r.km for r in ranges] )
    plt.ylim(0, 90)
    plt.show()
