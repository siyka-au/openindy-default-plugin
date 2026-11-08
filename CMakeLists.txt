cmake_minimum_required(VERSION 3.16)

project(OpenIndyDefaultPlugin LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Xml)

# --- Core dependency ---

include(FetchContent)

set(OI_CORE_LOCAL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../openindy-core" CACHE PATH
    "Local OpenIndy-Core checkout to build against instead of fetching from git")

if(EXISTS "${OI_CORE_LOCAL_DIR}/CMakeLists.txt")
    FetchContent_Declare(OpenIndyCore SOURCE_DIR "${OI_CORE_LOCAL_DIR}")
else()
    FetchContent_Declare(OpenIndyCore
        GIT_REPOSITORY git@github.com:siyka-au/openindy-core.git
        GIT_TAG        master
    )
endif()
FetchContent_MakeAvailable(OpenIndyCore)

# OpenIndyCore publishes this from include/plugin/plugin_abi.h. A core too old
# to define it cannot be checked for ABI compatibility at all, so fail loudly
# here rather than shipping a plugin whose metadata claims nothing.
if(NOT DEFINED OI_PLUGIN_ABI_VERSION OR OI_PLUGIN_ABI_VERSION STREQUAL "")
    message(FATAL_ERROR
        "OpenIndyCore did not publish OI_PLUGIN_ABI_VERSION. "
        "Update openindy-core to a revision that declares it in include/plugin/plugin_abi.h.")
endif()

set(OI_APP_LOCAL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../openindy" CACHE PATH
    "Local OpenIndy app checkout to auto-deploy this plugin into after each build")

# --- plugin metadata ---
# mirrors the compiler/OS strings OiMetaData::getCompiler()/getOperatingSys()
# (openindy/controller/oi_meta_data.cpp) report, since PluginCopier::checkPluginMetaData
# compares them literally against this file's contents at import time

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(OI_PLUGIN_COMPILER "clang 64bit")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(OI_PLUGIN_COMPILER "minGW")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(OI_PLUGIN_COMPILER "MVSC 64bit")
else()
    set(OI_PLUGIN_COMPILER "")
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(OI_PLUGIN_OS "linux")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(OI_PLUGIN_OS "windows")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(OI_PLUGIN_OS "mac os")
else()
    set(OI_PLUGIN_OS "")
endif()

set(COMPILER "${OI_PLUGIN_COMPILER}")
set(OS "${OI_PLUGIN_OS}")

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/meta_info.json.in"
    "${CMAKE_CURRENT_BINARY_DIR}/meta_info.json"
    @ONLY
)

# The metadata is compiled into the library as a string and handed out by the
# oi_plugin_metadata_json() entry point (OI_DECLARE_PLUGIN_WITH_METADATA), so
# the host can read it once the library is loaded.
file(READ "${CMAKE_CURRENT_BINARY_DIR}/meta_info.json" OI_PLUGIN_METADATA_JSON)
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/meta_info.h.in"
    "${CMAKE_CURRENT_BINARY_DIR}/generated/meta_info.h"
    @ONLY
)

# --- plugin library ---

