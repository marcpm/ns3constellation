from spacetrack import SpaceTrackClient
from skyfield.api import EarthSatellite, Topos, load
from .stconfig.auth import USR, PASS


if __name__ == "__main__":
    st = SpaceTrackClient(USR, PASS)
    tlelines = [st.tle_latest(norad_cat_id=45438)[0]["TLE_LINE1"], st.tle_latest(norad_cat_id=45438)[0]["TLE_LINE2"]]


    bcn = Topos(41.414850, 2.165036)
    ts = load.timescale()
    t0 = ts.utc(2020,12,23)
    t1 = ts.utc(2020,12,30)
    satellite = EarthSatellite(*tlelines, "STARKLINK-61", ts)
    t, events = satellite.find_events(bcn, t0, t1, altitude_degrees=30.0)

    for ti, event in zip(t, events):
        name = ('aos @ 30°', 'culminate', 'los @ < 30°')[event]
        if (ti.utc.hour >18 ):#and (event == 0):
            print(ti.utc_strftime('%Y %b %d %H:%M:%S'), name)
            difference = satellite - bcn
            topocentric = difference.at(ti)
            alt, az, distance = topocentric.altaz()
            print(" ",alt)
            print(" ",az)
            print(" ",int(distance.km), 'km')

