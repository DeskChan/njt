cmake_minimum_required(VERSION 3.7) 

include(ExternalProject)

project(crosscompile)

foreach(platform ${PLATFORMS})
	if(NOT DEFINED ONLY_NATIVE_TOOLCHAIN)
		set(OPT1 "-DCMAKE_TOOLCHAIN_FILE=${PROJECT_SOURCE_DIR}/../toolchains/${platform}.cmake")
	endif()
	if(NOT DEFINED OVERRIDE_JAVA_HOME)
		set(OPT2 "-DJAVA_HOME=${PROJECT_SOURCE_DIR}/../../jdks/${platform}")
	else()
		set(OPT2 "-DJAVA_HOME=${OVERRIDE_JAVA_HOME}")
	endif()
	ExternalProject_Add(native-lib-${platform}
		SOURCE_DIR ${PROJECT_SOURCE_DIR}/..
		PREFIX ${CMAKE_BINARY_DIR}/native-lib-prefix/${platform}
		BINARY_DIR ${CMAKE_BINARY_DIR}/native-lib/${platform}
		INSTALL_COMMAND ""
		CMAKE_ARGS ${OPT1}
		CMAKE_ARGS ${OPT2}
		CMAKE_ARGS "-DPLATFORM_NAME=${platform}"
	)
	add_custom_command(TARGET native-lib-${platform} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/native-lib/${platform}/lib*-${platform}.* ${CMAKE_BINARY_DIR}
	)
endforeach(platform)
