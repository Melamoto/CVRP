#pragma once

#include "chromosome.h"
#include <algorithm>
#include <vector>
#include <random>
#include <fstream>
#include <stdexcept>
#include <cmath>
#include <memory>

using namespace std;

namespace tsp{

    struct node{
        int x;
        int y;
        bool operator==(const node& other){
            return (x == other.x) && (y == other.y);
        }
    };

    double distance(node a, node b){
        return sqrt(((a.x-b.x)*(a.x-b.x))+((a.y-b.y)*(a.y-b.y)));
    }

    struct problemParameters{
        int dimension;
        vector<node> nodes;
    };

    class chromosome : public chromosomeBase{
    public:
        virtual double calcFitness(){
            double totalDistance = 0;
            totalDistance += distance(params->nodes[nodes[0]], params->nodes[nodes.back()]);
            for (size_t n = 0; n < nodes.size() - 1; ++n){
                totalDistance += distance(params->nodes[nodes[n]], params->nodes[nodes[n+1]]);
            }
            return 1/totalDistance;
        }
        virtual bool isValidSolution(){ return true; };
        vector<size_t> nodes;
        shared_ptr<problemParameters> params;
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
        problem.dimension = 0;
        return problem;
    }

    vector<chromosome> initialPopulation(problemParameters& problem, int populationSize, default_random_engine& rng)
    {
        vector<chromosome> population;
        for (int ch = 0; ch < populationSize; ++ch){
            chromosome specimen;
            for ()
        }
        return population;
    }

    // Uses Order Crossover with 2 parents to breed 2 offspring
    const int parentsPerCrossover = 2;
    vector<chromosome> crossover(vector<chromosome>& parents, double crossoverRate, default_random_engine& rng)
    {
        if (parents.size() != parentsPerCrossover) throw runtime_error("Invalid number of parents for crossover.");
        vector<chromosome> offspring = parents;
        size_t chLen = parents[0].nodes.size();
        uniform_int_distribution<size_t> points(0, chLen - 1);
        uniform_real_distribution<double> rand(0.f, 1.f);
        size_t pointA = points(rng);
        size_t pointB = points(rng);
        while (pointA == pointB) pointB = points(rng);
        if (pointA > pointB){
            size_t tmp = pointA;
            pointA = pointB;
            pointB = tmp;
        }
        for (size_t p1 = 0; p1 < 2; ++p1){
            // Each offspring has a 1-crossoverRate chance of being a clone
            if (crossoverRate < rand(rng)) continue;
            size_t p2 = 1 - p1;
            // Prepare a sorted list of the genes inherited from the first parent
            // Offspring starts as a clone of the first parent, so actually copying the genes is not necessary
            vector<size_t> swappedGenes;
            swappedGenes.insert(swappedGenes.begin(),
                                parents[p1].nodes.begin() + pointA,
                                parents[p1].nodes.begin() + pointB);
            sort(swappedGenes.begin(), swappedGenes.end());
            // Add all other genes to the offspring in the order in which they appear in the second parent
            size_t p2_pos = 0;
            size_t p2_gene;
            for (size_t i = 0; i < pointA; ++i){
                do{
                    p2_gene = parents[p2].nodes[p2_pos];
                    p2_pos++;
                } while (binary_search(swappedGenes.begin(), swappedGenes.end(), p2_gene));
                offspring[p1].nodes[i] = p2_gene;
            }
            for (size_t i = pointB + 1; i < chLen; ++i){
                do{
                    p2_gene = parents[p2].nodes[p2_pos];
                    p2_pos++;
                } while (binary_search(swappedGenes.begin(), swappedGenes.end(), p2_gene));
                offspring[p1].nodes[i] = p2_gene;
            }
        }
        return offspring;
    }

    // 2-opt mutation - reverses a random substring
    chromosome mutate(chromosome& base, default_random_engine& rng)
    {
        chromosome mutant = base;
        uniform_int_distribution<size_t> points(0, base.nodes.size());
        // Start of substring
        size_t pointA = points(rng);
        // End of substring
        size_t pointB = points(rng);
        // Ensure that the substring has length > 1 and ends before it begins
        while (abs((long)pointA - (long)pointB) <= 1) pointB = points(rng);
        if (pointA > pointB){
            size_t tmp = pointA;
            pointA = pointB;
            pointB = tmp;
        }
        reverse(mutant.nodes.begin() + pointA, mutant.nodes.begin() + pointB);
        return mutant;
    }

}