add_library(OpenIndyDefaultPlugin SHARED
    functions/nominal/p_nominal_point.cpp
    functions/construct/p_intersect_line_line.cpp
    functions/construct/p_intersect_line_plane.cpp
    functions/construct/p_intersect_plane_plane.cpp
    functions/construct/p_plane_from_plane.cpp
    functions/construct/p_line_from_plane_normal.cpp
    functions/construct/p_point_from_plane_origin.cpp
    functions/construct/p_average_from_points.cpp
    functions/construct/p_point_between_two_points.cpp
    functions/construct/p_point_from_points.cpp
    functions/fit/p_best_fit_circle.cpp
    functions/fit/p_best_fit_line.cpp
    functions/fit/p_best_fit_plane.cpp
    functions/fit/p_best_fit_point.cpp
    functions/fit/p_best_fit_cylinder.cpp
    functions/fit/p_best_fit_circle_in_plane.cpp
    functions/fit/p_best_fit_sphere.cpp
    functions/object_transformation/p_change_radius.cpp
    functions/object_transformation/p_register.cpp
    functions/object_transformation/p_translate_by_value.cpp
    functions/object_transformation/p_rectify_to_point.cpp
    functions/object_transformation/p_rectify_to_vector.cpp
    functions/object_transformation/p_shift.cpp
    functions/system_transformation/p_helmert7_param.cpp
    functions/system_transformation/p_helmert6_param.cpp
    exchange/p_oi_exchange_ascii.cpp
    sensors/common/tracker_error_model.cpp
    sensors/laser_tracker/pseudo_tracker/p_pseudo_tracker.cpp
    simulations/simple_polar_measurement/simple_polar_measurement.cpp
    cf/cf_util.cpp
    cf/cf_function_data.cpp
    cf/configured_function.cpp
    cf/function_config_parser.cpp
    cf/distance_between_two_points.cpp
    cf/configured_function_config.cpp
    cf/cf_context.cpp
    cf/cfc_parameter.cpp
    cf/xyz_filter.cpp
    cf/cf_visitor.cpp
    p_factory.cpp

    functions/nominal/p_nominal_point.h
    functions/construct/p_intersect_line_line.h
    functions/construct/p_intersect_line_plane.h
    functions/construct/p_intersect_plane_plane.h
    functions/construct/p_plane_from_plane.h
    functions/construct/p_line_from_plane_normal.h
    functions/construct/p_point_from_plane_origin.h
    functions/construct/p_average_from_points.h
    functions/construct/p_point_between_two_points.h
    functions/construct/p_point_from_points.h
    functions/fit/p_best_fit_circle.h
    functions/fit/p_best_fit_line.h
    functions/fit/p_best_fit_plane.h
    functions/fit/p_best_fit_point.h
    functions/fit/p_best_fit_cylinder.h
    functions/fit/p_best_fit_circle_in_plane.h
    functions/fit/p_best_fit_sphere.h
    functions/object_transformation/p_change_radius.h
    functions/object_transformation/p_register.h
    functions/object_transformation/p_translate_by_value.h
    functions/object_transformation/p_rectify_to_point.h
    functions/object_transformation/p_rectify_to_vector.h
    functions/object_transformation/p_shift.h
    functions/system_transformation/p_helmert7_param.h
    functions/system_transformation/p_helmert6_param.h
    exchange/p_oi_exchange_ascii.h
    sensors/laser_tracker/pseudo_tracker/p_pseudo_tracker.h
    simulations/simple_polar_measurement/simple_polar_measurement.h
    cf/cf_util.h
    cf/cf_function_data.h
    cf/configured_function.h
    cf/function_config_parser.h
    cf/distance_between_two_points.h
    cf/configured_function_config.h
    cf/cf_context.h
    cf/cfc_parameter.h
    cf/xyz_filter.h
    cf/cf_visitor.h
    p_factory.h
    tree_util.h
)

set_target_properties(OpenIndyDefaultPlugin PROPERTIES
    OUTPUT_NAME p_defaultPlugin
    PREFIX "lib"
)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(OpenIndyDefaultPlugin PRIVATE -Wall -Wextra -Wpedantic)
endif()

target_include_directories(OpenIndyDefaultPlugin
    PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}/generated
        ${CMAKE_CURRENT_SOURCE_DIR}
        simulations/simple_polar_measurement
        sensors/common
        sensors/laser_tracker/pseudo_tracker
        functions/nominal
        functions/construct
        functions/fit
        functions/object_transformation
        functions/system_transformation
        exchange
        cf
)

target_link_libraries(OpenIndyDefaultPlugin
    PRIVATE
        Qt6::Core
        Qt6::Widgets
        Qt6::Xml
        OpenIndyCore
)

# --- deploy into a sibling openIndy app build, and tests ---
# Both only make sense when this repo is the top-level build. When pulled in
# as a dependency (e.g. openindy's own CMakeLists FetchContent-ing this repo
# so the plugin is always built alongside the app), the app's own CMakeLists
# does the packaging step itself once it - not this plugin target - has
# actually finished linking, and building this repo's standalone test suite
# as a side effect of building the app would just be unwanted noise.

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)

add_custom_command(TARGET OpenIndyDefaultPlugin POST_BUILD
    COMMAND ${CMAKE_COMMAND}
        -DPLUGIN_SO=$<TARGET_FILE:OpenIndyDefaultPlugin>
        -DAPP_DIR=${OI_APP_LOCAL_DIR}/build/bin
        -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/DeployToApp.cmake
    VERBATIM
)

# --- tests ---

enable_testing()

find_package(Qt6 REQUIRED COMPONENTS Test)

add_executable(tst_function test/function/tst_function.cpp)
target_include_directories(tst_function PRIVATE
    test/include
    ${CMAKE_CURRENT_SOURCE_DIR}
    simulations/simple_polar_measurement
    sensors/common
    sensors/laser_tracker/pseudo_tracker
    functions/nominal
    functions/construct
    functions/fit
    functions/object_transformation
    functions/system_transformation
    exchange
    cf
)
target_link_libraries(tst_function PRIVATE Qt6::Test OpenIndyDefaultPlugin OpenIndyCore)
add_test(NAME function COMMAND tst_function)

add_executable(tst_loadplugin test/load_plugin/tst_load_plugin.cpp)
target_include_directories(tst_loadplugin PRIVATE test/include)
target_link_libraries(tst_loadplugin PRIVATE Qt6::Test OpenIndyCore OpenIndyMath)
target_compile_definitions(tst_loadplugin PRIVATE
    PLUGIN_PATH="$<TARGET_FILE:OpenIndyDefaultPlugin>")
