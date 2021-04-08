from spacetrack import SpaceTrackClient
from .stconfig.auth import USR, PASS

_PREDEFINED_CONSTELLATIONS = {
                             "starlink": [-1],
                             "oneweb": [-1],
                             "iridium": [-1],
                             "telesat": [-1],
                             "spire": [-1],
                             "planet": [-1],
                             }


def get_TLE_data( norad_ids:list[int]) -> list[dict]:

    st = SpaceTrackClient(USR, PASS)
    # ordinal=1 gets only the latest tle for each noradID.
    tle_obj_vec = st.tle_latest(ordinal=1,norad_cat_id=norad_ids, orderby="norad_cat_id")

    return tle_obj_vec

def get_pre_defined_constellations(constellation_name: str) -> list[dict]:
    if constellation_name:=constellation_name.lower()  not in _PREDEFINED_CONSTELLATIONS:
        raise TypeError(f"Constellation {constellation_name} not implemented, try supplying its individual TLES.")
    else:
         constell_norad_ids = _PREDEFINED_CONSTELLATIONS.get(constellation_name, [])

         return get_TLE_data(constell_norad_ids)


def write_TLEs_to_file(tle_obj_vec: list[dict], filepath:str="sats-TLE-example.json") -> None:
    import json 

    # save only tle lines from tle objects for lighter json dump
    tle_json_data = {item["NORAD_CAT_ID"]: [item["TLE_LINE1"], item["TLE_LINE2"]]  
                        for item in tle_obj_vec }  

    with open(filepath, "w") as file:
        json.dump(tle_json_data, file)
    
    print(f"TLE File written successfully at {filepath}.")


def read_TLEs_from_file(filepath: str) -> list[dict]:

    import json

    with open(filepath, "r") as file:
        tledata = json.load(file)

    return tledata