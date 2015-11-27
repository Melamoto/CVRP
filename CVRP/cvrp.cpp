#include "cvrp.h"
#include <iomanip>

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
// Adds a node to the route
void vehicle::addNode(node newNode){
  route.push_back(newNode);
}
// As above, for multiple nodes
void vehicle::addNodes(vector<node> newNodes){
  route.insert(route.end(), newNodes.begin(), newNodes.end());
}
// Removes a node from the route; returns true if the route contains any nodes other than the start after removal
bool vehicle::removeNode(uint16_t nodeNum){
  auto nPos = find_if(route.begin(), route.end(), [&](const node& n) { return n.num == nodeNum; });
  if (nPos != route.end()){
      node swap = *nPos;
      *nPos = route.back();
      route.back() = swap;
      route.erase(route.end()-1, route.end());
  }
  return route.size() > 1;
}
// As above, for multiple nodes
bool vehicle::removeNodes(vector<uint16_t> nodeNums){
  int removeCount = 0;
  for (auto nodeNum : nodeNums){
      auto nPos = find_if(route.begin(), route.end(), [&](const node& n) { return n.num == nodeNum; });
      if (nPos != route.end()){
          node swap = *nPos;
          *nPos = route[route.size()-1-removeCount];
          route[route.size()-1-removeCount] = swap;
          removeCount++;
      }
    }
  if (removeCount > 0){
      route.erase(route.end() - removeCount, route.end());
  }
  return route.size() > 1;
}
// Returns the total cost of the vehicle's route
double vehicle::getRouteCost(){
  double totalDistance = 0;
  for (size_t i = 0; i < route.size() - 1; i++){
      totalDistance += distance(route[i], route[i+1]);
  }
  totalDistance += distance(route[0], route.back());
  return totalDistance;
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
// Uses nearest-neighbour to optimise the TSP problem
void vehicle::optimiseRoute(){
  routeCost = 0;
  for (size_t i = 0; i < route.size() - 1; ++i){
      // Find the node with index > i with the minimum distance to i
      int16_t minSqrDistance = sqrDistance(route[i], route[i+1]);
      size_t minNode = i+1;
      for (size_t j = i + 2; j < route.size(); j++){
          int16_t jSqrDistance = sqrDistance(route[i], route[j]);
          if (jSqrDistance < minSqrDistance){
              minNode = j;
              minSqrDistance = jSqrDistance;
          }
      }
      // Swap the closest node to i so that it is in position i+1
      if (minNode != i + 1){
          node swap = route[i + 1];
          route[i + 1] = route[minNode];
          route[minNode] = swap;
      }
      // Add the distance of the nodes to the route total distance
      routeCost += sqrt(minSqrDistance);
  }
  // Add the distance to complete the circuit
  routeCost += distance(route[0], route.back());
}


double solution::getCost(){
    double totalCost = 0;
    for (auto v : vehicles){
        totalCost += v.getRouteCost();
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

int32_t cvrp::sqrDistance(node a, node b){
    int16_t distX = a.x - b.x;
    int16_t distY = a.y - b.y;
    return (distX*distX) + (distY*distY);
}

double cvrp::distance(node a, node b){
    return sqrt(sqrDistance(a,b));
}
