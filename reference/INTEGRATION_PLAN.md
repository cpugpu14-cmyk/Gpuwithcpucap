# GPGPU-Sim Integration Plan for Gpuwithcpucap

## Objective

Integrate patterns and concepts from GPGPU-Sim into the `Gpuwithcpucap` project to build a realistic GPU architecture simulator with CPU resource management.

## Phase 1: Study & Reference Materials (Current)

### Deliverables
- ✅ GPGPU_SIM_ANALYSIS.md - Overview and key concepts
- ✅ gpgpu_sim_ref_warp_scheduler.h - Warp scheduling reference
- ✅ gpgpu_sim_ref_memory_model.h - Memory hierarchy reference
- ✅ INTEGRATION_PLAN.md - This document

### Tasks
1. Read GPGPU_SIM_ANALYSIS.md to understand architecture
2. Study reference implementations in headers
3. Familiarize with GPGPU-Sim's GitHub repo

---

## Phase 2: Enhanced GPU Core Architecture

### Goal
Replace basic GPU core with realistic warp scheduler and memory model

### Files to Create/Modify

```
src/gpu/
  ├── warp_scheduler.hpp      (NEW) - Implement WarpScheduler class
  ├── warp_scheduler.cpp      (NEW) - Warp scheduling logic
  ├── gpu_core.hpp            (MODIFY) - Add warp management
  ├── gpu_core.cpp            (MODIFY) - Integrate scheduler
  └── gpu_kernel_executor.hpp (NEW) - Simple PTX instruction executor

src/memory/
  ├── cache.hpp               (NEW) - L1/L2 cache model
  ├── cache.cpp               (NEW) - Cache implementation
  ├── dram.hpp                (NEW) - DRAM simulator
  ├── dram.cpp                (NEW) - DRAM implementation
  └── memory_hierarchy.hpp    (NEW) - Complete memory system

include/
  ├── gpu/
  │   ├── warp.hpp            (NEW) - Warp data structures
  │   ├── thread_block.hpp    (NEW) - Thread block management
  │   └── kernel.hpp          (NEW) - Kernel representation
```

### Implementation Details

#### 2.1 Warp Scheduler (`src/gpu/warp_scheduler.cpp`)

**Features to implement:**
- [ ] Round-robin warp selection
- [ ] LRU-based scheduling
- [ ] Warp state tracking (active, stalled, finished)
- [ ] Dependency tracking (memory, synchronization)
- [ ] Core occupancy calculation
- [ ] Per-warp statistics

**Key metrics:**
- Warp occupancy per core
- Instructions per cycle (IPC)
- Warp stall cycles

**Reference:**
```cpp
// Round-robin scheduling
Warp* WarpScheduler::select_warp() {
    // Select from eligible warps in round-robin fashion
    for (int i = 0; i < warps_.size(); i++) {
        int idx = (rr_index_ + i) % warps_.size();
        if (warps_[idx].state == WarpState::ACTIVE) {
            rr_index_ = (idx + 1) % warps_.size();
            return &warps_[idx];
        }
    }
    return nullptr;
}
```

#### 2.2 Cache Model (`src/memory/cache.cpp`)

**Features:**
- [ ] LRU replacement policy
- [ ] Configurable line size (64-128 bytes)
- [ ] Configurable associativity
- [ ] Hit/miss tracking
- [ ] Write-back policy

**Configuration:**
```cpp
L1Cache config:
  - Size: 32-48 KB
  - Line size: 128 bytes
  - Associativity: 4-way
  - Policy: LRU
  - Latency: 1 cycle (hit), 100+ cycles (miss)

L2Cache config:
  - Size: 1.5-2 MB (shared)
  - Line size: 128 bytes
  - Associativity: 8-16-way
  - Latency: 30 cycles (hit)
```

#### 2.3 DRAM Simulator (`src/memory/dram.cpp`)

**Features:**
- [ ] Bank simulation
- [ ] Row buffer tracking
- [ ] Row hit/miss detection
- [ ] Access queuing
- [ ] Bandwidth limiting

**Configuration:**
```cpp
DRAM config:
  - Channels: 6
  - Banks per channel: 8
  - Rows per bank: 65536
  - Row hit latency: 25 cycles
  - Row miss latency: 100+ cycles
  - Bandwidth: 192-288 GB/s
```

### Testing

**Unit tests (`tests/gpu/test_warp_scheduler.cpp`):**
- Warp creation and deletion
- Scheduling policy correctness
- State transitions
- Occupancy calculations

**Unit tests (`tests/memory/test_cache.cpp`):**
- Cache hits and misses
- Replacement policy
- Write operations

---

## Phase 3: Kernel Execution & Statistics

### Goal
Execute simplified GPU kernels and collect performance statistics

### Files to Create

```
src/gpu/
  ├── kernel_executor.hpp     (NEW) - Execute GPU kernels
  ├── kernel_executor.cpp     (NEW)
  └── ptx_simulator.hpp       (NEW) - Simplified PTX execution

src/stats/
  ├── performance_stats.hpp   (NEW) - Performance counters
  ├── performance_stats.cpp   (NEW)
  └── kernel_stats.hpp        (NEW) - Per-kernel statistics
```

