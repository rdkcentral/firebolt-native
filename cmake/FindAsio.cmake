# FindAsio.cmake - Find Asio (standalone or Boost.Asio)
#
# This module defines:
#  Asio_FOUND - True if Asio is found
#  Asio_INCLUDE_DIRS - Include directories for Asio
#  Asio_LIBRARIES - Libraries to link for Asio (empty for standalone, Boost::system for Boost.Asio)
#  Asio_VERSION - Version of Asio
#  Asio_IS_STANDALONE - True if standalone Asio is found, false if Boost.Asio is used

# First try to find standalone Asio (header-only)
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
    set(Asio_LIBRARIES "")
    set(Asio_IS_STANDALONE TRUE)
    set(Asio_FOUND TRUE)
    
    # Try to get version from asio/version.hpp if available
    if(EXISTS "${Asio_INCLUDE_DIR}/asio/version.hpp")
        file(READ "${Asio_INCLUDE_DIR}/asio/version.hpp" _version_file)
        if(_version_file MATCHES "ASIO_VERSION \"([0-9]+)\"")
            set(Asio_VERSION ${CMAKE_MATCH_1})
        endif()
    endif()
else()
    # If standalone Asio not found, try Boost.Asio
    find_package(Boost COMPONENTS system QUIET)
    if(Boost_FOUND)
        # Boost.Asio is part of Boost, we need to find the asio headers in Boost
        find_path(Asio_INCLUDE_DIR
            NAMES boost/asio.hpp
            PATHS
                ${Boost_INCLUDE_DIRS}
                /usr/include
                /usr/local/include
                /opt/homebrew/include
                /opt/homebrew/Cellar/boost/*/include
                /opt/local/include
        )
        
        if(Asio_INCLUDE_DIR)
            set(Asio_INCLUDE_DIRS ${Asio_INCLUDE_DIR})
            set(Asio_LIBRARIES Boost::system)
            set(Asio_IS_STANDALONE FALSE)
            set(Asio_FOUND TRUE)
            set(Asio_VERSION ${Boost_VERSION})
        endif()
    endif()
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Asio
    REQUIRED_VARS Asio_INCLUDE_DIRS
    VERSION_VAR Asio_VERSION
)
