# GPU Architecture with CPU Capabilities

## Design Overview

This project implements a heterogeneous computing architecture that combines:

1. **GPU Core**: High-performance compute units for parallel workloads
2. **CPU Scheduler**: Resource-aware task scheduling with CPU-imposed limits
3. **Memory Manager**: Unified memory abstraction
4. **Resource Allocator**: Dynamic capability management

## Components

### GPU Core (`gpu/`)
- **gpu_core.hpp/cpp**: Main GPU architecture implementation
- **gpu_kernel.cu**: CUDA kernels (optional)
- Manages compute units, thread scheduling, and execution

### CPU Scheduler (`cpu/`)
- **scheduler.hpp/cpp**: Task scheduler with CPU constraints
- Enforces resource caps (threads, memory, power)
- Implements scheduling algorithms

### Memory Management (`memory/`)
- **memory_manager.hpp/cpp**: Unified memory allocation
- Tracks CPU and GPU memory usage
- Provides deallocation and utilization metrics

## Data Flow

```
Task Creation
      ↓
  Scheduler (CPU Constraints)
      ↓
  Resource Allocation
      ↓
  GPU Execution
      ↓
  Memory Management
      ↓
  Result Retrieval
```

## Key Concepts

### Capabilities
Defines the maximum resources available:
- Maximum concurrent threads
- Maximum memory capacity
- Maximum power consumption

### Resource Contention
When GPU demands exceed CPU-imposed caps, the scheduler queues or throttles tasks.

### Memory Unification
Both CPU and GPU access a unified memory pool, managed transparently by the MemoryManager.
