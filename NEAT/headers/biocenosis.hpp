#pragma once
#include "species.hpp"

/// <summary>
/// Comunidade de especies
/// </summary>
class Biocenosis
{
public:
	Biocenosis(size_t nInputs, size_t nOuts, bool recurrent);
	Biocenosis() {};
	Biocenosis(const Biocenosis& other);
	Biocenosis operator=(const Biocenosis& other);
	~Biocenosis();
	//Numero de entradas
	size_t _nInputs;
	//Numero de saidas
	size_t _nOuts;
	//Numero maximo de genomas
	size_t _maxGenomes;
	//Minimo que um especie deve sobreviver
	size_t _minLiveTime = 200;
	size_t _minIndividualLifeTime = 200;
	InnovationGenerator inivGen; //Gerador de inovacao
	vector<Species> _species;//Especies
	vector<Genome*> _genomes;//Genomas
	//Os genomas podem ser recorrenter
	bool _recurrent;

	/// <summary>
	/// Adiciona um individuo a comunidade
	/// </summary>
	bool AddGenome(Genome& genome);
	/// <summary>
	/// Evolui a comunidade
	/// </summary>
	void Evolve();

	Genome& operator[](size_t x);
	/// <summary>
	/// Numero de individuos
	/// </summary>
	size_t size();
private:
	size_t _size = 0;
};