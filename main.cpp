#include <iostream>
#include "FluidSolver.h"
#include "OpenGLRenderer.h"

int main() {
    // grid size
    int N = 150;

    // initialize FluidSolver with a grid size and Dirichlet boundary condition
    FluidSolver fluid(N, BoundaryCondition::DIRICHLET);

    // initialize OpenGLRenderer
    OpenGLRenderer renderer(800, 800, "Fluid Sim", N, &fluid);

    if (!renderer.initialize()) {
        return -1;
    }

    // run the renderer
    renderer.run();

    return 0;
}