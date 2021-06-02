import argparse

from data_tools.orbital.spacetrackdata.orbital_data import (get_TLE_data, 
                    get_predefined_constellation_data, write_TLEs_to_file)

# from data_tools.ground.examples

# from .analysis.core import basic_stats

if __name__ == "__main__":

    CONSTELLATION = "IRIDIUM"
    SIMULATION_TIME = "2021-05-12 00:00:00.000"




    parser = argparse.ArgumentParser()
    group = parser.add_mutually_exclusive_group()

    group.add_arguments("--known-constellation", type=str, )
    group.add_arguments("--norad-ids", type=str, ) # comma sepparated ids, e.g. 2321,12311,12345
    group.add_arguments("--custom-constellation", type=str, )
    group.add_arguments("--inputfile", type=str, action="store_true")
    
    parser.add_arguments("--postprocess", type=bool, required=False)
    args = parser.parse_args()
    

    if args.norad_ids is not None:
        norad_ids = [int(noradid) for noradid in args.norad_ids.split(",")]
        tle_data = get_TLE_data(norad_ids)
        filename = str(norad_ids[0]) + "-"  + str(norad_ids[-1]) + ".TLE"

    if args.known_constellation is not None:
        tledata = get_predefined_constellation_data(str(args.known_constellation).lower())

    if args.custom_constellation is not None:
        raise TypeError(f"Custom built constellation mode still in development")

    if SIMULATION_TIME is None:
        SIMULATION_TIME = tle_data[0].get("EPOCH").replace("T", " ") # fix T12:22:21.234234234 format
    
    # write first line timestamp
    with open(filename, "w") as file:
        file.write(f"{SIMULATION_TIME}\n")

    write_TLEs_to_file(tle_obj_vec=tle_data, filepath=filename)





