// NEAT3.cpp: define o ponto de entrada para o aplicativo.
//

#include "NEAT.h"

using namespace std;

struct World
{
	void set(std::string msg) { this->msg = msg; }
	std::string greet() { return msg; }
	std::string msg;
};

int main()
{
	//Test1Input();
	TestXOR();
	cout << "Olá, CMake." << endl;
	return 0;
}

void TestXOR() {

	//Entradas
	map < size_t, double> input1;
	input1.insert(std::make_pair(0, 1));
	input1.insert(std::make_pair(1, 1));
	//Entradas
	map < size_t, double> input2;
	input2.insert(std::make_pair(0, 1));
	input2.insert(std::make_pair(1, -1));
	//Entradas
	map < size_t, double> input3;
	input3.insert(std::make_pair(0, -1));
	input3.insert(std::make_pair(1, 1));
	//Entradas
	map < size_t, double> input4;
	input4.insert(std::make_pair(0, -1));
	input4.insert(std::make_pair(1, -1));

	Biocenosis biocenosis(2,1, false);//Comunidade de especies
	size_t count = 0;
	//Evolui
	while (true)
	{
		//Processa cada entrada
		for (size_t genomeId = 0; genomeId < biocenosis.size(); genomeId++)
		{
			//Se já processou pula
			if (biocenosis[genomeId]._lifeTime > 0)
			{
				continue;
			}
			vector<double> out;
			out = biocenosis[genomeId].Process(input1);
			biocenosis[genomeId]._fitness += pow((out[0] - (-1)), 2) / 4;

			out = biocenosis[genomeId].Process(input2);
			biocenosis[genomeId]._fitness += pow((out[0] - 1), 2) / 4;

			out = biocenosis[genomeId].Process(input3);
			biocenosis[genomeId]._fitness += pow((out[0] - 1), 2) / 4;

			out = biocenosis[genomeId].Process(input4);
			biocenosis[genomeId]._fitness += pow((out[0] - (-1)), 2) / 4;
		}
		biocenosis.Evolve();

		cout << "gen: " << count<< " error:"<< biocenosis._genomes[0]->_fitness << endl;
		count++;
	}
}

void Test1Input()
{
	//Entradas
	map < size_t, double> input1;
	input1.insert(std::make_pair(0, 1));
	//Entradas
	map < size_t, double> input2;
	input2.insert(std::make_pair(0, 1));
	//Entradas
	map < size_t, double> input3;
	input3.insert(std::make_pair(0, -1));
	//Entradas
	map < size_t, double> input4;
	input4.insert(std::make_pair(0, -1));

	Biocenosis biocenosis(1, 1, false);//Comunidade de especies
	size_t count = 0;
	//Evolui
	while (true)
	{
		//Processa cada entrada
		for (size_t genomeId = 0; genomeId < biocenosis.size(); genomeId++)
		{
			//Se já processou pula
			if (biocenosis[genomeId]._lifeTime > 0)
			{
				continue;
			}
			vector<double> out;
			out = biocenosis[genomeId].Process(input1);
			biocenosis[genomeId]._fitness += pow((out[0] - (-1)), 2) / 4;

			out = biocenosis[genomeId].Process(input2);
			biocenosis[genomeId]._fitness += pow((out[0] - 1), 2) / 4;

			out = biocenosis[genomeId].Process(input3);
			biocenosis[genomeId]._fitness += pow((out[0] - 1), 2) / 4;

			out = biocenosis[genomeId].Process(input4);
			biocenosis[genomeId]._fitness += pow((out[0] - (-1)), 2) / 4;
		}
		biocenosis.Evolve();

		cout << "gen: " << count << " error:" << biocenosis._genomes[0]->_fitness << endl;
		count++;
	}
}
