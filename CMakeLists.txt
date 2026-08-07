cmake_minimum_required(VERSION 3.16)

project(OpenIndyDefaultPlugin LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Xml SerialPort)

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

set(OI_APP_LOCAL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../openindy" CACHE PATH
    "Local OpenIndy app checkout to auto-deploy this plugin into after each build")

# --- plugin metadata ---
# mirrors the compiler/OS strings OiMetaData::getCompiler()/getOperatingSys()
# (openindy/controller/oimetadata.cpp) report, since PluginCopier::checkPluginMetaData
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
    "${CMAKE_CURRENT_SOURCE_DIR}/metaInfo.json.in"
    "${CMAKE_CURRENT_SOURCE_DIR}/metaInfo.json"
    @ONLY
)

# --- plugin library ---

add_library(OpenIndyDefaultPlugin SHARED
    functions/construct/p_intersectlineline.cpp
    functions/construct/p_intersectlineplane.cpp
    functions/construct/p_intersectplaneplane.cpp
    functions/construct/p_linefrompoints.cpp
    functions/construct/p_planefromplane.cpp
    functions/construct/p_planefrompoints.cpp
    functions/construct/p_pointbetweentwopoints.cpp
    functions/construct/p_pointfrompoints.cpp
    functions/construct/p_spherefrompoints.cpp
    functions/fit/p_bestfitcircle.cpp
    functions/fit/p_bestfitline.cpp
    functions/fit/p_bestfitplane.cpp
    functions/fit/p_bestfitpoint.cpp
    functions/fit/p_bestfitcylinder.cpp
    functions/fit/p_bestfitcylinderfrompoints.cpp
    functions/fit/p_bestfitcircleinplane.cpp
    functions/fit/p_bestfitcircleinplanefrompoints.cpp
    functions/fit/p_bestfitsphere.cpp
    functions/objectTransformation/p_changeradius.cpp
    functions/objectTransformation/p_register.cpp
    functions/objectTransformation/p_translatebyvalue.cpp
    functions/objectTransformation/p_rectifytopoint.cpp
    functions/objectTransformation/p_rectifytovector.cpp
    functions/objectTransformation/p_shift.cpp
    functions/systemTransformation/p_helmert7Param.cpp
    functions/systemTransformation/p_helmert6param.cpp
    exchange/p_oiexchangeascii.cpp
    sensors/laserTracker/pseudoTracker/p_pseudotracker.cpp
    sensors/tachymeter/LeicaGeoCom/p_leicatachymeter.cpp
    simulations/simplePolarMeasurement/simplepolarmeasurement.cpp
    cf/cfutil.cpp
    cf/cffunctiondata.cpp
    cf/configuredfunction.cpp
    cf/functionconfigparser.cpp
    cf/distancebetweentwopoints.cpp
    cf/configuredfunctionconfig.cpp
    cf/cfcontext.cpp
    cf/cfcparameter.cpp
    cf/xyzfilter.cpp
    cf/cfvisitor.cpp
    p_factory.cpp

    functions/construct/p_intersectlineline.h
    functions/construct/p_intersectlineplane.h
    functions/construct/p_intersectplaneplane.h
    functions/construct/p_linefrompoints.h
    functions/construct/p_planefromplane.h
    functions/construct/p_planefrompoints.h
    functions/construct/p_pointbetweentwopoints.h
    functions/construct/p_pointfrompoints.h
    functions/construct/p_spherefrompoints.h
    functions/fit/p_bestfitcircle.h
    functions/fit/p_bestfitline.h
    functions/fit/p_bestfitplane.h
    functions/fit/p_bestfitpoint.h
    functions/fit/p_bestfitcylinder.h
    functions/fit/p_bestfitcylinderfrompoints.h
    functions/fit/p_bestfitcircleinplane.h
    functions/fit/p_bestfitcircleinplanefrompoints.h
    functions/fit/p_bestfitsphere.h
    functions/objectTransformation/p_changeradius.h
    functions/objectTransformation/p_register.h
    functions/objectTransformation/p_translatebyvalue.h
    functions/objectTransformation/p_rectifytopoint.h
    functions/objectTransformation/p_rectifytovector.h
    functions/objectTransformation/p_shift.h
    functions/systemTransformation/p_helmert7Param.h
    functions/systemTransformation/p_helmert6param.h
    exchange/p_oiexchangeascii.h
    sensors/laserTracker/pseudoTracker/p_pseudotracker.h
    sensors/tachymeter/LeicaGeoCom/p_leicatachymeter.h
    simulations/simplePolarMeasurement/simplepolarmeasurement.h
    cf/cfutil.h
    cf/cffunctiondata.h
    cf/configuredfunction.h
    cf/functionconfigparser.h
    cf/distancebetweentwopoints.h
    cf/configuredfunctionconfig.h
    cf/cfcontext.h
    cf/cfcparameter.h
    cf/xyzfilter.h
    cf/cfvisitor.h
    p_factory.h
    treeutil.h
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
        ${CMAKE_CURRENT_SOURCE_DIR}
        simulations/simplePolarMeasurement
        sensors/laserTracker/pseudoTracker
        sensors/tachymeter/LeicaGeoCom
        functions/construct
        functions/fit
        functions/objectTransformation
        functions/systemTransformation
        exchange
        cf
)

target_link_libraries(OpenIndyDefaultPlugin
    PRIVATE
        Qt6::Core
        Qt6::Widgets
        Qt6::Xml
        Qt6::SerialPort
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

if(WIN32)
    set(OI_APP_EXE_NAME "openIndy.exe")
else()
    set(OI_APP_EXE_NAME "openIndy")
endif()

add_custom_command(TARGET OpenIndyDefaultPlugin POST_BUILD
    COMMAND ${CMAKE_COMMAND}
        -DPLUGIN_SO=$<TARGET_FILE:OpenIndyDefaultPlugin>
        -DAPP_DIR=${OI_APP_LOCAL_DIR}/build/bin
        -DOPENINDY_EXE=${OI_APP_LOCAL_DIR}/build/bin/${OI_APP_EXE_NAME}
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
    simulations/simplePolarMeasurement
    sensors/laserTracker/pseudoTracker
    sensors/tachymeter/LeicaGeoCom
    functions/construct
    functions/fit
    functions/objectTransformation
    functions/systemTransformation
    exchange
    cf
)
target_link_libraries(tst_function PRIVATE Qt6::Test OpenIndyDefaultPlugin OpenIndyCore)
add_test(NAME function COMMAND tst_function)

add_executable(tst_loadplugin test/loadplugin/tst_loadplugin.cpp)
target_include_directories(tst_loadplugin PRIVATE test/include)
target_link_libraries(tst_loadplugin PRIVATE Qt6::Test OpenIndyCore)
target_compile_definitions(tst_loadplugin PRIVATE
    PLUGIN_PATH="$<TARGET_FILE:OpenIndyDefaultPlugin>")
add_test(NAME loadplugin COMMAND tst_loadplugin)
add_dependencies(tst_loadplugin OpenIndyDefaultPlugin)

add_executable(tst_oiexchangeascii test/oiexchangeascii/tst_oiexchangeascii.cpp exchange/p_oiexchangeascii.cpp)
target_include_directories(tst_oiexchangeascii PRIVATE test/include exchange)
target_link_libraries(tst_oiexchangeascii PRIVATE Qt6::Test OpenIndyCore)
add_test(NAME oiexchangeascii COMMAND tst_oiexchangeascii)

endif()
