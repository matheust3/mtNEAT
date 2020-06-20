#include "headers/gene.hpp"

Gene::Gene(bool active,
	size_t node0,
	size_t node1,
	double weight,
	bool recurrent) :_active(active), _weight(weight), _recurrent(recurrent)
{
	_nodes[0] = node0; //To
	_nodes[1] = node1; //From
}
