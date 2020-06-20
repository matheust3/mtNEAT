# CMakeList.txt: projeto de CMake para NEAT3, incluir origem e definir
# lógica específica a um projeto aqui.
#
cmake_minimum_required (VERSION 3.8)

set(pythonLib False)

if(${CMAKE_SYSTEM_NAME} STREQUAL  "Windows")

	find_package(MATLAB)
	if(Matlab_FOUND)
		INCLUDE_DIRECTORIES(${Matlab_INCLUDE_DIRS})
		link_directories(${Matlab_LIBRARIES})

		message(STATUS "COM MATLAB")
		message("copilando neat.exe")
		# Adicione a origem ao executável deste projeto.
		add_executable (NEAT "NEAT.cpp" "NEAT.h" "species.cpp" "biocenosis.cpp" "gene.cpp" "genome.cpp" "innovationGenerator.cpp" "node.cpp" )
		if(${CMAKE_SYSTEM_NAME} STREQUAL  "Windows")
			target_link_libraries(NEAT ${Matlab_LIBRARIES})
		endif()
	endif()
endif()

if(pythonLib)

	message("copilando python module")
	enable_language(C)
	enable_language(CXX)

	find_package(pybind11 CONFIG REQUIRED)
	include_directories(${pybind11_INCLUDE_DIRS})
	message([MAIN] "Found pybind11 v${pybind11_VERSION}: ${pybind11_INCLUDE_DIRS}")

	MESSAGE( [Main] " pybind11_INCLUDE_DIRS = ${pybind11_INCLUDE_DIRS}")
	MESSAGE( [Main] " pybind11_LIBRARIES = ${pybind11_LIBRARIES}")
	#
	#   # Create an extension module
	#   add_library(mylib MODULE main.cpp)
	#   target_link_libraries(mylib pybind11::module)
	#
	#   # Or embed the Python interpreter into an executable
	#   add_executable(myexe main.cpp)
	#   target_link_libraries(myexe pybind11::embed)

	# method (1): generate `examplelib.pyd`
	#pybind11_add_module(examplelib pythonModule.cpp biocenosis.cpp)

	message("modulo python neat.pyd")
	add_library(mtNEAT MODULE pythonModule.cpp "species.cpp" "biocenosis.cpp" "gene.cpp" "genome.cpp" "innovationGenerator.cpp" "node.cpp")
	target_link_libraries(mtNEAT pybind11::module ${Matlab_LIBRARIES})
		set_target_properties(mtNEAT PROPERTIES SUFFIX ".pyd")
	MESSAGE( [Main] " pybind11_INCLUDE_DIRS = ${pybind11_INCLUDE_DIRS}")
	MESSAGE( [Main] " pybind11_LIBRARIES = ${pybind11_LIBRARIES}")
else()
	if(${CMAKE_SYSTEM_NAME} STREQUAL  "Linux")
		message("modulo python linux neat.so")
		add_library(mtNEAT SHARED "species.cpp" "biocenosis.cpp" "gene.cpp" "genome.cpp" "innovationGenerator.cpp" "node.cpp")
	endif()
endif()
