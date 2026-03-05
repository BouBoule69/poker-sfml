#ifndef POKER_GAME_BRIDGE_HPP
#define POKER_GAME_BRIDGE_HPP

#include "TableDePoker.hpp"
#include "poker2.hpp"
#include <vector>
#include <iostream>
#include <thread>
#include <chrono>

class PokerGameBridge {
private:
    TableDePoker& table;
    int nbJoueurs;
    
    // Variables du jeu (VALEURS, pas références !)
    std::vector<JetonsJoueur> jetons;
    std::vector<bool> joueurElimine;
    std::vector<MisesJoueur> misesJoueurs;
    std::vector<Carte> cartesCommunes;
    std::vector<std::vector<Carte>> mainsJoueurs;
    std::vector<Carte> conteneur;  // Le deck
    int pot;                        // Pas de & !
    int donneur;
    int miseActuelle;
    std::vector<bool> joueurActif;
    int grosseBlind;
    
    // Nouvelles variables
    int joueurCourant;
    int petiteBlind;
    int joueurPetiteBlind;
    int joueurGrosseBlind;
    
    enum class EtatJeu {
        ATTENTE,
        PREFLOP,
        FLOP,
        TURN,
        RIVER,
        SHOWDOWN
    };
    
    EtatJeu etatActuel;
    
public:
    PokerGameBridge(TableDePoker& tableRef, int nbJ);
    void demarrerMain();
    void jouerPreFlop();
    void jouerFlop();
    void jouerTurn();
    void jouerRiver();
    void jouerShowdown();
    void synchroniserAffichage();
    
    // Actions
    void actionFold(int joueur);
    void actionCheck(int joueur);
    void actionCall(int joueur);
    void actionRaise(int joueur, int montant);
};

#endif