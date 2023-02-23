#include <iostream>
#include <fstream>
#include <list>
#include "sistemaEleitoral.h"
#include <map>
#include <vector>
#include "csvReader.h"

//Retorna string com dados de votacao de um candidato
string stringVotacaoCandidato(candidato *candidato);

//Retorna string com dados de votacao de um candidato mas com outra formatacao
string stringVotacaoCandidatoTipo2(candidato *candidato);

//Retorna string com dados de votacao de um candidato
string stringVotacaoPartido(partido *partido);

//Serve para concatenar uma serie de valores de uma lista usando uma funcao que os converte em string
template <class T>
string stringListaElementos(list<T> &lista, string (*stringElemento)(T), vector<int> *indices, int quantidade);

//Serve para imprimir porcentagens
string stringPorcentagem(float parte, float todo);

//Serve para formatar quantidades de votos
string formataVotos(unsigned long value);

//Serve para formatar strings
string quote(string item);

//Serve para formatar strings
string unquote(string &item);

//Serve para formatar varias strings
void quoteAll(vector<string> selectedItems);

//Serve para formatar varias strings
vector<string> unquoteAll(vector<string> selectedItems);


//Serve como funcoes de ordenacao
bool maisVotosOuMaisVelhoCandidato(candidato *c1, candidato *c2);
bool menorNumeroPartido(partido *p1, partido *p2);
bool maisVotosOuMenorNumeroPartido(partido *p1, partido *p2);

//Serve para tirar whitespaces de uma strring
void tiraWhiteSpaceFinal(string &nome);

