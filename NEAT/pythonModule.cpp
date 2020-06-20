#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;
//NEAT
#include "headers/biocenosis.hpp"
#include "headers/genome.hpp"

PYBIND11_MODULE(tneat, m) {

	m.doc() = "Algoritimo NEAT por Matheus Toniolli";

	//CLASSES
	//Biocenosis
	py::class_<Biocenosis>(m, "Biocenosis")
		.def(py::init<size_t, size_t, bool>(),"Cria um novo objeto Biocenosis", py::arg("nInputs"), py::arg("nOuts"), py::arg("recurrent") = true)
		.def(py::init< const Biocenosis&>())
		.def(py::init<>())
		//Funções
		.def("Evolve", &Biocenosis::Evolve, "Evolui os genomas")
		//Membros
		.def_readonly("_recurrent",&Biocenosis::_recurrent)
		.def_readwrite("_genomes", &Biocenosis::_genomes,"Vetor de Genomas")
		.doc() = std::string("Comunidade de genomas")
		;
	//Genome
	py::class_<Genome>(m, "Genome")
		.def(py::init<const Genome&>())
		.def("Process", &Genome::Process, "Processa um mapa de entradas")
		//Membros
		.def_readwrite("_fitness", &Genome::_fitness, "Desempenho do genoma")
		.doc() = "Genoma"
		;
}
