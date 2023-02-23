#ifndef CANDIDATO
#define CANDIDATO

#include <string>
#include <sstream>
//forward declaration
class partido;

struct data{
    unsigned dia, mes, ano;

    data(std::string dataString){
        const char* dat_str = dataString.data();
        char ignorar;

        int retorno = sscanf(dat_str, "%u/%u/%u%c", &dia, &mes, &ano, &ignorar);
        if(retorno != 3){
            throw std::invalid_argument("O valor \"" + dataString  + "\" usado como argumento nao representa um formato valido de data");
        }
    }

    std::string formatString(){
        std::stringstream ss;
        ss << dia << "/" << mes << "/" << ano;

        return ss.str();
    };

    bool vemAntesDe(std::string dataString){
        struct data OutraData(dataString);

        if(this->ano < OutraData.ano){
            return true;
        }else if(this->ano == OutraData.ano){
            if(this->mes < OutraData.mes){
                return true;
            }else if(this->mes == OutraData.mes){
                if(this->dia < OutraData.dia){
                    return true;
                }else if(this->dia == OutraData.dia){
                    return false;
                }else{
                    return false;
                }
            }else{
                return false;
            }
        }else{
                return false;
        }
    }
};

class candidato{
    partido* partidoPertencente;
    struct data* dataDeNascimento;

    std::string nomeDeUrna;
    
    //2 -> masculino
    //4 -> feminino
    unsigned short genero;
    unsigned numeroCandidato;
    int numeroFederacao;
    bool candidatoVaiParaLegenda;
    bool candidatoEstaEleito;
    unsigned long totalVotos;

public:

    candidato(
    // argumentos do construtor
    std::string nome, std::string dataNascimento, short genero, unsigned numeroCandidato, 
    int numeroFederacao, partido* partidoCandidato, std::string destinoVotos, unsigned situacaoTurno
    );

    ~candidato();

    partido* getPartidoPertencente();
    std::string getDataNascimento();
    std::string getNome();
    unsigned short getGenero();
    unsigned getNumeroCandidato();
    int getNumeroFederacao();
    bool vaiParaLegenda();
    bool estaEleito();
    unsigned long getTotalVotos();
    short getIdade(std::string dataComparacao);
    void incrementaVotos(unsigned qtd);
    std::string stringInfoCandidato(bool infoPartido); 
};

#endif //CANDIDATO