int main(int argc, char *argv[])
{

	//TRATAMENTO DOS ARGUMENTOS
	if (argc <= 4)
	{
		std::cout
			<< "Usar ./deputados <opção_de_cargo(--federal ou --estadual)> <caminho_arquivo_candidatos> <caminho_arquivo_votacao> <data>" << endl;
		exit(0);
	}

	std::vector<std::string> argumentos(argv, argv + argc);

	int tipoEleicao = argumentos[1] == "--federal" ? 6 : argumentos[1] == "--estadual" ? 7
																					   : -1;

	if (tipoEleicao == -1)
	{
		std::cout << "Argumento \"" + argumentos[1] + "\" invalido. Usar \"--federal\" ou \"--estadual\"" << endl;
		exit(0);
	}

	string arquivoCandidatos = argumentos[2];
	string arquivoVotacao = argumentos[3];
	string dataEleicao = argumentos[4];

	//INFORMACOES SOBRE A LEITURA

	csvReader candidatos(arquivoCandidatos, ';');
	csvReader votacao(arquivoVotacao, ';');

	// valores que serao selecionados do arquivo
	string valuesCandidatos[] = {
		/*0*/ "\"CD_CARGO\"",
		/*1*/ "\"CD_SITUACAO_CANDIDATO_TOT\"",
		/*2*/ "\"NR_CANDIDATO\"",
		/*3*/ "\"NM_URNA_CANDIDATO\"",
		/*4*/ "\"NR_PARTIDO\"",
		/*5*/ "\"SG_PARTIDO\"",
		/*6*/ "\"NR_FEDERACAO\"",
		/*7*/ "\"DT_NASCIMENTO\"",
		/*8*/ "\"CD_SIT_TOT_TURNO\"",
		/*9*/ "\"CD_GENERO\"",
		/*10*/ "\"NM_TIPO_DESTINACAO_VOTOS\""};

	string valuesVotacoes[] = {
		/*0*/ "\"CD_CARGO\"",
		/*1*/ "\"NR_VOTAVEL\"",
		/*2*/ "\"QT_VOTOS\""};

	sistemaEleitoral sistema;

	//LEITURA DOS CANDIDATOS
	while (candidatos.hasNextValues())
	{
		std::vector<std::string> valores = candidatos.nextValues(valuesCandidatos, 11);

		int codigoCargo = stoi(unquote(valores[0]));
		unsigned situacaoCandidato = stoi(unquote(valores[1]));
		std::string tipoDestinacao = unquote(valores[10]);

		std::string siglaPartido = unquote(valores[5]);
		unsigned short numeroPartido = stoi(unquote(valores[4]));

		// nao faz parte desse cargo
		if (codigoCargo != tipoEleicao)
		{
			sistema.cadastraPartido(numeroPartido, siglaPartido);
			continue;
		}

		// se o candidato nao foi deferido e nao vai para a legenda
		// mesma instrução do if anterior por razão de legibilidade
		if ((situacaoCandidato != 2 && situacaoCandidato != 16) && (tipoDestinacao != "Válido (legenda)"))
		{
			sistema.cadastraPartido(numeroPartido, siglaPartido);
			continue;
		}

		unsigned numeroCandidato = stoi(unquote(valores[2]));
		std::string nomeUrna = unquote(valores[3]);
		tiraWhiteSpaceFinal(nomeUrna); // alguns candidatos vêm do arquivo com whitespace depois do nome
		unsigned numeroFederacao = stoi(unquote(valores[6]));
		std::string dataNascimento = unquote(valores[7]);
		unsigned situacaoTotalizacao = stoi(unquote(valores[8]));
		unsigned short genero = stoi(unquote(valores[9]));

		sistema.cadastraCandidato(numeroCandidato, nomeUrna, siglaPartido, numeroFederacao,
								  dataNascimento, situacaoTotalizacao, genero, tipoDestinacao);
	}

	//LEITURA DOS VOTOS
	while (votacao.hasNextValues())
	{
		std::vector<std::string> valores = votacao.nextValues(valuesVotacoes, 3);

		int codigoCargo = stoi(unquote(valores[0]));
		std::string numeroVotavel = unquote(valores[1]);
		unsigned quantidadeVotos = stoi(unquote(valores[2]));

		if (codigoCargo != tipoEleicao)
		{
			continue;
		}

		// voto legenda (vai direto para o partido)
		if (numeroVotavel.size() == 2)
		{
			sistema.declaraVotoLegenda(stoi(numeroVotavel), quantidadeVotos);
		}
		else
		{
			// inclui a votação dos candidatos para legenda
			sistema.declaraVotosNominais(stoi(numeroVotavel), quantidadeVotos);
		}
	}

	//COLETA DE DADOS PARA OS RELATORIOS

	list<partido *> partidosOrdenados;
	list<candidato *> candidatosOrdenados;

	map<partido *, pair<candidato *, candidato *>> primeirosUltimosCandidatosPartidos;

	// idades: {18,30, 40, 50, 60}
	int quantidadePorFaixaEtaria[5] = {0, 0, 0, 0, 0};
	// {feminino, masculino}
	int quantidadePorGenero[2] = {0, 0};

	// candidatos usados nos relatorios 4 e 5
	list<candidato *> eleitos;

	list<candidato *> beneficiados;
	vector<int> indicesBeneficiados;

	list<candidato *> naoBeneficiados;
	vector<int> indicesnaoBeneficiados;

	list<partido *> partidosRankeadosPorCandidato;

	// inicializa a lista de partidosOrdenados
	partidosOrdenados = sistema.getPartidos();
	partidosOrdenados.sort(maisVotosOuMenorNumeroPartido);

	// Serve para juntar todas as listas de candidatos dos partidos
	// cada lista precisa ser ordenada individualmente e juntada com
	// a lista geral   |
	//				  V

	for (partido *p : partidosOrdenados)
	{
		list<candidato *> candidatosPartido = p->getCandidatos();
		candidatosPartido.sort(maisVotosOuMaisVelhoCandidato);

		// associa o primeiro e ultimo no map para usar depois
		primeirosUltimosCandidatosPartidos[p] = pair<candidato *, candidato *>(candidatosPartido.front(), candidatosPartido.back());

		// junta as listas em uma lista maior
		candidatosOrdenados.merge(candidatosPartido, maisVotosOuMaisVelhoCandidato);
	}

	// usando a posicao do candidato na lista para saber
	// se ele foi eleito mas nao entrou nas vagas ou se ele nao foi eleito mas
	// entrou nas vagas (vagas usando um sistema majoritario)

	std::list<candidato *> candidatosMaisVotadosEmpatadosPorPartido;
	std::list<partido *> partidosEmpatadosPorCandidato;

	int indiceCandidato = 1;
	map<unsigned short, partido *> partidosJaContados;

	for (candidato *c : candidatosOrdenados)
	{
		// if(c->vaiParaLegenda())
			// cout << c->stringInfoCandidato(false) << endl;
		if (partidosJaContados.count(c->getPartidoPertencente()->getNumero()) == 0)
		{
			partidosJaContados.insert(pair<unsigned, partido*>(c->getPartidoPertencente()->getNumero(), c->getPartidoPertencente()));

			if (candidatosMaisVotadosEmpatadosPorPartido.empty())
			{

				candidatosMaisVotadosEmpatadosPorPartido.push_back(c);
			}
			else
			{
				// último candidato inserido dos empatados
				candidato *ultimoBuffer = candidatosMaisVotadosEmpatadosPorPartido.back();

				if (ultimoBuffer->getTotalVotos() == c->getTotalVotos())
				{
					candidatosMaisVotadosEmpatadosPorPartido.push_back(c);
				}
				else
				{
					for (candidato *a : candidatosMaisVotadosEmpatadosPorPartido)
					{

						partidosEmpatadosPorCandidato.push_back(a->getPartidoPertencente());
					}

					partidosEmpatadosPorCandidato.sort(menorNumeroPartido);
					partidosRankeadosPorCandidato.splice(partidosRankeadosPorCandidato.end(), partidosEmpatadosPorCandidato);

					candidatosMaisVotadosEmpatadosPorPartido.clear();
					candidatosMaisVotadosEmpatadosPorPartido.push_back(c);
				}
			}
		}

		if (c->estaEleito())
		{
			
			// colocar na lista de eleitos
			eleitos.push_back(c);

			if (indiceCandidato > sistema.getNumeroDeVagas())
			{
				// adicionar como beneficiado tbm nesse caso
				beneficiados.push_back(c);
				indicesBeneficiados.push_back(indiceCandidato);
			}

			// genero
			switch (c->getGenero())
			{
			case 4:
				quantidadePorGenero[0]++;
				break;
			case 2:
				quantidadePorGenero[1]++;
				break;

			default:
				std::cout << "ALGO DEU ERRADO COM O GENERO" << endl;
				break;
			}

			int idade = c->getIdade(dataEleicao);

			quantidadePorFaixaEtaria[0] += idade < 30 ? 1 : 0;
			quantidadePorFaixaEtaria[1] += idade >= 30 && idade < 40 ? 1 : 0;
			quantidadePorFaixaEtaria[2] += idade >= 40 && idade < 50 ? 1 : 0;
			quantidadePorFaixaEtaria[3] += idade >= 50 && idade < 60 ? 1 : 0;
			quantidadePorFaixaEtaria[4] += idade >= 60 ? 1 : 0;
		}
		else
		{
			if (indiceCandidato <= sistema.getNumeroDeVagas())
			{

				naoBeneficiados.push_back(c);
				indicesnaoBeneficiados.push_back(indiceCandidato);
			}
		}

		indiceCandidato++;
	}
	if (!candidatosMaisVotadosEmpatadosPorPartido.empty())
	{
		for (candidato *a : candidatosMaisVotadosEmpatadosPorPartido)
		{
			partidosEmpatadosPorCandidato.push_back(a->getPartidoPertencente());
		}
		partidosEmpatadosPorCandidato.sort(menorNumeroPartido);
		partidosRankeadosPorCandidato.splice(partidosRankeadosPorCandidato.end(), partidosEmpatadosPorCandidato);
	}

	std::cout << "Número de vagas: " << sistema.getNumeroDeVagas() << endl
		  << endl
		  << "Deputados ";

	if (tipoEleicao == 7)
	{
		std::cout << "estaduais ";
	}
	else if (tipoEleicao == 6)
	{
		std::cout << "federais ";
	}

	std::cout << "eleitos:" << endl;
	std::cout << stringListaElementos<candidato *>(eleitos, stringVotacaoCandidato, NULL, -1);
	std::cout << endl;

	std::cout << "Candidatos mais votados (em ordem decrescente de votação e respeitando número de vagas):" << endl;

	std::cout << stringListaElementos<candidato *>(candidatosOrdenados, stringVotacaoCandidato, NULL, sistema.getNumeroDeVagas());
	std::cout << endl;
	std::cout << "Teriam sido eleitos se a votação fosse majoritária, e não foram eleitos:" << endl
		  << "(com sua posição no ranking de mais votados)" << endl;

	std::cout << stringListaElementos<candidato *>(naoBeneficiados, stringVotacaoCandidato, &indicesnaoBeneficiados, -1);
	std::cout << endl;
	std::cout << "Eleitos, que se beneficiaram do sistema proporcional:" << endl
		  << "(com sua posição no ranking de mais votados)" << endl;

	std::cout << stringListaElementos<candidato *>(beneficiados, stringVotacaoCandidato, &indicesBeneficiados, -1);
	std::cout << endl;
	std::cout << "Votação dos partidos e número de candidatos eleitos:" << endl;

	std::cout << stringListaElementos<partido *>(partidosOrdenados, stringVotacaoPartido, NULL, -1);
	std::cout << endl;
	std::cout << "Primeiro e último colocados de cada partido:" << endl;

	int i = 1;
	for (partido *p : partidosRankeadosPorCandidato)
	{

		// verificar a ordem dos pares no mapa
		candidato *primeiro = primeirosUltimosCandidatosPartidos[p].first;
		candidato *segundo = primeirosUltimosCandidatosPartidos[p].second;

		std::cout << i << " - " << p->getSigla() << " - " << p->getNumero() << ", " << stringVotacaoCandidatoTipo2(primeiro) << " / " << stringVotacaoCandidatoTipo2(segundo) << endl;
		i++;
	}

	std::cout << endl;
	std::cout << "Eleitos, por faixa etária (na data da eleição):" << endl
		  << "      Idade < 30: " << quantidadePorFaixaEtaria[0] << " (" + stringPorcentagem(quantidadePorFaixaEtaria[0], sistema.getNumeroDeVagas()) + ")" << endl
		  << "30 <= Idade < 40: " << quantidadePorFaixaEtaria[1] << " (" + stringPorcentagem(quantidadePorFaixaEtaria[1], sistema.getNumeroDeVagas()) + ")" << endl
		  << "40 <= Idade < 50: " << quantidadePorFaixaEtaria[2] << " (" + stringPorcentagem(quantidadePorFaixaEtaria[2], sistema.getNumeroDeVagas()) + ")" << endl
		  << "50 <= Idade < 60: " << quantidadePorFaixaEtaria[3] << " (" + stringPorcentagem(quantidadePorFaixaEtaria[3], sistema.getNumeroDeVagas()) + ")" << endl
		  << "60 <= Idade     : " << quantidadePorFaixaEtaria[4] << " (" + stringPorcentagem(quantidadePorFaixaEtaria[4], sistema.getNumeroDeVagas()) + ")" << endl;

	std::cout << endl;
	std::cout << "Eleitos, por gênero:" << endl
		  << "Feminino: " << quantidadePorGenero[0] << " (" + stringPorcentagem(quantidadePorGenero[0], sistema.getNumeroDeVagas()) << ")" << endl
		  << "Masculino: " << quantidadePorGenero[1] << " (" + stringPorcentagem(quantidadePorGenero[1], sistema.getNumeroDeVagas()) << ")" << endl;

	std::cout << endl;
	std::cout << "Total de votos válidos: " << formataVotos(sistema.getTotalVotos()) << endl;
	std::cout << "Total de votos nominais: " << formataVotos(sistema.getVotosNominal()) << " (" << stringPorcentagem(sistema.getVotosNominal(), sistema.getTotalVotos()) << ")" << endl;
	std::cout << "Total de votos de legenda: " << formataVotos(sistema.getVotosLegenda()) << " (" << stringPorcentagem(sistema.getVotosLegenda(), sistema.getTotalVotos()) << ")" << endl;

	return 0;
}

