#include <iostream>
#include <unordered_map>
#include <queue>
#include <vector>
#include <limits>

using namespace std;

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

int main() {
    // Example usage
    unordered_map<int, unordered_map<int, int>> topology = {
        {1, {{2, 1}, {3, 4}}},
        {2, {{3, 2}}},
        {3, {{4, 7}}},
        {4, {{}}}
    };

    unordered_map<int, unordered_map<int, unordered_map<int, pair<int, int>>>> forwardingTable;

    int startNode = 1;
    dijkstra(topology, forwardingTable, startNode);

// Print the forwarding table
for (auto& node : forwardingTable) {
    cout << "Node: " << node.first << endl;
    for (auto& source : node.second) {
        for (auto& destination : source.second) {
            // Access the pair directly
            cout << "Source: " << source.first << " Destination: " << destination.first;
            cout << " NextHop: " << destination.second.first; // Accessing next hop
            cout << " Cost: " << destination.second.second << endl; // Accessing cost
        }
    }
}




    return 0;
}