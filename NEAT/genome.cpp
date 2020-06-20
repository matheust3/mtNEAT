#include "headers/genome.hpp"

Genome::Genome(size_t nInputs, size_t nOuts, InnovationGenerator* innovationGenerator, bool recurrent) :_innovationGenerator(innovationGenerator),
_nInputs(nInputs), _nOut(nOuts), _fitness(0), _recurrent(recurrent)
{
	_out = vector<double>(nOuts);
	_randomEng = std::mt19937_64(_randomDev());
	_dist = std::uniform_real_distribution <double>(-1.0, 1);
	//CRIA OS GENES DE ENTRADA E DE SAIDA
	//
	//I----\
	//      --------\
	//I--------------O
	//      --------/
	//I----/   ----/
	//        /
	//I------/
	//
	//Sem nodes nas camada escondidas e nas entradas (Comeca com o minimo de nodes possivel)

	//CRIA OS NODES DE SAIDA

	//Para cada saida
	for (size_t outId = 0; outId < _nOut; outId++)
	{
		//Liga cada entrada à cada saida por meio um gene sinaptico
		//Imaginariamente existem os neuronios de entrada cada um com seu id(mas só uma representacao deles)
		//Cria o node
		Node node(_nInputs + outId, (double)1000, (double)outId);
		node._innovationId = _innovationGenerator->getInnovation(node);
		node._bias = _dist(_randomEng);
		//Salva o node
		_nodes.push_back(node);
		//Cria um gene de ligacao ligando cada entrada a esse node de saida
		for (size_t inputId = 0; inputId < _nInputs; inputId++)
		{
			//Cria o gene
			Gene gene(true, inputId, _nInputs + outId, _dist(_randomEng), false);
			gene._innovationId = _innovationGenerator->getInnovation(gene, &_nInputs, &_nodes);
			//Salva o gene no mapa de genes
			_genes.insert(std::make_pair(gene._innovationId, gene));
			//Salva o endereco desse gene no vetor de genes do node de saida criado
			_nodes.back()._genes.push_back(&_genes.find(gene._innovationId)->second);
		}
	}
}

Genome::Genome(const Genome& genome)
{
	this->_lifeTime = genome._lifeTime;
	this->_recurrent = genome._recurrent;
	this->_fitness = genome._fitness;
	this->_dist = genome._dist;
	this->_genes = genome._genes;
	this->_innovationGenerator = genome._innovationGenerator;
	this->_nInputs = genome._nInputs;
	this->_nodes = genome._nodes;
	this->_nOut = genome._nOut;
	this->_randomEng = std::mt19937_64(_randomDev());
	_out = vector<double>(genome._nOut);

	//Ajusta os enderecos de cada gene dos nodes
	for (Node& node : this->_nodes)
	{
		//Para cada gene desse node
		for (size_t geneId = 0; geneId < node._genes.size(); geneId++)
		{
			node._genes[geneId] = &this->_genes.find(node._genes[geneId]->_innovationId)->second;
		}
	}
}

Genome Genome::operator=(const Genome& other) {
	this->_lifeTime = other._lifeTime;
	this->_recurrent = other._recurrent;
	this->_fitness = other._fitness;
	this->_dist = other._dist;
	this->_genes = other._genes;
	this->_innovationGenerator = other._innovationGenerator;
	this->_nInputs = other._nInputs;
	this->_nodes = other._nodes;
	this->_nOut = other._nOut;
	this->_randomEng = std::mt19937_64(_randomDev());
	_out = vector<double>(other._nOut);

	//Ajusta os enderecos de cada gene dos nodes
	for (Node& node : this->_nodes)
	{
		//Para cada gene desse node
		for (size_t geneId = 0; geneId < node._genes.size(); geneId++)
		{
			node._genes[geneId] = &this->_genes.find(node._genes[geneId]->_innovationId)->second;
		}
	}
	return *this;
}

