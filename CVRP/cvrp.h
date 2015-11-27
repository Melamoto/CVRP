#pragma once

#include <vector>
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
        uint16_t usedCapacity();
        // Returns a copy of this vehicle's route
        vector<node> getRoute();
        // Returns true if this vehicle's route contains the given node
        bool containsNode(uint16_t nodeNum) const;
        // Adds a node to the route
        void addNode(node newNode);
        // As above, for multiple nodes
        void addNodes(vector<node> newNodes);
        // Removes a node from the route; returns true if the route contains any nodes other than the start after removal
        bool removeNode(uint16_t nodeNum);
        // As above, for multiple nodes
        bool removeNodes(vector<uint16_t> nodeNums);
        // Returns the total cost of the vehicle's route
        double getRouteCost();
        // Returns a string of the route in the form "a->b->c->...->a"
        string getRouteString();
        // Uses nearest-neighbour to optimise the TSP problem
        void optimiseRoute();
        vector<node> route;
    private:
        double routeCost;
    };

    class solution{
    public:
        double getCost();
        vector<vehicle>::iterator containingVehicle(uint16_t nodeNum);
        vector<vehicle> vehicles;
        void printSolution(ostream& out);
    };

    struct problemParameters{
        int dimension;
        int capacity;
        vector<node> nodes;
    };

    problemParameters getParameters(string filename);

}
