#pragma once

class chromosomeBase{
public:
    virtual double calcFitness() =0;
    virtual bool isValidSolution() =0;
    // Only calculates the fitness at the time it is required
    // Reduces the load of creating many chromosomes without checking their fitness
    double getFitness(){
        if (!fitnessReady){
            fitness = calcFitness();
            fitnessReady = true;
        }
        return fitness;
    }
protected:
    void onChangeGenes(){ fitnessReady = false; }
private:
    double fitness;
    bool fitnessReady;
};
