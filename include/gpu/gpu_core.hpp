#pragma once

#include <vector>
#include <cstdint>
#include <memory>

namespace gpu {

struct ComputeUnit {
    uint32_t id;
    uint32_t max_threads;
    bool available;
};

class GPUCore {
public:
    GPUCore();
    ~GPUCore();
    
    /**
     * Initialize GPU with specified number of compute units
     */
    void initialize(uint32_t num_compute_units);
    
    /**
     * Execute a task on the GPU
     */
    void execute(const void* task);
    
    /**
     * Get current GPU utilization
     */
    float get_utilization() const;
    
    /**
     * Shutdown GPU
     */
    void shutdown();
    
private:
    std::vector<ComputeUnit> compute_units_;
    bool initialized_;
};

} // namespace gpu