string stringPorcentagem(float parte, float todo)
{
	char retorno[7];
	string restornoSt;

	sprintf(retorno, "%.2f%%", (parte / todo) * 100);
	if((parte/todo)*100 == 100){
		retorno[3] = ',';
	}
	else if ((parte / todo) * 100 >= 10)
	{
		retorno[2] = ',';
	}
	else
	{
		retorno[1] = ',';
	}

	restornoSt.append(retorno);
	return restornoSt;
}

string quote(string item)
{
	return "\"" + item + "\"";
}

string unquote(string &item)
{
	item.erase(item.begin());
	item.pop_back();
	return item;
}

vector<string> unquoteAll(vector<string> selectedItems)
{
	for (long unsigned i = 0; i < selectedItems.size(); i++)
	{
		selectedItems[i] = unquote(selectedItems[i]);
		// std::cout << selectedItems[i] << endl;
	}
	return selectedItems;
}

string stringVotacaoCandidato(candidato *candidato)
{
	stringstream streamRetorno;
	streamRetorno.imbue(locale("pt_BR.UTF-8"));

	// formato: <Nome> (<SiglaPartido>, <TotalVotos> votos)

	string nomeFormatado = candidato->getNome();
	if (candidato->getNumeroFederacao() != -1)
	{
		nomeFormatado = "*" + nomeFormatado;
	}

	streamRetorno << nomeFormatado
				  << " (" << candidato->getPartidoPertencente()->getSigla() << ", "
				  << candidato->getTotalVotos() << " votos)";
	return streamRetorno.str();
}

