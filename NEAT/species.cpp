#include "headers/species.hpp"

bool Species::AddGenome(Genome& genome)
{
	if (_elements.size() == 0)
	{
		_elements.push_back(&genome);
		return true;
	}
	if (SpeciesScore(genome) < 0.25)
	{
		_elements.push_back(&genome);
		return true;
	}
	return false;
}

double Species::SpeciesScore(Genome& genome)
{
	size_t numDisjoint = 0; //Genes disjuntos
	size_t numExcess = 0; //Genes em excesso
	size_t numMatched = 1; //Genes iguais (1 para evitar divisao por 0)
	size_t numMatchedBias = 1; //nodes iguais iguais (1 para evitar divisao por zero)
	double weightDifference = 0; //Diferenca de pesos
	double biasDifference = 0; //Diferenca nos bias

	//Gene 1 e 2
	size_t g1 = 0, g2 = 0;
	//iteradores de cada genoma
	auto gene1 = _elements[0]->_genes.begin();
	auto gene2 = genome._genes.begin();
	//Genes
	while ((g1 < _elements[0]->_genes.size()-1) || (g2 < genome._genes.size()-1))
	{
		//Se esta no final do genoma1 mas não do genoma2
		//incrementa só o numero de genes disjuntos
		if (g1 == _elements[0]->_genes.size() - 1)
		{
			g2++;
			numExcess++;
			continue;
		}
		//and vice versa
		if (g2 == genome._genes.size() - 1)
		{
			g1++;
			numExcess++;
			continue;
		}
		//Pega o id de inovacao de cada genenoma nesse ponto
		size_t id1 = gene1->second._innovationId;
		size_t id2 = gene2->second._innovationId;
		//innovation numbers are identical so increase the matched score
		if (id1 == id2)
		{
			++g1;
			++g2;
			//Avanca os iteradores
			std::advance(gene1, 1);
			std::advance(gene2, 1);
			++numMatched;
			//Pega a diferenca entre os dois genes
			weightDifference += abs(gene1->second._weight -
				gene2->second._weight);
		}
		//innovation numbers are different so increment the disjoint score
		if (id1 < id2)
		{
			++numDisjoint;
			++g1;
			//Avanca o iterador
			std::advance(gene1, 1);
		}
		if (id1 > id2)
		{
			++numDisjoint;
			++g2;
			//Avanca o iterador
			std::advance(gene2, 1);
		}
	}//end while
	
	 //Nodes
	size_t n1 = 0, n2 = 0;
	auto node1 = _elements[0]->_nodes.begin(); //Node do individuo de comparacao da especie
	auto node2 = genome._nodes.begin(); //Node de comparracao
	//Ordena os nodes com base nos ids de inovacao
	std::sort(_elements[0]->_nodes.begin(), _elements[0]->_nodes.end(), [](const Node& n1, const Node& n2) { return n1._innovationId < n2._innovationId; });
	std::sort(genome._nodes.begin(), genome._nodes.end(), [](const Node& n1, const Node& n2) { return n1._innovationId < n2._innovationId; });
	
	//Nodes
	while ((n1 < _elements[0]->_nodes.size()) && (n2 < genome._nodes.size()))
	{
		if (node1->_innovationId == node2->_innovationId) {
			biasDifference += std::abs(node1->_bias - node2->_bias);
			++numMatchedBias;
			++n2;
			std::advance(node2, 1);
			++n1;
			std::advance(node1, 1);
		}
		else if(node1->_innovationId > node2->_innovationId)
		{
			++n2;
			std::advance(node2, 1);
		}
		else if (node1->_innovationId < node2->_innovationId)
		{
			++n1;
			std::advance(node1, 1);
		}
	}

	//get the length of the longest genome
	int longest = genome._genes.size();
	if (_elements[0]->_genes.size() > longest)
	{
		longest = _elements[0]->_genes.size();
	}
	//these are multipliers used to tweak the final score.
	const double mDisjoint = 1;
	const double mExcess = 1;
	const double mMatched = 0.4;
	//finally calculate the scores
	double score = (mExcess * numExcess / (double)longest) +
		(mDisjoint * numDisjoint / (double)longest) +
		(mMatched * weightDifference / numMatched) +
		(mMatched * biasDifference / numMatchedBias);

	//Ordena os nodes pelo eixo x
	//Na ordem crescente o x nessa implementacao representa as camadas
	std::sort(_elements[0]->_nodes.begin(), _elements[0]->_nodes.end(), [](const Node& n1, const Node& n2) { return n1._x < n2._x; });
	std::sort(genome._nodes.begin(), genome._nodes.end(), [](const Node& n1, const Node& n2) { return n1._x < n2._x; });
	
	return score;
}