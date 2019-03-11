# Default CMake Setup. Used for Mac OS x builds
EXECUTE_PROCESS( COMMAND uname -m COMMAND tr -d '\n' OUTPUT_VARIABLE ARCHITECTURE )
EXECUTE_PROCESS( COMMAND xcode-select -p COMMAND tr -d '\n' OUTPUT_VARIABLE XCODE_DEV_PATH )
message( STATUS "Architecture: ${ARCHITECTURE}" )

# For INTeL x86/x64 Mac OS X 10.5+
set(CMAKE_CXX_FLAGS " -O2 -force_cpusubtype_ALL -arch ${ARCHITECTURE} -m32 -isysroot ${XCODE_DEV_PATH}/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk -mmacosx-version-min=10.7")

include_directories("${lib_base}/include/" "${XCODE_DEV_PATH}/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/usr/X11/include/")

set(CMAKE_CXX_FLAGS "-stdlib=libc++")