void Genome::AddConnection()
{

	std::random_device dev;
	std::mt19937_64 rng(dev());
	bool mutate(false);
	//Distribuicao de saida pode conter as entradas
	std::uniform_int_distribution<size_t> firstNodeIdDist(0, _nodes.size() + _nInputs - 1);
	//A distribuicao de entrada só poderao ser nós
	std::uniform_int_distribution<size_t> secondNodeIdDist(0, _nodes.size() - 1);
	//Usado para saber se será recorrent
	std::uniform_int_distribution<size_t> halfDist(0, 1);
	size_t count = 0;
	do
	{
		//NODE DE PARTIDA
		size_t firstId = firstNodeIdDist(rng);
		//Id real do node
		size_t firstNodeId = firstId < _nInputs ? firstId : _nodes[firstId - _nInputs]._id; //Leva em conta as entradas
		//SEGUNDO NODE
		size_t secondId = secondNodeIdDist(rng);
		//Id real do node
		size_t secondNodeId = _nodes[secondId]._id; //Não leva em conta as entradas
		//Conexão recorrente?
		bool recurrent;
		//Se o primeiro node esta na mesma camada ou em uma camada mais atrás eh obrigatóriamente recorrente
		if (firstId >= _nInputs && _nodes[firstId - _nInputs]._x >= _nodes[secondId]._x)
		{
			//Se pode ser recorrente
			if (_recurrent)
			{
				//E recorrente
				recurrent = true;
			}
			else
			{
				//Acha outra combinacao de nodes
				continue;
			}
		}
		else
		{
			//Se pode ser recorrente
			if (_recurrent)
				//Recorrente ou nao
				recurrent = halfDist(rng);
			else
				recurrent = false;
		}
		//Verifica em cada gene do node se ele ja não tinha essa conexão
		for (auto geneId = 0; geneId < _nodes[secondId]._genes.size(); geneId++)
		{
			//Se essa conexao ja existe
			if (_nodes[secondId]._genes[geneId]->_nodes[0] == firstNodeId && _nodes[secondId]._genes[geneId]->_recurrent == recurrent)
				//Se a conexao esta desativada
				if (!_nodes[secondId]._genes[geneId]->_active)
				{
					//Ativa a conexao
					_nodes[secondId]._genes[geneId]->_active = true;
					mutate = true;
					break;
				}
				else
				{
					break;
				}
			//Cria um novo gene ligando as duas conexoes
			if (geneId == _nodes[secondId]._genes.size() - 1)
			{
				std::uniform_real_distribution<double> weight(-1, 1);
				Gene gene(true, firstNodeId, secondNodeId, weight(rng), recurrent);
				gene._innovationId = _innovationGenerator->getInnovation(gene, &_nInputs, &_nodes);
				//Adiciona o novo gene ao mapa de genes
				_genes.insert(std::make_pair(gene._innovationId, gene));
				_nodes[secondId]._genes.push_back(&_genes.find(gene._innovationId)->second);
				mutate = true;//Marca a mutacao como feita
				break;
			}
		}
		count++;
	} while (!mutate && count < 10);
}

