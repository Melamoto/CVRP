#include "savings.h"
#include <stdint.h>
#include <vector>
#include <algorithm>
#include "cvrp.h"
#include <iostream>

using namespace std;
using namespace cvrp;

saving::saving(node depot, node a, node b){
    nodeA = a.num;
    nodeB = b.num;
    double aDist = cvrp::distance(depot, a);
    double bDist = cvrp::distance(depot, b);
    double abDist = cvrp::distance(a, b);
    saved = aDist + bDist - abDist;
}

// Returns the complete list of savings between the nodes in 'nodes', sorted
// by saving value ascending (the last element has the greatest saving)
// Discounts the first node as the depot
vector<saving> calculateSavings(const vector<node> nodes){
    vector<saving> savings;
    if (nodes.size() < 2) return savings;
    savings.reserve(((nodes.size()-1)*(nodes.size()-2))/2);
    for (size_t i = 1; i < nodes.size() - 1; i++){
        for (size_t j = i + 1; j < nodes.size(); j++){
            saving ijSaving(nodes[0], nodes[i], nodes[j]);
            vector<saving>::iterator pos = lower_bound(savings.begin(), savings.end(), ijSaving);
            savings.insert(pos, ijSaving);
        }
    }
    return savings;
}
