#include <array>
#include "cvrp.h"
#include "savings.h"
#include "tabu.h"

using namespace cvrp;
using namespace std;

// Given the set of nodes (with the first node being the depot) and their corresponding savings,
// returns the heuristic solution given by the Clarke-Wright algorithm
solution calculateClarkeWrightSolution(const vector<node>& nodes, vector<saving> savings, uint16_t vehicleCapacity){
    solution result;
    for (size_t i = 1; i < nodes.size(); i++){
        result.vehicles.emplace_back(nodes[0]);
        result.vehicles.back().addNode(nodes[i]);
    }
    bool nodesMatched = true;
    // At the end of each loop:
    //   'savings' only contains savings for nodes that can be "attached" to other routes,
    //   i.e. are the first or last node visited by their respective vehicle
    //   'result' is a valid solution
    while (nodesMatched){
        nodesMatched = false;
        for (int s = savings.size() - 1; s >= 0; s--){
            uint16_t nodeA = savings[s].nodeA;
            uint16_t nodeB = savings[s].nodeB;
            auto vehicleA = result.containingVehicle(nodeA);
            auto vehicleB = result.containingVehicle(nodeB);
            uint16_t totalCapacity = vehicleA->usedCapacity() + vehicleB->usedCapacity();
            // Merge the paths of vehicleA and vehicleB into vehicleA, such that nodeA and nodeB are adjacent, and remove vehicleB
            if (totalCapacity <= vehicleCapacity && vehicleA != vehicleB){
                vector<node> routeA = vehicleA->route;
                vector<node> routeB = vehicleB->route;
                // Remove savings containing nodeA and nodeB if they are no longer the first or last node of their
                // respective vehicle, i.e. if their current vehicle's route does not contain only them, and remove
                // the current saving
                savings.erase(savings.begin() + s);
                if (routeA.size() > 2){
                    auto newEnd = remove_if(savings.begin(), savings.end(), [&](const saving& sav) { return sav.contains(nodeA); });
                    savings.erase(newEnd, savings.end());
                }
                if (routeB.size() > 2){
                    auto newEnd = remove_if(savings.begin(), savings.end(), [&](const saving& sav) { return sav.contains(nodeB); });
                    savings.erase(newEnd, savings.end());
                }
                // WARNING: From this point onwards, previously defined references to elements of 'savings' are invalid.
                // Set routeA so that the depot is at the front and nodeA is at the end
                if (routeA.back().num != nodeA){
                    reverse(routeA.begin()+1, routeA.end());
                }
                // Set routeB so that the depot is removed and nodeB is at the front
                if (routeB.back().num != nodeB){
                    routeB.erase(routeB.begin(), routeB.begin()+1);
                }
                else{
                    reverse(routeB.begin(), routeB.end());
                    routeB.pop_back();
                }
                // Combine the two routes such that nodeA and nodeB are adjacent, merge the result into vehicle A, and erase vehicle B
                routeA.insert(routeA.end(), routeB.begin(), routeB.end());
                vehicleA->route = routeA;
                result.vehicles.erase(vehicleB, vehicleB + 1);
                // Start back from the top of the loop
                nodesMatched = true;
                break;
            }
        }
    }
    return result;
}
