#include <iostream>
#include "FluidSolver.h"
#include "OpenGLRenderer.h"

int main() {

    int N = 150;
    FluidSolver fluid(N, BoundaryCondition::DIRICHLET);
    OpenGLRenderer renderer(800, 800, "Fluid Sim", N, &fluid);

    if (!renderer.initialize()) {
        return -1;
    }

    renderer.run();

    return 0;
}