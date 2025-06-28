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

find_package(ament_cmake_ros REQUIRED)
find_package(pluginlib REQUIRED)
find_package(rclcpp REQUIRED)
find_package(image_transport REQUIRED)

# Build republish
add_executable(republish src/republish.cpp)
ament_target_dependencies(republish
  pluginlib
  rclcpp
  image_transport
)

# Install executables
install(
  TARGETS republish
  RUNTIME DESTINATION lib/${PROJECT_NAME}
)

ament_export_dependencies(rclcpp pluginlib image_transport)

ament_package()
