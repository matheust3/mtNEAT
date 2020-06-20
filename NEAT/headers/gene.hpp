#pragma once
#include <random>

class Gene
{
public:
	/// <summary>
	/// Cria um gene de conexao
	/// </summary>
	Gene(bool active, size_t node0, size_t node1, double _weight, bool recurrent );
	Gene() {};
	//Ativo?
	bool _active;
	//Recorrente?
	bool _recurrent;
	//Id de inovacao
	size_t _innovationId;
	//Peso sinaptico
	double _weight;
	//Nodes
	size_t _nodes[2]; //[0]->TO  [1]->FROM
private:

};