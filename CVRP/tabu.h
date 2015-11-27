#pragma once

#include <random>
#include "cvrp.h"

namespace cvrp{

    namespace tabu{

        const size_t tabuDurationMin = 5;
        const size_t tabuDurationMax = 10;

        const double tabuPenaltyScaling = 0.01;

        const size_t feasibilityModTime = 10;

        const size_t geniNeighbours = 5;

        vector<node> geniType1(const vector<node>& initial, node newNode, size_t i, size_t j, size_t k);
        vector<node> geniType2(const vector<node>& initial, node newNode, size_t i, size_t j, size_t k, size_t l);

        vector<node> geniInsert(const vector<node>& initial, node newNode);

        solution search(solution initial, vector<uint16_t> movableNodes, uint16_t selectionCount, size_t iterations);
        
        solution taburoute(vector<node> nodes, uint16_t vehicleCapacity, default_random_engine& rng);

    }

}
