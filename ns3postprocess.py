
import sys
import argparse
# try:
from data_tools.analysis.core import basic_network_stats
# except ImportError:
    



if __name__ == "__main__":


    parser = argparse.ArgumentParser()

    parser.add_argument("--flowmon-path", type=str, required=True, help="Path to Flowmon ns3 output file")
    parser.add_argument("--results-file", type=str, default=None, required=False, help="Path to this script's results, if none, will print to terminal stderr only.") # comma sepparated ids, e.g. 2321,12311,12345

    args = parser.parse_args()


    

    basic_network_stats(flowmon_filepath=args.flowmon_path, output_path=args.results_file )