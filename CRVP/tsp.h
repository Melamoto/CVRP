#pragma once

#include "chromosome.h"
#include "helpers.h"
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
        unsigned dimension;
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
    public:
        vector<size_t> nodes;
        shared_ptr<problemParameters> params;
    };

    problemParameters getParameters(string filename)
    {
        problemParameters problem;
        ifstream fileIn(filename);
        if (fileIn){
            // Read dimension
            fileIn.ignore(numeric_limits<streamsize>::max(), ' ');
            fileIn.ignore(numeric_limits<streamsize>::max(), ' ');
            fileIn >> problem.dimension;
            // Ignore NODE_COORD_SECTION
            fileIn.ignore(numeric_limits<streamsize>::max(), '\n');
            fileIn.ignore(numeric_limits<streamsize>::max(), '\n');
            // Read node coordinates
            for (unsigned city = 0; city < problem.dimension; ++city){
                int num, x, y;
                fileIn >> num >> x >> y;
                problem.nodes.push_back({ x, y });
            }
        }
        else{
            throw runtime_error("Filename is invalid.");
        }
        return problem;
    }

    vector<chromosome> initialPopulation(shared_ptr<problemParameters>& problem, int populationSize, default_random_engine& rng)
    {
        vector<chromosome> population;
        chromosome base;
        for (size_t city = 0; city < problem->dimension; ++city) base.nodes.push_back(city);
        base.params = shared_ptr<problemParameters>(problem);
        for (int ch = 0; ch < populationSize; ++ch){
            shuffle(base.nodes.begin(), base.nodes.end(), rng);
            population.push_back(base);
        }
        return population;
    }

    // Generates an adjacency matrix for the union of each parent's path
    // Used for edge recombination
    vector<vector<size_t>> generateAdjacencyUnion(const vector<chromosome>& parents)
    {
        vector<vector<size_t>> edges;
        size_t chLen = parents[0].nodes.size();
        edges.insert(edges.begin(), chLen, vector<size_t>());
        for (auto parent : parents){
            for (size_t n = 0; n < chLen - 1; ++n){
                if (find(edges[parent.nodes[n]].begin(), edges[parent.nodes[n]].end(), parent.nodes[n + 1])
                    != edges[parent.nodes[n]].end())
                {
                    edges[parent.nodes[n]].push_back(parent.nodes[n+1]);
                    edges[parent.nodes[n+1]].push_back(parent.nodes[n]);
                }
            }
            if (find(edges[parent.nodes[chLen-1]].begin(), edges[parent.nodes[chLen-1]].end(), parent.nodes[0])
                != edges[parent.nodes[chLen-1]].end())
            {
                edges[parent.nodes[chLen-1]].push_back(parent.nodes[0]);
                edges[parent.nodes[0]].push_back(parent.nodes[chLen-1]);
            }
        }
        return edges;
    }

    // Uses Edge Crossover with 2 parents to breed 1 offspring
    const int parentsPerCrossover = 2;
    chromosome edgeCrossover(vector<chromosome>& parents, double crossoverRate, default_random_engine& rng)
    {
        if (parents.size() != parentsPerCrossover) throw runtime_error("Invalid number of parents for crossover.");
        uniform_real_distribution<double> rand(0.f, 1.f);
        if (crossoverRate < rand(rng)) return *randomElement(parents, rng);
        size_t chLen = parents[0].nodes.size();
        vector<vector<size_t>> edges = generateAdjacencyUnion(parents);
        // Tracks nodes that have not yet been added to the offspring
        vector<size_t> unusedNodes;
        for (size_t n = 0; n < chLen; ++n) unusedNodes.push_back(n);
        // Select start node from a random parent
        size_t currentNode = randomElement(parents, rng)->nodes[0];
        chromosome offspring = parents[0];
        for (size_t n = 0; n < chLen-1; ++n){
            offspring.nodes[n] = currentNode;
            removeFromSortedVector(unusedNodes, currentNode);
            if (edges[currentNode].size() > 0){
                // Remove current node from all neighbouring node lists and select the neighbour(s) with the fewest neighbours
                vector<size_t> minNeighbours;
                size_t minSize = numeric_limits<size_t>::max();
                for (auto neighbour : edges[currentNode]){
                    removeFromVector(edges[neighbour], currentNode);
                    if (edges[neighbour].size() < minSize){
                        minNeighbours.clear();
                        minNeighbours.push_back(neighbour);
                        minSize = edges[neighbour].size();
                    }
                    else if (edges[neighbour].size() == minSize){
                        minNeighbours.push_back(neighbour);
                    }
                }
                // Set the next node to a random neighbour with the smallest number of neighbours
                currentNode = *randomElement(minNeighbours, rng);
            }
            else{
                // Selects a random unused node
                currentNode = *randomElement(unusedNodes, rng);
            }
        }
        offspring.nodes[chLen-1] = currentNode;
        return offspring;
    }
    vector<chromosome> orderCrossover(vector<chromosome>& parents, double crossoverRate, default_random_engine& rng)
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
    inline vector<chromosome> crossover(vector<chromosome>& parents, double crossoverRate, default_random_engine& rng){
        return orderCrossover(parents, crossoverRate, rng);
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
        shuffle(mutant.nodes.begin(), mutant.nodes.end(), rng);
        return mutant;
    }

}