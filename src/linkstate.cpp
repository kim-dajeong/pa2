/**
 * @file linkstate.cpp
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
#include <sstream>
#include <unordered_set>
#include <queue>
#include <limits>


using namespace std;

/* Algorithm:
	- Read from topology file
	- Place into a datastructure
	- Use an algorithm to get information out of the datastructure to place in forwarding table
	- Write forwarding table output of each node to a file
	- Implement changes to the existing topology

	Link State Flooding == A node has access to the topology file.
		- Run djikestras on each node. 
			-> On event of a change 

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


// Run Djikestras shortest path algorithm to update the paths for every node using the unordered graph
// Populate the forwarding table by running djikestras at every source node
void runAlgorithm(unordered_map<int, unordered_map<int, int> > &topology, unordered_map<int, unordered_map<int, pair<int, int> > > &routing_table, set<int>&nodes){
	
	int numNodes = nodes.size();
	unordered_map<int, int> routing_table;

	for (int n; n < numNodes - 1; n++) {

		for (int node : nodes) {

		dijkstra(topology, routing_table, node);

		}

	}
	
}

// Custom comparator for priority queue
struct CompareDist {
    bool operator()(const pair<int, int>& lhs, const pair<int, int>& rhs) const {
        return lhs.second > rhs.second; // Compare distances
    }
};


/// Dijkstra's Algorithm
void dijkstra(unordered_map<int, unordered_map<int, int> > &topology, unordered_map<int, int> &routing_table, int start){

	unordered_map<int, int> dist;
    unordered_map<int, int> prev;
    priority_queue<pair<int, int>, vector<pair<int, int>>, CompareDist> pq;

    /// Initialize distances
    for (const auto& pair : topology) {

        int node = pair.first;
        dist[node] = numeric_limits<int>::max();
        prev[node] = -1;

    }

    dist[start] = 0;
    pq.push({start, 0});

    while (!pq.empty()) {

        int u = pq.top().first;
        int u_dist = pq.top().second;
        pq.pop();

        /// Skip if there already exists a better path
        if (u_dist > dist[u]) continue;

        for (const auto& neighbor : topology.at(u)) {

            int v = neighbor.first;
            int weight = neighbor.second;

            int alt = dist[u] + weight;

            if (alt < dist[v]) {

                dist[v] = alt;
                prev[v] = u;
                pq.push({v, alt});

            }
        }
    }

    /// Store shortest distances in the routing_table for the node
    routing_table = dist;

}

/// Converge topology
void converge(int changedsrc, int changeddest, unordered_map<int, unordered_map<int, unordered_map<int, int> > &topologyPerNode) {



}


/// apply changes from changesfile and redo calculations
void applyChanges(string changesfile, unordered_map<int, unordered_map<int, unordered_map<int, int> > &topologyPerNode) {

	ifstream change(changesfile);

    int src, dest, cost;

    if (change.is_open()) {

        while (change >> src >> dest >> cost) {

            topology[src][dest] = cost;
            topology[dest][src] = cost;

            converge(src, dest, topologyPerNode);

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
	
	/// Testing ReadTopology it works fine!
	/*
	readTopology("topologyFile.txt", topology);
	
	cout << "Topology:" << endl;
    for (auto& node : topology) {
        cout << "Node " << node.first << ":" << endl;
        for (auto& neighbor : node.second) {
            cout << "  -> Node " << neighbor.first << ", Weight: " << neighbor.second << endl;
        }
    }
	*/
	
	if (argc != 4) {
        printf("Usage: ./linkstate topofile messagefile changesfile\n");
        return -1;
    }

	string topologyfile = argv[1];
    string messagefile = argv[2];
    string changesfile = argv[3];

	/// initial topology 
	unordered_map<int, unordered_map<int, int> > topology;

	/// forwarding table
	unordered_map<int, unordered_map<int, pair<int, int> > > routing_table; // src, des, nexthop, cost

	unordered_map<int, unordered_map<int, unordered_map<int, int> > > topologyPerNode;

	set<int> nodes;

	/// Save initial topology to unordered_map
	readTopology(topologyfile, topology, nodes);

	/// Run Dijkstra's algorithm on all nodes and get forwarding tables
	runAlgorithm(topology, routing_table, nodes);

	/// Get topology changes
	applyChanges(changesfile, topologyPerNode);

	/// Save output

}
