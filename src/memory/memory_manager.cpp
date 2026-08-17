#include "memory/memory_manager.hpp"
#include <cstring>
#include <stdexcept>

namespace memory {

MemoryManager::MemoryManager(uint64_t total_size)
    : total_size_(total_size), used_size_(0) {}

MemoryManager::~MemoryManager() {}

void* MemoryManager::allocate(uint64_t size) {
    if (used_size_ + size > total_size_) {
        throw std::runtime_error("Insufficient memory");
    }
    
    // For now, return a simple pointer
    // In production, implement proper memory management
    void* ptr = new char[size];
    used_size_ += size;
    return ptr;
}

void MemoryManager::deallocate(void* ptr) {
    if (ptr) {
        delete[] static_cast<char*>(ptr);
    }
}

float MemoryManager::get_utilization() const {
    return static_cast<float>(used_size_) / total_size_;
}

} // namespace memory
