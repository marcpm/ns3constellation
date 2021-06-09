import datetime
    
from spacetrack import SpaceTrackClient
from .stconfig import auth
from .constellation_data import STARLINK_NORAD_IDS, ONEWEB_NORAD_IDS, IRIDIUM_NORAD_IDS

_PREDEFINED_CONSTELLATIONS = {
                             "starlink": STARLINK_NORAD_IDS,
                             "oneweb": ONEWEB_NORAD_IDS,
                             "iridium": IRIDIUM_NORAD_IDS,
                             "telesat": [],
                             "spire": [],
                             "planet": [],
                             }


def get_TLE_data( norad_ids:list[int]) -> list[dict]:

    USR = auth["USR"]
    PASS = auth["PASS"]
    st = SpaceTrackClient(USR, PASS)
    # ordinal=1 gets only the latest tle for each noradID.
    tle_obj_vec = st.tle_latest(ordinal=1,norad_cat_id=norad_ids, orderby="norad_cat_id")

    return tle_obj_vec

def get_predefined_constellation_data(constellation_name: str) -> list[dict]:
    constellation_name = constellation_name.lower()
    if constellation_name  not in _PREDEFINED_CONSTELLATIONS:
        raise TypeError(f"Constellation {constellation_name} not implemented, try supplying its individual TLES.")
    else:
        constell_norad_ids = _PREDEFINED_CONSTELLATIONS.get(constellation_name)
        
        return get_TLE_data(constell_norad_ids)


def write_TLEs_to_file(tle_obj_vec: list[dict], filepath:str="sats-TLE-example.json") -> None:


    # # save only tle lines from tle objects for lighter json dump
    # tle_json_data = {item["NORAD_CAT_ID"]: [item["TLE_LINE1"], item["TLE_LINE2"]]  
    #                     for item in tle_obj_vec }  
    with open(filepath, "a") as file:
        # file.write("\n")
        for sat in tle_obj_vec:
            obj_name = sat["OBJECT_NAME"]
            line1 = sat["TLE_LINE1"]
            line2 = sat["TLE_LINE2"]
            file.write(f"{obj_name}\n{line1}\n{line2}\n")
    
    print(f"TLE File written successfully at {filepath}.")



def write_json_TLEs_to_file(tle_obj_vec: list[dict], filepath:str="sats-TLE-example.json") -> None:
    import json 

    # save only tle lines from tle objects for lighter json dump
    tle_json_data = {item["NORAD_CAT_ID"]: [item["TLE_LINE1"], item["TLE_LINE2"]]  
                        for item in tle_obj_vec }  

    with open(filepath, "w") as file:
        json.dump(tle_json_data, file, indent=6)
    
    print(f"TLE File written successfully at {filepath}.")


def write_PV_to_file_test(tle_obj_vec: list[dict], filepath:str="sats-TLE-example.json") -> None:
    import json 
    import datetime
    from sgp4.api import Satrec, jday

    json_posveldata = {}
    # all TLEs (w/ diff epochs) will be propagated to current time.

    now = datetime.datetime.now()
    
    jd, fr = jday (now.year, now.month, now.day, now.hour, now.minute, now.seconds,0)

    for item in tle_obj_vec:
        sat_name = item["NORAD_CAT_ID"]
        
        sat = Satrec.twoline2rv(item["TLE_LINE1"], item["TLE_LINE2"])
        e, r, v = sat.sgp4(jd, fr)
        r = [poskm * 1000 for poskm in r]
        v = [velkms * 1000 for velkms in v]
        json_posveldata[sat_name] = [r, v ]

    # # tle_json_data = {item["NORAD_CAT_ID"]: Satrec.twoline2rv(item["TLE_LINE1"], item["TLE_LINE2"])  
    #                     for item in tle_obj_vec }  

    with open(filepath, "w") as file:
        json.dump(json_posveldata, file, indent=6)
    
    print(f"PV File written successfully at {filepath}.")
    
    
