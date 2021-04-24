from spacetrackdata.orbital_data import (get_TLE_data, get_pre_defined_constellations, write_PV_to_file, write_TLEs_to_file)



def spacetrack_test():
    # "starlink", "oneweb", "iridium"
    spacetrack_objs = get_pre_defined_constellations(constellation_name="iridium")
    write_TLEs_to_file(spacetrack_objs, "iridiumTLEs.json")
    write_PV_to_file(spacetrack_objs, "iridiumTLESPV.json")

if __name__ == "__main__":
    spacetrack_test()