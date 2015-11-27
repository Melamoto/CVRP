#include "tabu.h"
#include "helpers.h"
#include "savings.h"
#include <algorithm>

using namespace std;
using namespace cvrp;

vector<node> tabu::geniType1(const vector<node>& initial, node newNode, size_t i, size_t j, size_t k){
    vector<node> result;
    result.push_back(initial[i]);
    result.push_back(newNode);
    result.push_back(initial[j]);
    // Reverse (i+1,...,j)
    size_t next = cyclePrev(initial.size(), j);
    while (next != i) {
        result.push_back(initial[next]);
        next = cyclePrev(initial.size(), next);
    }
    // Reverse (j+1,...,k)
    result.push_back(initial[k]);
    next = cyclePrev(initial.size(), k);
    while (next != j) {
        result.push_back(initial[next]);
        next = cyclePrev(initial.size(), next);
    }
    next = cycleNext(initial.size(), k);
    while (next != i) {
        result.push_back(initial[next]);
        next = cycleNext(initial.size(), next);
    }
    return result;
}
vector<node> tabu::geniType2(const vector<node>& initial, node newNode, size_t i, size_t j, size_t k, size_t l){
    vector<node> result;
    result.push_back(initial[i]);
    result.push_back(newNode);
    result.push_back(initial[j]);
    size_t prevL = cyclePrev(initial.size(), l);
    // Reverse (j,...,l)
    size_t next = cyclePrev(initial.size(), j);
    while (next != prevL){
        result.push_back(initial[next]);
        next = cyclePrev(initial.size(), next);
    }
    next = cycleNext(initial.size(), j);
    while (next != k){
        result.push_back(initial[next]);
        next = cycleNext(initial.size(), next);
    }
    // Reverse (i+1,...,l-1)
    next = prevL;
    while (next != i){
        result.push_back(initial[next]);
        next = cyclePrev(initial.size(), next);
    }
    next = k;
    while (next != i){
        result.push_back(initial[next]);
        next = cycleNext(initial.size(), next);
    }
    return result;
}

vector<node> tabu::geniInsert(const vector<node>& initial, node newNode){
    // Initialize a distance matrix containing square distances between every pair of
    // nodes in the initial set of nodes, preserving indexing
    vector<vector<uint32_t>> distanceMatrix(initial.size(), vector<uint32_t>(initial.size(), 0));
    for (size_t i = 0; i < initial.size()-1; i++){
        for (size_t j = i+1; j < initial.size(); j++){
            uint32_t ijDist = sqrDistance(initial[i], initial[j]);
            distanceMatrix[i][j] = ijDist;
            distanceMatrix[j][i] = ijDist;
        }
    }
    vector<size_t> nodeList;
    for (size_t i = 0; i < initial.size(); i++) nodeList.push_back(i);
    // Find the nearest 'geniNeighbours' nodes to the new node
    vector<node> nearest = initial;
    std::sort(nearest.begin(), nearest.end(), [&](const node& a, const node& b) { return sqrDistance(newNode,a) < sqrDistance(newNode,b); });
    if (nearest.size() > tabu::geniNeighbours){
        nearest.erase(nearest.begin() + tabu::geniNeighbours - 1, nearest.end());
    }
    // Iterate through all possible values for i,j,k,l and save the best solution
    vector<node> bestResult;
    uint64_t bestDistance = numeric_limits<uint64_t>::max();
    /////////
    // I loop
    for (size_t i = 0; i < nearest.size(); i++){
        // Index of i
        size_t iIdx = find_if(initial.begin(), initial.end(),
                              [&](const node& n) { return n.num == nearest[i].num; }) - initial.begin();
        size_t iNext = cycleNext(initial.size(), iIdx);
        /////////
        // J loop
        for (size_t j = 0; j < nearest.size(); j++){
            if (i == j) continue;
            // Index of j
            size_t jIdx = find_if(initial.begin(), initial.end(),
                                  [&](const node& n) { return n.num == nearest[j].num; }) - initial.begin();
            size_t jNext = cycleNext(initial.size(), jIdx);
            vector<size_t> kCandidates = nodeList;
            std::sort(kCandidates.begin(), kCandidates.end(),
                      [&](const size_t& a, const size_t& b) { return distanceMatrix[iNext][a] < distanceMatrix[iNext][b]; });
            /////////
            // K loop
            for (size_t k = 1; k <= tabu::geniNeighbours; k++){
                // Index of k
                size_t kIdx = kCandidates[k];
                if (!cycleBetween(initial.size(), kIdx, jIdx, iIdx)) continue;
                if (kIdx != iIdx){
                    vector<node> trialResult = geniType1(initial, newNode, iIdx, jIdx, kIdx);
                    uint64_t trialDistance = sqrCost(trialResult);
                    if (trialDistance < bestDistance){
                        bestDistance = trialDistance;
                        bestResult = trialResult;
                    }
                }
                vector<size_t> lCandidates = nodeList;
                std::sort(lCandidates.begin(), lCandidates.end(),
                          [&](const size_t& a, const size_t& b) { return distanceMatrix[jNext][a] < distanceMatrix[jNext][b]; });
                /////////
                // L loop
                for (size_t l = 1; l <= tabu::geniNeighbours; l++){
                    // Index of l
                    size_t lIdx = lCandidates[l];
                    if (!cycleBetween(initial.size(), lIdx, iIdx, kIdx)) continue;
                    if (kIdx != jNext && lIdx != iNext){
                        vector<node> trialResult = geniType2(initial, newNode, iIdx, jIdx, kIdx, lIdx);
                        uint64_t trialDistance = sqrCost(trialResult);
                        if (trialDistance < bestDistance){
                            bestDistance = trialDistance;
                            bestResult = trialResult;
                        }
                    }
                }
            }
        }
    }
    return bestResult;
}

// Tabu Search not currently implemented
solution tabu::search(solution initial, vector<uint16_t> movableNodes, uint16_t selectionCount, size_t iterations){
    return initial;
}

solution tabu::taburoute(vector<node> nodes, uint16_t vehicleCapacity, default_random_engine& rng){
    vector<saving> savings = calculateSavings(nodes);
    // Stage 1: Calculate initial heuristic estimate
    solution solution = calculateClarkeWrightSolution(nodes, savings, vehicleCapacity);
    // Stage 2: Improve initial estimate with tabu search
    vector<uint16_t> movableNodes;
    for (uint16_t i = 2; i <= nodes.size(); i++) movableNodes.push_back(i);
    uint16_t selectionCount = 5 * solution.vehicles.size();
    solution = tabu::search(solution, movableNodes, selectionCount, 50*nodes.size());
    return solution;
}
