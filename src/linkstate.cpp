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

/// initial topology 
unordered_map<int, unordered_map<int, int> > topology;

/// forwarding table
unordered_map<int, unordered_map<int, unordered_map<int, pair<int, int> > > > forwardingTable; // node, src, des, nexthop, cost

// topology stored at each node
unordered_map<int, unordered_map<int, unordered_map<int, int> > > nodeTopology; // node, src, des, cost

set<int> nodes;


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

// Custom comparator for priority queue
struct CompareDist {
    bool operator()(const pair<int, int>& lhs, const pair<int, int>& rhs) const {
        return lhs.second > rhs.second; // Compare distances
    }
};


/// Dijkstra's Algorithm
void dijkstra(unordered_map<int, unordered_map<int, int>>& topology,
              unordered_map<int, unordered_map<int, unordered_map<int, pair<int, int>>>>& forwardingTable,
              int start) {

    // Priority queue to store nodes and their distances
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;

    // Initialize distances with infinity
    unordered_map<int, int> dist;

    for (auto& node : topology) {

        dist[node.first] = numeric_limits<int>::max();
    }

    // Set distance of source node to 0
    dist[start] = 0;

    // Push source node into priority queue
    pq.push({0, start});

    while (!pq.empty()) {

        int u = pq.top().second;  // Get the node with the smallest distance
        pq.pop();

        // Iterate through all neighboring nodes of u
        for (auto& neighbor : topology[u]) {

            int v = neighbor.first;  // Neighbor node
            int weight = neighbor.second;  // Edge weight

            // If a shorter path is found
            if (dist[v] > dist[u] + weight) {

                // Update distance
                dist[v] = dist[u] + weight;

                // Update forwarding table
                forwardingTable[v][start][v].first = v;
                forwardingTable[v][start][v].second = dist[v];

                // Push the updated distance and node into the priority queue
                pq.push({dist[v], v});

            }
        }
    }
}

/// Write topology to output file
void writeTopologyToFile(const unordered_map<int, unordered_map<int, int>>& topology, const string& outputfilename) {

    ofstream outputFile(outputfilename);

    if (!outputFile.is_open()) {

        cerr << "Error: Unable to open file " << outputfilename << endl;
        return;

    }

    for (const auto& node : topology) {

        int source = node.first;

        for (const auto& neighbor : node.second) {

            int destination = neighbor.first;
            int cost = neighbor.second;
            outputFile << source << " " << destination << " " << cost << endl;

        }
    }

    outputFile.close();
}


void sendMessage(const string& messageFile, const unordered_map<int, unordered_map<int, unordered_map<int, pair<int, int>>>>& forwardingTable, const string& outputFile) {
    
	ifstream inFile(messageFile);
    ofstream outFile(outputFile);
    
    if (!inFile.is_open()) {
        cerr << "Error: Unable to open input file " << messageFile << endl;
        return;
    }
    if (!outFile.is_open()) {
        cerr << "Error: Unable to open output file " << outputFile << endl;
        inFile.close();
        return;
    }

    string line;
    while (getline(inFile, line)) {
        stringstream ss(line);
        int source, destination;
        string message;
        ss >> source >> destination;
        getline(ss, message);

        // Find next hop and cost from forwarding table
        int current = source;
        int hops = -999;
        int cost = -999;

        vector<int> path;
        path.push_back(source);

        while (current != destination) {
            int nextHop = forwardingTable.at(current).at(source).at(destination).first;
            int edgeCost = forwardingTable.at(current).at(source).at(destination).second;

            cost += edgeCost;
            current = nextHop;
            hops++;

            path.push_back(current);
        }

		if(cost == -999){
            outFile << "from " << source << " to " << destination << " cost infinite hops unreachable message " << message << endl;
            break;
        }

        // Write to output file
        outFile << "from " << source << " to " << destination << " cost " << cost << " hops ";
        for (int i = 1; i < path.size() - 1; ++i) {
            outFile << path[i] << " ";
        }
        outFile << "message " << message << endl;
    }

    inFile.close();
    outFile.close();
}

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
void runAlgorithm(unordered_map<int, unordered_map<int, int> > &topology, unordered_map<int, unordered_map<int, unordered_map<int, pair<int, int> > > > &forwardingTable, set<int>&nodes){
	
	int numNodes = nodes.size();

	for (int n; n < numNodes - 1; n++) {

		for (int node : nodes) {
			int start = node;

			dijkstra(topology, forwardingTable, start);

		}
	}
}




/// apply changes from changesfile and redo calculations
void applyChanges(string changesfile, unordered_map<int, unordered_map<int, int>>& topology, unordered_map<int, unordered_map<int, unordered_map<int, pair<int, int>>>> &forwardingTable, string &messagefile, string& outputfilename) {

	ifstream change(changesfile);
    ofstream outFile(outputfilename);

    int src, dest, cost;

    if (change.is_open()) {

        while (change >> src >> dest >> cost) {

			/// Update topology changes
            topology[src][dest] = cost;
            topology[dest][src] = cost;

			runAlgorithm(topology, forwardingTable, nodes);
			writeTopologyToFile(topology, outputfilename);
			sendMessage(messagefile, forwardingTable, outputfilename);

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

	string outputfilename = "outputfile.txt";



	/// Save initial topology to unordered_map
	readTopology(topologyfile, topology, nodes);

	/// Run Dijkstra's algorithm on all nodes and get forwarding tables
	runAlgorithm(topology, forwardingTable, nodes);

	/// Get topology changes
	applyChanges(changesfile, topology, forwardingTable, messagefile, outputfilename);

	/// Save output


}
