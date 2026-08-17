# GPGPU-Sim Analysis & Integration Guide

## Overview

GPGPU-Sim is a cycle-level GPU architecture simulator from University of British Columbia. It models contemporary GPU hardware running CUDA/OpenCL workloads.

**GitHub:** https://github.com/gpgpu-sim/gpgpu-sim_distribution  
**Citation:** Khairy et al., ISCA 2020 (Accel-Sim framework)

---

## Key Architecture Components

### 1. GPU Simulation Core (`gpu-sim.h/cc`)

**Purpose:** Main GPU simulator managing execution of CUDA kernels

**Key Classes:**
- `gpgpu_sim`: Main GPU simulator class
- `shader_core_cluster`: Collection of shader cores
- `memory_partition_unit`: L2 cache and memory controller
- `memory_sub_partition`: L2 sub-partitions for memory hierarchy

**Key Functions:**
```cpp
void cycle();                    // Execute one GPU cycle
void issue_block2core();         // Schedule thread blocks to cores
void shader_print_cache_stats(); // Print performance statistics
```

### 2. Shader Core Architecture

**Warp Scheduling:**
- Manages SIMT thread execution (32 threads per warp on NVIDIA)
- Implements dynamic warp formation
- Tracks warp status: active, stalled, finished

**Memory Subsystem:**
- L1 cache per core
- Shared memory (local to core)
- L2 cache (shared across cores)
- DRAM with configurable banks/channels

### 3. Power Model (AccelWattch)

**Classes:**
- `gpgpu_sim_wrapper`: Wrapper for power simulation
- `power_stat_t`: Tracks power-related statistics
- `ParseXML`: Parses AccelWattch XML configuration

**Power Components Modeled:**
- Instruction execution units (ALU, FPU)
- Memory subsystem (caches, DRAM)
- Register files
- Control logic

---

## How GPGPU-Sim Works

### Execution Flow

```
1. Load CUDA Binary (PTX instructions)
   ↓
2. Functional Simulation Layer
   - CUDA Thread Execution
   - Register/Local Memory Management
   ↓
3. Performance Model Layer (Cycle-Level)
   - Warp Scheduling
   - Pipeline Simulation
   - Memory Hierarchy
   - Cache Statistics
   ↓
4. Power Model Layer (Optional - AccelWattch)
   - Component Power Consumption
   - Dynamic/Static Power
   ↓
5. Statistics Collection & Output
```

### Key Simulation Parameters

**From Configuration Files (e.g., GTX480 config):**

```
-gpgpu_n_clusters <num>          # Number of GPU clusters
-gpgpu_n_cores_per_cluster <num> # Cores per cluster
-gpgpu_warps_per_shader <num>    # Concurrent warps (max)
-gpgpu_shader_registers <num>    # Register file size
-gpgpu_mem_unit_ports <num>      # Memory unit ports
-cache:dl1 <cache_config>         # L1D cache config
-cache:il1 <cache_config>         # L1I cache config
-cache:l2 <cache_config>          # L2 cache config
-dram <dram_config>               # DRAM configuration
-power_simulation_enabled <0/1>   # Enable power modeling
```

---

## Integration with Your Project

### Recommended Approach

**Phase 1: Reference Implementation**
- Study GPGPU-Sim's core scheduler (`src/gpgpu-sim/shader.h`)
- Understand warp scheduling algorithms
- Learn cache modeling patterns

**Phase 2: Lightweight Simulator**
Build a simplified C++ GPU simulator in your project:
- Core execution engine
- Warp scheduler
- Cache simulator
- Statistics collection

**Phase 3: Power Modeling**
- Integrate basic power model
- Track per-component power
- Report total GPU power consumption

**Phase 4: CPU-GPU Orchestration**
- Use CPU scheduler to cap GPU resources
- Implement dynamic frequency scaling
- Manage heterogeneous workloads

---

## Code Patterns to Adopt

### 1. Configuration System

