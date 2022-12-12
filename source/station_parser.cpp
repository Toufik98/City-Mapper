
#include "Generic_station_parser.hpp"
#include "station_parser.h"

#include <fstream>
#include <iostream>
#include <algorithm>

#define ERROR_READ_FILE 1


void Station_parser::read_stations(const std::string & _filename){
    
    //@param _filename : 

    travel::Station S;                                  

    std::fstream file(_filename);                       
    std::string line= "";

    
    if ( file.fail() ) {
		throw (ERROR_READ_FILE);
    } else {
        std::getline(file, line);                      
        while (file.good()) {                          
            
            std::getline(file, S.name, ',');
            
            std::getline(file, line, ',');
            std::getline(file, S.line_id, ',');
            std::getline(file, S.address, ',');
            std::getline(file, S.line_name);

            
            uint64_t ID = std::strtoul(line.c_str(), NULL, 10);
            
            this->stations_hashmap.insert( { ID, S} );
        }
    }
    
    file.close();
}


void Station_parser::set_stations(const std::string & _filename){
    read_stations(_filename);
}


void Station_parser::read_connections(const std::string & _filename) {
    

    //@param _filename : file to read

    std::fstream file(_filename);								
    std::string line = "";
    std::string Station_begin, Station_end, Time_str;

    if (file.fail()) {
		throw (ERROR_READ_FILE);								
    } else {
        std::getline(file, line);
        while (!file.eof()) {
            
            std::getline(file, Station_begin, ','); 			
            std::getline(file, Station_end, ','); 				
            std::getline(file, Time_str); 						

			
            uint64_t station_begin_id = std::strtoul(Station_begin.c_str(), NULL, 10);
            uint64_t station_end_id = std::strtoul(Station_end.c_str(), NULL, 10);
            uint64_t time = std::strtoul(Time_str.c_str(), NULL, 10);
			
            
            this->connections_hashmap[station_begin_id][station_end_id] = time;			
        }

    }
    file.close();												// Fermeture du fichier
}


void Station_parser::set_connections(const std::string& _filename){
    read_connections(_filename);
}



std::vector<std::pair<uint64_t,uint64_t> > Station_parser::compute_travel(uint64_t _start, uint64_t _end){

    /*Fonction qui permet de calculer et de renvoyer le plus court chemin entre deux stations à l'aide de l'algorithme de Djisktra*/
    // _start : ID de la station de départ
    // _end : ID de la station d'arrivé
    // Vecteur de couple (ID, temps) pour stocker le plus court chemin
    std::vector<std::pair<uint64_t,uint64_t>> shortest_path;
    //Vecteur pour stocker les noeuds à visiter
	std::vector<std::pair<uint64_t, uint64_t>> open;
    //Vecteur pour stocker les noeuds visites  
	std::vector<std::pair<uint64_t, uint64_t>> closed;

    // Sorte de Dictionnaire pour remmonté l'arbre
	std::unordered_map<uint64_t, uint64_t> parent_of;

    // Pair pour mémoriser noeuds courant / Initialisation du noeud de début (Station de départ)
	std::pair<uint64_t, uint64_t> curr_node(_start,0);
    // Enfant du noeaud courant
	std::pair<uint64_t, uint64_t> child_node;

    // On ajoute le noeud de la station de départ
	open.push_back(curr_node);
	if(_start == _end){
	    return open;
	}
    // Tant que je n'ai pas visites tout les noeuds
	while(!open.empty()){
        // Recuperation du premier element 
	    curr_node = open.front();
        // On ajoute le noeud courant dans la liste des noeuds visitées 
	    closed.push_back(curr_node);

        // On supprime le premier elements 
	    open.erase(open.begin());

        // Si le noeud courant et le noeud de la station d'arrivee
	    if (curr_node.first == _end) {
        
		uint64_t parent_id(parent_of[curr_node.first]);
		shortest_path.insert(shortest_path.begin(), curr_node);

		//On parcourt closed à l'envers en ne gardant que les parents à partir du noeud d'arrivee jusqu'au noeud de depart 
		//On ajoute ces noeuds dans shortest_path pour creer le chemin
		for(std::vector<std::pair<uint64_t, uint64_t>>::const_reverse_iterator it(closed.rbegin()); it != closed.rend(); it++){
		    if(it->first == parent_id){
			child_node.first = it->first;
			child_node.second = it->second;
			parent_id = parent_of[it->first];
			shortest_path.insert(shortest_path.begin(), child_node);
		    }
		}
        // On sort de la boucle car on a trouver le chemin le plus court
		break;
	    }
        //Sinon si ce n'est pas le noeud d'arrivee
	    else{

		std::vector<uint64_t> child_nodes_id;//Le vecteur qui va contenir l'id des fils de la station courante (correspond aux connections a cette station)
		for (std::unordered_map<uint64_t, uint64_t>::const_iterator it(this->connections_hashmap[curr_node.first].begin());it != this->connections_hashmap[curr_node.first].end();it++){
		    child_nodes_id.push_back(it->first);
		}
		//On verifie si les stations connectees a la station courante ne sont pas deja visitees
		for(unsigned int i = 0; i < child_nodes_id.size(); i++){
		    bool in_closed(false);
		    child_node.first = child_nodes_id.at(i);
		    child_node.second = this->connections_hashmap[curr_node.first][child_nodes_id.at(i)] + curr_node.second;//Cout de developpement du noeud
		    for(std::vector<std::pair<uint64_t, uint64_t>>::const_iterator it(closed.begin()); it != closed.end(); it++){
			if(it->first == child_node.first){
			    in_closed = true;
			    break;
			}
		    }
		    //Si elle n'est pas visite on verifie si elle est dans open
		    if(!in_closed){
			bool in_open(false);
			for(std::vector<std::pair<uint64_t, uint64_t>>::const_iterator it(open.begin()); it != open.end(); it++){
			    if(it->first == child_node.first){
				in_open = true;
				break;
			    }
			}
			//Si elle n'est pas dans open, on la rajoute
			if(!in_open){
			    open.push_back(child_node);
			    parent_of[child_node.first] = curr_node.first;
			}
			//Sinon on cherche cette station dans open et on compare les couts de developpements des deux chemins pour cette station
			else{
			    for(std::vector<std::pair<uint64_t, uint64_t>>::iterator it(open.begin()); it != open.end(); it++){
				if(it->first == child_node.first){
				    //Si le cout de developpement de la station dans open est superieure a celle de
				    //la nouvelle connection: on met a jour le cout de developpement et on change 
				    //son pere
				    if(it->second > child_node.second){
					it->second = child_node.second;
					parent_of[it->first] = curr_node.first;
					break;
				    }
				}
			    }
			}
		    }
		}
	    }
	    std::sort(open.begin(), open.end(), sort_by_cost);//On range par ordre croissant du cout de developpement de chaque noeud
	}
	return shortest_path;
}

