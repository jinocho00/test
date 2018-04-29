CMAKE_MINIMUM_REQUIRED (VERSION 2.8)

PROJECT (gsource)

SET (dependents "glib-2.0")

INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES (sample_pkgs REQUIRED ${dependents})

SET (SRCS
  src/main.cpp
)

include_directories (
  ${CMAKE_SOURCE_DIR}/src
  PRIVATE
  ${sample_pkgs_INCLUDE_DIRS}
)

add_executable (${PROJECT_NAME} ${SRCS})
target_link_libraries (${PROJECT_NAME} ${sample_pkgs_LDFLAGS})

