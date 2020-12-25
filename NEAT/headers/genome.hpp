#pragma once

#include <iostream>
#include <algorithm>
#include <vector>
#include <random>
#include <map>
#include "gene.hpp"
#include "node.hpp"
#include "innovationGenerator.hpp"

using std::cout;
using std::cin;
using std::vector;
using std::map;

class Genome
{
public:
	Genome(size_t nInputs, size_t nOuts, InnovationGenerator* innovationGenerator, bool recurrent);
	Genome(const Genome& genome);
	Genome operator=(const Genome& other);
	//Adiciona uma nova conexao entre dois neuronios
	void AddConnection();
	//Adiciona um node ao genoma
	void AddNode();
	/// <summary>
	/// Funcao de ativacao
	/// </summary>
	double ActivationFunction(const double& x, const Node&node);
	/// <summary>
	/// Faz o crossover de dois genomas
	/// O genoma da fun��o � escolhido como melhor
	/// </summary>
	Genome CrossOver(const Genome& genome);
	/// <summary>
	/// Cria uma mutacao no genoma
	/// </summary>
	void Mutation(bool noTopology = false);


	/// <summary>
	/// Processa uma entrada
	/// </summary>
	/// <returns>Retorna um vetor com as saidas</returns>
	vector<double> Process(const map<size_t, double>& input);
	/// <summary>
	/// Apaga os estados da rede
	/// </summary>
	void Forget();
	//Desempenho do genoma
	double _fitness;
	//Recorrente?
	bool _recurrent;
	//Tempo de vida
	size_t _lifeTime = 0;
	//Numero de entradas e numero de saidas
	size_t _nInputs, _nOut;
	//Mapa de gene de conexao
	map<size_t, Gene> _genes;
	//Vetor de nodes
	vector< Node> _nodes;
	//Gerador de numeros de inovacao
	InnovationGenerator* _innovationGenerator;
	//Saida da rede
	vector<double> _out;
	//Mapa de estados
	map<size_t, double> _state;


private:
	std::random_device  _randomDev;
	std::mt19937_64 _randomEng;
	std::uniform_real_distribution <double> _dist;
};