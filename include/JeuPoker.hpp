#ifndef JEU_POKER_HPP
#define JEU_POKER_HPP

#include "TableDePoker.hpp"
#include <vector>
#include <iostream>

// On inclut votre poker2.cpp plus tard
// Pour l'instant, on déclare les structures nécessaires

class JeuPoker {
private:
    TableDePoker& table;
    int nbJoueurs;
    
    // Variables de jeu (copiées de votre poker2.cpp)
    std::vector<bool> joueurElimine;
    std::vector<int> jetons;
    int donneur;
    
public:
    JeuPoker(TableDePoker& tableRef, int nbJ);
    void demarrerPartie();
    void jouerMain();
    void mettreAJourAffichage();
};

#endif