# core's Qt-free oi-cli runs a plane fit through this plugin, saves and reloads
# the protobuf
if(TARGET oi-cli)
    add_test(NAME oi_cli_default_plugin
        COMMAND oi-cli --selftest --plugin $<TARGET_FILE:OpenIndyDefaultPlugin>)
    set_tests_properties(oi_cli_default_plugin PROPERTIES SKIP_RETURN_CODE 77)
    add_dependencies(oi-cli OpenIndyDefaultPlugin)
endif()
add_test(NAME loadplugin COMMAND tst_loadplugin)
add_dependencies(tst_loadplugin OpenIndyDefaultPlugin)

add_executable(tst_oiexchangeascii test/oi_exchange_ascii/tst_oi_exchange_ascii.cpp exchange/p_oi_exchange_ascii.cpp)
target_include_directories(tst_oiexchangeascii PRIVATE test/include exchange)
target_link_libraries(tst_oiexchangeascii PRIVATE Qt6::Test OpenIndyCore)
add_test(NAME oiexchangeascii COMMAND tst_oiexchangeascii)

add_executable(tst_trackererrormodel
    test/tracker_error_model/tst_tracker_error_model.cpp
    sensors/common/tracker_error_model.cpp
)
target_include_directories(tst_trackererrormodel PRIVATE test/include sensors/common)
target_link_libraries(tst_trackererrormodel PRIVATE Qt6::Test OpenIndyCore OpenIndyMath)
add_test(NAME trackererrormodel COMMAND tst_trackererrormodel)

add_executable(tst_simulatedscene
    test/tracker_error_model/tst_simulated_scene.cpp
    sensors/common/tracker_error_model.cpp
    functions/fit/p_best_fit_plane.cpp
)
target_include_directories(tst_simulatedscene PRIVATE test/include sensors/common functions/fit)
target_link_libraries(tst_simulatedscene PRIVATE Qt6::Test OpenIndyCore OpenIndyMath)
add_test(NAME simulatedscene COMMAND tst_simulatedscene)

add_executable(tst_calibratedconstellation
    test/calibrated_constellation/tst_calibrated_constellation.cpp
    sensors/common/tracker_error_model.cpp
)
target_include_directories(tst_calibratedconstellation PRIVATE
    test/include
    sensors/common
    functions/system_transformation
)
target_link_libraries(tst_calibratedconstellation PRIVATE Qt6::Test OpenIndyDefaultPlugin OpenIndyCore OpenIndyMath)
add_test(NAME calibratedconstellation COMMAND tst_calibratedconstellation)

add_executable(tst_collectionbinding
    test/point_collection/tst_collection_binding.cpp
    functions/fit/p_best_fit_plane.cpp
    functions/construct/p_average_from_points.cpp
)
target_include_directories(tst_collectionbinding PRIVATE test/include functions/construct functions/fit)
target_link_libraries(tst_collectionbinding PRIVATE Qt6::Test OpenIndyCore OpenIndyMath)
add_test(NAME collectionbinding COMMAND tst_collectionbinding)

add_executable(tst_derivedfeatures
    test/derived_features/tst_derived_features.cpp
    functions/construct/p_line_from_plane_normal.cpp
    functions/construct/p_point_from_plane_origin.cpp
    functions/construct/p_average_from_points.cpp
)
target_include_directories(tst_derivedfeatures PRIVATE test/include functions/construct)
target_link_libraries(tst_derivedfeatures PRIVATE Qt6::Test OpenIndyCore OpenIndyMath)
add_test(NAME derivedfeatures COMMAND tst_derivedfeatures)

add_executable(tst_ownedplaneaccessors
    test/owned_plane_accessors/tst_owned_plane_accessors.cpp
    functions/construct/p_line_from_plane_normal.cpp
    functions/construct/p_point_from_plane_origin.cpp
)
target_include_directories(tst_ownedplaneaccessors PRIVATE test/include functions/construct)
target_link_libraries(tst_ownedplaneaccessors PRIVATE Qt6::Test OpenIndyCore OpenIndyMath)
add_test(NAME ownedplaneaccessors COMMAND tst_ownedplaneaccessors)

add_executable(tst_measuredfit
    test/measured_fit/tst_measured_fit.cpp
    functions/fit/p_best_fit_plane.cpp
    functions/fit/p_best_fit_cylinder.cpp
)
target_include_directories(tst_measuredfit PRIVATE test/include functions/fit)
target_link_libraries(tst_measuredfit PRIVATE Qt6::Test OpenIndyCore OpenIndyMath)
add_test(NAME measuredfit COMMAND tst_measuredfit)

add_executable(tst_pipelinefilter
    test/pipeline_filter/tst_pipeline_filter.cpp
    functions/fit/p_best_fit_plane.cpp
    functions/fit/p_best_fit_point.cpp
)
target_include_directories(tst_pipelinefilter PRIVATE test/include functions/fit)
target_link_libraries(tst_pipelinefilter PRIVATE Qt6::Test OpenIndyCore OpenIndyMath)
add_test(NAME pipelinefilter COMMAND tst_pipelinefilter)

endif()
