CMAKE_MINIMUM_REQUIRED (VERSION 2.8)

PROJECT (thread)

SET (dependents "glib-2.0 gthread-2.0")

INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES (sample_pkgs REQUIRED ${dependents})

SET (SRCS
  src/main.cpp
)

INCLUDE_DIRECTORIES (
  ${CMAKE_SOURCE_DIR}/src
  PRIVATE
  ${sample_pkgs_INCLUDE_DIRS}
)

ADD_EXECUTABLE (${PROJECT_NAME} ${SRCS})
TARGET_LINK_LIBRARIES (${PROJECT_NAME} ${sample_pkgs_LDFLAGS})
