# FindWebSocketPP.cmake - Find WebSocket++
#
# This module defines:
#  WebSocketPP_FOUND - True if WebSocket++ is found
#  WebSocketPP_INCLUDE_DIRS - Include directories for WebSocket++
#  WebSocketPP_VERSION - Version of WebSocket++

# WebSocket++ is a header-only library, so we only need to find the headers
find_path(WebSocketPP_INCLUDE_DIR
    NAMES websocketpp/config/asio.hpp
    PATHS
        /usr/include
        /usr/local/include
        /opt/homebrew/include
        /opt/homebrew/Cellar/websocketpp/*/include
        /opt/local/include
        /usr/include/websocketpp
        /usr/local/include/websocketpp
    PATH_SUFFIXES
        websocketpp
)

if(WebSocketPP_INCLUDE_DIR)
    set(WebSocketPP_INCLUDE_DIRS ${WebSocketPP_INCLUDE_DIR})
    set(WebSocketPP_FOUND TRUE)
    
    # Try to get version from config file if available
    if(EXISTS "${WebSocketPP_INCLUDE_DIR}/websocketpp/version.hpp")
        file(READ "${WebSocketPP_INCLUDE_DIR}/websocketpp/version.hpp" _version_file)
        if(_version_file MATCHES "MAJOR_VERSION ([0-9]+)")
            set(WebSocketPP_VERSION_MAJOR ${CMAKE_MATCH_1})
        endif()
        if(_version_file MATCHES "MINOR_VERSION ([0-9]+)")
            set(WebSocketPP_VERSION_MINOR ${CMAKE_MATCH_1})
        endif()
        if(_version_file MATCHES "PATCH_VERSION ([0-9]+)")
            set(WebSocketPP_VERSION_PATCH ${CMAKE_MATCH_1})
        endif()
        if(DEFINED WebSocketPP_VERSION_MAJOR AND DEFINED WebSocketPP_VERSION_MINOR AND DEFINED WebSocketPP_VERSION_PATCH)
            set(WebSocketPP_VERSION "${WebSocketPP_VERSION_MAJOR}.${WebSocketPP_VERSION_MINOR}.${WebSocketPP_VERSION_PATCH}")
        endif()
    endif()
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(WebSocketPP
    REQUIRED_VARS WebSocketPP_INCLUDE_DIRS
    VERSION_VAR WebSocketPP_VERSION
)
