#include "candidato.h"
#include "partido.h"
#include <iostream>

candidato::candidato(
// argumentos do construtor
std::string nome, std::string dataNascimento, short genero, unsigned numeroCandidato, 
int numeroFederacao, partido* partidoCandidato, std::string destinoVotos, unsigned situacaoTurno
){
    this->nomeDeUrna = nome;
    this->dataDeNascimento = new data(dataNascimento);
    this->genero = genero;
    this->numeroCandidato = numeroCandidato;
    this->numeroFederacao = numeroFederacao;
    this->totalVotos = 0;
    
    if(partidoCandidato == NULL){
        throw std::invalid_argument("O partido informado para a construcao do candidato nao possui referencia valida");
    }else{
        this->partidoPertencente = partidoCandidato;
    }

    this->candidatoVaiParaLegenda = (destinoVotos == "Válido (legenda)") ? true: false;
    this->candidatoEstaEleito = (situacaoTurno == 2 || situacaoTurno == 3) ? true: false;
}

candidato::~candidato(){
    delete this->dataDeNascimento;
}




partido* candidato::getPartidoPertencente(){
    return this->partidoPertencente;
}
std::string candidato::getDataNascimento(){
    return this->dataDeNascimento->formatString();
}

std::string candidato::getNome(){
    return this->nomeDeUrna;
}
unsigned short candidato::getGenero(){
    return this->genero;
}
unsigned candidato::getNumeroCandidato(){
    return this->numeroCandidato;
}
int candidato::getNumeroFederacao(){
    return this->numeroFederacao;
}
bool candidato::vaiParaLegenda(){
    return this->candidatoVaiParaLegenda;
}
bool candidato::estaEleito(){
    return this->candidatoEstaEleito;
}
unsigned long candidato::getTotalVotos(){
    return this->totalVotos;
} 
short candidato::getIdade(std::string dataComparacao){
    data comp(dataComparacao);
    if(this->dataDeNascimento->mes < comp.mes){
        return comp.ano - this->dataDeNascimento->ano;  
    } else if(this->dataDeNascimento->mes == comp.mes){
        if(this->dataDeNascimento->dia < comp.dia){
            return comp.ano - this->dataDeNascimento->ano;
        }
    }
    return comp.ano - this->dataDeNascimento->ano - 1;
}
void candidato::incrementaVotos(unsigned qtd){
    this->totalVotos+=qtd;
}
std::string candidato::stringInfoCandidato(bool infoPartido){
    std::stringstream ss;
    ss << "-----------------------------------------------" << std::endl
       << "Nome do candidato: " << this->nomeDeUrna << std::endl
       << "Numero do candidato: " << this->numeroCandidato << std::endl
       << "Numero de federacao do candidato: " << this->numeroFederacao << std::endl
       << "Se o candidato vai para legenda: " << this->candidatoVaiParaLegenda << std::endl
       << "Se o candidato esta eleito: " << this->candidatoEstaEleito << std::endl
       << "Genero do candidato: " << this->genero << std::endl
       << "Total de votos do candidato: " << this->totalVotos << std::endl
       << "Data de nascimento do candidato: " << this->dataDeNascimento->formatString() << std::endl
       << "Idade do Candidato: " << this -> getIdade("02/10/2022") << std::endl;

    if(infoPartido){
       ss<< "Informacoes sobre o partido do candidato: \n" << std::endl
         << this->partidoPertencente->stringInfoPartido() << std::endl;
    }else{
        ss << "-----------------------------------------------" << std::endl;
    }
    return ss.str();
} 