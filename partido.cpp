#include "candidato.h"
#include "partido.h"

partido::partido(short numero, std::string sigla){
    this->numeroPartido = numero;
    this->siglaPartido = sigla;

    this->votosLegenda = 0;
    this->votosNominais = 0;
    this->candidatosEleitos = 0;
}
void partido::adicionaCandidato(candidato* cand){
    //nao garante que o candidato ja foi adicionado
    if(cand->estaEleito()){
        this->candidatosEleitos++;
    }
    this->candidatosPertencentes.push_back(cand);
}
void partido::incrementaVotosNominais(unsigned qtd){
    this->votosNominais+=qtd;
}
void partido::incrementaVotosLegenda(unsigned qtd){
    this->votosLegenda+=qtd;
}
std::list<candidato*> partido::getCandidatos(){
    return this->candidatosPertencentes;
}
std::string partido::getSigla(){
    return this->siglaPartido;
}
unsigned short partido::getNumero(){
    return this->numeroPartido;
}
unsigned long partido::getTotalVotos(){
    return this->votosLegenda + this->votosNominais;
}
unsigned long partido::getVotosLegenda(){
    return this->votosLegenda;
}
unsigned long partido::getVotosNominal(){
    return this->votosNominais;
}
unsigned long partido::getQtdCandidatosEleitos(){
    return this->candidatosEleitos;
}

std::string partido::stringInfoPartido(){
    std::stringstream ss;
    ss << "-----------------------------------------------" << std::endl
       << "Numero do Partido: " << this->numeroPartido << std::endl
       << "Sigla do Partido: " << this->siglaPartido << std::endl
       << "Total de votos em legenda do Partido: " << this->votosLegenda << std::endl
       << "Total de votos nominais do Partido: " << this->votosNominais << std::endl
       << "Total de votos do Partido: " << this->votosNominais + this->votosLegenda << std::endl
       << "Quantidade de candidatos eleitos do Partido: " << this->candidatosEleitos <<std::endl
       << "-----------------------------------------------" << std::endl;
    
    return ss.str();
}