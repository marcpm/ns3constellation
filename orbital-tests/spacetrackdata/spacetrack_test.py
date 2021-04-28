from spacetrackdata.orbital_data import (get_TLE_data, get_pre_defined_constellations, 
                                        write_PV_to_file_test, write_TLEs_to_file, write_PV_to_file)



def constellation_test():
    import datetime
    

    constellation = "iridium"     # "starlink", "oneweb", "iridium"
    file_name = f"{constellation}_TLES_PV.json"
    
    t_0 = datetime.datetime.now() # t initial = PC runtime
    # t_0 = datetime.datetime(2021,4,25, 22,10,0) # t initial = predefined date
    
    t_f = t_0 + datetime.timedelta(hours=2) # tfinal = t_0 + 2hours
    time_step = 60 # seconds
    

    spacetrack_objs = get_pre_defined_constellations(constellation_name=constellation)
    # write_TLEs_to_file(spacetrack_objs, "iridiumTLEs.json") # writes raw TLEs in json format
    
    
    write_PV_to_file(spacetrack_objs, t0=t_0, tf=t_f, timestep=time_step, filepath=file_name)




def custom_constellation_test():
    import datetime
    
    # buscar NORAD_IDS en celestrak / spacetrack
    norad_ids = [43928,43929,43930,43931]
    filename = "selected-sats"
    
    
    file_name = f"{constellation}_TLES_PV.json"
    
    t_0 = datetime.datetime.now() # t initial = PC runtime
    # t_0 = datetime.datetime(2021,4,25, 22,10,0) # t initial = predefined date
    
    t_f = t_0 + datetime.timedelta(hours=2) # tfinal = t_0 + 2hours
    time_step = 60 # seconds

    spacetrack_objs = get_TLE_data(norad_ids)
    # write_TLEs_to_file(spacetrack_objs, f"{filename}TLE.json")
    write_PV_to_file(spacetrack_objs,t0=t_0, tf=t_f, timestep=time_step, filepath=file_name) 
        
if __name__ == "__main__":
    constellation_test()