#include "headers/biocenosis.hpp"

Biocenosis::Biocenosis(size_t nInputs, size_t nOuts, bool recurrent) :_nInputs(nInputs), _nOuts(nOuts), _maxGenomes(50),
_recurrent(recurrent)
{
	//Cria os dois primeiros genomas
	_genomes.push_back(new Genome(nInputs, nOuts, &inivGen, recurrent));
	AddGenome(*_genomes.at(0));
	_genomes.push_back(new Genome(nInputs, nOuts, &inivGen, recurrent));
	AddGenome(*_genomes.at(1));
}

Biocenosis::Biocenosis(const Biocenosis& other) {
	this->inivGen = other.inivGen;
	//Copia os genomes
	for (size_t i = 0; i < other._genomes.size(); i++)
	{
		this->_genomes.push_back(new Genome(*other._genomes[i]));
		_genomes[i]->_innovationGenerator = &this->inivGen;
		this->AddGenome(*_genomes.at(i));
	}
	this->_maxGenomes = other._maxGenomes;
	this->_minIndividualLifeTime = other._minIndividualLifeTime;
	this->_minLiveTime = other._minLiveTime;
	this->_nInputs = other._nInputs;
	this->_nOuts = other._nOuts;
	this->_recurrent = other._recurrent;
	this->_size = other._size;
}

Biocenosis Biocenosis::operator=(const Biocenosis& other) {
	this->inivGen = other.inivGen;
	//Copia os genomes
	for (size_t i = 0; i < other._genomes.size(); i++)
	{
		this->_genomes.push_back(new Genome(*other._genomes[i]));
		_genomes[i]->_innovationGenerator = &this->inivGen;
		this->AddGenome(*_genomes.at(i));
	}
	this->_maxGenomes = other._maxGenomes;
	this->_minIndividualLifeTime = other._minIndividualLifeTime;
	this->_minLiveTime = other._minLiveTime;
	this->_nInputs = other._nInputs;
	this->_nOuts = other._nOuts;
	this->_recurrent = other._recurrent;
	this->_size = other._size;
	return *this;
}

Biocenosis::~Biocenosis()
{
	for (size_t i = 0; i < _genomes.size(); i++)
	{
		delete _genomes[i];
	}
}

bool Biocenosis::AddGenome(Genome& genome)
{
	//Se não tem especies ainda 
	if (_species.size() == 0)
	{
		Species species;
		species.AddGenome(genome);
		_species.push_back(species);
		_size++;
		return true;
	}
	else
	{
		//Tenta por o individuo em uma especie
		for (size_t speciesId = 0; speciesId < _species.size(); speciesId++)
		{
			if (_species[speciesId].AddGenome(genome))
			{
				_size++;
				break;
			}

			if (speciesId == _species.size() - 1)
			{
				//Cria uma nova specie e adiciona o genoma
				Species species;
				species.AddGenome(genome);
				_species.push_back(species);
				_size++;
				break;
			}
		}
		return true;
	}
	return false;
}

