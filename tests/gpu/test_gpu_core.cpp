#include "gpu/gpu_core.hpp"
#include <cassert>
#include <iostream>

int main() {
    std::cout << "Testing GPU Core..." << std::endl;
    
    gpu::GPUCore gpu;
    gpu.initialize(4);
    
    float util = gpu.get_utilization();
    assert(util >= 0.0f && util <= 1.0f);
    
    std::cout << "GPU utilization: " << util << std::endl;
    std::cout << "GPU Core test PASSED" << std::endl;
    
    return 0;
}
