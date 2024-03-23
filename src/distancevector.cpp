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

// global topology information
unordered_map<int, unordered_map<int, int> > topology;

/// initial topology 
unordered_map<int, unordered_map<int, pair<int, int> > > node_topology;

/// forwarding table
unordered_map<int, unordered_map<int, pair<int, int> > > forwarding_table; // src, des, nexthop? cost

set<int> nodes;

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


void tableSetup(unordered_map<int, unordered_map<int, int>>&topology, unordered_map<int, unordered_map<int, int>>&forwarding_table, set<int>&nodes){
    // Iterate over each node
    for (int source : nodes) {
        for (int destination : nodes) {
            if (source == destination) {
                forwarding_table[source][destination] = 0; // Set direct connection cost to 0
            } else {
                if (!connectionExists(source, destination, topology)) {
                    forwarding_table[source][destination] = inf; // Set non-existent connection cost to infinity
                } else {
					if (connectionExists(source, destination, topology)) {
                    forwarding_table[source][destination] = topology[source][destination]; // Give cost to nodes with a connection
                	}
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
  
}