string stringVotacaoCandidatoTipo2(candidato *candidato)
{
	stringstream streamRetorno;
	streamRetorno.imbue(locale("pt_BR.UTF-8"));

	// formato: <Nome> (<Numero>, <TotalVotos> votos)

	streamRetorno << candidato->getNome()
				  << " (" << to_string(candidato->getNumeroCandidato()) << ", "
				  << candidato->getTotalVotos() << ((candidato->getTotalVotos() > 1) ? " votos" : " voto") << ")";
	return streamRetorno.str();
}

string stringVotacaoPartido(partido *partido)
{

	stringstream streamRetorno;
	streamRetorno.imbue(locale("pt_BR.UTF-8"));

	streamRetorno << partido->getSigla() << " - " << partido->getNumero()
				  << ", " << partido->getTotalVotos() << ((partido->getTotalVotos() > 1) ? " votos" : " voto")
				  << " (" << partido->getVotosNominal() << ((partido->getVotosNominal() > 1) ? " nominais" : " nominal") << " e " << partido->getVotosLegenda() << " de legenda), "
				  << partido->getQtdCandidatosEleitos() << ((partido->getQtdCandidatosEleitos() > 1) ? " candidatos" : " candidato") << ((partido->getQtdCandidatosEleitos() > 1) ? " eleitos" : " eleito");
	return streamRetorno.str();
}

