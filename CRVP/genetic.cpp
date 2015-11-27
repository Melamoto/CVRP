#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <random>
#include <chrono>
#include "cvrp.h"
#include "tsp.h"

using namespace std;
using namespace tsp;

struct geneticParameters{
    int maxGenerations;
    unsigned populationSize;
    unsigned breederCount;
    double mutationRate;
    double crossoverRate;
};

const geneticParameters defaultGeneticParams = {
    10000,
    50000,
    15000,
    1.15,
    0.85
};

// Selects a weighted random sample of size 'breederCount' from 'population', using a (pre-seeded) 'rng'
// Uses stochastic universal sampling
vector<chromosome> selection(vector<chromosome>& population, unsigned breederCount, default_random_engine& rng)
{
    vector<chromosome> breeders;
    // Calculate the total fitness, and the cumulative fitness for each specimen in the population (in their current order)
    double totalFitness = 0;
    vector<double> cumulativeFitness;
    for (auto ch : population){
        totalFitness += ch.getFitness();
        cumulativeFitness.push_back(totalFitness);
    }
    // Set the sample spacing such that breederCount breeders will be selected
    double sampleSpacing = totalFitness / (double)breederCount;
    // Select a random sample starting point from the range [0, sampleSpacing)
    uniform_real_distribution<double> prob(0.0, sampleSpacing);
    double sample = prob(rng);
    // Stochastic universal sampling
    unsigned currentSpecimen = 0;
    for (unsigned br = 0; br < breederCount; ++br){
        while (sample > cumulativeFitness[currentSpecimen]) ++currentSpecimen;
        breeders.push_back(population[currentSpecimen]);
        sample += sampleSpacing;
    }
    return breeders;
}

chromosome evolveSolution(shared_ptr<problemParameters>& problem, geneticParameters& gen, default_random_engine& rng)
{
    vector<chromosome> population = initialPopulation(problem, gen.populationSize, rng);
    chromosome bestSolution = population[0];
    for (auto ch : population){
        if (ch.getFitness() > bestSolution.getFitness()) bestSolution = ch;
    }
    uniform_real_distribution<double> percent(0.0, 1.0);
    for (int it = 0; it < gen.maxGenerations; ++it){
        if (it % 100 == 0) cout << it << endl;
        // Selection
        vector<chromosome> breeders = selection(population, gen.breederCount, rng);
        // Crossover
        population.empty();
        while (population.size() < gen.populationSize){
            vector<chromosome> parents;
            vector<size_t> parentIndices;
            while (parents.size() < parentsPerCrossover){
                size_t nextParent = rng() % breeders.size();
                if (find(parentIndices.begin(), parentIndices.end(), nextParent) != parentIndices.end()) continue;
                parentIndices.push_back(nextParent);
            }
            for (auto index : parentIndices) parents.push_back(breeders[index]);
            //vector<chromosome> children = crossover(parents, gen.crossoverRate, rng);
            //population.insert(population.end(), children.begin(), children.end());
            chromosome child = edgeCrossover(parents, gen.crossoverRate, rng);
            population.push_back(child);
        }
        // Mutation & find best solution
        for (auto ch : population){
            if (percent(rng) <= gen.mutationRate) ch = mutate(ch, rng);
            if (ch.isValidSolution() && ch.getFitness() > bestSolution.getFitness()){
                bestSolution = ch;
            }
        }
    }
    return bestSolution;
}
