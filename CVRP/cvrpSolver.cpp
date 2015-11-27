#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <random>
#include <chrono>
#include "cvrp.h"
#include "tabu.h"

using namespace cvrp;

default_random_engine rng;

int main(int argc, char** argv){
    if (argc != 2){
        cout << "Invalid argument count." << endl;
        return 0;
    }
    string filename(argv[1]);
    double mutationRate = 0.1;
    problemParameters problem = getParameters(filename);
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    rng = default_random_engine(seed);
    vector<saving> savings = calculateSavings(problem.nodes);
    solution solution = calculateClarkeWrightSolution(problem.nodes, savings, problem.capacity);
    solution.printSolution(cout);
    cin.get();
    return 0;
}