std::vector<std::pair<uint64_t, uint64_t> > Station_parser::compute_and_display_travel(uint64_t _start, uint64_t _end){
	std::vector<std::pair<uint64_t,uint64_t>>shortest_path(this->compute_travel(_start,_end));
	std::string curr_line_name(this->stations_hashmap[shortest_path.begin()->first].line_name);
	std::pair<uint64_t,uint64_t> same_line_station(shortest_path.at(0));//utilise pour garder la premiere station en memoire jusqu'a l'arrivee ou a un changement de ligne
	std::pair <uint64_t, uint64_t> curr_station(shortest_path.at(0));//utilise pour suivre la station actuelle
	std::cout<<std::endl;	
	std::cout<<"Le chemin le plus rapide de "<<this->stations_hashmap[_start].name<<" (ligne "<<this->stations_hashmap[_start].line_id<<") a ";
	std::cout<<this->stations_hashmap[_end].name<<" (ligne "<<this->stations_hashmap[_end].line_id<<") est:"<<std::endl;
	//Cette clause if permet de verifier que l'on ne va pas afficher simplement la ligne si elle est suivie d'un changement de direction (cout 0)
	//Si il y a un changement de direction, on affichera la ligne apres ce changement
	if(curr_line_name == this->stations_hashmap[(shortest_path.begin()+1)->first].line_name){
	    std::cout<<"\t<Prendre la ligne "<<this->stations_hashmap[same_line_station.first].line_id;
	    std::cout<<"> "<<curr_line_name<<std::endl;
	}
	for(std::vector<std::pair<uint64_t,uint64_t>>::const_iterator it(shortest_path.begin());it!=shortest_path.end();it++){
	    //Changement de ligne
	    if(curr_line_name != this->stations_hashmap[it->first].line_name){
		//On verifie qu'il y a eu un trajet sur la ligne avant le changement de ligne
		if(curr_station.second != same_line_station.second){
		    //Affiche le cout du trajet sur la ligne
		    std::cout<<"\t\tDe "<<this->stations_hashmap[same_line_station.first].name<<", a ";
		    std::cout<<this->stations_hashmap[curr_station.first].name<<" (";
		    sec_to_hhmmss(curr_station.second-same_line_station.second);
		    std::cout<<")"<<std::endl;
		}
		if(it->second != curr_station.second){
		    //On affiche le cout du changement
		    std::cout<<"\tMarcher jusqu'a "<<this->stations_hashmap[curr_station.first].name<<", ligne ";
		    std::cout<<this->stations_hashmap[it->first].line_id<<" (";
		    sec_to_hhmmss(it->second-curr_station.second);
		    std::cout<<")"<<std::endl;
		}
		curr_line_name = this->stations_hashmap[it->first].line_name;
		//Si on est pas arrive a destination, on affiche le changement de ligne
		if(this->stations_hashmap[it->first].name != this->stations_hashmap[_end].name){
		    if(this->stations_hashmap[it->first].line_name == this->stations_hashmap[(it+1)->first].line_name){
			std::cout<<"\t<Prendre la ligne "<<this->stations_hashmap[it->first].line_id<<"> ";
			std::cout<<curr_line_name<<std::endl;
		    }	
		}
		//On met a jour same_line_station avec la station de la nouvelle ligne
		same_line_station.first = it->first;
		same_line_station.second = it->second;
	    }
	    //On met a jour curr_station avec la station courante
	    curr_station.first = it->first;
	    curr_station.second = it->second;
	}
	if(curr_station.second != same_line_station.second){
	    std::cout<<"\t\tDe "<< this->stations_hashmap[same_line_station.first].name << " a ";
	    std::cout<<this->stations_hashmap[curr_station.first].name << " (";
	    sec_to_hhmmss(curr_station.second - same_line_station.second);
	    std::cout<<")"<<std::endl;
	}
	std::cout<<"Apres ";
	sec_to_hhmmss(curr_station.second);
	std::cout<<", vous etes arrive a destination !"<<std::endl;
	return shortest_path;
    }



