message("hi mom")

set( libname libplatform )

if( ${MSVC} )
    set(platdir msft_windows)
endif()

file(GLOB_RECURSE ${libname}_c_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} source/*.c ${platdir}/*.c )
file(GLOB_RECURSE ${libname}_cpp_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} source/*.cpp ${platdir}/*.cpp)
file(GLOB_RECURSE ${libname}_h_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} include/*.h ${platdir}/*.h)
file(GLOB_RECURSE ${libname}_tests   RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} tests/*.c tests/*.cpp)

set( ${libname}_source ${${libname}_c_files} ${${libname}_cpp_files} ${${libname}_h_files})

source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES  ${${libname}_source} )

message("ln=${libname}")
message("${libname}_source")
message("${${libname}_source}")
add_library( ${libname} ${${libname}_source} )
target_include_directories( ${libname} PUBLIC include )

foreach( srcfilename ${${libname}_tests}} )
   get_filename_component( testname ${srcfilename} NAME_WE )
   message("tn=${testname}")
   
   add_executable( ${libname}_${testname} ${srcfilename} )
endforeach( srcfilename )


