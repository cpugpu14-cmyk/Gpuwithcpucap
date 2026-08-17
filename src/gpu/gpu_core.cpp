#include "gpu/gpu_core.hpp"

namespace gpu {

GPUCore::GPUCore() : initialized_(false) {}

GPUCore::~GPUCore() {
    if (initialized_) {
        shutdown();
    }
}

void GPUCore::initialize(uint32_t num_compute_units) {
    compute_units_.reserve(num_compute_units);
    
    for (uint32_t i = 0; i < num_compute_units; ++i) {
        compute_units_.push_back({
            .id = i,
            .max_threads = 128,
            .available = true
        });
    }
    
    initialized_ = true;
}

void GPUCore::execute(const void* task) {
    // TODO: Implement task execution
}

float GPUCore::get_utilization() const {
    if (!initialized_ || compute_units_.empty()) {
        return 0.0f;
    }
    
    uint32_t busy_units = 0;
    for (const auto& unit : compute_units_) {
        if (!unit.available) {
            ++busy_units;
        }
    }
    
    return static_cast<float>(busy_units) / compute_units_.size();
}

void GPUCore::shutdown() {
    compute_units_.clear();
    initialized_ = false;
}

} // namespace gpu
