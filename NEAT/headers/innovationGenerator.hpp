#pragma once
#include <map>
#include <string>
#include "node.hpp"

class InnovationGenerator
{
public:
	//Mapa contendo os genes para a inovacao
	std::map<std::string, size_t> _dataBase;
	//Mapa contendo os ids dos neuronios
	std::map<std::size_t, size_t> _idDataBase;
	size_t _innovationNumber;
	InnovationGenerator();
	size_t getInnovation(const Gene &, const size_t *, const vector<Node>*);
	size_t getInnovation(Node &);
private:

};