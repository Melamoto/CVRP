#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <random>
#include <chrono>
#include "cvrp.h"
#include "tsp.h"

using namespace std;
using namespace crvp;

struct geneticParameters{
    int maxGenerations;
    unsigned populationSize;
    unsigned breederCount;
    double mutationRate;
    double crossoverRate;
};

const geneticParameters defaultGeneticParams = {
    100,
    1000,
    300,
    0.05,
    0.7
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

chromosome evolveSolution(problemParameters problem, geneticParameters gen, default_random_engine& rng)
{
    vector<chromosome> population = initialPopulation(problem, gen.populationSize, rng);
    chromosome bestSolution = population[0];
    for (auto ch : population){
        if (ch.getFitness() > bestSolution.getFitness()) bestSolution = ch;
    }
    uniform_real_distribution<double> percent(0.0, 1.0);
    for (int it = 0; it < gen.maxGenerations; ++it){
        // Selection
        vector<chromosome> breeders = selection(population, gen.breederCount, rng);
        // Crossover
        population.empty();
        while (population.size() < gen.populationSize){
            vector<chromosome> parents;
            for (int p = 0; p < parentsPerCrossover; ++p){
                parents.push_back(breeders[rng() % breeders.size()]);
            }
            vector<chromosome> children = crossover(parents, gen.crossoverRate, rng);
            population.insert(population.end(), children.begin(), children.end());
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

int main(int argc, char** argv){
    if (argc != 3){
        cout << "Invalid argument count." << endl;
        return 0;
    }
    string filename(argv[1]);
    int maxIterations = stoi(string(argv[2]));
    double mutationRate = 0.1;
    problemParameters problem = getParameters(filename);
    geneticParameters gen = defaultGeneticParams;
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    default_random_engine rng(seed);
    chromosome solution = evolveSolution(problem, gen, rng);
}
