#include "sistemaEleitoral.h"
#include "candidato.h"
#include <cstring>
#include <fstream>
#include <iostream>

void sistemaEleitoral::cadastraCandidato(
// argumentos do cadastro
unsigned numeroCandidato, std::string nomeUrna,std::string siglaPartido, unsigned numeroFederacao, 
std::string dataNascimento, unsigned situacaoTotalizacao, unsigned short genero, std::string tipoDestinacao){
    
    char numeroPartido[3] = "";
    char numeroCandidato_st[100];
    sprintf(numeroCandidato_st, "%d", numeroCandidato);
    strncpy(numeroPartido, numeroCandidato_st, 2);


    int numeroIntPartido = 0;
    sscanf(numeroPartido, "%d", &numeroIntPartido);

    partido* p; 

    //se o partido nao foi cadastrado ainda que assim seja cadastrado
    if(!mapaPartidosCadastrados.count(numeroIntPartido)){
        p = new partido(numeroIntPartido, siglaPartido);
        mapaPartidosCadastrados.insert(std::pair<unsigned, partido*>(numeroIntPartido, p));
        listaPartidosCadastrados.push_back(p);
    }else{
        p = mapaPartidosCadastrados[numeroIntPartido];
    }

    //se o candidato nao foi cadastrado ainda que seja cadastrado
    int mapaval = mapaCandidatosCadastrados.count(numeroCandidato);
    //std::cout << "passei por aqui" << std::endl;

    if(!mapaval){

    

    
    candidato*c = new candidato(nomeUrna, dataNascimento, genero, numeroCandidato,
                                              numeroFederacao, p, tipoDestinacao, situacaoTotalizacao);

    if(c->estaEleito()){
        this->numeroDeVagas++;
    }

    mapaCandidatosCadastrados.insert(std::pair<unsigned, candidato*>(numeroCandidato, c));

        // o candidato de legenda ainda está sendo colocado no mapa de candidatos, mas não nas listas
        // os votos de tal candidato ainda são considerados
        if(!c->vaiParaLegenda())
        {
            listaCandidatosCadastrados.push_back(c);
            p->adicionaCandidato(c);
        }
       
    }
}

//talvez nao precisa implementar
void sistemaEleitoral::cadastraPartido(unsigned numeroPartido,std::string siglaPartido){
    partido* p; 
    if(!mapaPartidosCadastrados.count(numeroPartido)){
        p = new partido(numeroPartido, siglaPartido);
        mapaPartidosCadastrados.insert(std::pair<unsigned, partido*>(numeroPartido, p));
        listaPartidosCadastrados.push_back(p);
    }
}

std::list<partido*> sistemaEleitoral::getPartidos(){
    return this->listaPartidosCadastrados;
}

std::list<candidato*> sistemaEleitoral::getCandidatos(){
    return this->listaCandidatosCadastrados;
}

unsigned short sistemaEleitoral::getNumeroDeVagas(){
    return this->numeroDeVagas;
}

unsigned long sistemaEleitoral::getVotosNominal(){
    return this->totalVotosNominais;
}

unsigned long sistemaEleitoral::getVotosLegenda(){
    return this->totalVotosLegenda;
}

unsigned long sistemaEleitoral::getTotalVotos(){
    return this->totalVotosLegenda + this->totalVotosNominais;
}

void sistemaEleitoral::declaraVotosNominais(int numeroCandidato, int quantidade){

    candidato* c = mapaCandidatosCadastrados[numeroCandidato];
    
    if(c == NULL){
        return;
    }

    partido* p = c->getPartidoPertencente();
    
    if(c->vaiParaLegenda()){
        // std::cout << c->getNome() << std::endl;

        p->incrementaVotosLegenda(quantidade);
        this->totalVotosLegenda+=quantidade;
       
    }else{
        p->incrementaVotosNominais(quantidade);
        this->totalVotosNominais+=quantidade;
    }

    c->incrementaVotos(quantidade);
}

void sistemaEleitoral::declaraVotoLegenda(int numeroPartido, int quantidade){

    partido* p = mapaPartidosCadastrados[numeroPartido];

    if(p == NULL){
        return;
    }

    p->incrementaVotosLegenda(quantidade);
    this->totalVotosLegenda+=quantidade;
}