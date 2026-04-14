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
    int PJ=0, PG=0 , PE=0, PP=0 , GF=0 , GC=0 , DG=0, PTS=0; 
};

// Funciones de lectura de archivos.

ConfigLigue LireConfig(){ // Leer config.txt
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
            
        }
        Iconfig.close();
        return config;
    } else{
    std::cout << "\033[31mHubo un error al abrir el archivo\033[0m";
    std::cout <<  '\a';
    exit(1);
    }
}

std::vector<ConfigParti> LirePartidos(){ // Leer partidos.txt
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

void LireJournee(){ // Leer fechas.txt
    std::ifstream IFechas("../data/fechas.txt");
    std::string i;
    if(IFechas.is_open()){
      while(std::getline(IFechas, i)){
        std::cout << i << "\n";
    }  
    IFechas.close();
    }else if(IFechas.peek()){
        std::cout << "\033[33mNo hay jornadas registradas en el momento.\033[0m \n";
        return;
    }else{
        std::cout << "\033[31mHa habido un error al leer fechas.txt \033[0m \n";
        return;
    }
}

// Funciones de escritura de archivos. 
void RecevoirParti(ConfigParti Parti){ // Guardar Partido
    std::ofstream Oparti("../data/partidos.txt", std::ios::app);
    if(Oparti.is_open()){
        Oparti << Parti.date << "|" << Parti.Elocal << "|" << Parti.Evisiteurs << "|" << Parti.golL << "|" << Parti.golV << "\n";
        Oparti.close();
    }else{
        std::cout << "\033[31mError guardando partido\033[0m \n";
    }
}

void RecevoirJournee(ConfigParti Parti){ // Guardar Jornada
    std::ofstream OFechas("../data/fechas.txt" , std::ios::app);
    if(OFechas.is_open()){
         OFechas << "Jornada: \n" << Parti.date << "| " 
         << Parti.Elocal << " " 
         << Parti.golL << "-" 
         << Parti.golV << " "
         << Parti.Evisiteurs << "\n" 
         << "Fin de la jornada" << '\n';
         OFechas.close();
    }else{
        std::cout << "\033[31mHa ocurrido un error con el archivo\033[0m \n";
    }
}

// Funciones de logica de la liga.
void ActualiserEquipe(ConfigTable* Ddata, ConfigParti parti, ConfigLigue ConfigL){ // Actualizar los datos de la tabla.
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

std::vector<ConfigTable> CreeTable(std::vector<ConfigParti> Partis, ConfigLigue configL){ // Crear la tabla. 
    std::vector<ConfigTable> Equipes;
    for(std::string prenom : configL.Equipes){
        ConfigTable v;
        v.nom = prenom;
        Equipes.push_back(v);
    }
        for(ConfigParti parti : Partis){
            for(ConfigTable &vq : Equipes){
                ActualiserEquipe(&vq, parti, configL);
            }
        }
    
    return Equipes;
}

bool ComparerEquipes(ConfigTable ComparerE1, ConfigTable ComparerE2){ // Comparar Equipos
    if(ComparerE1.PTS != ComparerE2.PTS){
        return ComparerE1.PTS > ComparerE2.PTS;
    }
    if(ComparerE1.DG != ComparerE2.DG){
        return ComparerE1.DG > ComparerE2.DG;
    }

    return ComparerE1.GF > ComparerE2.GF;
}

void ClasserTable(std::vector<ConfigTable> &equipes){ // Ordenar Tabla
    std::sort(equipes.begin(), equipes.end(), ComparerEquipes);
}

std::string AdapterTexte(std::string texte, int large){ // Ordenar Texto
    if(texte.length() >= large){
        return texte;
    }
    return texte + std::string(large - texte.length(), ' ');
}

void MontrerTable(std::vector<ConfigTable> Equipes){ // Montar la tabla
    std::cout << AdapterTexte("Equipo", 15);
    std::cout << AdapterTexte("PJ", 5);
    std::cout << AdapterTexte("PG", 5);
    std::cout << AdapterTexte("PE", 5);
    std::cout << AdapterTexte("PP", 5);
    std::cout << AdapterTexte("GF", 5);
    std::cout << AdapterTexte("GC", 5);
    std::cout << AdapterTexte("DG", 5);
    std::cout << AdapterTexte("PTS", 5);
    std::cout << std::endl;

    for(ConfigTable Eq : Equipes){
        std::cout << AdapterTexte(Eq.nom, 15);
        std::cout << AdapterTexte(std::to_string(Eq.PJ), 5);
        std::cout << AdapterTexte(std::to_string(Eq.PG), 5);
        std::cout << AdapterTexte(std::to_string(Eq.PE), 5);
        std::cout << AdapterTexte(std::to_string(Eq.PP), 5);
        std::cout << AdapterTexte(std::to_string(Eq.GF), 5);
        std::cout << AdapterTexte(std::to_string(Eq.GC), 5);
        std::cout << AdapterTexte(std::to_string(Eq.DG), 5);
        std::cout << AdapterTexte(std::to_string(Eq.PTS), 5);
        std::cout << std::endl;
    }
}

void GarderTable(std::vector<ConfigTable> Equipes){ // Exportar Tabla5
    std::ofstream OTabla("../data/tabla.txt");
    if(OTabla.is_open()){
        OTabla << "# Equipo PJ PG PE PP GF GC DG PTS \n";

        for(ConfigTable Eq : Equipes){
            OTabla << Eq.nom << " " << Eq.PJ << " " << Eq.PG << " " << Eq.PE << " " << Eq.PP << " " << Eq.GF << " " << Eq.GC << " " << Eq.DG << " " << Eq.PTS << "\n";
        }
    }else{
        std::cout << "\033[31mHubo un error al guardar la tabla.\033[0m";
    }
}

void Antecedents_dAffrontement(std::vector<ConfigParti> partis, ConfigLigue configL){ // Antecedentes de Enfrentamiento (Leer)
    int ComparerE1, ComparerE2;

    for(int i = 0; i < configL.Equipes.size(); i++){
        std::cout << i << " - " << configL.Equipes[i] << std::endl;
    }
        std::cin >> ComparerE1 >> ComparerE2;

        if(ComparerE1 == ComparerE2 || ComparerE1 < 0 || ComparerE2 < 0 || ComparerE1 >= configL.Equipes.size() || ComparerE2 >= configL.Equipes.size()){
            std::cout << "\033 [34m Es invalida la seleccion dada \033 [0m \n";
            return;
        }
        std::string Eq1 = configL.Equipes[ComparerE1];
        std::string Eq2 = configL.Equipes[ComparerE2];

        bool trouver = false;

        for(ConfigParti pt : partis){
            if((pt.Elocal == Eq1 && pt.Evisiteurs == Eq2) || (pt.Elocal == Eq2 && pt.Evisiteurs == Eq1)){
                std::cout << pt.date << " ";
                std::cout << pt.Elocal << " ";
                std::cout << pt.golL << "-";
                std::cout << pt.golV << " ";
                std::cout << pt.Evisiteurs << std::endl;

                trouver = true;
            }
        }
    if(trouver == false){
        std::cout << "\033[34m No hay enfrentamientos registrados \033[0m \n";
    }
}

ConfigParti IngreseParti(ConfigLigue ConfigL){ // Ingresar Partido 
    ConfigParti parti;
    int local, visiteur;

    std::cout << "\033[35m Por favor ingrese una fecha (de la forma MM/DD/AAAA): \033[0m ";
    std::cin >> parti.date;
    do{
        for(int i = 0; i < ConfigL.Equipes.size(); i++){
            std::cout << i << " - " << ConfigL.Equipes[i] << std::endl;
        }
        std::cout << "\033[35m Equipo Local: \033[0m";
        std::cin >> local;
        std::cout << "\n";
        std::cout << "\033[35m Equipo Visitante: \033[0m ";
        std::cin >> visiteur;
        std::cout << '\n';

        if(local == visiteur){
            std::cout << "\033[31mHa habido un error ya que has seleccionado el mismo equipo.\033[0m \n";
        }
    
    }while(local == visiteur || local < 0 || visiteur < 0|| local >= ConfigL.Equipes.size() || visiteur >= ConfigL.Equipes.size());
     parti.Elocal = ConfigL.Equipes[local];
    parti.Evisiteurs = ConfigL.Equipes[visiteur];

    std::cout << "\033[34m Por favor ingrese el marcador: \033[0m \n";
    std::cout << "\033[34m Goles local: \033[0m";
    std::cin >> parti.golL;
    std::cout << "\n";
    std::cout << "\033[34m Goles visitante: \033[0m";
    std::cin >> parti.golV;
    return parti;
}

    // Funciones de Interfaz de Usuario (UI)
int InfoMenu(std::string NomDeLaLigue){  // Informacion del Menu
    
    int opt;

    std::cout << "\n *|" << NomDeLaLigue<< "|* \n";
    std::cout << "\033[96m1. Ver tabla de posiciones.\033[0m \n";
    std::cout << "\033[96m2. Registrar el resultado de un partido.\033[0m  \n";
    std::cout << "\033[96m3. Ver el historial de jornadas.\033[0m  \n";
    std::cout << "\033[96m4. Ver todos los partidos jugados.\033[0m  \n";
    std::cout << "\033[96m5. Ver el historial de enfrentamientos.\033[0m  \n";
    std::cout << "\033[96m6. Salir \033[0m";
    std::cout << "\033[36mPor favor indique su opcion: \033[0m";
    std::cin >> opt;

    return opt;

   
}

int run(){ // Funcionamiento del proyecto
     ConfigLigue config = LireConfig();
     std::vector <ConfigParti> partis = LirePartidos();

     int opt;
    
    do{
        opt = InfoMenu(config.NomDeLaLigue);
        switch(opt){
            case 1:{
                std::cout << "\033[36 Has elegido ver la tabla de posiciones.\033[0m  \n";
                std::vector<ConfigTable> table = CreeTable(partis, config);
                ClasserTable(table);
                MontrerTable(table);

                char conserver;
                std::cout << "\033[33m Desea usted guardar la tabla? (Y/N):\033[0m  ";
                std::cin >> conserver;
                if(conserver == 'Y' || conserver == 'y'){
                    GarderTable(table);
                }
                break;
            }
            case 2:{
                std::cout << "\033[36m Has elegido registrar un resultado de un partido.\033[0m  \n";

                ConfigParti parti = IngreseParti(config);
                RecevoirParti(parti);
                RecevoirJournee(parti);
                partis.push_back(parti);
                break;
            }
            case 3:{
                std::cout << "\033[36m Has elegido ver el historial de jornadas.\033[0m  \n";
                LireJournee();
                break;
            }
            case 4:{
                std::cout << "\033[36m Has elegido ver los partidos jugados.\033[0m  \n";
                std::cout << "\033[35m *|PARTIDOS|* \033[0m \n";
                for(ConfigParti parti : partis){
                    std::cout << parti.date << " ";
                    std::cout << parti.Elocal << " ";
                    std::cout << parti.golL << "-";
                    std::cout << parti.golV << " ";
                    std::cout << parti.Evisiteurs << "\n";
                }
                break;
            }
            case 5:{
               std::cout << "Has elegido ver el historial de enfrentamientos.\033[0m  \n";
               Antecedents_dAffrontement(partis ,config);
               break;
            }
            case 6:{
                 std::cout << "\033[92m Has salido del programa.\033[0m  \n";
                break;
            }
            default:{
                std::cout << "\033[31m Opcion no valida/Error al elegir un valor.\033[0m  \n";
                break;
            }
        }
       
    } while(opt != 6);

}

int main(){
    run();
    return 0;
}
