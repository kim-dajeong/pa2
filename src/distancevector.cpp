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

using namespace std;

#define inf INT_MAX

/* Algorithm:
	- Read from topology file
	- Place into a datastructure
	- only read from forwarding tables of neighbours and cycle through them multiple times
	- Use an algorithm to get information out of the datastructure to place in forwarding table
	- Implement changes to the existing topology
*/

// Function to parse a line containing source, destination, and cost
bool parseLine(const string& line, int& src, int& dest, int& cost) {

    istringstream iss(line);

    if (!(iss >> src >> dest >> cost)) {
        return false;  // Parsing failed
    }
	else {
		return true;  // Parsing succeeded
	}

}

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
                    forwarding_table[source][destination] = make_pair(inf, -1); // Set non-existent connection cost to infinity and next hop to -1
                } else {
                    forwarding_table[source][destination] = make_pair(topology[source][destination], destination); // Give cost to nodes with a connection and set next hop
                }
            }
        }
    }
}

// Function to populate forwarding tables using Bellman-Ford algorithm
void populateForwardingTables(unordered_map<int, unordered_map<int, int>> &topology, unordered_map<int, unordered_map<int, pair<int, int>>> &forwarding_table, set<int> &nodes) {
    // Initialize forwarding tables
    tableSetup(topology, forwarding_table, nodes);

    // Run Bellman-Ford algorithm for each node
    for (int k = 0; k < nodes.size(); ++k) {
        for (int source : nodes) {
            for (int destination : nodes) {
                if (topology[source].find(destination) != topology[source].end() && forwarding_table[source][k].first + topology[source][destination] < forwarding_table[source][destination].first) {
                    forwarding_table[source][destination].first = forwarding_table[source][k].first + topology[source][destination];
                    forwarding_table[source][destination].second = forwarding_table[source][k].second; // Update next hop
                }
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
        printf("Usage: ./linkstate topofile messagefile changesfile\n");
        return -1;
    }

	string topologyfile = argv[1];
    string messagefile = argv[2];
    string changesfile = argv[3];

	// global topology information
	unordered_map<int, unordered_map<int, int> > topology;

	/// forwarding table
	unordered_map<int, unordered_map<int, pair<int, int> > > forwarding_table; // src, des, nexthop? cost

	set<int> nodes;

	// Populate forwarding tables using Bellman-Ford algorithm
    populateForwardingTables(topology, forwarding_table, nodes);

    // Output forwarding tables to files
    for (auto& entry : forwarding_table) {
        int source = entry.first;
        ofstream outFile("forwarding_table_" + to_string(source) + ".txt");
        if (outFile.is_open()) {
            for (auto& item : entry.second) {
                outFile << "Destination: " << item.first << ", Cost: " << item.second.first << ", Next hop: " << item.second.second << endl;
            }
            outFile.close();
        } else {
            cerr << "Unable to open file for writing!";
            return 1;
        }
    }

    return 0;

  
}
