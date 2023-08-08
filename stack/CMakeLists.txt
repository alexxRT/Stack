add_library (stack STATIC src/stack.cpp src/stack_debug.cpp)

target_include_directories(stack PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib)