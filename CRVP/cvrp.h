#pragma once

#include "chromosome.h"
#include <vector>
#include <random>

using namespace std;

namespace crvp{

    struct node{
        int x;
        int y;
        int demand;
    };

    class chromosome : public chromosomeBase{
    public:
        virtual double calcFitness(){ return 0; }
        virtual bool isValidSolution(){ return true; };
        vector<node> nodes;
        vector<size_t> vehiclePos;
    };

    struct problemParameters{
        int dimension;
        int capacity;
        vector<node> nodes;
    };

    problemParameters getParameters(string filename)
    {
        problemParameters problem;
        ifstream fileIn(filename);
        if (fileIn){
            // Read file
        }
        else{
            throw runtime_error("Filename is invalid.");
        }
        problem.capacity = 0;
        problem.dimension = 0;
        return problem;
    }

    vector<chromosome> initialPopulation(problemParameters& problem, int populationSize, default_random_engine& rng)
    {
        vector<chromosome> population;
        return population;
    }
    const int parentsPerCrossover = 3;
    vector<chromosome> crossover(vector<chromosome>& parents, double crossoverRate, default_random_engine& rng)
    {
        vector<chromosome> children;
        return children;
    }
    chromosome mutate(chromosome base, default_random_engine& rng)
    {
        return base;
    }

}