cmake_minimum_required(VERSION 3.5)
project(custom_image_republisher)

# Default to C++17
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic -Wnon-virtual-dtor -Woverloaded-virtual)
endif()

find_package(ament_cmake_auto REQUIRED)
ament_auto_find_build_dependencies()

# Build republish
ament_auto_add_library(republish_node SHARED
  src/republish.cpp
)
rclcpp_components_register_node(republish_node
  PLUGIN "custom_image_republisher::Republisher"
  EXECUTABLE republish
)

ament_auto_package()
