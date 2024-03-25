/**
 *  @file distancevector.cpp
 *  @brief Uses distance vector routing protocol to update a forwarding table. 
 *
 *  @author Ana Bandari (anabandari)
 *  @author Dajeong Kim (dkim2)
 * 
 *  @bug None known at this time. 
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
#include <limits> 
#include <utility> 

using namespace std;

/// Setting infinity to be some arbitrarily large number 
#define inf 2000000000

/**
 * @brief Reads from the topology file and writes it to an unordered map and a set of nodes for easy reference
 * 
 * @param filename The name of the topology file i.e. "topologyFile.txt"
 * @param topology Topology map, an unordered map containing information from the topologyFile for easy reference
 * @param nodes A set of all the nodes present in the network
 * 
 * @return void
 * 
*/
void readTopology(string filename, unordered_map<int, unordered_map<int, int>>&topology, set<int>&nodes){
    /// File input setup
	ifstream topologyFile;
	topologyFile.open(filename); 
	if (!topologyFile) {
        cerr << "Error: Unable to open file!\n";
		exit(0);
    }

    /// Initializing variable names
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


/**
 * @brief Function to check if a direct connection between source and destination exists 
 * 
 * @param source Value of source node
 * @param destination  Value of destination node 
 * @param topology Topology map
 * 
 * @return boolean true or false 
 * 
*/
bool connectionExists(int source, int destination, unordered_map<int, unordered_map<int, int>>&topology){
    /// Find source in the topology
    auto i = topology.find(source);
    
    /// If source not found return false 
    if (i != topology.end()) {
        /// Find destination in the topology
        auto j = i->second.find(destination);
        /// If destination not found return false 
        if (j != i->second.end()){
            /// Source and destination have a direct connection 
            return true;
        }
    }
    return false; 
}

/**
 * @brief Initialize the forwarding table with known information from the topology file
 * 
 * @param topology Topology map [source][destination] = cost
 * @param forwarding_table Forwarding table [source][destination] = pair(cost, next hop)
 * @param nodes Set of all nodes active in the topology
 * 
 * @return void
 * 
*/
void tableSetup(unordered_map<int, unordered_map<int, int>> &topology, 
                unordered_map<int, unordered_map<int, pair<int, int>>> &forwarding_table, 
                set<int> &nodes) {
    // Iterate over each node
    for (int source : nodes) {
        for (int destination : nodes) {
            if (source == destination) {
                forwarding_table[source][destination] = make_pair(0, destination); // Set direct connection cost to 0 and next hop to destination
            } else {
                if (!connectionExists(source, destination, topology)) {
                    forwarding_table[source][destination] = make_pair(inf, -999); // Set non-existent connection cost to infinity and next hop to -999
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
	for(int i; i < nodes.size(); i++){
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

            if(node != destination){
            // Update forwarding table entry
            forwarding_table[node][destination] = make_pair(cost, nexthop);
            }
        }
    }
	}
}

// FILL OUT LATER!!!!!!!!
/**
 * @brief main function 
 * 
 * @param arcg argument count, indicating number of arguments passed from the command line
 * @param argv argument vector, pointing to an array of strings, each of which contains an argument passed from the command line
 * 
 * @return 0
 * 
*/
void message(string filename, 
            ofstream &outFile, 
            unordered_map<int, unordered_map<int, pair<int, int>>> &forwarding_table){
    
    ifstream messageFile;
	messageFile.open(filename); 
	if (!messageFile) {
        cerr << "Error: Unable to open messageFile!\n";
		exit(0);
    }

    int source, destination, cost, hops;
    string line;
    while(getline(messageFile, line)){
        stringstream ss(line);
        if (!(ss >> source >> destination)) {
            cerr << "Error reading source and destination!\n";
            continue; // Skip to next iteration
        }
        string text;
        getline(ss, text);
        
        cost = forwarding_table[source][destination].first;
        hops = source; 
        
        if(cost == -999){
            outFile << "from " << source << " to " << destination << " cost infinite hops unreachable message" << text << endl;
        }
        
        //implement hops!

        outFile << "from " << source << " to " << destination << " cost " << cost << " hops " ; 
        
        while(hops != destination){
            outFile << hops << " "; 
            hops = forwarding_table[hops][destination].second;
        }
        
        outFile << "message" << text << endl;
        
    }
    messageFile.close(); // Close file after reading
}


/**
 * @brief main function 
 * 
 * @param argc argument count, indicating number of arguments passed from the command line
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
    ofstream outFile("output.txt");
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

            // debug outFile << "Node:" << source << " " << destination << " " << cost << " " << next_hop << endl;
        	outFile << destination << " " << next_hop << " " << cost << endl;
    	}
	}

    /// send message
    message(messagefile, outFile, forwarding_table);

    /// apply changes
    ifstream changefile;

	changefile.open(changesfile); 

	if (!changefile) {
        cerr << "Error: Unable to open file!\n";
		exit(0);
    }
    int newSource, newDestination, newCost;
    while (changefile >> newSource >> newDestination >> newCost){
        if(newCost == -999){
        topology[newSource].erase(newDestination);
        topology[newDestination].erase(newSource);
        }
        else {
        topology[newSource][newDestination] = newCost;
        topology[newDestination][newSource] = newCost;
        }
        /// Check if source node exists in set
		if (nodes.find(newSource) == nodes.end()) {
			/// If it doesn't exist, insert into ordered set
			nodes.insert(newSource);
		}

		/// Check if destination node exists in set
		if (nodes.find(newDestination) == nodes.end()) {
			/// If it doesn't exist, insert into ordered set
			nodes.insert(newDestination);
		}
        
        decentralizedBellmanFord(topology, forwarding_table, nodes);

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

                // debug outFile << "Node:" << source << " " << destination << " " << cost << " " << next_hop << endl;
        	    outFile << destination << " " << next_hop << " " << cost << endl;
    	    }
	    }

        message(messagefile, outFile, forwarding_table);

    }


    outFile.close();
    return 0;
}
