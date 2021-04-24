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

def get_pre_defined_constellations(constellation_name: str) -> list[dict]:
    constellation_name = constellation_name.lower()
    if constellation_name  not in _PREDEFINED_CONSTELLATIONS:
        raise TypeError(f"Constellation {constellation_name} not implemented, try supplying its individual TLES.")
    else:
        constell_norad_ids = _PREDEFINED_CONSTELLATIONS.get(constellation_name)
        
        return get_TLE_data(constell_norad_ids)


def write_TLEs_to_file(tle_obj_vec: list[dict], filepath:str="sats-TLE-example.json") -> None:
    import json 

    # save only tle lines from tle objects for lighter json dump
    tle_json_data = {item["NORAD_CAT_ID"]: [item["TLE_LINE1"], item["TLE_LINE2"]]  
                        for item in tle_obj_vec }  

    with open(filepath, "w") as file:
        json.dump(tle_json_data, file, indent=6)
    
    print(f"TLE File written successfully at {filepath}.")


def write_PV_to_file(tle_obj_vec: list[dict], filepath:str="sats-TLE-example.json") -> None:
    import json 
    import datetime
    from sgp4.api import Satrec, jday

    json_posveldata = {}
    # all TLEs (w/ diff epochs) will be propagated to current time.

    now = datetime.datetime.now()
    jd, fr = jday (now.year, now.month, now.day, now.minute, 0,0)

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

def read_TLEs_from_file(filepath: str) -> list[dict]:

    import json

    with open(filepath, "r") as file:
        tledata = json.load(file)

    return tledata



