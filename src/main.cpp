#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <ctime>

struct ConfigLigue {
    std::string NomDeLaLigue;  // Nombre de la liga
    int vic, matnul, def; // Victoria, Empate y Derrota
    std::vector<std::string> Equipes; // Equipos
};

struct ConfigParti {
    std::string date, Elocal, Evisiteurs; // Fecha, Equipo local, Equipo visitante
    int golL, golV; // Goles del equipo Local, Goles del equipo visitante. 
};

struct ConfigTable{
    std::string nom;
    int PJ, PG , PE, PP , GF , GC , DG, PTS; 
};

// Funciones de lectura de archivos.


ConfigLigue LireConfig(){
    std::ifstream Iconfig("../data/config.txt");
    ConfigLigue config;
    std::string i;
    if(Iconfig.is_open()){
        std::cout << "Se ha leido correctamente el archivo \n";
        while(std::getline(Iconfig , i)){
            if(i.empty() || i[0] == '#'){
                 continue;
            }

            std::string cle, valeur; // clave, valor
            std::stringstream ss(i); //stringstream se usa para separar partes de un string

            std::getline(ss , cle , '=');
            std::getline(ss, valeur);

            if(cle == "Liga"){
                config.NomDeLaLigue = valeur;
            }else if(cle == "Victoria"){
                config.vic = std::stoi(valeur); // stoi() se usa para convertir un string en un numero entero
            }else if(cle == "Empate"){
                config.matnul = std::stoi(valeur);
            }else if(cle == "Derrota"){
                config.def = std::stoi(valeur);
            }else if(cle == "equipo"){
                config.Equipes.push_back(valeur); // vector.push_back es para agregar un valor al final de un vector. 
            }

            Iconfig.close();
            return config;

        }
    } else{
    std::cout << "\033[31mHubo un error al abrir el archivo\033[0m";
    std::cout <<  '\a';
    exit(1);
    }
}

std::vector<ConfigParti> LirePartidos(){
    std::ifstream Iparti("../data/partidos.txt");
    std::vector<ConfigParti> partis;

    if(Iparti.is_open()){
        std::string v;
        while(std::getline(Iparti , v)){
            std::stringstream ss(v);
            ConfigParti parti;

            std::getline(ss, parti.date, '|');
            std::getline(ss, parti.Elocal, '|');
            std::getline(ss, parti.Evisiteurs, '|');

            std::string gaL, gaV;

            std::getline(ss, gaL, '|');
            std::getline(ss, gaV, '|');

            parti.golL = std::stoi(gaL);
            parti.golV = std::stoi(gaV);

            partis.push_back(parti);
            Iparti.close();
        };
}else{
    return partis;
}
}

void LireJournee(){
    std::ifstream IFechas("../data/fechas.txt");
    std::string i;
    if(IFechas.is_open()){
      while(std::getline(IFechas, i)){
        std::cout << i << "\n";
        IFechas.close();
    }  
    }else if(i.empty() && IFechas.is_open()){
        std::cout << "No hay jornadas registradas en el momento. \n";
        return;
    }else{
        std::cout << "Ha habido un error al leer fechas.txt";
    }
}

// Funciones de escritura de archivos. 
void RecevoirParti(ConfigParti Parti){
    std::ofstream Oparti("../data/partidos.txt", std::ios::app);
    if(Oparti.is_open()){
        Oparti << Parti.date << "|" << Parti.Elocal << "|" << Parti.Evisiteurs << "|" << Parti.golL << "|" << Parti.golV << "\n";
        Oparti.close();
    }else{
        std::cout << "Error guardando partido \n";
    }
}

void RecevoirJournee(ConfigParti Parti){
    std::ofstream OFechas("../data/fechas.txt" , std::ios::app);
    if(OFechas.is_open()){
         OFechas << "Jornada: \n" << Parti.date << "|" << Parti.Elocal << " " << Parti.golL << "-" << Parti.golV << Parti.Evisiteurs << "|" << "\n" << "Fin de la jornada" << '\n';
         OFechas.close();
    }else{
        std::cout << "Ha ocurrido un error con el archivo \n";
    }
}

