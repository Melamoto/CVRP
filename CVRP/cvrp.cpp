#include "cvrp.h"
#include <iomanip>
#include "savings.h"

using namespace std;
using namespace cvrp;

// Returns the total used capacity of this vehicle
uint16_t vehicle::usedCapacity(){
    uint16_t totalCapacity = 0;
    for (auto n : route) totalCapacity += n.demand;
    return totalCapacity;
}
// Returns a copy of this vehicle's route
vector<node> vehicle::getRoute(){
    return route;
}
// Returns true if this vehicle's route contains the given node
bool vehicle::containsNode(uint16_t nodeNum) const{
    auto nPos = find_if(route.begin(), route.end(), [&](const node& n) { return n.num == nodeNum; });
    return nPos != route.end();
}
// Returns the total cost of the vehicle's route
double vehicle::getRouteCost(){
    return cost(route);
}
// Returns a string of the route in the form "a->b->c->...->a"
string vehicle::getRouteString(){
    string output;
    for (auto n : route){
        output += to_string(n.num);
        output += "->";
    }
    output += to_string(route[0].num);
    return output;
}


double solution::getCost(){
    double totalCost = 0;
    for (auto v : vehicles){
        totalCost += v.getRouteCost();
    }
    return totalCost;
}
double solution::getInfeasibleCost(uint16_t vehicleCapacity, double scaling){
    double totalCost = 0;
    for (auto v : vehicles){
        totalCost += v.getRouteCost();
        int16_t overCapacity = v.usedCapacity() - vehicleCapacity;
        if (overCapacity > 0) totalCost += overCapacity;
    }
    return totalCost;
}
vector<vehicle>::iterator solution::containingVehicle(uint16_t nodeNum){
    auto vPos = find_if(vehicles.begin(), vehicles.end(), [&](const vehicle& v) { return v.containsNode(nodeNum); });
    if (vPos == vehicles.end()){
        cout << nodeNum << '\n';
        throw std::invalid_argument("Node is not contained in solution.");
    }
    return vPos;
}
void solution::printSolution(ostream& out){
    out << "cost " << setprecision(10) << getCost();
    for (auto v : vehicles){
        out << '\n' << v.getRouteString();
    }
    out << endl;
}
// Checks that each node is included in exactly 1 vehicle route, and no vehicle is over-capacity
bool solution::isFeasible(size_t nodeCount, int16_t vehicleCapacity){
    vector<bool> nodeFound(nodeCount,false);
    // Depot should not be in any vehicle route except as the first node
    nodeFound[0] = true;
    for (auto v : vehicles){
        // Check that vehicles are not over-capacity
        if (v.usedCapacity() > vehicleCapacity) return false;
        // Check that the first node on the vehicle's route is the depot
        if (v.route[0].num != 1) return false;
        // Check that no node in the vehicle's route is duplicated anywhere
        for (size_t i = 1; i < v.route.size(); i++){
            size_t nodeIndex = v.route[i].num - 1;
            if (nodeFound[nodeIndex]) return false;
            nodeFound[nodeIndex] = true;
        }
    }
    return true;
}

problemParameters cvrp::getParameters(string filename){
    problemParameters problem;
    ifstream fileIn(filename);
    if (fileIn){
        string in;
        while (fileIn){
            fileIn >> in;
            if (in == "DIMENSION"){
                fileIn >> in;
                fileIn >> problem.dimension;
            }
            else if (in == "CAPACITY"){
                fileIn >> in;
                fileIn >> problem.capacity;
            }
            else if (in == "NODE_COORD_SECTION"){
                for (int i = 0; i < problem.dimension; i++){
                    node n;
                    fileIn >> n.num;
                    fileIn >> n.x;
                    fileIn >> n.y;
                    problem.nodes.push_back(n);
                }
            }
            else if (in == "DEMAND_SECTION"){
                for (int i = 0; i < problem.dimension; i++){
                    fileIn >> in;
                    fileIn >> problem.nodes[i].demand;
                }
            }
        }
    }
    else{
        throw runtime_error("Filename is invalid.");
    }
    return problem;
}

uint32_t cvrp::sqrDistance(node a, node b){
    int16_t distX = a.x - b.x;
    int16_t distY = a.y - b.y;
    return (distX*distX) + (distY*distY);
}

double cvrp::distance(node a, node b){
    return sqrt(sqrDistance(a,b));
}


uint64_t cvrp::sqrCost(vector<node> nodes){
    uint64_t totalCost = 0;
    for (size_t i = 0; i < nodes.size() - 1; i++){
        totalCost += sqrDistance(nodes[i], nodes[i+1]);
    }
    totalCost += sqrDistance(nodes[0], nodes.back());
    return totalCost;
}
double cvrp::cost(vector<node> nodes){
    double totalCost = 0;
    for (size_t i = 0; i < nodes.size() - 1; i++){
        totalCost += distance(nodes[i], nodes[i+1]);
    }
    totalCost += distance(nodes[0], nodes.back());
    return totalCost;
}