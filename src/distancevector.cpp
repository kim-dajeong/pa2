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

void getTopology (string topologyFile, unordered_map<int, unordered_map<int, int> > &topology) {

	// Declare an input file stream
    ifstream globalTopology(topologyFile); 

    // Check if the file is opened successfully
    if (!globalTopology) {
        cerr << "Error: Unable to open file!" << endl;
        cout << "open file error!" << endl;
    }

	    int src, dest, cost;

		while (globalTopology >> src >> dest >> cost) {

			topology[src][dest] = cost;
			topology[dest][src] = cost;

			if (nodes.find(src) == nodes.end()) {
				nodes.insert(src);
			}

			if (nodes.find(dest) == nodes.end()) {
				nodes.insert(dest);
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