// Funciones de logica de la liga.
void ActualiserEquipe(ConfigTable* Ddata, ConfigParti parti, ConfigLigue ConfigL){
    if(Ddata->nom == parti.Elocal){
        Ddata->PJ++;
        Ddata->GF += parti.golL;
        Ddata->GC += parti.golV;

        if(parti.golL > parti.golV){
            Ddata->PG++;
            Ddata->PTS += ConfigL.vic;
        }else if(parti.golL == parti.golV){
            Ddata->PE++;
            Ddata->PTS += ConfigL.matnul;
        }else{
            Ddata->PP++;
            Ddata->PTS += ConfigL.def;
        }
    }
    if(Ddata->nom == parti.Evisiteurs){
        Ddata->PJ++;
        Ddata->GF += parti.golV;
        Ddata->GC += parti.golL;

        if(parti.golV > parti.golL){
            Ddata->PG++;
            Ddata->PTS += ConfigL.vic;
        }else if(parti.golV == parti.golL){
            Ddata->PE++;
            Ddata->PTS += ConfigL.matnul;
        }else{
            Ddata->PP++;
            Ddata-> PTS += ConfigL.def;
        }
    }
    Ddata->DG = Ddata->GF - Ddata->GC;
}

std::vector<ConfigTable> CreeTable(std::vector<ConfigParti> Partis, ConfigLigue configL){
    std::vector<ConfigTable> Equipes;
    for(std::string prenom : configL.Equipes){
        ConfigTable v;
        v.nom = prenom;
        Equipes.push_back(v);

        for(ConfigParti parti : Partis){
            for(ConfigTable &vq : Equipes){
                ActualiserEquipe(&vq, parti, configL);
            }
        }
    }
    return Equipes;
}

bool ComparerEquipes(ConfigTable ComparerE1, ConfigTable ComparerE2){
    if(ComparerE1.PTS != ComparerE2.PTS){
        return ComparerE1.PTS > ComparerE2.PTS;
    }
    if(ComparerE1.DG != ComparerE2.DG){
        return ComparerE1.DG > ComparerE2.DG;
    }

    return ComparerE1.GF > ComparerE2.GF;
}

void ClasserTable(std::vector<ConfigTable> &equipes){
    std::sort(equipes.begin(), equipes.end(), ComparerEquipes);
}


    // Funciones de Interfaz de Usuario (UI)
int InfoMenu(std::string NomDeLaLigue){
    
    int opt;

    std::cout << "\n *|" << NomDeLaLigue<< "|* \n";
    std::cout << "1. Ver tabla de posiciones. \n";
    std::cout << "2. Registrar resultado de un partido. \n";
    std::cout << "3. Ver historial de jornadas. \n";
    std::cout << "4. Ver todos los partidos jugados. \n";
    std::cout << "5. Salir del programa. \n";
    std::cout << "Por favor indique su opcion: ";
    std::cin >> opt;

    return opt;

   
}

int menu(){
     ConfigLigue config = LireConfig();

     int opt;

    
    do{
        opt = InfoMenu(config.NomDeLaLigue);
        switch(opt){
            case 1:{
                std::cout << "Has elegido ver la tabla de posiciones. \n";
                break;
            }
            case 2:{
                std::cout << "Has elegido registrar un resultado de un partido. \n";
                break;
            }
            case 3:{
                std::cout << "Has elegido ver el historial de jornadas. \n";
                LireJournee();
                break;
            }
            case 4:{
                std::cout << "Has elegido ver los partidos jugados. \n";
                break;
            }
            case 5:{
                std::cout << "Has salido del programa. \n";
                break;
            }
            default:{
                std::cout << "Opcion no valida/Error al elegir un valor. \n";
                break;
            }
        }
       
    } while(opt != 5);

}

int main(){
    menu();
    return 0;
}
