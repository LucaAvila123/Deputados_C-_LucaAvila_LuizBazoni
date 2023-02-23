#ifndef PARTIDO
#define PARTIDO

#include "candidato.h"
#include <string>
#include <list>

class partido{
    std::list<candidato*> candidatosPertencentes;
    std::string siglaPartido;

    unsigned short numeroPartido;
    unsigned long votosLegenda;
    unsigned long votosNominais;
    unsigned long candidatosEleitos;

public:
    partido(short numero, std::string sigla);
    void adicionaCandidato(candidato* cand);
    void incrementaVotosNominais(unsigned qtd);
    void incrementaVotosLegenda(unsigned qtd);
    std::list<candidato*> getCandidatos();
    std::string getSigla();
    unsigned short getNumero();
    unsigned long getTotalVotos();
    unsigned long getVotosLegenda();
    unsigned long getVotosNominal();
    unsigned long getQtdCandidatosEleitos();
    std::string stringInfoPartido(); 
};

#endif //PARTIDO