void Genome::AddNode()
{
	std::random_device dev;
	std::mt19937_64 rng(dev());
	//distribuicao contendo o numero de genes
	std::uniform_int_distribution<size_t> geneDist(0, _genes.size() - 1);
	//Garante que esse novo nó já não existe
	while (true)
	{
		//Itaretor do gene que será afetado
		auto gene = _genes.begin();
		//Desloca o gene para um gene aleatório
		std::advance(gene, geneDist(rng));
		//Desativa o gene
		gene->second._active = false;
		//COORDENADAS DO NOVO NODE
		double x1, x2, y1, y2;
		//Mapeia os nodes do gene
		std::map<const size_t, const Node*> nodesMap;
		for (size_t i = 0; i < _nodes.size(); i++)
		{
			nodesMap.insert(std::make_pair(_nodes.at(i)._id, &_nodes.at(i)));
		}
		//Se x1 for uma entrada
		if (gene->second._nodes[0] < _nInputs)
			x1 = 0;
		else
			x1 = nodesMap[gene->second._nodes[0]]->_x;
		//Se x2 for uma entrada
		if (gene->second._nodes[1] < _nInputs)
			x2 = 0;
		else
			x2 = nodesMap[gene->second._nodes[1]]->_x;
		//Se y1 for uma entrada
		if (gene->second._nodes[0] < _nInputs)
			y1 = gene->second._nodes[0];
		else
			y1 = nodesMap[gene->second._nodes[0]]->_y;
		//Se y2 for uma entrada
		if (gene->second._nodes[1] < _nInputs)
			y2 = gene->second._nodes[0];
		else
			y2 = nodesMap[gene->second._nodes[1]]->_y;

		double x = (x1 + x2) / 2;
		double y = (y1 + y2) / 2;
		bool exist = false;
		//Verifica se ja existe um nó nessa posicao
		for (Node& node : _nodes) {
			if (node._x == x && node._y == y)
			{
				exist = true;
				break;
			}
		}
		//Se existe tenta de novo
		if (exist)
		{
			continue;
		}
		//CRIA UM NOVO NODE
		Node newNode(_nodes.size() + _nInputs, x, y); //Cria o node
		newNode._innovationId = _innovationGenerator->getInnovation(newNode); //Id de inovacao
		_nodes.push_back(newNode); //Adiciona o novo node ao vetor de nodes

		//CRIA UM UM GENE DE CONEXA0 LIGANDO ESSE NOVO NODE AO SEGUNDO NODE DO GENE ANTIGO
		Gene newGene1(true, newNode._id, gene->second._nodes[1], gene->second._weight, gene->second._recurrent);
		newGene1._innovationId = _innovationGenerator->getInnovation(newGene1, &_nInputs, &_nodes); //Id de inovacao
		//Salva o gene na lisata de genes
		_genes.insert(std::make_pair(newGene1._innovationId, newGene1));
		_nodes[gene->second._nodes[1] - _nInputs]._genes.push_back(&_genes.find(newGene1._innovationId)->second);
		//CRIA UM GENE LIGANDO O PRIMEIRO NODE AO NOVO NODE CRIADO
		Gene newGene2(true, gene->second._nodes[0], newNode._id, 1.0, gene->second._recurrent);
		newGene2._innovationId = _innovationGenerator->getInnovation(newGene2, &_nInputs, &_nodes);
		//Salva o gene na lista de genes
		_genes.insert(std::make_pair(newGene2._innovationId, newGene2));
		_nodes.back()._genes.push_back(&_genes.find(newGene2._innovationId)->second);

		//Se chegou aqui finaliza pois a mutacao ocorreu
		break;
	}
}

double Genome::ActivationFunction(const double& x, const Node& node)
{
	double x2 = 0;

	switch (node._funcType)
	{
	case Node::FUNCTION_TYPE::SIG:
		x2 = 1 / (1 + std::abs(-x));
		break;
	case Node::FUNCTION_TYPE::TANH:
		//Evita estouro de memoria
		if (x > 15 || x < -15) {
			x2 = x > 15 ? 1 : -1;
		}
		else
			x2 = (1 - std::exp(-2 * x)) / (1 + std::exp(-2 * x));
		break;
	case Node::FUNCTION_TYPE::NEG_SIG:
		x2 = -1 * (1 / (1 + std::abs(-x)));
		break;
	case Node::FUNCTION_TYPE::COS:
		x2 = std::cos(x);
		break;
	case Node::FUNCTION_TYPE::NEG_COS:
		x2 = -1 * std::cos(x);
		break;
	case Node::FUNCTION_TYPE::SIN:
		x2 = std::sin(x);
		break;
	case Node::FUNCTION_TYPE::NEG_SIN:
		x2 = -1 * std::sin(x);
		break;
	default:
		break;
	}
	return x2;
}

Genome Genome::CrossOver(const Genome& genome)
{
	//Novo Genoma <- o genoma que chamu a função (melhor genoma)
	Genome newGenome(*this);
	std::uniform_int_distribution<int> half(0, 1);

	//Verifica cada gene desse novo genoma em busca de genes excessivos nos outro genoma
	for (auto& gene : newGenome._genes) {
		//Se o outro genoma tem esse gene
		if (genome._genes.count(gene.second._innovationId) > 0)
		{
			//Copiar o gene?
			if (half(_randomDev))
			{
				gene.second = genome._genes.find(gene.second._innovationId)->second;
			}
		}
	}
	//Busca os nodes compativeis para copiar o bias
	for (Node& node : newGenome._nodes) {
		std::vector<Node>::const_iterator  nodeGenome;

		for (std::vector<Node>::const_iterator nodeI = genome._nodes.begin(); nodeI != genome._nodes.end(); nodeI++)
		{
			nodeGenome = nodeI;
			if (nodeGenome->_innovationId == node._innovationId)
			{
				break;
			}
		}
		//Copiar o bias?
		if (half(_randomDev) && nodeGenome->_innovationId == node._innovationId)
		{
			node._bias = nodeGenome->_bias;
		}
	}
	newGenome._fitness = 0;
	newGenome._lifeTime = 0;
	newGenome._out = vector<double>(_nOut);
	newGenome._state.clear();
	return newGenome;
}

