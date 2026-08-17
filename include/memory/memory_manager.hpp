#pragma once

#include <cstdint>
#include <memory>

namespace memory {

class MemoryManager {
public:
    MemoryManager(uint64_t total_size);
    ~MemoryManager();
    
    /**
     * Allocate memory block
     */
    void* allocate(uint64_t size);
    
    /**
     * Deallocate memory block
     */
    void deallocate(void* ptr);
    
    /**
     * Get memory utilization
     */
    float get_utilization() const;
    
private:
    uint64_t total_size_;
    uint64_t used_size_;
};

} // namespace memory