### Key Metrics to Track

```cpp
struct PerformanceStats {
    // Execution
    uint64_t total_cycles;
    uint64_t total_instructions;
    float avg_ipc;                    // Instructions per cycle
    
    // Warp execution
    float avg_warp_occupancy;         // Active warps / max warps
    float avg_active_threads;
    uint64_t warp_stall_cycles;
    
    // Cache
    uint64_t l1_hits, l1_misses;
    uint64_t l2_hits, l2_misses;
    float l1_hit_ratio, l2_hit_ratio;
    
    // Memory
    uint64_t dram_row_hits, dram_row_misses;
    float dram_utilization;           // Bandwidth used / available
    uint64_t memory_stall_cycles;
    
    // Power (if enabled)
    double total_power_watts;
    double dynamic_power;
    double static_power;
};
```

### Execution Model

**Each cycle:**
1. Select warp via scheduler
2. Fetch instruction at program counter
3. Execute instruction (simplified)
4. Handle memory operations if needed
5. Update warp state
6. Collect statistics

```cpp
void GPUCore::cycle() {
    Warp* active_warp = scheduler_.select_warp();
    if (active_warp) {
        Instruction instr = fetch_instruction(active_warp->pc);
        execute(instr, active_warp);
        if (instr.is_memory_op()) {
            memory_hierarchy_.access(instr.address, instr.access_type);
        }
        update_stats();
    }
}
```

---

## Phase 4: CPU-GPU Resource Management

### Goal
Integrate CPU scheduler to manage GPU resource allocation

### Files to Modify/Create

```
src/cpu/
  ├── scheduler.hpp           (MODIFY) - Add GPU resource management
  ├── scheduler.cpp           (MODIFY)
  ├── gpu_resource_manager.hpp (NEW) - GPU resource allocation
  ├── gpu_resource_manager.cpp (NEW)
  └── heterogeneous_scheduler.hpp (NEW) - CPU-GPU task scheduling

src/
  └── heterogeneous_system.hpp  (NEW) - Complete CPU-GPU system
```

### Resource Constraints

```cpp
struct GPUResourceCap {
    uint32_t max_active_warps;     // CPU-imposed warp limit
    uint32_t max_active_threads;   // Total active threads
    float max_power_watts;         // Power budget
    uint64_t max_memory_bandwidth; // Bandwidth cap
    float max_core_frequency;      // Frequency cap (DVFS)
};
```

### Dynamic Frequency Scaling (DVFS)

```cpp
class DVFSController {
    void update_frequency_based_on_power(float current_power, float power_budget);
    void adjust_core_voltage(float new_voltage);
};
```

---

## Phase 5: Power Modeling

### Goal
Add realistic power consumption tracking

### Files to Create

```
src/power/
  ├── power_model.hpp        (NEW) - Power calculation
  ├── power_model.cpp        (NEW)
  └── accelwattch_model.hpp  (NEW) - Simplified AccelWattch
```

### Power Components

```cpp
struct PowerModel {
    // Component power (Watts)
    double exe_unit_power;    // Execution unit
    double regfile_power;     // Register file
    double cache_power;       // L1/L2 cache
    double memory_power;      // DRAM
    double noc_power;         // Interconnect
    double idle_power;        // Static leakage
    
    double total_power() const {
        return exe_unit_power + regfile_power + cache_power +
               memory_power + noc_power + idle_power;
    }
};
```

---

## Implementation Timeline

| Phase | Milestone | Estimated Time | Status |
|-------|-----------|-----------------|--------|
| 1 | Reference & Analysis | ✅ Complete | ✅ |
| 2 | Warp Scheduler & Memory | 2-3 weeks | ⏳ Not Started |
| 3 | Kernel Execution & Stats | 2-3 weeks | ⏳ Not Started |
| 4 | CPU-GPU Integration | 1-2 weeks | ⏳ Not Started |
| 5 | Power Modeling | 1-2 weeks | ⏳ Not Started |

---

## Testing Strategy

### Unit Tests
```bash
make test
```

### Integration Tests
- Load simple GPU kernels
- Execute and verify results
- Check statistics match expectations

### Performance Tests
- Run CUDA benchmarks
- Compare against real hardware
- Validate simulator accuracy

---

## Success Criteria

- [ ] Warp scheduler correctly manages 32-48 concurrent warps
- [ ] Cache hit ratios within 5% of hardware
- [ ] Memory access latencies realistic
- [ ] Statistics collection accurate
- [ ] CPU-GPU resource management functional
- [ ] Power model shows reasonable values
- [ ] All unit/integration tests passing

---

## References

1. GPGPU-Sim: https://github.com/gpgpu-sim/gpgpu-sim_distribution
2. Accel-Sim: https://github.com/accelsim/accelsim
3. GPGPU-Sim Manual: http://gpgpu-sim.org/manual/
4. ISCA 2020 Paper: Accel-Sim Framework
