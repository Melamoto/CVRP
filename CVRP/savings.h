#pragma once

#include <stdint.h>
#include <vector>
#include <algorithm>
#include "cvrp.h"

using namespace cvrp;

class saving{
public:
    saving(node depot, node a, node b);
    bool contains(uint16_t n) const { return n == nodeA || n == nodeB; }
    uint16_t nodeA;
    uint16_t nodeB;
    double saved;
};

inline bool operator< (const saving& lhs, const saving& rhs){ return lhs.saved < rhs.saved; }
inline bool operator> (const saving& lhs, const saving& rhs){ return rhs < lhs; }
inline bool operator<=(const saving& lhs, const saving& rhs){ return !(lhs > rhs); }
inline bool operator>=(const saving& lhs, const saving& rhs){ return !(lhs < rhs); }

// Returns the complete list of savings between the nodes in 'nodes', sorted
// by saving value ascending (the last element has the greatest saving)
// Discounts the first node as the depot
vector<saving> calculateSavings(const vector<node> nodes);

// Given the set of nodes (with the first node being the depot) and their corresponding savings,
// returns the heuristic solution given by the Clarke-Wright algorithm
solution calculateClarkeWrightSolution(const vector<node>& nodes, vector<saving> savings, uint16_t vehicleCapacity);

