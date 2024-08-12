#ifndef GRID_H
#define GRID_H

#define IX(i, j) ((i) + (N + 2) * (j))
#define SWAP(x, y) { float* tmp = x; x = y; y = tmp; }

class FluidSolver;

// for functions that select fields to operate on
enum FieldType {
    DENSITY,
    VELOCITY_U,
    VELOCITY_V
};

class Grid {

public:

    // ==================================================
    // VARIABLES
    // ==================================================


    // ==================================================
    // FUNCTIONS
    // ==================================================

    // constructor
    Grid(int n);

    // destructor
    ~Grid();

    // getters for rendering
    float* GetDensity() const { return dens; }
    float* GetVelocityU() const { return u; }
    float* GetVelocityV() const { return v; }


private:

    friend class FluidSolver;

    // ==================================================
    // VARIABLES
    // ==================================================

    // N = non-boundary grid dimension
    // size = array size including boundary cells
    int N;
    int size;

    // u = horizontal velocity
    // v = vertical velocity
    // dens = fluid density
    float *u, *v, *u_prev, *v_prev, *dens, *dens_prev;

    // ==================================================
    // FUNCTIONS
    // ==================================================

    // swap the current and previous buffers for the specified field
    void SwapBuffers(FieldType fieldType);
};

#endif // GRID_H
