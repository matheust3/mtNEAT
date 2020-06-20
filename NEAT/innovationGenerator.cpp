#include "headers/innovationGenerator.hpp"

InnovationGenerator::InnovationGenerator() :_innovationNumber(0)
{
	_dataBase.clear();
}


size_t InnovationGenerator::getInnovation(const Gene& gene, const size_t* nInputs, const vector<Node>* nodes)
{
	std::string x1;
	std::string x2;
	std::string y1;
	std::string y2;
	//Mapeia os nodes do gene
	std::map<const size_t,const Node*> nodesMap;
	for (size_t i = 0; i < nodes->size(); i++)
	{
		nodesMap.insert(std::make_pair(nodes->at(i)._id,&nodes->at(i)));
	}
	if (gene._nodes[0] < *nInputs)
	{
		x1 = "0";
		y1 = std::to_string(gene._nodes[0]);
	}
	else {
		x1 = std::to_string(nodesMap[gene._nodes[0] ]->_x);
		y1 = std::to_string(nodesMap[gene._nodes[0]]->_y);
	}

	if (gene._nodes[1] < *nInputs)
	{
		x2 = "0";
		y2 = std::to_string(gene._nodes[1]);
	}
	else {
		x2 = std::to_string(nodesMap[gene._nodes[1]]->_x);
		y2 = std::to_string(nodesMap[gene._nodes[1]]->_y);
	}

	std::string code = x1 + "-" + y1 + "-" + x2 + "-" + y2 +"-"+std::to_string(gene._recurrent);
	if (_dataBase.count(code) > 0) {
		return _dataBase[code];
	}
	else {
		_innovationNumber++;
		_dataBase.insert(std::make_pair(code, _innovationNumber));
		return _innovationNumber;
	}
}

size_t InnovationGenerator::getInnovation(Node& node)
{
	std::string code = std::to_string(node._x) + "-" + std::to_string(node._y);
	if (_dataBase.count(code) > 0) {
		size_t innovNumber = _dataBase[code];
		node._id = _idDataBase[innovNumber];
		return innovNumber;
	}
	else {
		_innovationNumber++;
		_dataBase.insert(std::make_pair(code, _innovationNumber));
		_idDataBase.insert(std::make_pair(_innovationNumber, node._id));
		return _innovationNumber;
	}

}
