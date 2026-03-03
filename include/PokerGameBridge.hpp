#ifndef POKER_GAME_BRIDGE_HPP
#define POKER_GAME_BRIDGE_HPP

#include "TableDePoker.hpp"
#include "poker2.hpp"
#include <vector>
#include <iostream>
#include <thread>
#include <chrono>

// On inclut votre fichier poker2.cpp
// Note: Normalement on inclut un .hpp, mais pour simplifier, on inclut le .cpp
// Plus tard, on pourra séparer en .hpp et .cpp


class PokerGameBridge {
private:
    TableDePoker& table;
    int nbJoueurs;
    
    // Variables du jeu (références vers celles de poker2.cpp)
    std::vector<JetonsJoueur>& jetons;
    std::vector<bool>& joueurElimine;
    std::vector<MisesJoueur>& misesJoueurs;
    int& pot;
    int donneur;
    
    // État du jeu
    enum class EtatJeu {
        ATTENTE,
        PREFLOP,
        FLOP,
        TURN,
        RIVER,
        SHOWDOWN
    };
    
    EtatJeu etatActuel;
    std::vector<Carte> cartesCommunes;
    std::vector<std::vector<Carte>> mainsJoueurs;
    vector<Carte> conteneur;  // Le deck
    
public:
    // Constructeur qui initialise les références
    PokerGameBridge(TableDePoker& tableRef, int nbJ);
    
    // Fonctions de jeu
    void demarrerMain();
    void jouerPreFlop();
    void jouerFlop();
    void jouerTurn();
    void jouerRiver();
    void jouerShowdown();
    
    
    // Mise à jour de l'affichage
    void synchroniserAffichage();
    
    // Fonctions d'action des joueurs (seront appelées par les boutons)
    void actionFold(int joueur);
    void actionCheck(int joueur);
    void actionCall(int joueur);
    void actionRaise(int joueur, int montant);
};

#endif