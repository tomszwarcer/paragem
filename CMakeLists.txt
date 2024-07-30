cmake_minimum_required(VERSION 3.9 FATAL_ERROR)
project(gem)

if(NOT TARGET Garfield::Garfield)
  find_package(Garfield REQUIRED)
endif()

add_executable(scint_perev_par scint_perev_par.C)
target_link_libraries(scint_perev_par Garfield::Garfield)
