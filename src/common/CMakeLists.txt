# Common library with shared runtime infrastructure
add_library(firebolt-common SHARED
    src/Logging.cpp
    src/Utils.cpp
    src/OperationState.cpp
    src/SubscriptionState.cpp
    src/FireboltObject.cpp
    src/StateFactory.cpp
)

target_include_directories(firebolt-common PUBLIC
    include
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(firebolt-common PUBLIC
    nlohmann_json::nlohmann_json
)
