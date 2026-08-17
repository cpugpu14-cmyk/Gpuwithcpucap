# GPU Architecture Library

add_library(gpuwithcpucap
    gpu/gpu_core.cpp
    cpu/scheduler.cpp
    memory/memory_manager.cpp
    common/utils.cpp
)

target_include_directories(gpuwithcpucap
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

if(ENABLE_GPU)
    enable_language(CUDA)
    target_sources(gpuwithcpucap PRIVATE gpu/gpu_kernel.cu)
    target_link_libraries(gpuwithcpucap PUBLIC cudart)
endif()
