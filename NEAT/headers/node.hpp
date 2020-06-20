#pragma once
#include "gene.hpp"
#include <vector>

using std::vector;

class Node
{
public:
	//Tipo da funcao de ativacao
	enum FUNCTION_TYPE {
		SIG, //Sigmoid
		TANH, //Tangente hiperbolica
		NEG_SIG, //Sigmoid negativa
		COS, //Cos
		NEG_COS, // -Cos 
		SIN, // Sen 
		NEG_SIN, // -Sen 
		FIRST = SIG,
		LAST = NEG_SIN
	}_funcType = TANH;
	/// <summary>
	/// Cria um node objeto
	///<param name="id"> Id do node</param>
	///<param name="x">Posicao x no grid</param>
	///<param name="y">Posicao y no grid</param>
	/// </summary>
	Node(size_t id, double x, double y);
	size_t _innovationId;
	double _x;
	double _y;
	double _bias;
	size_t _id;
	vector<Gene*> _genes;
private:

};