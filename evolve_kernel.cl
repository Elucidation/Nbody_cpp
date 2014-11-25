#define G 1.0
#define ETA 0.1 // Ignore distances less than this

// Given position, velocity, update positions/velocities over the timestep
__kernel void evolve_kernel(__global float4 *pos, __global float4 *vel, __global float4 *forces, const float dt) {
 
    // Get the index of the current element to be processed
    int i = get_global_id(0);
    int n = get_global_size(0);

    // // Half-step positions
    // // pos += vel * dt/2
    pos[i] += vel[i] * 0.5f * dt;

    // Calculate forces applied to each body
    barrier(CLK_GLOBAL_MEM_FENCE); // wait till position fully updated
    float4 d, r, r2, r3, f;
    forces[i] = (float4)(0.0f,0.0f,0.0f,0.0f); // zero init

    // Brute force calculation, not even half-brute
    for (int j = 0; j < n; ++j)
    {
        d = pos[j] - pos[i];
        d.w = 0; // ignore mass
        r2 = d.x*d.x+d.y*d.y+d.z*d.z;
        r = sqrt(r2);
        r3 = r*r2;

        f = d * (G / (r3+ETA)); 
        forces[i] += f;
    }

    // Full-step velocity
    // vel += forces * dt
    vel[i] += forces[i] * dt;

    // Half-step positions
    barrier(CLK_GLOBAL_MEM_FENCE); // wait till velocity fully updated
    // pos += vel * dt/2
    pos[i] += vel[i] * 0.5f * dt;
}