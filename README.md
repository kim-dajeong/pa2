# pa2

How to Run 
 - Use the makefile and run Make all
 - Alternatively use Make dvr or Make lsr to build separately

**An Overview of Our Approach**

distance-vector routing

1. Read topology from provided topology.txt
2. Populate forwarding tables using decentralized Bellman-Ford algorithm
3. Write resulting forwarding tables to output file
4. Sort the entries in forwarding_table by source and destination
5. Send message from source node to destination node by travelling through hops as given by forwarding table
6. Write resulting hops to output file together with source and destination nodes and the message sent
6. Read and apply changes from changes file and update topology
7. Populate forwarding tables using decentralized Bellman-Ford algorithm with updated topology
8. Send new message and write to output file, repeat until no new changes are left

link-state routing

1. Read topology from provided topology.txt
2. Save initial topology at each node
3. Populate routing tables using decentralized Dijkstra's algorithm
4. Create forwarding tables by with nexthop and cost information
5. Send message from source node to destination node by travelling through hops as given by forwarding table
6. Write resulting hops to output file together with source and destination nodes and the message sent
6. Read and apply changes from changes file and update topology
7. Populate forwarding tables using decentralized Bellman-Ford algorithm with updated topology
8. Send new message and write to output file, repeat until no new changes are left