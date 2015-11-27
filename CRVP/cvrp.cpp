
#include "cvrp.h"

using namespace std;
using namespace cvrp;

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
    if (a.num == 213 && b.num == 229){
        cout << "Oh dear." << '\n';
    }
    double result = sqrt(sqrDistance(a,b));
    return sqrt(sqrDistance(a,b));
}