def write_PV_to_file(tle_obj_vec: list[dict], t0: datetime, tf: datetime, timestep: float, filepath:str="sats-TLE-example.json") -> None:
    import json 
    from collections import OrderedDict
    
    import numpy as np
    from sgp4.api import Satrec, SatrecArray, jday
    from skyfield.api import load
    from skyfield.sgp4lib import TEME
    from skyfield.units import Velocity, Distance
    from skyfield.framelib import ecliptic_J2000_frame
    from skyfield.positionlib import ICRF

    json_posveldata = OrderedDict()
    # all TLEs (w/ diff epochs) will be propagated to current time.

    epoch_error_flag = False
    time_vec = [t0]
    t = t0
    while t < tf:
        t += datetime.timedelta(seconds=timestep)
        time_vec.append(t)
    
    years, months, days, hours, minutes, seconds = zip(*[ (t.year, t.month, t.day, t.hour, t.minute, t.second)  for t in time_vec  ])
    
    years, months, days, hours, minutes, seconds = np.array(years), np.array(months), np.array(days), np.array(hours), np.array(minutes), np.array(seconds)
    
    jds, frs = jday (years, months, days, hours, minutes, seconds)

    satrecs_vec = []
    for item in tle_obj_vec:
        sat_name = item["NORAD_CAT_ID"]
        epoch = datetime.datetime.fromisoformat(item["EPOCH"])
        
        if  ( tf > (epoch + datetime.timedelta(days=5)) ) or ( tf < (epoch - datetime.timedelta(days=5)) ):
            # raise TypeError(f" Final propagation date  {tf} is more than 5 days away from latest TLE epoch ({epoch}), for satellite {sat_name}} ")
            epoch_error_flag = True
        
        sat = Satrec.twoline2rv(item["TLE_LINE1"], item["TLE_LINE2"])
        satrecs_vec.append(sat)
        json_posveldata[sat_name] = {"P": [], "V": []} 
    
    
    satrecs = SatrecArray(satrecs_vec)

    errors, rs, vs = satrecs.sgp4(jds, frs) # r,v in TEME frame
    
    

    ts = load.timescale()
    for idxsat, satdata in enumerate(json_posveldata.values()):
        
        for idxtime, (jdi, fri) in enumerate(zip(jds,frs)):
            
            # convert to J2000
            ttime = ts.ut1_jd(jdi + fri)
            
            rvicrf = ICRF.from_time_and_frame_vectors(t=ttime,frame=TEME, 
                    distance=Distance(km=rs[idxsat][idxtime]), velocity= Velocity(km_per_s= vs[idxsat][idxtime]))
            # rvj2000 = rvicrf.frame_xyz_and_velocity(ecliptic_J2000_frame)
            # pos_timestep_j2000, vel_timestep_j2000 = rvj2000[0].m, rvj2000[1].m_per_s

            pos_timestep_j2000, vel_timestep_j2000 = rvicrf.position.m, rvicrf.velocity.m_per_s
            
            satdata["P"].append( pos_timestep_j2000.tolist() )
            satdata["V"].append( vel_timestep_j2000.tolist() )

        # satdata["P"] = rs[idxsat].tolist() #TEME
        # satdata["V"] = vs[idxsat].tolist() #TEME

    
    if errors.any():
        print("SGP4 errors found.") # check errror type.
    if epoch_error_flag: 
        print(f"Warning: Results obtained might be inaccurate. Final propagation date  {tf} is more than 5 days away from latest TLE epoch ({epoch}), for satellite {sat_name} ")


    with open(filepath, "w") as file:
        json.dump(json_posveldata, file, indent=6)
    
    print(f"PV File written successfully at {filepath}.")


def read_TLEs_from_file(filepath: str) -> list[dict]:

    import json

    with open(filepath, "r") as file:
        tledata = json.load(file)

    return tledata



