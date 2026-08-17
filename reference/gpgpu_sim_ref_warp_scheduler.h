/**
 * GPU Warp Scheduler Reference Implementation
 * Based on GPGPU-Sim: https://github.com/gpgpu-sim/gpgpu-sim_distribution
 * 
 * This is a simplified reference implementation for understanding how
 * GPGPU-Sim manages warp scheduling and execution.
 */

#pragma once

#include <vector>
#include <queue>
#include <cstdint>
#include <bitset>

namespace reference {

// CUDA/GPU Constants
const uint32_t WARP_SIZE = 32;           // Threads per warp
const uint32_t MAX_WARPS_PER_CORE = 48;  // Max concurrent warps (Kepler)
const uint32_t MAX_REGISTERS_PER_CORE = 65536;
const uint32_t MAX_SHARED_MEMORY = 98304;  // 96 KB

// Warp States
enum class WarpState : uint8_t {
    ACTIVE,              // Warp has work to execute
    STALLED,             // Blocked on memory/dependency
    FINISHED,            // Warp completed all instructions
    SLEEPING             // Not yet launched
};

/**
 * Represents a single CUDA warp (32 threads)
 * 
 * A warp is the unit of execution on a GPU. All threads in a warp
 * execute the same instruction on different data (SIMT model).
 */
struct Warp {
    uint32_t warp_id;                          // Unique ID within core
    uint32_t thread_block_id;                  // Which thread block
    uint32_t kernel_id;                        // Which kernel
    
    WarpState state;
    uint32_t program_counter;                  // Current instruction address
    
    // SIMT Execution Mask
    std::bitset<WARP_SIZE> active_threads;    // Which threads are active
    std::bitset<WARP_SIZE> stalled_threads;   // Which threads are stalled
    
    // Register usage
    uint32_t registers_used;
    
    // Memory operations
    uint32_t pending_memory_ops;               // Unresolved memory requests
    uint64_t last_memory_op_cycle;             // When last memory op issued
    
    // Statistics
    uint64_t cycles_active;                    // Total active cycles
    uint64_t instructions_executed;
    
    Warp() : warp_id(0), thread_block_id(0), kernel_id(0),
             state(WarpState::SLEEPING), program_counter(0),
             registers_used(0), pending_memory_ops(0),
             last_memory_op_cycle(0), cycles_active(0),
             instructions_executed(0) {}
};

/**
 * GPU Core Warp Scheduler
 * 
 * Responsible for:
 * - Managing active warps
 * - Selecting which warp to execute each cycle (warp scheduling policy)
 * - Tracking warp dependencies and stalls
 * - Reporting utilization and statistics
 */
class WarpScheduler {
public:
    /**
     * Scheduling Policies
     * - RoundRobin: Cycle through warps in order
     * - LRU: Schedule least recently used warp
     * - GTO: Greedy then Old (low latency then stalled)
     * - Oldest: Schedule oldest warp first
     */
    enum class SchedulingPolicy {
        RoundRobin,
        LRU,
        GTO,
        Oldest
    };
    
    WarpScheduler(uint32_t max_warps = MAX_WARPS_PER_CORE,
                  SchedulingPolicy policy = SchedulingPolicy::RoundRobin);
    
    /**
     * Add a new warp to the scheduler
     * @param warp: Warp to add
     */
    void add_warp(const Warp& warp);
    
    /**
     * Select a warp to execute this cycle
     * @return: Pointer to selected warp, or nullptr if no eligible warp
     */
    Warp* select_warp();
    
    /**
     * Update warp state after cycle execution
     * Called after instruction execution to update dependencies
     */
    void update_warp_state(uint32_t warp_id, uint32_t pending_memory_ops);
    
    /**
     * Mark warp as finished (all instructions completed)
     */
    void finish_warp(uint32_t warp_id);
    
    /**
     * Get number of active warps this cycle
     */
    uint32_t get_active_warp_count() const;
    
    /**
     * Get core utilization (active threads / max possible threads)
     */
    float get_core_occupancy() const;
    
    /**
     * Get average number of active threads across all cycles
     */
    float get_avg_active_threads() const;
    
    /**
     * Print scheduler statistics
     */
    void print_stats() const;
    
private:
    std::vector<Warp> warps_;
    SchedulingPolicy policy_;
    uint32_t max_warps_;
    uint32_t rr_index_;  // For round-robin scheduling
    
    // Statistics
    uint64_t total_cycles_;
    uint64_t total_warp_cycles_;  // Sum of all warp active cycles
    uint32_t total_warps_completed_;
};

/**
 * GPU Core (Shader Core)
 * 
 * Simulates execution of a single GPU core containing:
 * - Warp scheduler
 * - Execution units (ALU, FPU, etc.)
 * - L1 cache
 * - Local (shared) memory
 * - Register file
 */
class GPUCore {
public:
    GPUCore(uint32_t core_id, uint32_t max_warps = MAX_WARPS_PER_CORE);
    
    /**
     * Execute one cycle on this core
     */
    void cycle();
    
    /**
     * Add warp to this core
     */
    void add_warp(const Warp& warp);
    
    /**
     * Get core occupancy (percentage of max possible threads active)
     */
    float get_occupancy() const;
    
    /**
     * Check if core has capacity for new warp
     */
    bool has_capacity() const;
    
private:
    uint32_t core_id_;
    WarpScheduler scheduler_;
    
    // Local memory simulation
    std::vector<uint8_t> shared_memory_;
    uint32_t shared_mem_used_;
    
    // Statistics
    uint64_t cycles_executed_;
    uint64_t instructions_executed_;
};

/**
 * GPU Cluster
 * 
 * A cluster is a collection of GPU cores that share L2 cache
 * and other resources. Typical GPU has 14-15 clusters.
 */
class GPUCluster {
public:
    GPUCluster(uint32_t cluster_id, uint32_t cores_per_cluster);
    
    /**
     * Execute one cycle on all cores
     */
    void cycle();
    
    /**
     * Get average core occupancy across all cores in cluster
     */
    float get_average_occupancy() const;
    
private:
    uint32_t cluster_id_;
    std::vector<GPUCore> cores_;
};

}  // namespace reference
