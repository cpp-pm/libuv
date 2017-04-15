cmake_minimum_required(VERSION 2.8)

project(libuv)

include_directories(include src)
set(SOURCES
  src/fs-poll.c
  src/inet.c
  src/loop-watcher.c
  src/threadpool.c
  src/timer.c
  src/uv-common.c
  src/version.c)

if(WIN32)
  add_definitions(-DWIN32_LEAN_AND_MEAN -D_WIN32_WINNT=0x0600 -D_CRT_SECURE_NO_WARNINGS -Dsnprintf=_snprintf)
  include_directories(src/win)
  set(SOURCES ${SOURCES}
    src/win/async.c
    src/win/core.c
    src/win/detect-wakeup.c
    src/win/dl.c
    src/win/error.c
    src/win/fs.c
    src/win/fs-event.c
    src/win/getaddrinfo.c
    src/win/getnameinfo.c
    src/win/handle.c
    src/win/pipe.c
    src/win/poll.c
    src/win/process.c
    src/win/process-stdio.c
    src/win/req.c
    src/win/signal.c
    src/win/stream.c
    src/win/tcp.c
    src/win/thread.c
    src/win/tty.c
    src/win/udp.c
    src/win/util.c
    src/win/winapi.c
    src/win/winsock.c)
else()
  if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    add_definitions(-D_GNU_SOURCE -D_POSIX_C_SOURCE=200112)
    set(SOURCES ${SOURCES}
      src/unix/linux-syscalls.c
      src/unix/linux-core.c
      src/unix/linux-inotify.c
      src/unix/proctitle.c)
  elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    add_definitions(-D_DARWIN_USE_64_BIT_INODE=1 -D_DARWIN_UNLIMITED_SELECT=1)
    set(SOURCES ${SOURCES}
      src/unix/darwin.c
      src/unix/darwin-proctitle.c
      src/unix/fsevents.c
      src/unix/kqueue.c
      src/unix/proctitle.c)
  elseif(${CMAKE_SYSTEM_NAME} STREQUAL "FreeBSD")
    set(SOURCES ${SOURCES}
      src/unix/freebsd.c)
  elseif(${CMAKE_SYSTEM_NAME} STREQUAL "OpenBSD")
    set(SOURCES ${SOURCES}
      src/unix/openbsd.c)
  elseif(${CMAKE_SYSTEM_NAME} STREQUAL "NetBSD")
    set(SOURCES ${SOURCES}
      src/unix/kqueue.c
      src/unix/netbsd.c)
  elseif(${CMAKE_SYSTEM_NAME} STREQUAL "SunOS")
    add_definitions(-D__EXTENSIONS__ -D_XOPEN_SOURCE=500)
    set(SOURCES ${SOURCES}
      src/unix/sunos.c)
  endif()

  include_directories(src/unix)
  set(SOURCES ${SOURCES}
    src/unix/async.c
    src/unix/core.c
    src/unix/dl.c
    src/unix/fs.c
    src/unix/getaddrinfo.c
    src/unix/getnameinfo.c
    src/unix/loop.c
    src/unix/pipe.c
    src/unix/poll.c
    src/unix/process.c
    src/unix/signal.c
    src/unix/stream.c
    src/unix/tcp.c
    src/unix/thread.c
    src/unix/tty.c
    src/unix/udp.c)
endif()

add_library(uv ${SOURCES})

find_package(Threads)

if(WIN32)
  target_link_libraries(uv ws2_32 psapi iphlpapi shell32 userenv)
else()
  if (NOT APPLE)
    include(CheckCXXSymbolExists)
    check_cxx_symbol_exists("clock_gettime" "time.h" _has_clock_gettime)
    if (NOT _has_clock_gettime)
      set(extra_libs rt)
    endif()
  endif()
  set(extra_libs ${extra_libs} ${CMAKE_DL_LIBS})
  target_link_libraries(uv PUBLIC Threads::Threads ${extra_libs})
endif()

if (BUILD_SHARED_LIBS)
  target_compile_definitions(
    uv
    PRIVATE BUILDING_UV_SHARED)
endif()

# Layout. This works for all platforms:
#   * <prefix>/lib/cmake/<PROJECT-NAME>
#   * <prefix>/lib/
#   * <prefix>/include/
set(config_install_dir "lib/cmake/${PROJECT_NAME}")
set(include_install_dir "include")

set(generated_dir "${CMAKE_CURRENT_BINARY_DIR}/generated")

message("Building ${PROJECT_NAME}")

# Configuration
set(version_config "${generated_dir}/${PROJECT_NAME}ConfigVersion.cmake")
set(project_config "${generated_dir}/${PROJECT_NAME}Config.cmake")
set(targets_export_name "${PROJECT_NAME}Targets")
set(namespace "${PROJECT_NAME}::")

install(TARGETS uv
  EXPORT ${targets_export_name}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  INCLUDES DESTINATION include)

install(DIRECTORY include/ DESTINATION include)

# Include module with fuction 'write_basic_package_version_file'
include(CMakePackageConfigHelpers)

set(PROJECT_VERSION 2.0.0)

# Configure '<PROJECT-NAME>ConfigVersion.cmake'
# Note: PROJECT_VERSION is used as a VERSION
write_basic_package_version_file(
  "${version_config}" VERSION ${PROJECT_VERSION} COMPATIBILITY AnyNewerVersion
  )

# Configure '<PROJECT-NAME>Config.cmake'
# Use variables:
#   * targets_export_name
#   * PROJECT_NAME
configure_package_config_file(
  "cmake/Config.cmake.in"
  "${project_config}"
  INSTALL_DESTINATION "${config_install_dir}"
  )

# Config
#   * <prefix>/lib/cmake/libuv/libuvConfig.cmake
#   * <prefix>/lib/cmake/libuv/libuvConfigVersion.cmake
install(
  FILES "${project_config}" "${version_config}"
  DESTINATION "${config_install_dir}"
  )

# Config
#   * <prefix>/lib/cmake/libuv/libuvTargets.cmake
install(
  EXPORT "${targets_export_name}"
  NAMESPACE "${namespace}"
  DESTINATION "${config_install_dir}"
  )
