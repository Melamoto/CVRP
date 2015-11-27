#pragma once

#include <vector>
#include <random>
#include <fstream>
#include <string>
#include <algorithm>
#include <stdexcept>
#include <stdint.h>
#include <iostream>

using namespace std;

namespace cvrp{

    struct node{
        uint16_t num;
        int16_t x;
        int16_t y;
        uint16_t demand;
    };

    int32_t sqrDistance(node a, node b);

    double distance(node a, node b);

    class vehicle{
    public:
        vehicle(node depot)
            : route(1, depot), routeCost(0) {}
        // Returns the total used capacity of this vehicle
        uint16_t usedCapacity(){
            uint16_t totalCapacity = 0;
            for (auto n : route) totalCapacity += n.demand;
            return totalCapacity;
        }
        // Returns a copy of this vehicle's route
        vector<node> getRoute(){ return route; }
        // Returns true if this vehicle's route contains the given node
        bool containsNode(uint16_t nodeNum) const{
            auto nPos = find_if(route.begin(), route.end(), [&](const node& n) { return n.num == nodeNum; });
            return nPos != route.end();
        }
        // Adds a node to the route
        void addNode(node newNode){
            route.push_back(newNode);
        }
        // As above, for multiple nodes
        void addNodes(vector<node> newNodes){
            route.insert(route.end(), newNodes.begin(), newNodes.end());
        }
        // Removes a node from the route; returns true if the route contains any nodes other than the start after removal
        bool removeNode(uint16_t nodeNum){
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
        bool removeNodes(vector<uint16_t> nodeNums){
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
        double getRouteCost(){
            double totalDistance = 0;
            for (size_t i = 0; i < route.size() - 1; i++){
                totalDistance += distance(route[i], route[i+1]);
            }
            totalDistance += distance(route[0], route.back());
            return totalDistance;
        }
        // Returns a string of the route in the form "a->b->c->...->a"
        string getRouteString(){
            string output;
            for (auto n : route){
                output += to_string(n.num);
                output += "->";
            }
            output += to_string(route[0].num);
            return output;
        }
        // Uses nearest-neighbour to optimise the TSP problem
        void optimiseRoute(){
            routeCost = 0;
            for (size_t i = 0; i < route.size() - 1; ++i){
                // Find the node with index > i with the minimum distance to i
                int16_t minSqrDistance = sqrDistance(route[i], route[i+1]);
                int minNode = i+1;
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
        vector<node> route;
    private:
        double routeCost;
    };

    class solution{
    public:
        double getCost(){
            double totalCost = 0;
            for (auto v : vehicles){
                totalCost += v.getRouteCost();
            }
            return totalCost;
        }
        vector<vehicle>::iterator containingVehicle(uint16_t nodeNum){
            auto vPos = find_if(vehicles.begin(), vehicles.end(), [&](const vehicle& v) { return v.containsNode(nodeNum); });
            if (vPos == vehicles.end()){
                cout << nodeNum << '\n';
                throw std::invalid_argument("Node is not contained in solution.");
            }
            return vPos;
        }
        vector<vehicle> vehicles;
        void printSolution(ostream& out){
            out << "cost " << getCost();
            for (auto v : vehicles){
                out << '\n' << v.getRouteString();
            }
            out << endl;
        }
    };

    struct problemParameters{
        int dimension;
        int capacity;
        vector<node> nodes;
    };

    problemParameters getParameters(string filename);
    
}