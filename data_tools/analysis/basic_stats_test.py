import sys
# try:
from core import basic_network_stats
# except ImportError:
    

    


if __name__ == "__main__":
    # basic_stats("/home/tieg/plathon/ns3-constellations/ns3constellation/orbital-tests/analysis/leo-satellite-example-5-4-2000.flowmon")
    # basic_network_stats("/home/tieg/plathon/ns3-constellations/ns3constellation/case-studies/testcase2/results-basic-satellite-constellation-example-ONEWEB-1623279541_2215083.flowmon")
# 

    basic_network_stats( sys.argv[1] )