#include "FluidSolver.h"
#include <iostream>

// constructor
FluidSolver::FluidSolver(int n, BoundaryCondition b) : N(n), grid(n), bc(b)
{
    std::cout << "FluidSolver constructor called. Initializing with N = " << N << "." << std::endl;
}

// destructor
FluidSolver::~FluidSolver() {
    
}

void FluidSolver::AddInputToField(FieldType fieldType, int i, int j, float s)
{
    // calculate the index in the array using the IX function from Grid
    int index = grid.IX(i, j);

    switch (fieldType) {
    case DENSITY:
        grid.dens[index] += s * dt;
        break;

    case VELOCITY_U:
        grid.u[index] += s * dt;
        break;

    case VELOCITY_V:
        grid.v[index] += s * dt;
        break;

    default:
        std::cerr << "Error: Invalid field type." << std::endl;
        break;
    }
}

// set boundary conditions (Dirichlet, Neumann, Periodic)
void FluidSolver::SetBoundary(FieldType fieldType)
{
    float* x;
    switch (fieldType) {
    case DENSITY:
        x = grid.dens;
        break;
    case VELOCITY_U:
        x = grid.u;
        break;
    case VELOCITY_V:
        x = grid.v;
        break;
    default:
        std::cerr << "Error: Invalid field type." << std::endl;
        return;
    }

    switch (bc) {
    case BoundaryCondition::DIRICHLET:
        // Dirichlet  (fixed boundary values)
        for (int i = 1; i <= N; i++) {
            x[grid.IX(0, i)] = 0.0f;             // Left boundary
            x[grid.IX(N + 1, i)] = 0.0f;         // Right boundary
            x[grid.IX(i, 0)] = 0.0f;             // Bottom boundary
            x[grid.IX(i, N + 1)] = 0.0f;         // Top boundary
        }
        // Handle the corners
        x[grid.IX(0, 0)] = 0.0f;                // Bottom-left corner
        x[grid.IX(0, N + 1)] = 0.0f;            // Top-left corner
        x[grid.IX(N + 1, 0)] = 0.0f;            // Bottom-right corner
        x[grid.IX(N + 1, N + 1)] = 0.0f;        // Top-right corner
        break;

    case BoundaryCondition::NEUMANN:
        // TODO: Implement Neumann boundary condition (zero-gradient)
        std::cout << "Neumann boundary condition not yet implemented." << std::endl;
        break;

    case BoundaryCondition::PERIODIC:
        // TODO: Implement Periodic boundary condition (wrap-around)
        std::cout << "Periodic boundary condition not yet implemented." << std::endl;
        break;

    default:
        std::cerr << "Error: Invalid boundary condition." << std::endl;
        break;
    }
}

// diffuse function using Gauss-Seidel relaxation
void FluidSolver::Diffuse(FieldType fieldType, int NumIters)
{
    float a = dt * diff * N * N;
    float* x, * x0;

    // Determine which field we're working with
    switch (fieldType) {
    case DENSITY:
        x = grid.dens;
        x0 = grid.dens_prev;
        break;
    case VELOCITY_U:
        x = grid.u;
        x0 = grid.u_prev;
        break;
    case VELOCITY_V:
        x = grid.v;
        x0 = grid.v_prev;
        break;
    default:
        std::cerr << "Error: Invalid field type." << std::endl;
        return;
    }

    // Gauss-Seidel relaxation
    for (int k = 0; k < NumIters; k++) { // typically 20 iterations for convergence
        for (int i = 1; i <= N; i++) {
            for (int j = 1; j <= N; j++) {
                x[grid.IX(i, j)] = (x0[grid.IX(i, j)] + a * (x[grid.IX(i - 1, j)] + x[grid.IX(i + 1, j)] +
                    x[grid.IX(i, j - 1)] + x[grid.IX(i, j + 1)])) / (1 + 4 * a);
            }
        }
        // apply boundary condition
        SetBoundary(fieldType);
    }
}

