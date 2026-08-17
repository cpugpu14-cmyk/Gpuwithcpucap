#include "cpu/scheduler.hpp"
#include <stdexcept>

namespace cpu {

Scheduler::Scheduler(const Capabilities& caps)
    : capabilities_(caps), allocated_threads_(0), allocated_memory_(0) {}

Scheduler::~Scheduler() {}

void* Scheduler::allocate_task(uint32_t threads, uint64_t memory) {
    if (allocated_threads_ + threads > capabilities_.max_threads) {
        throw std::runtime_error("Insufficient thread capacity");
    }
    
    if (allocated_memory_ + memory > capabilities_.max_memory_bytes) {
        throw std::runtime_error("Insufficient memory capacity");
    }
    
    allocated_threads_ += threads;
    allocated_memory_ += memory;
    
    // Return a dummy handle for now
    return reinterpret_cast<void*>(allocated_tasks_++);
}

void Scheduler::release_task(void* task_handle) {
    // TODO: Implement proper task tracking and release
}

float Scheduler::get_utilization() const {
    return static_cast<float>(allocated_threads_) / capabilities_.max_threads;
}

} // namespace cpu
