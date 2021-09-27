import argparse
import subprocess
import time
from data_tools.orbital.spacetrackdata.orbital_data import (get_TLE_data, 
                    get_predefined_constellation_data, write_TLEs_to_file)

# from data_tools.ground.examples

# from .analysis.core import basic_stats

def call_ns3_constellation(tlepath, gspath, airpath=None):
    subprocess.call(f"./waf --run '--satPath {tlepath} --gsPath {gspath}' ")

    return -1



if __name__ == "__main__":

    # CONSTELLATION = "ONEWEB"
    # SIMULATION_TIME = "2021-05-12 00:00:00.000"
    SIMULATION_TIME = None


    parser = argparse.ArgumentParser()


    group = parser.add_mutually_exclusive_group()

    parser.add_argument("--predef-constellation", type=str, required=False)
    parser.add_argument("--norad-ids", type=str, required=False) # comma sepparated ids, e.g. 2321,12311,12345
    parser.add_argument("--custom-constellation", type=str, required=False )
    # parser.add_argument("--inputfile", type=str, action="store_true", required=False)
    parser.add_argument("--ground-stations", type=str, required=False)
    
    parser.add_argument("--postprocess", type=bool, required=False)
    args = parser.parse_args()
    
    # if args.inputfile is not None:
    #     call_ns3_constellation("")


    if args.norad_ids is not None:
        norad_ids = [int(noradid) for noradid in args.norad_ids.split(",")]
        tle_data = get_TLE_data(norad_ids)
        filename = str(norad_ids[0]) + "-"  + str(norad_ids[-1]) + str(time.time()).replace(".","_") + ".TLE"

    if args.predef_constellation is not None:
        tle_data = get_predefined_constellation_data(str(args.predef_constellation).lower())
        filename = args.predef_constellation.upper() + "-"+ str(time.time()).replace(".","_") + ".TLE"
    
    if args.custom_constellation is not None:
        raise TypeError(f"Custom built constellation mode still in development")


    if SIMULATION_TIME is None:
        SIMULATION_TIME = tle_data[0].get("EPOCH").replace("T", " ") # fix T12:22:21.234234234 format
    
    # write first line timestamp
    with open(filename, "w") as file:
        file.write(f"{SIMULATION_TIME}\n")

    write_TLEs_to_file(tle_obj_vec=tle_data, filepath=filename)





