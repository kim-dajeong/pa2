/**
 *  @file linkstate.cpp
 *  @brief Uses linkstate routing protocol to update a forwarding table. 
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
    /// Iterate over each node
    for (int source : nodes) {
        /// Iterate over each destination
        for (int destination : nodes) {
            /// If the source is equal to the destination set the cost to be 0 and next hop to be the node itself 
            if (source == destination) {
                forwarding_table[source][destination] = make_pair(0, destination); // Set direct connection cost to 0 and next hop to destination
            } 
            /// Otherwise check if a direct connection to a neighbour exists. If not set the cost to infinity and the next hop to -999 (disconnected)
            else {
                if (!connectionExists(source, destination, topology)) {
                    forwarding_table[source][destination] = make_pair(inf, -999); // Set non-existent connection cost to infinity and next hop to -999
                } 
                /// Update the forwarding table for the direct neighbours 
                else {
                    forwarding_table[source][destination] = make_pair(topology[source][destination], destination); // Give cost to nodes with a connection and set next hop
                }
            }
        }
    }
}

/**
 * @brief Running a decentralized dijkstra algorithm as the main part of calculating the routing table for a linkstate protocol 
 * 
 * @param topology The total topology of the system collected from the topologyFile 
 * @param forwarding_table Forwarding table data structure, The forwarding table is as follows: [source][destination] = pair(cost, next hop) 
 * @param nodes A set of all possible nodes in the topology 
 * 
 * @return void 
 * 
*/
void decentralizedDijkstra(unordered_map<int, unordered_map<int, int>> &topology,
                           unordered_map<int, unordered_map<int, pair<int, int>>> &forwarding_table,
                           set<int> &nodes) {

    for (int source : nodes) {
        /// Initialize cost, visited, and next_hop arrays
        unordered_map<int, int> costs;
        unordered_map<int, bool> visited;
        unordered_map<int, int> next_hop;

        /// Repeat through all nodes in topology and initialize cost, visited, and next_hop
        for (int node : nodes) {
            costs[node] = INT_MAX;
            visited[node] = false;
            next_hop[node] = -1;

            /// Check if path is leading back to source and set cost to 0
            if(node == source){
                /// Initialize cost of source node
                forwarding_table[source][node] = make_pair(0, source);
                costs[source] = 0;
                next_hop[source] = source; 
            }
        }

        for (int count = 0; count < nodes.size(); ++count) {
            /// Find the node with the minimum cost that has not been visited
            int min_cost = INT_MAX;
            int min_index = -1;
            /// Look for lowest cost path and update
            for (int v : nodes) {
                if (!visited[v] && costs[v] < min_cost) {
                    /// Update new minimum cost and node if found
                    min_cost = costs[v];
                    min_index = v;
                }
            }

            /// Mark the selected node as visited
            visited[min_index] = true;

            /// Update costs and next hops of neighboring nodes (relaxation step)
            for (auto& neighbor : topology[min_index]) {
                int v = neighbor.first;
                int cost = neighbor.second;
                /// check if node has been visited
                if (!visited[v]) {
                    if (min_cost + cost < costs[v]) {
                        /// Update cost to node if lower than current min
                        costs[v] = min_cost + cost;
                        next_hop[v] = (min_index == source) ? v : next_hop[min_index];
                        /// Update new next hop and cost on forwarding table
                        forwarding_table[source][v] = {costs[v], next_hop[v]};
                    }
                }
            }
        }
    }
}


/**
 * @brief message function to output messages to the output file
 * 
 * @param filename The filename of messageFile (i.e. messageFile.txt)
 * @param outFile An access point to outFile so this function can write to output.txt
 * @param forwarding_table The complete forwarding table unordered map in form [source][destination] = pair(cost, next hop)  
 * 
 * @return void
 * 
*/
void message(string filename, 
            ofstream &outFile, 
            unordered_map<int, unordered_map<int, pair<int, int>>> &forwarding_table){
    /// File input for the messageFile
    ifstream messageFile;
	messageFile.open(filename); 
	if (!messageFile) {
        cerr << "Error: Unable to open messageFile!\n";
		exit(0);
    }

    /// Initializing variables
    int source, destination, cost, hops;
    string line;

    /// While there lines of data in messagefile 
    while(getline(messageFile, line)){
        /// First two values in the line are source and destination respectively 
        stringstream ss(line);
        if (!(ss >> source >> destination)) {
            cerr << "Error reading source and destination!\n";
            continue; // Skip to next iteration
        }

        /// The rest of the values in the line are the message string. Save that value to text.
        string text;
        getline(ss, text);
        
        /// Set values for cost and hops
        cost = forwarding_table[source][destination].first;
        hops = source; 
        
        /// If the cost is -999 therre is no way to access that node so return infinite hops
        if(cost == -999){
            outFile << "from " << source << " to " << destination << " cost infinite hops unreachable message" << text << endl;
        }
        else{
            /// Write data to file 
            outFile << "from " << source << " to " << destination << " cost " << cost << " hops " ; 
        
            /// Keep track of the intermediate hops taken to reach destination 
            while(hops != destination){
                outFile << hops << " "; 
                hops = forwarding_table[hops][destination].second;
            }
        
            /// Write the output message to the output.txt file 
            outFile << "message" << text << endl;
        }
    }
    messageFile.close(); /// Close file after reading
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

	/// Read from topology file and save to data structure
	readTopology(topologyfile, topology, nodes);

    /// Run dijkstra's algorithm on all nodes
    decentralizedDijkstra(topology, forwarding_table, nodes);

	/// Output forwarding tables to files
    ofstream outFile("output.txt");
    if (!outFile.is_open()) {
        cerr << "Unable to open file for writing!" << endl;
        return -1;
    }

    /// Sort the entries in forwarding_table by source and destination
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

            // write to outFile << "Node:" << source << " " << destination << " " << cost << " " << next_hop << endl;
        	outFile << destination << " " << next_hop << " " << cost << endl;
    	}
	}

    /// send message
    message(messagefile, outFile, forwarding_table);

    /// apply changes
    ifstream changefile;
	changefile.open(changesfile); 

    /// Check if file was opened correctly
	if (!changefile) {
        cerr << "Error: Unable to open file!\n";
		exit(0);
    }
    /// Apply new changes to topology
    int newSource, newDestination, newCost;
    while (changefile >> newSource >> newDestination >> newCost){
        if(newCost == -999){
        /// Check if connection still remains
        topology[newSource].erase(newDestination);
        topology[newDestination].erase(newSource);
        }
        else {
        /// Update link cost to new cost from changes
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
        
        /// Run Diijkstra's algorithm after changing topology
        decentralizedDijkstra(topology, forwarding_table, nodes);

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

    /// Send messages and write output to outputFile
    message(messagefile, outFile, forwarding_table);

    }


    outFile.close();
    return 0;
}
