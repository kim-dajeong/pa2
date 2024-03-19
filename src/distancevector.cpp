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

using namespace std;

/* Algorithm:
	- Read from topology file
		- Place into a datastructure
	- Use an algorithm to get information out of the datastructure to place in routing table
	- 
	- Implement changes to the existing topology


*/

/// initial topology 
unordered_map<int, unordered_map<int, int> > topology;

/// routing table
unordered_map<int, unordered_map<int, pair<int, int> > > routing_table; // src, des, nexthop? cost

set<int> nodes;



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
