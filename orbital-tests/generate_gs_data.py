from pykml import parser as pykmlparser



#OneWeb Talkeetna AL USA
#OneWeb Westcott UK,51.843372,-0.974278,
#OneWeb Gaithersburg MD USA,39.150246,-77.236266
#OneWeb Clewiston FL USA,26.744979,-80.941665


#
def generate_starlink_gateways_data(kml_filepath: str, output_gs_filepath: str):
    with open(kml_file, "r") as f:
        doc = pykmlparser.parse(f).getroot()


    with open(output_gs_filepath, "w") as f:
        for gs in doc.findall(".//{http://www.opengis.net/kml/2.2}Placemark"):
            
            
            gs_name = gs.name.replace(",", " ")
            gs_lonlatalt = (gs.Point.coordinates.text.split(","))
            gs_lat = gs_lonlatalt[1]
            gs_lon = gs_lonlatalt[0]
            gs_alt = gs_lonlatalt[2]

            f.write(f"{gs_name},{gs_lat},{gs_lon},{gs_alt},25,5.36Gbps,1")

    


if __name__ == "__main__":

    generate_starlink_gateways_data("data/groundstations/USgateways.kml", "US-starlink-gateways.GS")
