message("hi mom")
set( _DEBUG 0 )
set( libname libplatform )

if( ${MSVC} )
    set(platdir msft_windows)
endif()

file(GLOB_RECURSE ${libname}_c_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} source/*.c ${platdir}/*.c )
if( ${_DEBUG} )
	message("C file: ${${libname}_c_files}" )
endif()

file(GLOB_RECURSE ${libname}_cpp_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} source/*.cpp ${platdir}/*.cpp)
if( ${_DEBUG} )
	message("Cpp file: ${${libname}_cpp_files}" )
endif()

file(GLOB_RECURSE ${libname}_h_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} include/*.h ${platdir}/*.h)
if( ${_DEBUG} )
	message("H file: ${${libname}_h_files}" )
endif()

file(GLOB_RECURSE ${libname}_tests   RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} tests/*.c tests/*.cpp)
if( ${_DEBUG} )
	message("test file: ${${libname}_tests}" )
endif()

# combine the sources
set( ${libname}_source ${${libname}_c_files} ${${libname}_cpp_files} ${${libname}_h_files})

# in IDEs, it is better to reflect the directory layout
# rather then have the source files tree "flattened"
source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES  ${${libname}_source} )

# This creates the library
add_library( ${libname} ${${libname}_source} )

# The library obviously has some include directories
target_include_directories( ${libname} PUBLIC include ${${libname}_extra_includes} )
message("Created library: ${libname}")

# the library may have some unit tests
foreach( srcfilename ${${libname}_tests} )
	get_filename_component( testname ${srcfilename} NAME_WE )
	if( ${_DEBUG} )
		message("Testname: ${testname}")
	endif()
   set( exe_name "${testname}_${libname}" )
   add_executable( ${exe_name} ${srcfilename} )
   source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES  ${srcfilename} )
   target_include_directories( ${exe_name} PUBLIC include )
   target_link_libraries( ${exe_name} ${libname} ${${libname}_extra_libraries} )
   message("unit-test-${libname}: ${exe_name}")
endforeach( srcfilename )


