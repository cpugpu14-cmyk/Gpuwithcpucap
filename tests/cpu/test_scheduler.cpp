#include "cpu/scheduler.hpp"
#include <cassert>
#include <iostream>

int main() {
    std::cout << "Testing CPU Scheduler..." << std::endl;
    
    auto caps = cpu::Capabilities::default();
    cpu::Scheduler scheduler(caps);
    
    // Test task allocation
    try {
        void* task = scheduler.allocate_task(2, 1024 * 1024);
        assert(task != nullptr);
        
        float util = scheduler.get_utilization();
        assert(util > 0.0f);
        
        std::cout << "Scheduler utilization: " << util << std::endl;
        std::cout << "CPU Scheduler test PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
