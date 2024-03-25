/**
 * @file distancevector.cpp
 *  @brief 
 *
 *  @author Ana Bandari (anabandari)
 *  @author Dajeong Kim (dkim2)
 * 
 *  @bug 
 * 
 */


/*   Includes   */
#include <iostream>
#include <unordered_map>
#include <fstream>
#include <set>
#include <vector>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <limits> // for INT_MAX
#include <utility> // for std::pair

using namespace std;

#define inf 20000

/* Algorithm:
	- Read from topology file
	- Place into a datastructure
	- only read from forwarding tables of neighbours and cycle through them multiple times
	- Use an algorithm to get information out of the datastructure to place in forwarding table
	- Implement changes to the existing topology
*/

/**
 * @brief main function 
 * 
 * @param 
 * @param 
 * 
 * @return 0
 * 
*/
/// Read from the topology file and save information to unordered_map topology and ordered_set nodes
void readTopology(string filename, unordered_map<int, unordered_map<int, int>>&topology, set<int>&nodes){

	ifstream topologyFile;

	topologyFile.open(filename); 

	if (!topologyFile) {
        cerr << "Error: Unable to open file!\n";
		exit(0);
    }

	int source, destination, cost;

	/// Iterate through all lines in topologyFile and read for source, destination, and cost
	while (topologyFile >> source >> destination >> cost) {

		/// Update new topology connection and cost in both directions
		topology[source][destination] = cost;
		topology[destination][source] = cost;

		/// Check if source node exists in set
		if (nodes.find(source) == nodes.end()) {

			/// If it doesn't exist, insert into ordered set
			nodes.insert(source);
		}

		/// Check if destination node exists in set
		if (nodes.find(destination) == nodes.end()) {

			/// If it doesn't exist, insert into ordered set
			nodes.insert(destination);

		}
	}

	/// Close the file
	topologyFile.close();

}

// Function to check if a connection exists in the topology
bool connectionExists(int source, int destination, unordered_map<int, unordered_map<int, int>>&topology) {
    auto i = topology.find(source);
    if (i != topology.end()) {
        auto j = i->second.find(destination);
        if (j != i->second.end()) {
            return true; // Connection exists
        }
    }
    return false; // Connection does not exist
}


// Function to setup initial forwarding tables
void tableSetup(unordered_map<int, unordered_map<int, int>> &topology, unordered_map<int, unordered_map<int, pair<int, int>>> &forwarding_table, set<int> &nodes) {
    // Iterate over each node
    for (int source : nodes) {
        for (int destination : nodes) {
            if (source == destination) {
                forwarding_table[source][destination] = make_pair(0, destination); // Set direct connection cost to 0 and next hop to destination
            } else {
                if (!connectionExists(source, destination, topology)) {
                    forwarding_table[source][destination] = make_pair(inf, -999); // Set non-existent connection cost to infinity and next hop to -1
                } else {
                    forwarding_table[source][destination] = make_pair(topology[source][destination], destination); // Give cost to nodes with a connection and set next hop
                }
            }
        }
    }
}

///FILL THIS OUT LATER!!!!! 
/**
 * @brief main function 
 * 
 * @param arcg argument count, indicating number of arguments passed from the command line
 * @param argv argument vector, pointing to an array of strings, each of which contains an argument passed from the command line
 * 
 * @return 0
 * 
*/
// Decentralized Bellman-Ford algorithm
void decentralizedBellmanFord(unordered_map<int, unordered_map<int, int>> &topology,
                              unordered_map<int, unordered_map<int, pair<int, int>>> &forwarding_table,
                              set<int> &nodes) {
    tableSetup(topology, forwarding_table, nodes);
	
	/// Run Bellman Ford Size-1 Times
	for(int i; i < nodes.size()-1; i++){
	// Iterate over each node
    for (int node : nodes) {
        // Iterate over each destination
        for (int destination : nodes) {
            int cost = inf;
            int nexthop = -1;

            // Check if direct link exists from node to destination
            if (topology.find(node) != topology.end() && topology.at(node).find(destination) != topology.at(node).end()) {
                cost = topology.at(node).at(destination);
                nexthop = destination;
            }

            // Iterate over neighbors to find better paths
            for (int neighbor : nodes) {
                if (topology.find(node) != topology.end() && topology.at(node).find(neighbor) != topology.at(node).end()) {
                    // Check if the neighbor has a route to the destination
                    if (forwarding_table.find(neighbor) != forwarding_table.end() &&
                        forwarding_table.at(neighbor).find(destination) != forwarding_table.at(neighbor).end()) {
                        int temp = topology.at(node).at(neighbor) + forwarding_table.at(neighbor).at(destination).first;
                        if (temp < cost) {
                            cost = temp;
                            nexthop = neighbor;
                        }
                    }
                }
            }
            // Update forwarding table entry
            forwarding_table[node][destination] = make_pair(cost, nexthop);
        }
    }
	}
}


/**
 * @brief main function 
 * 
 * @param arcg argument count, indicating number of arguments passed from the command line
 * @param argv argument vector, pointing to an array of strings, each of which contains an argument passed from the command line
 * 
 * @return 0
 * 
*/
int main(int argc, char** argv){
	if (argc != 4) {
        printf("usage: you need more args (try again)\n\n");
        exit(1);
    }

	string topologyfile = argv[1];
    string messagefile = argv[2];
    string changesfile = argv[3];

	// global topology information
	unordered_map<int, unordered_map<int, int> > topology;

	/// forwarding table
	unordered_map<int, unordered_map<int, pair<int, int> > > forwarding_table; // src, des, nexthop? cost

	set<int> nodes;

	//read from topology file
	readTopology(topologyfile, topology, nodes);

	// Populate forwarding tables using Bellman-Ford algorithm
    decentralizedBellmanFord(topology, forwarding_table, nodes);

	 // Output forwarding tables to files
    ofstream outFile("forwarding_table.txt");
    if (!outFile.is_open()) {
        cerr << "Unable to open file for writing!" << endl;
        return -1;
    }

    // Sort the entries in forwarding_table by source and destination
    vector<pair<int, pair<int, int>>> sorted_sources;
    for (auto& entry : forwarding_table) {
        sorted_sources.emplace_back(entry.first, make_pair(0, 0));
    }
    sort(sorted_sources.begin(), sorted_sources.end());

    for (auto& entry : sorted_sources) {
    int source = entry.first;
    	// Iterate over all possible destinations
    	for (int destination : nodes) {
			int cost, next_hop; 
			cost = forwarding_table[source][destination].first;
        	next_hop = forwarding_table[source][destination].second;

        	outFile << "Source: " << source << ", Destination: " << destination
                << ", Cost: " << cost << ", Next hop: " << next_hop << endl;
    	}
	}

    outFile.close();
    return 0;
}