GPGPU-Sim uses `OptionParser` for configuration:
```cpp
void gpgpu_sim_config::reg_options(OptionParser *opp) {
    option_parser_register(opp, "-gpgpu_n_clusters", OPT_INT32,
                          &num_clusters, "Number of clusters", "16");
}
```

**For your project:** Create similar config system for GPU parameters.

### 2. Performance Statistics

GPGPU-Sim tracks detailed stats:
```cpp
class shader_core_stats {
    unsigned long long m_num_sim_insn;     // Instructions
    unsigned long long m_cache_hits;       // Cache hits
    unsigned long long m_cache_misses;     // Cache misses
    float avg_duty_cycle;                  // Core utilization
};
```

### 3. Memory Hierarchy Simulation

Model cache behavior:
```cpp
class gpu_cache_t {
    enum cache_request_status access(mem_fetch *mf);
    void cycle();  // Update cache state each cycle
    void print_stats();
};
```

### 4. Warp Scheduler

Key scheduling responsibility:
- Select which warp executes in each cycle
- Track warp dependencies (memory, sync)
- Handle warp divergence and convergence

```cpp
class warp_inst_t {
    simt_mask_t active_mask;  // Which threads are active
    unsigned warp_id;
    addr_t pc;                // Program counter
};
```

---

## Performance Metrics

Key metrics GPGPU-Sim reports:

| Metric | Meaning |
|--------|----------|
| `Instructions Per Cycle (IPC)` | Throughput |
| `Cache Hit Ratio` | L1/L2 cache effectiveness |
| `Memory Bandwidth Utilization` | DRAM utilization |
| `Warp Occupancy` | Average warps per core |
| `Memory Stalls` | Cycles blocked on memory |
| `Power (W)` | Total power consumption |

---

## Files to Study in GPGPU-Sim

### Essential
1. `src/gpgpu-sim/gpu-sim.h` - Main GPU simulator interface
2. `src/gpgpu-sim/shader.h` - Warp and core simulation
3. `src/gpgpu-sim/gpu-cache.h` - Cache modeling
4. `src/gpgpu-sim/dram.h` - DRAM simulator

### Power & Configuration
5. `src/accelwattch/gpgpu_sim_wrapper.h` - Power modeling
6. `src/gpgpu-sim/gpu-sim.cc` - Configuration options

### Advanced
7. `src/gpgpu-sim/icnt_wrapper.h` - Interconnection network
8. `src/gpgpu-sim/l2cache.h` - L2 cache hierarchy

---

## Building GPGPU-Sim (Reference)

```bash
git clone https://github.com/gpgpu-sim/gpgpu-sim_distribution.git
cd gpgpu-sim_distribution
export CUDA_INSTALL_PATH=/usr/local/cuda
source setup_environment
make
```

**Dependencies:**
- GCC 4.5+
- CUDA Toolkit 3.1+
- Bison, Flex
- zlib

---

## Recommended Next Steps for Your Project

1. **Create Reference Implementations**
   - Extract simplified versions of key GPGPU-Sim components
   - Create example warp scheduler
   - Implement basic cache model

2. **Design Your GPU Model**
   - Define configurable parameters
   - Create cycle-level simulator
   - Implement statistics collection

3. **Integrate CPU Scheduler**
   - Use GPGPU-Sim patterns for resource tracking
   - Implement CPU-imposed GPU resource caps
   - Add heterogeneous workload scheduling

4. **Add Power Modeling**
   - Adapt AccelWattch concepts
   - Track per-component power
   - Implement DVFS (Dynamic Voltage/Frequency Scaling)

---

## Resources

- **Manual:** http://gpgpu-sim.org/manual/
- **Bug Reports:** http://www.gpgpu-sim.org/bugs/
- **Google Groups:** https://groups.google.com/forum/#!forum/gpgpu-sim
- **Accel-Sim Fork:** https://github.com/accelsim/accelsim (More modern)
