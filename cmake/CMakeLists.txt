cmake_minimum_required (VERSION 2.6.2)
project (RealDashCanServer)

set(DCMAKE default.cmake)
set(CMDIR ../cmake)

# Source location
set(main_cpp_base ../src)

# Include relevant CMAKE file (default, mingw, pandora etc.)
if(TARGET)
    set(TCMAKE ${TARGET}.cmake)
    if(EXISTS ${CMDIR}/${TCMAKE})
        message("CMake files for ${TARGET} found, using ${TCMAKE}")
        include(${TCMAKE})
    else()
        message("CMake files for ${TARGET} not found, using ${DCMAKE}")
        include(${DCMAKE})
    endif()
else()
    message("TARGET not passed, using ${DCMAKE}")
    include(${DCMAKE})
endif(TARGET)


# Source files
set(src_main
    "${main_cpp_base}/can_server.c"
    )

# Group files into relevant filters for Visual Studio build
SOURCE_GROUP(src FILES ${src_main})

set(SRCS
    ${src_main}
)

add_executable(can_server ${SRCS})