void Genome::Mutation(bool noTopology)
{
	std::uniform_int_distribution<int> half(0, 1);
	std::uniform_int_distribution<int> prob(0, 80);

	//Criar uma mutacao no peso de um gene aleatorio?
	if (prob(_randomEng) || noTopology)
	{
		size_t nMutations;
		if (_genes.size() > 1) {
			std::uniform_int_distribution<size_t> nMutationsDist(1, _genes.size() * 0.8);
			//Numero de mutacoes
			nMutations = nMutationsDist(_randomEng);
		}
		else
		{
			nMutations = 1;
		}
		for (size_t nMutation = 0; nMutation < nMutations; nMutation++)
		{
			//Probabilidade de sofrer mutacao nos pesos
			prob = std::uniform_int_distribution<int>(0, 95);
			//Se for mutacao nos pesos
			if (prob(_randomEng))
			{
				//Probabilidade de sofrer mutacao nos pesos ou nas funcoes de ativacao
				prob = std::uniform_int_distribution<int>(0, 90);
				//Se e pra mudar os pesos ou os unicos nodes sao de saida
				if (prob(_randomEng)) {
					//Numeor de tentativas de fazer a mutacao
					size_t count = 0;
					bool mutate(false);
					//Probabilidade de sofrer mutacao nos pesos seguindo a distribuição gauseana
					prob = std::uniform_int_distribution<int>(0, 90);
					do
					{
						//QUAL GENE SOFRERA MUTACAO?
						std::uniform_int_distribution<size_t> geneIdDist(0, _genes.size() - 1);
						auto geneId = _genes.begin();
						std::advance(geneId, geneIdDist(_randomEng));
						//Se o gene esta ativado
						if (geneId->second._active)
						{
							if (prob(_randomEng))
							{
								std::normal_distribution<double> weightDist(0, 0.05); //Distribuicao
								geneId->second._weight += weightDist(_randomEng); //muda o peso (Seguindo uma distribuicao normal)
							}
							else
							{
								std::uniform_real_distribution<double> weightDist(-1, 1); //Distribuicao
								geneId->second._weight = weightDist(_randomEng); //muda o peso (Seguindo uma distribuicao uniforme)
							}
							mutate = true; //Seta como mutado
						}
						count++;
					} while (!mutate && count < 10);
				}
				else {
					//Node que recebera a mutacao
					std::uniform_int_distribution<size_t> nodeIdDist(0, _nodes.size() - 1);
					size_t nodeId = nodeIdDist(_randomEng);
					//Funcao de ativacao
					std::uniform_int_distribution<size_t> funcTypeDist((size_t)Node::FIRST, (size_t)Node::LAST);
					//Tipo da funcao de ativacao
					size_t funcType = funcTypeDist(_randomEng);
					//MUDA A FUNCAO DE ATIVACAO
					//Se a funcao nao mudara
					if (_nodes[nodeId]._funcType == funcType)
					{
						//Se for a ultima
						if ((Node::FUNCTION_TYPE)funcType == Node::FUNCTION_TYPE::LAST)
						{
							_nodes[nodeId]._funcType = (Node::FUNCTION_TYPE)(--funcType);
						}
						else
						{
							_nodes[nodeId]._funcType = (Node::FUNCTION_TYPE)(++funcType);
						}
					}
					else {
						_nodes[nodeId]._funcType = (Node::FUNCTION_TYPE)funcType;
					}

				}
			}
			//Mutacao no bias de um node
			else
			{
				//QUAL NODE SOFRERA MUTACAO?
				std::uniform_int_distribution<size_t> nodeIdDist(0, _nodes.size() - 1);
				std::normal_distribution<double> biasDist(0, 0.05); //Distribuicao
				_nodes[nodeIdDist(_randomEng)]._bias += biasDist(_randomEng);
			}
		}
	}
	else {
		//ADD CONECTION
		if (half(_randomEng))
		{
			AddConnection();
		}
		//ADD NODE
		else
		{
			AddNode();
		}
		//Ordena os nodes pelo eixo x
		//Na ordem crescente o x nessa implementacao representa as camadas
		std::sort(_nodes.begin(), _nodes.end(), [](const Node& n1, const Node& n2) { return n1._x < n2._x; });
	}
}