void Biocenosis::Evolve()
{
	//ELIMINA OS PIORES INDIVIDUOS

	//Numero de especies protegidas
	size_t numProtectedSpecies = 0;
	//Numero de individuos nas especies protegidas
	size_t numGenomesProtectedSpecies = 7;

	for (size_t i = 0; i < _species.size(); i++)
	{
		//Se esta em protecao
		if (_species[i]._livingTime < _minLiveTime)
			numProtectedSpecies++;
		if (_species[i]._elements.size() > 2) {
			//Ordena a melhor rede na especie
			size_t g1Id = _species[i]._elements[0]->_fitness > _species[i]._elements[1]->_fitness ? 1 : 0;
			size_t g2Id = _species[i]._elements[0]->_fitness > _species[i]._elements[1]->_fitness ? 0 : 1;
			if (g1Id > g2Id) {
				Genome* tempGen = _species[i]._elements[0];
				_species[i]._elements[0] = _species[i]._elements[1];
				_species[i]._elements[1] = tempGen;
				g1Id = 0;
				g2Id = 1;
			}
			//Eleimina os individuos ruins
			for (size_t genomeId = 2; genomeId < _species[i]._elements.size(); genomeId++)
			{
				if (_species[i]._elements[g1Id]->_fitness > _species[i]._elements[genomeId]->_fitness)
				{

					//REPOSICIONA NOS VETORES
					//Antigo ponteiro
					Genome* oldGen = _species[i]._elements[g2Id];
					//Reposiciona o novo melhor
					_species[i]._elements[g2Id] = _species[i]._elements[g1Id];
					_species[i]._elements[g1Id] = _species[i]._elements[genomeId];

					//Remove da especie
					_species[i]._elements.erase(_species[i]._elements.begin() + genomeId);
					//Remove o g2Id do vetor de genes
					_genomes.erase(std::find(_genomes.begin(), _genomes.end(), oldGen));
					//Apaga da memoria
					delete oldGen;
					--_size;
					genomeId--;

				}
				else if (_species[i]._elements[g2Id]->_fitness > _species[i]._elements[genomeId]->_fitness) {
					//REPOSICIONA NOS VETORES
					//Antigo ponteiro
					Genome* oldGen = _species[i]._elements[g2Id];
					_species[i]._elements[g2Id] = _species[i]._elements[genomeId];

					//Remove da especie
					_species[i]._elements.erase(_species[i]._elements.begin() + genomeId);
					//Remove o g2Id do vetor de genes
					_genomes.erase(std::find(_genomes.begin(), _genomes.end(), oldGen));
					//Apaga da memoria
					delete oldGen;
					--_size;
					genomeId--;
				}
				else
				{
					//REPOSICIONA NOS VETORES
					//Antigo ponteiro
					Genome* oldGen = _species[i]._elements[genomeId];

					//Remove da especie
					_species[i]._elements.erase(_species[i]._elements.begin() + genomeId);
					//Remove o g2Id do vetor de genes
					_genomes.erase(std::find(_genomes.begin(), _genomes.end(), oldGen));
					//Apaga da memoria
					delete oldGen;
					--_size;
					genomeId--;
				}
			}
			//Soma o tempo de vida dessa especie
			_species[i]._elements[1]->_lifeTime++;
		}
		_species[i]._elements[0]->_lifeTime++;

	}
	//CRIA UMA NOVA POPULACAO DE GENOMAS

	//Ordena os genomas pelo desempenho
	std::sort(_genomes.begin(), _genomes.end(), [](const Genome* g1, const Genome* g2) { return g1->_fitness < g2->_fitness; });
	//Ordena as especies com base no melhor individuo de cada uma
	std::sort(_species.begin(), _species.end(), [](const Species s1, const Species s2) { return s1._elements[0]->_fitness < s2._elements[0]->_fitness; });

	//Numero de individuos por especie
	size_t numPerSpecies;
	//Numero de especies
	size_t numSpecies = _species.size() > 4 ? 4 : _species.size();
	if (numSpecies - numProtectedSpecies == 0)
	{
		numPerSpecies = _maxGenomes / numSpecies;
	}
	else
	{
		numPerSpecies = (_maxGenomes - (numProtectedSpecies * numGenomesProtectedSpecies)) / (numSpecies - numProtectedSpecies);
	}
	//Como cada especie já tem dois individuos pais a especie para evoluir deve ter pelo menos tres individuos
	numPerSpecies = numPerSpecies < 3 ? 3 : numPerSpecies;

	//Chegou ao maximo em algum momento
	bool max = false;
	for (size_t i = 0; i < _species.size(); i++)
	{
		size_t g1Id = 0;
		size_t g2Id = 1;

		//Se esta em protecao
		if (_species[i]._livingTime < _minLiveTime)
		{
			//Cria novos individuos a partir dos melhores
			for (size_t newGenomeId = _species[i]._elements.size(); newGenomeId < numGenomesProtectedSpecies; newGenomeId++)
			{
				Genome newGenome = _species[i]._elements.size() > 1 ?
					_species[i]._elements[g1Id]->CrossOver(*_species[i]._elements[g2Id]) :
					_species[i]._elements[g1Id]->CrossOver(*_species[i]._elements[g1Id]);
				newGenome.Mutation(true); //Cria uma mutacao nos pesos somente
				//Salva o genome
				Genome* newGenPointer = new Genome(newGenome);
				_genomes.push_back(newGenPointer);
				AddGenome(*newGenPointer);
			}
			//Adiciona tempo de vida dessa especie
			_species[i]._livingTime++;
		}
		else {
			//Se ainda tem lugar para mais individuos
			if (_size < _maxGenomes && !max || i == 0)
			{
				//Cria os individuos a partir dos melhores (a especie ja tem dois pais)
				for (size_t newGenomeId = _species[i]._elements.size(); newGenomeId < numPerSpecies; newGenomeId++)
				{
					Genome newGenome = _species[i]._elements[g1Id]->CrossOver(*_species[i]._elements[g2Id]);
					newGenome.Mutation(); //Cria uma mutacao
					//Salva o genome
					Genome* newGenPointer = new Genome(newGenome);
					_genomes.push_back(newGenPointer);
					AddGenome(*newGenPointer);
				}
				//Adiciona tempo de vida dessa especie
				_species[i]._livingTime++;
			}
			else
			{
				max = true;
				//ELIMINA ESSA ESPECIE

				//Elimina os individuos da especie
				while (_species[i]._elements.size() > 0)
				{
					//Apaga da lista de genomas
					_genomes.erase(
						std::find(_genomes.begin(), _genomes.end(), _species[i]._elements[0])
					);
					//Apaga da memoria
					delete _species[i]._elements[0];
					//Apaga da lista de individuos da especie
					_species[i]._elements.erase(_species[i]._elements.begin());
					--_size;
				}
				//Apaga a especie da lista de especies
				if (_species[i]._elements.size() == 0)
				{
					_species.erase(_species.begin() + i);
					--i;
				}
			}
		}
	}

	//completa a populacao
	while (_genomes.size() < _maxGenomes)
	{
		Genome newGenome = _genomes[0]->CrossOver(*_genomes[1]);
		newGenome.Mutation(); //Cria uma mutacao
		//Salva o genome
		Genome* newGenPointer = new Genome(newGenome);
		_genomes.push_back(newGenPointer);
		AddGenome(*newGenPointer);
	}
}

Genome& Biocenosis::operator[](size_t x)
{
	return *_genomes[x];
}

size_t Biocenosis::size()
{
	return _size;
}

