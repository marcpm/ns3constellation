import xml.etree.ElementTree as et

import xmltodict

class Scenario:
    pass

def basic_stats(flowmon_filepath: str, simulation_scenario: Scenario=None, verbose=True) -> None:
    with open(flowmon_filepath, "r") as xmlfile:
        # xml = et.parse(xmlfile.read())
        xml = xmltodict.parse(xmlfile.read())

    flowdata = xml["FlowMonitor"]["FlowStats"]["Flow"]

    rtt_avg = 0.0
    n_hops_avg = 0.0
    n_lostpackets_avg = 0.0

    n_flows = len(flowdata)
    for flow in flowdata:
        n_packets = int(flow["@txPackets"])
        
        rtt_avg += float(flow["@delaySum"][:-2])*1e-9 / n_packets * 2 / n_flows  # extract|convert nanoseconds unit from string, n_packets *2 == return trip
        n_hops_avg += float(flow["@timesForwarded"]) / n_packets * 2 / n_flows # return trip
        n_lostpackets_avg += float(flow["@lostPackets"]) /n_packets * 2 / n_flows if float(flow["@lostPackets"]) != 0.0 else 0.0

    if verbose:
        print()
        print("**********************")
        print(f"RTT (avg): {rtt_avg} [s] \nNum Hops (avg) : {n_hops_avg} \nLost Packets (avg): {n_lostpackets_avg}")
        print("**********************")
        print()

    return {"rtt": rtt_avg,
            "hops": n_hops_avg,
            "lostpackets": n_lostpackets_avg}
