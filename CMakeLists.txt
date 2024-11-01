cmake_minimum_required (VERSION 3.21)

set(CPOP_VERSION 0.1.0)

project(cpop 
  VERSION ${CPOP_VERSION} 
  DESCRIPTION "A config populator" 
  LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)

if(PROJECT_IS_TOP_LEVEL) 
  set(CMAKE_CXX_STANDARD 23)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
  set(CMAKE_CXX_EXTENSIONS OFF)
endif()

add_library(cpop INTERFACE)
add_library(cpop::cpop ALIAS cpop)

include(GNUInstallDirs)

target_include_directories( cpop
    INTERFACE
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_compile_features(cpop INTERFACE cxx_std_23)

find_package(Boost REQUIRED) # Requires header-only boost::pfr
target_include_directories(cpop INTERFACE ${Boost_INCLUDE_DIRS})

option(CPOP_BUILD_TESTS "Enable building tests." OFF)
option(CPOP_ENABLE_INSTALL "Enable the install target" ON)

option(CPOP_USE_SANITIZERS "Enable sanitizers by adding -fsanitize=address -fno-omit-frame-pointer -fsanitize=undefined flags if available." OFF)
option(CPOP_USE_STATIC_ANALYZERS "Enable static analyzers" OFF)
option(CPOP_DEV_MODE "Set defaults for developing." OFF)

if(CPOP_DEV_MODE)
  set(CPOP_USE_SANITIZERS ON)
  set(CPOP_USE_STATIC_ANALYZERS ON)
  set(CPOP_BUILD_TESTS ON)
  set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # for LSP support
endif()

if(CPOP_USE_SANITIZERS)
  if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
      target_compile_options(cpop INTERFACE $<$<CONFIG:Debug>:-fsanitize=address -fno-omit-frame-pointer -fsanitize=undefined>)
      target_link_libraries(cpop INTERFACE $<$<CONFIG:Debug>:-fsanitize=address -fno-omit-frame-pointer -fsanitize=undefined>)
  else()
    message("Sanitizers are only enabled for gcc and clang")
  endif()
endif()

if(CPOP_USE_STATIC_ANALYZERS)
  set(CPOP_ENABLE_CPPCHECK ON) 
  set(CPOP_ENABLE_INCLUDE_WHAT_YOU_USE ON)
  include(StaticAnalyzers)
endif()

#
# Install cpop
#
if(CPOP_ENABLE_INSTALL)
  include(CMakePackageConfigHelpers)

  install(
      TARGETS cpop
      EXPORT cpopTargets
  )

  install(
      EXPORT cpopTargets
      DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cpop
      NAMESPACE cpop::
  )

  configure_package_config_file(
      ${PROJECT_SOURCE_DIR}/cmake/in/cpopConfig.cmake.in
      cpopConfig.cmake
      INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cpop
  )

  write_basic_package_version_file(
      ${PROJECT_BINARY_DIR}/cpopConfigVersion.cmake
      VERSION ${PROJECT_VERSION}
      COMPATIBILITY AnyNewerVersion
  )

  install(
      FILES
          ${PROJECT_BINARY_DIR}/cpopConfig.cmake
          ${PROJECT_BINARY_DIR}/cpopConfigVersion.cmake
      DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cpop
  )

  install( DIRECTORY ${PROJECT_SOURCE_DIR}/src/
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    FILES_MATCHING 
    PATTERN "*.hpp"
  )
endif()

if(CPOP_BUILD_TESTS)
  add_subdirectory(test)
endif()
