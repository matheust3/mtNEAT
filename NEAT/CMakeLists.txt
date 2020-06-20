# CMakeList.txt: projeto de CMake para NEAT3, incluir origem e definir
# lógica específica a um projeto aqui.
#
cmake_minimum_required (VERSION 3.8)

set(pythonLib True)

if(${CMAKE_SYSTEM_NAME} STREQUAL  "Windows")

	find_package(MATLAB)
	if(Matlab_FOUND)
		INCLUDE_DIRECTORIES(${Matlab_INCLUDE_DIRS})
		link_directories(${Matlab_LIBRARIES})

		message(STATUS "COM MATLAB")
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

	if(${CMAKE_SYSTEM_NAME} STREQUAL  "Windows")
		message("modulo python Windows neat.pyd")
		# method (2): generate `examplelib.dll` rename to `examplelib.pyd`
		add_library(mtNEAT MODULE pythonModule.cpp "species.cpp" "biocenosis.cpp" "gene.cpp" "genome.cpp" "innovationGenerator.cpp" "node.cpp")
		target_link_libraries(mtNEAT pybind11::module ${Matlab_LIBRARIES})
			set_target_properties(mtNEAT PROPERTIES SUFFIX ".pyd")
		MESSAGE( [Main] " pybind11_INCLUDE_DIRS = ${pybind11_INCLUDE_DIRS}")
		MESSAGE( [Main] " pybind11_LIBRARIES = ${pybind11_LIBRARIES}")
	else()
		message("modulo python linux neat.so")
		pybind11_add_module(mtNEAT pythonModule.cpp "species.cpp" "biocenosis.cpp" "gene.cpp" "genome.cpp" "innovationGenerator.cpp" "node.cpp")
		set_target_properties(mtNEAT PROPERTIES SUFFIX ".so")
	endif()
	#add_executable(cpp_use_python cpp_use_python.cpp)
	#target_link_libraries(cpp_use_python PRIVATE pybind11::embed)
else()
	message("copilando neat.exe")
	# Adicione a origem ao executável deste projeto.
	add_executable (NEAT "NEAT.cpp" "NEAT.h" "species.cpp" "biocenosis.cpp" "gene.cpp" "genome.cpp" "innovationGenerator.cpp" "node.cpp" )
	if(${CMAKE_SYSTEM_NAME} STREQUAL  "Windows")
		target_link_libraries(NEAT ${Matlab_LIBRARIES})
	endif()
endif()


# TODO: Adicione testes e instale destinos, se necessário.
