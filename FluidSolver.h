#ifndef FLUIDSOLVER_H
#define FLUIDSOLVER_H

#include "Grid.h"

enum class BoundaryCondition {
    DIRICHLET,
    NEUMANN,
    PERIODIC 
};

class FluidSolver {

public:

    // ==================================================
    // VARIABLES
    // ==================================================


    // ==================================================
    // FUNCTIONS
    // ==================================================

    // constructor
    FluidSolver(int n, BoundaryCondition b);

    // destructor
    ~FluidSolver();

    // add input to a specified field at a location
    void AddInputToField(FieldType fieldType, int i, int j, float s);

    // step
    void Step();

    // getters for rendering
    float* GetDensity() const { return grid.GetDensity(); }
    float* GetVelocityU() const { return grid.GetVelocityU(); }
    float* GetVelocityV() const { return grid.GetVelocityV(); }

private:

    // ==================================================
    // VARIABLES
    // ==================================================

    // grid width/height (not including boundary)
    int N;
    // Grid object to manage grid data
    Grid grid;
    // boundary condition
    BoundaryCondition bc;

    // delta time
    float dt = 0.8f;
    // diffusion coefficient
    float diff = 0.0001f;

    // ==================================================
    // FUNCTIONS
    // ==================================================

    void SetBoundary(FieldType fieldType);
    void Diffuse(FieldType fieldType, int NumIters=20);
    void Advect(FieldType fieldType);
    void StepDensity();
    void Project();
    void StepVelocity();
    
};

#endif // FLUIDSOLVER_H