void FluidSolver::Advect(FieldType fieldType)
{
    float* d, * d0, * u, * v;

    switch (fieldType) {
    case DENSITY:
        d = grid.dens;
        d0 = grid.dens_prev;
        break;
    case VELOCITY_U:
        d = grid.u;
        d0 = grid.u_prev;
        break;
    case VELOCITY_V:
        d = grid.v;
        d0 = grid.v_prev;
        break;
    default:
        std::cerr << "Error: Invalid field type." << std::endl;
        return;
    }

    // use the velocity fields for advection
    u = grid.u;
    v = grid.v;

    float dt0 = dt * N;
    int i0, j0, i1, j1;
    float x, y, s0, t0, s1, t1;

    for (int i = 1; i <= N; i++) {
        for (int j = 1; j <= N; j++) {
            x = i - dt0 * u[grid.IX(i, j)];
            y = j - dt0 * v[grid.IX(i, j)];

            if (x < 0.5f) x = 0.5f;
            if (x > N + 0.5f) x = N + 0.5f;
            i0 = static_cast<int>(x);
            i1 = i0 + 1;

            if (y < 0.5f) y = 0.5f;
            if (y > N + 0.5f) y = N + 0.5f;
            j0 = static_cast<int>(y);
            j1 = j0 + 1;

            s1 = x - i0;
            s0 = 1.0f - s1;
            t1 = y - j0;
            t0 = 1.0f - t1;

            d[grid.IX(i, j)] = s0 * (t0 * d0[grid.IX(i0, j0)] + t1 * d0[grid.IX(i0, j1)]) +
                s1 * (t0 * d0[grid.IX(i1, j0)] + t1 * d0[grid.IX(i1, j1)]);
        }
    }

    // apply the boundary
    SetBoundary(fieldType);
}

void FluidSolver::StepDensity()
{
    // swap the previous and current density buffers
    grid.SwapBuffers(DENSITY);

    // diffuse the density
    Diffuse(DENSITY);

    // swap the buffers again to prepare for the advection step
    grid.SwapBuffers(DENSITY);

    // advect the density using the velocity fields
    Advect(DENSITY);
}

void FluidSolver::Project()
{
    int i, j, k;
    float h = 1.0f / N;
    float* u = grid.u;
    float* v = grid.v;
    float* p = grid.u_prev; // u_prev to store pressure temporarily
    float* div = grid.v_prev; // v_prev to store divergence temporarily

    // compute divergence of the velocity field
    for (i = 1; i <= N; i++) {
        for (j = 1; j <= N; j++) {
            div[grid.IX(i, j)] = -0.5f * h * (u[grid.IX(i + 1, j)] - u[grid.IX(i - 1, j)] +
                v[grid.IX(i, j + 1)] - v[grid.IX(i, j - 1)]);
            p[grid.IX(i, j)] = 0;
        }
    }

    // apply boundary conditions to div and p
    SetBoundary(DENSITY); // for div
    SetBoundary(DENSITY); // for p

    // solve for the pressure using Gauss-Seidel relaxation
    for (k = 0; k < 20; k++) {
        for (i = 1; i <= N; i++) {
            for (j = 1; j <= N; j++) {
                p[grid.IX(i, j)] = (div[grid.IX(i, j)] + p[grid.IX(i - 1, j)] + p[grid.IX(i + 1, j)] +
                    p[grid.IX(i, j - 1)] + p[grid.IX(i, j + 1)]) / 4.0f;
            }
        }
        SetBoundary(DENSITY); // apply boundary conditions to p after each iteration
    }

    // subtract the pressure gradient from the velocity field
    for (i = 1; i <= N; i++) {
        for (j = 1; j <= N; j++) {
            u[grid.IX(i, j)] -= 0.5f * (p[grid.IX(i + 1, j)] - p[grid.IX(i - 1, j)]) / h;
            v[grid.IX(i, j)] -= 0.5f * (p[grid.IX(i, j + 1)] - p[grid.IX(i, j - 1)]) / h;
        }
    }

    // apply boundary conditions to the velocity field
    SetBoundary(VELOCITY_U);
    SetBoundary(VELOCITY_V);
}

void FluidSolver::StepVelocity()
{

    grid.SwapBuffers(VELOCITY_U);
    grid.SwapBuffers(VELOCITY_V);

    Diffuse(VELOCITY_U, 20);
    Diffuse(VELOCITY_V, 20);

    // project the velocity field to ensure it's divergence-free
    Project();

    // swap the buffers to prepare for the advection step
    grid.SwapBuffers(VELOCITY_U);
    grid.SwapBuffers(VELOCITY_V);

    // advect the velocity fields
    Advect(VELOCITY_U);
    Advect(VELOCITY_V);

    // project the velocity field again to ensure it's divergence-free after advection
    Project();
}

void FluidSolver::Step()
{
    // step velocity field
    StepVelocity();

    // step density field
    StepDensity();
}