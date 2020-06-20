#pragma once

#include <vector>
#include "genome.hpp"

class Species
{
public:

	//Geracaoes viva
	size_t _livingTime = 0;
	//Elementos dessa especie
	std::vector<Genome*> _elements;

	/// <summary>
	/// Adiciona um genoma a essa especie
	/// </summary>
	/// <returns>Retorna true se o genoma eh dessa especie</returns>
	bool AddGenome(Genome& genome);
	/// <summary>
	/// Calcula a pontuacao de um individuo de ser dessa especie
	/// </summary>
	double SpeciesScore(Genome& genome);

};