#ifdef _WIN32
void Genome::Plot()
{
	using namespace matlab::engine;

	//Abre a janela do matlab
	const std::vector<String>& options = std::vector<String>(1, u"-desktop");
	//const std::vector<String>& options = std::vector<String>(1, u"");
	std::unique_ptr<MATLABEngine> matlabPtr = startMATLAB(options); //Inicia o matlab
	matlab::data::ArrayFactory factory; //Fucoes para lidar com dados no matlab
	//PLOTA OS NODES
	matlabPtr->eval(u"nodeCallback = @(src,evt) msgbox({ strcat('Id:', num2str(src.UserData(1))),strcat('Func:', src.UserData(2))})");
	//Para cada entrada
	for (size_t inputId = 0; inputId < _nInputs; inputId++)
	{
		std::string nodeX = std::to_string(0);
		std::string nodeY = std::to_string(inputId);
		std::string nodeId = std::to_string(-1);
		//plota o node
		matlabPtr->eval(u"plot(" + std::basic_string<char16_t>(nodeX.begin(), nodeX.end()) +
			u"," + std::basic_string<char16_t>(nodeY.begin(), nodeY.end()) + +u",'o','ButtonDownFcn', nodeCallback,'UserData',[" +
			std::basic_string<char16_t>(nodeId.begin(), nodeId.end()) + u" \"input\"]); hold on");
	}
	//Para cada node
	for (size_t nodeId = 0; nodeId < _nodes.size(); nodeId++)
	{
		std::string nodeX = std::to_string(_nodes[nodeId]._x);
		std::string nodeY = std::to_string(_nodes[nodeId]._y);
		std::string nodeIdSTR = std::to_string(nodeId);
		std::basic_string<char16_t> functionType;
		switch (_nodes[nodeId]._funcType)
		{
		case(Node::FUNCTION_TYPE::COS):
			functionType = u"\"cos\"";
			break;
		case(Node::FUNCTION_TYPE::NEG_COS):
			functionType = u"\"-cos\"";
			break;
		case(Node::FUNCTION_TYPE::NEG_SIG):
			functionType = u"\"-sig\"";
			break;
		case(Node::FUNCTION_TYPE::NEG_SIN):
			functionType = u"\"-sen\"";
			break;
		case(Node::FUNCTION_TYPE::SIG):
			functionType = u"\"sig\"";
			break;
		case(Node::FUNCTION_TYPE::SIN):
			functionType = u"\"sen\"";
			break;
		case(Node::FUNCTION_TYPE::TANH):
			functionType = u"\"tanH\"";
			break;
		default:
			break;
		}
		//plota o node
		matlabPtr->eval(u"plot(" + std::basic_string<char16_t>(nodeX.begin(), nodeX.end()) +
			u"," + std::basic_string<char16_t>(nodeY.begin(), nodeY.end()) + +u",'o','ButtonDownFcn', nodeCallback,'UserData',[" +
			std::basic_string<char16_t>(nodeIdSTR.begin(), nodeIdSTR.end()) + u" " + functionType + u"]); hold on");
	}
	//cria a funcao que desenha as sinapses
	matlabPtr->eval(u"drawArrow = @(x,y,varargin) quiver( x(1),y(1),x(2)-x(1),y(2)-y(1),0, varargin{:} )");
	matlabPtr->eval(u"synapseCallback = @(src,evt) msgbox({ strcat('wieght:', num2str(src.UserData(1))), strcat('InnovationId:', num2str(src.UserData(2)))})");
	//PLOTA AS SINAPSES
	size_t geneId = 0;
	for (size_t countGene = 0; countGene < _genes.size(); countGene++)
	{
		while (_genes.count(geneId) < 1)
		{
			geneId++;
			continue;
		}
		//PARA O X
		std::string x1 = "x1=[";
		//Se for de entrada
		if (_genes[geneId]._nodes[0] < _nInputs)
		{
			x1 += "0 ";
		}
		else
		{
			x1 += std::to_string(_nodes[_genes[geneId]._nodes[0] - _nInputs]._x) + " ";
		}
		//Se for de entrada
		if (_genes[geneId]._nodes[1] < _nInputs)
		{
			x1 += "0]";
		}
		else
		{
			x1 += std::to_string(_nodes[_genes[geneId]._nodes[1] - _nInputs]._x) + "]";
		}
		//PARA O Y
		std::string y1 = "y1=[";
		//Se for de entrada
		if (_genes[geneId]._nodes[0] < _nInputs)
		{
			y1 += std::to_string(_genes[geneId]._nodes[0]) + " ";
		}
		else
		{
			y1 += std::to_string(_nodes[_genes[geneId]._nodes[0] - _nInputs]._y) + " ";
		}
		//Se for de entrada
		if (_genes[geneId]._nodes[1] < _nInputs)
		{
			y1 += std::to_string(_genes[geneId]._nodes[1]) + "]";
		}
		else
		{
			y1 += std::to_string(_nodes[_genes[geneId]._nodes[1] - _nInputs]._y) + "]";
		}
		//Escreve o x e y
		matlabPtr->eval(std::basic_string<char16_t>(x1.begin(), x1.end()));
		matlabPtr->eval(std::basic_string<char16_t>(y1.begin(), y1.end()));
		std::string weightSTR(std::to_string(_genes[geneId]._weight));
		std::string InnovationSTR(std::to_string(_genes[geneId]._innovationId));
		std::basic_string<char16_t> userData = u"[" + std::basic_string<char16_t>(weightSTR.begin(), weightSTR.end()) + u" " + std::basic_string<char16_t>(InnovationSTR.begin(), InnovationSTR.end()) + u"]";
		if (_genes[geneId]._recurrent)
		{
			if (_genes[geneId]._active)
				matlabPtr->eval(u"drawArrow(x1,y1,'linewidth',0.05,'color','g','ShowArrowHead', 'off', 'Marker','s', 'MarkerSize',4, 'MarkerFaceColor',[0 0 1],'ButtonDownFcn', synapseCallback,'UserData', " + userData + u")");
			else
				matlabPtr->eval(u"drawArrow(x1,y1,'linewidth',0.05,'color','y','ShowArrowHead', 'off', 'Marker','s', 'MarkerSize',4, 'MarkerFaceColor',[0 0 1],'ButtonDownFcn', synapseCallback,'UserData', " + userData + u")");
		}
		else
		{
			if (_genes[geneId]._active)
				matlabPtr->eval(u"drawArrow(x1,y1,'linewidth',0.05,'color','r','ShowArrowHead', 'off', 'Marker','s', 'MarkerSize',4, 'MarkerFaceColor',[0 0 1],'ButtonDownFcn', synapseCallback,'UserData', " + userData + u")");
			else
				matlabPtr->eval(u"drawArrow(x1,y1,'linewidth',0.05,'color','y','ShowArrowHead', 'off', 'Marker','s', 'MarkerSize',4, 'MarkerFaceColor',[0 0 1],'ButtonDownFcn', synapseCallback,'UserData', " + userData + u")");
		}
		geneId++;
	}
	std::cout << "press enter to end\n";
	std::cin.ignore(std::cin.rdbuf()->in_avail());
	std::cin.ignore();
}
#endif
vector<double> Genome::Process(const map<size_t, double>& input)
{
	map<size_t, double> outs = input;
	size_t outId = 0;
	//Para cada node
	for (size_t nodeId = 0; nodeId < _nodes.size(); nodeId++)
	{
		double sum = _nodes[nodeId]._bias; //Bias
		//Para cada gene do node
		for (size_t geneId = 0; geneId < _nodes[nodeId]._genes.size(); geneId++)
		{
			if (!_nodes[nodeId]._genes[geneId]->_active)
			{
				continue;
			}
			//Se for um gene recorrente e ha estados recorrentes
			if (_nodes[nodeId]._genes[geneId]->_recurrent && _state.size() > 0)
			{
				//Peso * estado
				sum += _nodes[nodeId]._genes[geneId]->_weight *
					_state[_nodes[nodeId]._genes[geneId]->_nodes[0]];
			}
			else {
				//Peso * entrada
				sum += _nodes[nodeId]._genes[geneId]->_weight *
					outs[_nodes[nodeId]._genes[geneId]->_nodes[0]];
			}
		}
		double outRes = ActivationFunction(sum, _nodes[nodeId]);
		//Funcao de ativacao
		outs.insert(std::make_pair(_nodes[nodeId]._id, outRes));
		//Se for uma saida salva a resposta
		if (_nodes[nodeId]._id < _nOut + _nInputs && _nodes[nodeId]._id >= _nInputs)
		{
			_out[_nodes[nodeId]._id - _nInputs] = outRes;
		}
	}
	_state = outs;
	return _out;
}

void Genome::Forget()
{
	if (!_state.empty())
		_state.clear();
}