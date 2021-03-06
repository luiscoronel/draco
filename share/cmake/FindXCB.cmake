# - FindXCB
# Modified for Draco
# Copyright 2015 Valve Coporation

find_package(PkgConfig)

#option(USE_XINPUT "Use xcb-xinput" OFF)
if(NOT XCB_FIND_COMPONENTS)
    set(XCB_FIND_COMPONENTS
        xcb
        xcb-util
        xcb-randr
        xcb-ewmh
        xcb-icccm
        xcb-image
        xcb-composite
        xcb-damage
        xcb-shm
        )
#    if(USE_XINPUT)
#        message("-- Using XINPUT")
#        set(XCB_FIND_COMPONENTS "${XCB_FIND_COMPONENTS};xcb-xinput")
#    endif()
endif()
#message("-- Find XCB Components: ${XCB_FIND_COMPONENTS}")

include(FindPackageHandleStandardArgs)
set(XCB_FOUND true)
set(XCB_INCLUDE_DIRS "")
set(XCB_LIBRARIES "")
foreach(comp ${XCB_FIND_COMPONENTS})
    # component name
    string(TOUPPER ${comp} compname)
    string(REPLACE "-" "_" compname ${compname})
    # header name
    string(REPLACE "xcb-" "" headername xcb/${comp}.h)
    # library name
    set(libname ${comp})
    # fixes
    if(comp STREQUAL "xcb-ewmh" OR
       comp STREQUAL "xcb-icccm" OR
       comp STREQUAL "xcb-image" OR
       comp STREQUAL "xcb-util")
        string(REPLACE "-" "_" headername xcb/${comp}.h)
    endif()

#    message("check for ${comp} ${compname} ${headername} ${libname}")

    pkg_check_modules(PC_${comp} QUIET ${comp})

    find_path(${compname}_INCLUDE_DIR NAMES ${headername}
        HINTS
        ${PC_${comp}_INCLUDEDIR}
        ${PC_${comp}_INCLUDE_DIRS}
        )

    find_library(${compname}_LIBRARY NAMES ${libname}
        HINTS
        ${PC_${comp}_LIBDIR}
        ${PC_${comp}_LIBRARY_DIRS}
        )

    find_package_handle_standard_args(${comp}
        FOUND_VAR ${comp}_FOUND
        REQUIRED_VARS ${compname}_INCLUDE_DIR ${compname}_LIBRARY)
    mark_as_advanced(${compname}_INCLUDE_DIR ${compname}_LIBRARY)

    list(APPEND XCB_INCLUDE_DIRS ${${compname}_INCLUDE_DIR})
    list(APPEND XCB_LIBRARIES ${${compname}_LIBRARY})

    if(NOT ${comp}_FOUND)
        set(XCB_FOUND false)
    endif()
endforeach()

list(REMOVE_DUPLICATES XCB_INCLUDE_DIRS)