template <class T>

// Concatena elementos de uma lista convertendo-os em strings
// se indices for NULL os indices iniciam em 1 e sao incrementados
// se quantidade for -1 todos os elementos da lista sao usados

string stringListaElementos(list<T> &lista, string (*stringElemento)(T), vector<int> *indices, int quantidade)
{

	string retorno;

	if (indices != NULL)
	{
		if (lista.size() != indices->size())
		{
			throw std::invalid_argument("O vetor de indices deve ter o mesmo tamanho da lista de elementos");
		}
	}

	int i = 1;
	for (T e : lista)
	{

		if (quantidade > 0)
		{
			if (i > quantidade)
			{
				break;
			}
		}

		if (indices == NULL)
		{
			retorno += to_string(i);
		}
		else
		{
			retorno += to_string((*indices)[i - 1]);
		}

		retorno += " - " + stringElemento(e) + "\n";
		i++;
	}
	return retorno;
}

bool maisVotosOuMenorNumeroPartido(partido *p1, partido *p2)
{

	if (p1->getTotalVotos() == p2->getTotalVotos())
	{
		if (p1->getNumero() < p2->getNumero())
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	if (p1->getTotalVotos() > p2->getTotalVotos())
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool menorNumeroPartido(partido *p1, partido *p2)
{

	if (p1->getNumero() < p2->getNumero())
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool maisVotosOuMaisVelhoCandidato(candidato *c1, candidato *c2)
{

	if (c1->getTotalVotos() == c2->getTotalVotos())
	{

		struct data nascimentoC1(c1->getDataNascimento());
		struct data nascimentoC2(c2->getDataNascimento());

		if (nascimentoC1.vemAntesDe(nascimentoC2.formatString()))
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	if (c1->getTotalVotos() > c2->getTotalVotos())
	{
		return true;
	}
	else
	{
		return false;
	}
}

void tiraWhiteSpaceFinal(string &nome)
{
	int qtdWhiteSpace = 0;

	for (int i = nome.size() - 1; nome[i] == ' '; i--)
	{
		qtdWhiteSpace++;
	}
	int x = 0;
	while (x < qtdWhiteSpace)
	{
		x++;
		nome.pop_back();
	}
}

string formataVotos(unsigned long value){
	// adaptado de https://stackoverflow.com/questions/7276826/format-number-with-commas-in-c
    std::stringstream ss;
    ss.imbue(std::locale("pt_BR.UTF-8"));
    ss << std::fixed << value;
    return ss.str();
}
