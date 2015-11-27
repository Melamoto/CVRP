#pragma once

#include <array>
#include "cvrp.h"
#include "savings.h"

using namespace cvrp;
using namespace std;

// Given the set of nodes (with the first node being the depot) and their corresponding savings,
// returns the heuristic solution given by the Clarke-Wright algorithm
solution calculateClarkeWrightSolution(const vector<node>& nodes, vector<saving> savings, uint16_t vehicleCapacity);
