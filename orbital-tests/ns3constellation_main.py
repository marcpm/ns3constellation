import argparse

from .spacetrack.orbital_data import get_TLE_data, write_TLEs_to_file


# from .analysis.core import basic_stats
s
if __name__ == "__main__":

    parser = argparse.ArgumentParser()
    group = parser.add_mutually_exclusive_group()

    group.add_arguments("--known-constellation", type=str, )
    group.add_arguments("--norad-ids", type=str, ) # comma sepparated ids, e.g. 2321,12311,12345
    group.add_arguments("--custom-constellation", type=str, )
    group.add_arguments("--inputfile" type=str, action="store_true")
    
    parser.add_arguments("--postprocess", type=bool, required=False)


    args = parser.parse_args()

    norad_ids = [int(noradid) for noradid in norad_ids.split(",")]
    tledata = get_TLE_data(args.norad_ids)
    write_TLEs_to_file(tledata)





