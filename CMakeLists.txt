cmake_minimum_required(VERSION 3.20.4)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

## PROJECT
project(example VERSION 1.0.0 LANGUAGES CXX)
set(target test_volume)

find_package(VTK REQUIRED COMPONENTS
  CommonCore
  FiltersCore
  FiltersParallel
)

## TARGET
add_executable(${target} test_volume.cpp)

set(vtk_modules
  VTK::CommonCore
  VTK::FiltersCore
  VTK::FiltersParallel
)

target_link_libraries(${target} PRIVATE ${vtk_modules})

vtk_module_autoinit(
  TARGETS ${target}
  MODULES ${vtk_modules}
)

