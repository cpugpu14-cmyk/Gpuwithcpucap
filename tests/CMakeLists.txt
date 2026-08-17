# Test configuration

add_executable(test_gpu_core gpu/test_gpu_core.cpp)
target_link_libraries(test_gpu_core PRIVATE gpuwithcpucap)
add_test(NAME GpuCoreTest COMMAND test_gpu_core)

add_executable(test_scheduler cpu/test_scheduler.cpp)
target_link_libraries(test_scheduler PRIVATE gpuwithcpucap)
add_test(NAME SchedulerTest COMMAND test_scheduler)
