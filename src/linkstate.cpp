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

/// initial topology 
unordered_map<int, unordered_map<int, int> > topology;

/// forwarding table
unordered_map<int, unordered_map<int, pair<int, int> > > forwarding_table; // src, des, nexthop, cost

set<int> nodes;

//reading from the topology file and saving to the address of topology
void readTopology(string filename, unordered_map<int, unordered_map<int, int> > &topology){
	ifstream topologyFile;
	topologyFile.open(filename); 
	if (!topologyFile) {
        cerr << "Error: Unable to open file!\n";
		exit(0);
    }

	int source, destination, cost;

	while (topologyFile >> source >> destination >> cost) {
		topology[source][destination] = cost;
		topology[destination][source] = cost;

		// Updates set of nodes
		if (nodes.find(source) == nodes.end()) {
			nodes.insert(source);
		}

		if (nodes.find(destination) == nodes.end()) {
			nodes.insert(destination);
		}
	}

	//closing the topology file
	topologyFile.close();
}

// Run Djikestras shortest path algorithm to update the paths for every node using the unordered graph
// Populate the forwarding table by running djikestras at every source node
void runAlgorithm(unordered_map<int, unordered_map<int, int> > &topology, unordered_map<int, unordered_map<int, pair<int, int> > > &forwarding_table){
	

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

}
