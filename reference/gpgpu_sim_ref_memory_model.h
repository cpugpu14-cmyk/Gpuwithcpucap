/**
 * GPU Memory Hierarchy Reference Implementation
 * Based on GPGPU-Sim: https://github.com/gpgpu-sim/gpgpu-sim_distribution
 * 
 * Models the GPU memory system:
 * - L1 Cache (per core)
 * - Shared Memory / Local Memory (per core)
 * - L2 Cache (shared across cluster/GPU)
 * - DRAM (main memory)
 */

#pragma once

#include <cstdint>
#include <vector>
#include <unordered_map>
#include <deque>

namespace reference {

// Memory Constants
const uint32_t L1_CACHE_SIZE = 32 * 1024;  // 32 KB per core
const uint32_t L1_CACHE_LINE_SIZE = 128;   // Bytes
const uint32_t L2_CACHE_SIZE = 1536 * 1024; // 1.5 MB (shared)
const uint32_t DRAM_BANDWIDTH = 192;  // GB/s (GTX Titan X)

// Memory Access Types
enum class AccessType : uint8_t {
    READ,
    WRITE,
    ATOMIC  // Atomic operation (read-modify-write)
};

/**
 * Represents a single memory request
 */
struct MemoryRequest {
    uint32_t requester_id;      // Which warp/core made request
    uint64_t address;           // Memory address
    AccessType access_type;
    uint32_t data_size;         // Bytes to read/write
    uint64_t issued_cycle;      // When request was issued
    uint64_t completed_cycle;   // When request will complete
    
    MemoryRequest() : requester_id(0), address(0), access_type(AccessType::READ),
                     data_size(0), issued_cycle(0), completed_cycle(0) {}
};

/**
 * Cache Line
 */
struct CacheLine {
    uint64_t tag;               // Address tag
    bool valid;                 // Is line present in cache?
    bool dirty;                 // Has line been modified?
    uint64_t last_access_time;  // For LRU replacement
    std::vector<uint8_t> data;  // Actual cache line data
    
    CacheLine(uint32_t size = L1_CACHE_LINE_SIZE)
        : tag(0), valid(false), dirty(false), last_access_time(0),
          data(size, 0) {}
};

/**
 * Cache Model (L1 or L2)
 * 
 * Simulates cache behavior:
 * - Hit/miss detection
 * - Replacement policy (LRU)
 * - Write-back policy
 */
class CacheModel {
public:
    enum class ReplacementPolicy {
        LRU,      // Least Recently Used
        FIFO,     // First In First Out
        Random
    };
    
    CacheModel(uint32_t size, uint32_t line_size,
               uint32_t associativity,
               ReplacementPolicy policy = ReplacementPolicy::LRU);
    
    /**
     * Access cache
     * @return: true if hit, false if miss
     */
    bool access(uint64_t address, AccessType type);
    
    /**
     * Get latency for this access (0 for hit, miss latency otherwise)
     */
    uint32_t get_latency() const { return last_access_was_hit_ ? 1 : miss_latency_; }
    
    /**
     * Get cache statistics
     */
    uint64_t get_hits() const { return hits_; }
    uint64_t get_misses() const { return misses_; }
    float get_hit_ratio() const {
        return (hits_ + misses_) > 0 ? float(hits_) / (hits_ + misses_) : 0.0f;
    }
    
    /**
     * Print cache statistics
     */
    void print_stats() const;
    
private:
    uint32_t size_;             // Total cache size in bytes
    uint32_t line_size_;        // Cache line size
    uint32_t associativity_;    // Set associativity
    uint32_t num_sets_;         // Number of cache sets
    
    ReplacementPolicy policy_;
    std::vector<std::vector<CacheLine>> cache_lines_;  // [set][way]
    
    // Statistics
    uint64_t hits_;
    uint64_t misses_;
    uint32_t miss_latency_;     // Cycles for cache miss
    bool last_access_was_hit_;
    
    // Helper functions
    uint32_t get_set_index(uint64_t address) const;
    uint64_t get_tag(uint64_t address) const;
    int find_cache_line(uint32_t set, uint64_t tag);
    void replace_line(uint32_t set, uint32_t way, uint64_t tag);
};

/**
 * DRAM Simulator
 * 
 * Models DRAM behavior:
 * - Bank conflicts
 * - Row buffer hits/misses
 * - Queue delays
 * - Bandwidth limitations
 */
class DRAMSimulator {
public:
    struct Config {
        uint32_t num_banks;          // Number of banks
        uint32_t num_channels;       // Number of channels
        uint32_t rows_per_bank;      // Rows per bank
        uint32_t cols_per_row;       // Columns per row
        uint64_t bandwidth_bytes_per_cycle;  // DRAM bandwidth
        uint32_t row_miss_latency;   // Cycles for row miss
        uint32_t row_hit_latency;    // Cycles for row hit
    };
    
    DRAMSimulator(const Config& config);
    
    /**
     * Issue a memory request to DRAM
     */
    uint32_t access(const MemoryRequest& req);
    
    /**
     * Advance DRAM simulation by one cycle
     */
    void cycle();
    
    /**
     * Get queue length
     */
    uint32_t get_queue_length() const { return request_queue_.size(); }
    
    /**
     * Get statistics
     */
    uint64_t get_row_hits() const { return row_hits_; }
    uint64_t get_row_misses() const { return row_misses_; }
    
private:
    Config config_;
    
    // Row buffers per bank (track last accessed row)
    std::vector<uint64_t> row_buffers_;
    std::deque<MemoryRequest> request_queue_;
    
    // Statistics
    uint64_t row_hits_;
    uint64_t row_misses_;
    uint64_t current_cycle_;
    
    uint64_t get_row_number(uint64_t address) const;
};

/**
 * Memory Hierarchy
 * 
 * Complete memory system including:
 * - L1 cache (per core)
 * - L2 cache (shared)
 * - DRAM
 */
class MemoryHierarchy {
public:
    MemoryHierarchy();
    
    /**
     * Access memory with automatic cache/DRAM handling
     * Returns latency in cycles
     */
    uint32_t access(uint64_t address, AccessType type);
    
    /**
     * Execute one cycle of memory operations
     */
    void cycle();
    
    /**
     * Get memory statistics
     */
    void print_stats() const;
    
private:
    CacheModel l1_cache_;
    CacheModel l2_cache_;
    DRAMSimulator dram_;
};

}  // namespace reference
