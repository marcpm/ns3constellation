from pykml import parser as pykmlparser



#OneWeb Talkeetna AL USA
#OneWeb Westcott UK,51.843372,-0.974278,
#OneWeb Gaithersburg MD USA,39.150246,-77.236266
#OneWeb Clewiston FL USA,26.744979,-80.941665


#
def generate_starlink_gateways_data(kml_filepath: str, output_gs_filepath: str):
    with open(kml_filepath, "r") as f:
        doc = pykmlparser.parse(f).getroot()


    with open(output_gs_filepath, "w") as f:
        for idx, gs in enumerate(doc.findall(".//{http://www.opengis.net/kml/2.2}Placemark")):
            
            
            gs_name = str(gs.name).replace(",", " ")
            gs_lonlatalt = (gs.Point.coordinates.text.split(","))
            gs_lat = gs_lonlatalt[1].strip()
            gs_lon = gs_lonlatalt[0].strip()
            gs_alt = gs_lonlatalt[2].strip()

            # if idx % 10 == 0:
            #     print()
            #     print(type(gs_lat))
            #     print(gs_lon)
            #     print()

            #TODO  EXTRA ANTENNA SPECS -- future use
            # gs_specs = gs.description.text.split("<br>")
            # gs_num_antennas = gs_specs[0].replace("Antenna count: ", "")
            # antenna_diameter = gs_specs[1].replace("Antenna diameter: ", "")
            # gs_antenna_diameter = 0.3048 * float(antenna_diameter.replace(" feet", "")) if "feet" in antenna_diameter else float(antenna_diameter.replace(" meters", ""))
            # gs_uplink = gs_specs[3]
            # gs_downlink = gs_specs[4]
            # gs_fcc_link = gs_specs[6]
            # gs_date = gs_specs[5]

            f.write(f"{idx},{gs_name},{gs_lat},{gs_lon},{gs_alt},25,5.36Gbps,1\n")

    print (f"File written successfully at {output_gs_filepath}.")


if __name__ == "__main__":

    generate_starlink_gateways_data("examples/US-Kagateways.kml", "US-starlink-gateways.GS")
