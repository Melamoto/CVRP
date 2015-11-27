#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <random>
#include <chrono>
#include <stdexcept>
#include "cvrp.h"
#include "tabu.h"

using namespace std;

int main(int argc, char** argv){
    if (argc != 2){
        cout << "Invalid argument count." << endl;
        return 0;
    }
    string filename(argv[1]);
    cvrp::problemParameters problem = cvrp::getParameters(filename);
    unsigned seed = static_cast<unsigned>(std::chrono::system_clock::now().time_since_epoch().count());
    default_random_engine rng(seed);
    // Find solution using Taburoute
    cvrp::solution solution = cvrp::tabu::taburoute(problem.nodes, problem.capacity, rng);
    // Output solution
    cout << "login sl12754 58774" << '\n';
    cout << "name Stephen Tozer" << '\n';
    cout << "algorithm Tabu Search with savings heuristic" << '\n';
    solution.printSolution(cout);
    return 0;
}
