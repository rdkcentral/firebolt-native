# FindAsio.cmake - Find standalone Asio
#
# This module defines:
#  Asio_FOUND - True if standalone Asio is found
#  Asio_INCLUDE_DIRS - Include directories for Asio
#  Asio_VERSION - Version of Asio (if available)
#
# Note: This module only supports standalone Asio. Boost.Asio is not supported
# because the WebSocket transport unconditionally uses standalone Asio macros
# (ASIO_STANDALONE, WEBSOCKETPP_STANDALONE_ASIO) and includes <asio.hpp>.

find_path(Asio_INCLUDE_DIR
    NAMES asio.hpp asio/io_service.hpp
    PATHS
        /usr/include
        /usr/local/include
        /opt/homebrew/include
        /opt/homebrew/Cellar/asio/*/include
        /opt/local/include
        /usr/include/asio
        /usr/local/include/asio
    PATH_SUFFIXES
        asio
)

if(Asio_INCLUDE_DIR)
    set(Asio_INCLUDE_DIRS ${Asio_INCLUDE_DIR})
    set(Asio_FOUND TRUE)
    
    # Try to get version from asio/version.hpp if available
    if(EXISTS "${Asio_INCLUDE_DIR}/asio/version.hpp")
        file(READ "${Asio_INCLUDE_DIR}/asio/version.hpp" _version_file)
        if(_version_file MATCHES "ASIO_VERSION \"([0-9]+)\"")
            set(Asio_VERSION ${CMAKE_MATCH_1})
        endif()
    endif()
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Asio
    REQUIRED_VARS Asio_INCLUDE_DIRS
    VERSION_VAR Asio_VERSION
)
