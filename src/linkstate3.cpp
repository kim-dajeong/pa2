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
#include <limits> 
#include <utility> 
#include <iostream>
#include <unordered_map>
#include <queue>
#include <vector>
#include <climits>

using namespace std;

#define inf 2000000000

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
// Function to setup initial forwarding tables
void tableSetup(unordered_map<int, unordered_map<int, int>> &topology, unordered_map<int, unordered_map<int, pair<int, int>>> &forwarding_table, set<int> &nodes) {
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


using namespace std;

void dijkstra(unordered_map<int, unordered_map<int, int>> &topology,
              unordered_map<int, unordered_map<int, pair<int, int>>> &forwarding_table,
              set<int> &nodes,
              int source) {
    // Initialize distance map
    unordered_map<int, int> distance;
    for (int node : nodes)
        distance[node] = (node == source) ? 0 : INT_MAX;

    // Priority queue for selecting next node to explore
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;
    pq.push({0, source});

    while (!pq.empty()) {
        int node = pq.top().second;
        int dist = pq.top().first;
        pq.pop();

        // Iterate over neighbors of the current node
        for (auto &[neighbor, cost] : topology[node]) {
            if (distance[node] != INT_MAX && distance[node] + cost < distance[neighbor]) {
                distance[neighbor] = distance[node] + cost;
                forwarding_table[source][neighbor] = make_pair(distance[neighbor], node);
                pq.push({distance[neighbor], neighbor});
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
    for (int start : nodes) {
        dijkstra(topology, forwarding_table, nodes, start);
    }
    
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
        
    for (int start : nodes) {
        dijkstra(topology, forwarding_table, nodes, start);
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

        message(messagefile, outFile, forwarding_table);

    }


    outFile.close();
    return 0;
}
