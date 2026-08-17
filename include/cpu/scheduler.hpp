#pragma once

#include <cstdint>
#include <memory>
#include <vector>

namespace cpu {

struct Capabilities {
    uint32_t max_threads;
    uint64_t max_memory_bytes;
    float max_power_watts;
    
    static Capabilities default() {
        return {8, 1024 * 1024 * 1024, 150.0f};
    }
};

class Scheduler {
public:
    explicit Scheduler(const Capabilities& caps);
    ~Scheduler();
    
    /**
     * Allocate a task with given resource requirements
     */
    void* allocate_task(uint32_t threads, uint64_t memory);
    
    /**
     * Release a previously allocated task
     */
    void release_task(void* task_handle);
    
    /**
     * Get current resource utilization
     */
    float get_utilization() const;
    
private:
    Capabilities capabilities_;
    uint32_t allocated_threads_;
    uint64_t allocated_memory_;
};

} // namespace cpu