// Fonction qui permet de vérifier si l'ID en entré de l'utilisateur est valide sinon lui redemande de faire entrer un nouveau
uint64_t Station_parser::choose_station_id(const std::string& _type){
	bool id_exist(false);
	std::cout<<"Saisir l'ID de la station de "<<_type<<" et appuyer sur <ENTREE>: ";
	uint64_t station_id = valid_int_input();
	//On verifie si l'id choisi par l'utilisateur correspond a une station
	for(std::unordered_map<uint64_t,travel::Station>::const_iterator it(this->stations_hashmap.begin());it!=this->stations_hashmap.end();it++){
	    if(station_id == it->first){
		id_exist = true;
	    }
	}
	if(id_exist){
	    std::cout<<this->stations_hashmap[station_id]<<std::endl;
	    return station_id;
	}
	std::string user_answer;
	bool valid_input(false);
	//On propose a l'utilisateur d'entrer un id correct ou de quitter le programme
	while(!valid_input){
	    std::cout<<"Aucune station associee à cet ID (" << station_id << ") !"<<std::endl<<"Voulez vous reessayer? (y/n): ";	
	    std::cin>>user_answer;
	    if(compare_string(user_answer,"n")){
		    std::cout<<std::endl<<"Merci d'avoir utiliser RATP mapper. A bientot !"<<std::endl;
            exit(EXIT_SUCCESS);
	    }
	    else if(compare_string(user_answer,"y")){
		valid_input = true;
	    }
	    else{
		    std::cin.clear();//clear l'erreur sur cin pour que l'on puisse le reutiliser
	    }
	}
	return this->choose_station_id(_type);
}





/*Fonction qui permet de trier par cout*/
bool sort_by_cost(const std::pair<uint64_t, uint64_t>& p1, const std::pair<uint64_t, uint64_t>&p2){
    return (p1.second < p2.second);
}

/*Fonction qui permet de convertir des secondes en heures + minutes */
void sec_to_hhmmss(const uint64_t temps_sec){
    if (temps_sec < 60){
        std::cout<<temps_sec<<"s";
    }
    else if(temps_sec < 3600){
        if(temps_sec % 60 == 0){
            std::cout << temps_sec / 60 << "mn";
            std::cout << "00s";
        }
        else{
            std::cout << temps_sec / 60 << "mn";
            if(temps_sec % 60 < 10){
                std::cout << "0" << temps_sec % 60 << "s";
            }
            else {
                std::cout << temps_sec % 60 << "s";
            }
        }
    }
    else{
        if(temps_sec % 3600 == 0){
            std::cout << temps_sec / 3600 << "h";
            std::cout << "00mn";
            std::cout << "00s";
        }
        else{
            std::cout << temps_sec / 3600 << "h";
            int mmss = temps_sec%3600;//minutes:secondes
            if(mmss % 60 == 0){
                if (mmss / 60 < 10) {
                    std::cout << "0" << mmss / 60 << "mn";
                    }
                else{
                    std::cout << mmss / 60 << "mn";
                }
                std::cout << "00s";
            }
            else{
                if(mmss / 60 < 10){
                    std::cout << "0" << mmss / 60 << "mn";
                }
                else{
                    std::cout << mmss / 60 << "mn";
                }
                if(mmss % 60 < 10){
                    std::cout << "0" << mmss % 60 << "s";
                }
                else{
                    std::cout << mmss % 60 << "s";
                }
            }
        }
    }
}




/*Fonctions qui permettent de comparer entre deux chaines de caracteres*/
bool compare_char(const char& c1, const char& c2){
    if(c1 == c2){
        return true;
    }
    else if(std::tolower(c1) == std::tolower(c2)){
        return true;
    }
    return false;
}



bool compare_string(const std::string& s1, const std::string& s2){
    return((s1.size() == s2.size()) && std::equal(s1.begin(),s1.end(), s2.begin(), &compare_char));
}


// Fonction qui permet de faire valider l'entree 
uint64_t valid_int_input(){
    uint64_t my_int;
    bool valid_input(false);
    while(!valid_input){
        std::cin>>my_int;
        if(std::cin.fail()){
            std::cout<<"/!\\ L'entree n'est pas valide /!\\"<<std::endl;
            std::cout<<"/!\\ Vous devez saisir un entier /!\\"<<std::endl;
            std::cout<<"Veuillez reessayer: ";
        }
        else{
            valid_input = true;
        }
    }
    return my_int;
}