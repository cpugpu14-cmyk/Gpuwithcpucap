# GPU with CPU Cap

A C++ implementation of GPU architecture with CPU capability management and resource scheduling.

## Overview

This project provides a modular GPU architecture framework that integrates CPU scheduling and resource management. It explores heterogeneous computing patterns where GPU workloads operate under CPU-imposed constraints and capabilities.

## Project Structure

```
Gpuwithcpucap/
├── src/
│   ├── gpu/              # GPU core architecture and compute kernels
│   ├── cpu/              # CPU capability and scheduling logic
│   ├── memory/           # Memory management and allocation
│   ├── scheduler/        # Task scheduling and resource allocation
│   └── common/           # Shared utilities and helpers
├── include/              # Header files (.h/.hpp)
├── tests/                # Unit and integration tests
├── benchmarks/           # Performance benchmarks
├── docs/                 # Architecture documentation
├── CMakeLists.txt        # Build configuration
├── .gitignore
└── README.md
```

## Building

### Prerequisites
- C++17 or later
- CMake 3.15+
- CUDA Toolkit (optional, for GPU support)

### Build Steps

```bash
mkdir build
cd build
cmake ..
make
```

### Build Options

```bash
# With GPU support
cmake .. -DENABLE_GPU=ON

# With benchmarks
cmake .. -DENABLE_BENCHMARKS=ON

# Debug build
cmake .. -DCMAKE_BUILD_TYPE=Debug
```

## Running Tests

```bash
cmake --build . --target test
# or
ctest
```

## Usage

Basic example:

```cpp
#include "gpu/gpu_core.hpp"
#include "cpu/scheduler.hpp"

int main() {
    // Initialize GPU
    gpu::GPUCore gpu;
    
    // Create CPU scheduler with resource constraints
    cpu::Scheduler scheduler(cpu::Capabilities::default());
    
    // Schedule work
    auto task = scheduler.allocate_task(/*...params...*/);  
    gpu.execute(task);
    
    return 0;
}
```

See `docs/` for detailed usage documentation.

## Features

- **GPU Architecture**: Modular GPU core with configurable compute units
- **CPU Scheduling**: Constraint-aware task scheduling
- **Memory Management**: Unified memory abstraction for CPU/GPU
- **Resource Allocation**: Dynamic resource cap management
- **Benchmarking**: Performance measurement utilities

## Documentation

- [Architecture Guide](docs/ARCHITECTURE.md)
- [API Reference](docs/API.md)
- [Building & Development](docs/BUILD.md)

## License

MIT License

## Contributing

Contributions welcome! Please see CONTRIBUTING.md for guidelines.
