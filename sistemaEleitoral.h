#ifndef SISTEMA
#define SISTEMA

#include <list>
#include <map>
#include "partido.h"
#include "candidato.h"
#include <fstream>


//essa classe tera o pattern de singleton
class sistemaEleitoral{

    unsigned long totalVotosNominais;
    unsigned long totalVotosLegenda;
    unsigned short numeroDeVagas;

    std::map<unsigned, candidato*> mapaCandidatosCadastrados;
    std::map <unsigned short, partido*> mapaPartidosCadastrados;

    std::list<candidato*> listaCandidatosCadastrados;
    std::list<partido*> listaPartidosCadastrados;

public:
    sistemaEleitoral(){
        this->numeroDeVagas = 0;
        this->totalVotosLegenda = 0;
        this->totalVotosNominais = 0; 
    }

    ~sistemaEleitoral(){
        
        for(partido* p : listaPartidosCadastrados){
            delete(p);
        }

        //deletando candidatos
        for(candidato* c : listaCandidatosCadastrados){
            delete(c);
        }
    }

    void cadastraCandidato(
    // argumentos do cadastro
    unsigned numeroCandidato, std::string nomeUrna,std::string siglaPartido, unsigned numeroFederacao, 
	std::string dataNascimento, unsigned situacaoTotalizacao, unsigned short genero, std::string tipoDestinacao);

    //talvez nao precisa implementar
    void cadastraPartido(unsigned numeroCandidato,std::string siglaPartido);

    std::list<partido*> getPartidos();
    std::list<candidato*> getCandidatos();
    unsigned short getNumeroDeVagas();
    unsigned long getVotosNominal();
    unsigned long getVotosLegenda();
    unsigned long getTotalVotos();

    void declaraVotosNominais(int numeroCandidato, int quantididade);
    void declaraVotoLegenda(int numeroPartido, int quantidade);
};

#endif //SISTEMA