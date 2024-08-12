#include "Grid.h"
#include <iostream>

// constructor implementation
Grid::Grid(int n) : N(n), size((N + 2) * (N + 2)) 
{
    std::cout << "Grid constructor called. Initializing with N = " << N << ", size = " << size << "." <<  std::endl;

    // allocate memory for arrays
    u = new float[size];
    v = new float[size];
    u_prev = new float[size];
    v_prev = new float[size];
    dens = new float[size];
    dens_prev = new float[size];
    std::cout << "Memory allocated for arrays." << std::endl;

    // initialize arrays to zero
    for (int i = 0; i < size; ++i) {
        u[i] = v[i] = u_prev[i] = v_prev[i] = 0.0f;
        dens[i] = dens_prev[i] = 0.0f;
    }
    std::cout << "Arrays initialized to zero." << std::endl;
}

// destructor implementation
Grid::~Grid() 
{
    // deallocate memory
    std::cout << "Grid destructor called. Deallocating memory." << std::endl;
    delete[] u;
    delete[] v;
    delete[] u_prev;
    delete[] v_prev;
    delete[] dens;
    delete[] dens_prev;
    std::cout << "Memory deallocated." << std::endl;
}


void Grid::SwapBuffers(FieldType fieldType)
{
    switch (fieldType) {
    case DENSITY:
        SWAP(dens_prev, dens);
        break;
    case VELOCITY_U:
        SWAP(u_prev, u);
        break;
    case VELOCITY_V:
        SWAP(v_prev, v);
        break;
    default:
        break;
    }
}
