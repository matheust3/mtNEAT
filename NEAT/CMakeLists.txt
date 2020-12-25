# CMakeList.txt: projeto de CMake para NEAT3, incluir origem e definir
# lógica específica a um projeto aqui.
#
cmake_minimum_required (VERSION 3.8)

set(pythonLib True)

if(pythonLib)
	set(pybind11_DIR C:\\Users\\geant\\Desktop\\pybind11\\installer\\share\\cmake\\pybind11)
	message("copilando python module")
	enable_language(C)
	enable_language(CXX)

	find_package(pybind11 CONFIG REQUIRED)
	include_directories(${pybind11_INCLUDE_DIRS})
	message([MAIN] "Found pybind11 v${pybind11_VERSION}: ${pybind11_INCLUDE_DIRS}")

	MESSAGE( [Main] " pybind11_INCLUDE_DIRS = ${pybind11_INCLUDE_DIRS}")
	MESSAGE( [Main] " pybind11_LIBRARIES = ${pybind11_LIBRARIES}")

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
