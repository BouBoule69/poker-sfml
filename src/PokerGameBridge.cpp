#include "../include/PokerGameBridge.hpp"
#include <iostream>
#include <thread>
#include <random>

extern std::mt19937 gen;  // Déclaré dans poker2.cpp

// Constructeur
PokerGameBridge::PokerGameBridge(TableDePoker& tableRef, int nbJ) 
    : table(tableRef), 
      nbJoueurs(nbJ),
      jetons(*new std::vector<JetonsJoueur>(nbJ)),  // Note: À améliorer pour éviter la fuite mémoire
      joueurElimine(*new std::vector<bool>(nbJ, false)),
      misesJoueurs(*new std::vector<MisesJoueur>(nbJ)),
      pot(*new int(0)),
      donneur(0),
      etatActuel(EtatJeu::ATTENTE)
{
    std::cout << "Bridge de jeu initialisé avec " << nbJoueurs << " joueurs" << std::endl;
    
    // Initialiser les jetons des joueurs (comme dans poker2.cpp)
    for(int i = 0; i < nbJoueurs; ++i) {
        jetons[i].jeton1 = 14;
        jetons[i].jeton2 = 8;
        jetons[i].jeton5 = 6;
        jetons[i].jeton10 = 4;
        jetons[i].jeton25 = 2;
        jetons[i].jeton50 = 1;
    }
    
     // Créer le deck
     for (int s = 0; s < 4; ++s) {
        for (int r = 0; r < 13; ++r) {
            Carte c;
            c.couleur = static_cast<Couleur>(s);
            c.rang = static_cast<Rang>(r);
            conteneur.push_back(c);
        }
    }
    // Initialiser l'affichage
    synchroniserAffichage();
}

// Synchroniser l'état du jeu avec l'affichage
void PokerGameBridge::synchroniserAffichage() {
    // Mettre à jour les jetons de chaque joueur
    for (int i = 0; i < nbJoueurs; i++) {
        if (!joueurElimine[i]) {
            table.mettreAJourJetons(i, jetons[i].total());
        } else {
            table.mettreAJourJetons(i, 0);
        }
    }
    
    // Mettre à jour les cartes communes si elles existent
    if (!cartesCommunes.empty()) {
        table.mettreAJourCartesCommunes(cartesCommunes);
    }
    
    // Mettre à jour les mains des joueurs
    for (int i = 0; i < nbJoueurs && i < static_cast<int>(mainsJoueurs.size()); i++) {
        if (!mainsJoueurs[i].empty()) {
            table.mettreAJourMainJoueur(i, mainsJoueurs[i]);
        }
    }
}

// Démarrer une nouvelle main
void PokerGameBridge::demarrerMain() {
    std::cout << "\n=== NOUVELLE MAIN ===" << std::endl;
    std::cout << "Donneur: joueur " << donneur + 1 << std::endl;
    
    // Réinitialiser les variables pour cette main
    pot = 0;
    cartesCommunes.clear();
    
    // Déterminer les blinds
    int joueurPetiteBlind = (donneur + 1) % nbJoueurs;
    int joueurGrosseBlind = (donneur + 2) % nbJoueurs;
    
    std::cout << "Petite blind: joueur " << joueurPetiteBlind + 1 << std::endl;
    std::cout << "Grosse blind: joueur " << joueurGrosseBlind + 1 << std::endl;
    
    // Ici, on appellera les fonctions de poker2.cpp pour :
    // - Distribuer les cartes
    // - Prendre les blinds
    // - etc.
    
    // Simulation pour le test
    std::cout << "Distribution des cartes..." << std::endl;
    
    // Mettre à jour l'affichage
    synchroniserAffichage();
    
    // Passer à l'état PREFLOP
    etatActuel = EtatJeu::PREFLOP;
}

// Jouer le pré-flop
void PokerGameBridge::jouerPreFlop() {
    std::cout << "\n--- PRE-FLOP ---" << std::endl;
    
    // Ici, on appellera tourDeMise de poker2.cpp
    
    std::cout << "Tour de mise pré-flop terminé" << std::endl;
    
    // Passer au flop
    etatActuel = EtatJeu::FLOP;
}

// Jouer le flop
void PokerGameBridge::jouerFlop() {
    std::cout << "\n--- FLOP ---" << std::endl;
    
    // Ajouter 3 cartes communes
    for (int i = 0; i < 3; i++) {
        Carte c;
        c.couleur = static_cast<Couleur>(i % 4);
        c.rang = static_cast<Rang>(i + 2);
        cartesCommunes.push_back(c);
    }
    
    // Mettre à jour l'affichage
    table.mettreAJourCartesCommunes(cartesCommunes);
    
    std::cout << "3 cartes retournées" << std::endl;
    
    // Ici, on appellera tourDeMise de poker2.cpp
    
    // Passer au turn
    etatActuel = EtatJeu::TURN;
}

// Jouer le turn
void PokerGameBridge::jouerTurn() {
    std::cout << "\n--- TURN ---" << std::endl;
    
    // Ajouter 1 carte
    Carte c;
    c.couleur = static_cast<Couleur>(1);
    c.rang = static_cast<Rang>(10);
    cartesCommunes.push_back(c);
    
    // Mettre à jour l'affichage
    table.mettreAJourCartesCommunes(cartesCommunes);
    
    std::cout << "1 carte retournée (Turn)" << std::endl;
    
    // Ici, on appellera tourDeMise de poker2.cpp
    
    // Passer à la river
    etatActuel = EtatJeu::RIVER;
}

// Jouer la river
void PokerGameBridge::jouerRiver() {
    std::cout << "\n--- RIVER ---" << std::endl;
    
    // Ajouter 1 carte
    Carte c;
    c.couleur = static_cast<Couleur>(2);
    c.rang = static_cast<Rang>(13);
    cartesCommunes.push_back(c);
    
    // Mettre à jour l'affichage
    table.mettreAJourCartesCommunes(cartesCommunes);
    
    std::cout << "1 carte retournée (River)" << std::endl;
    
    // Ici, on appellera tourDeMise de poker2.cpp
    
    // Passer au showdown
    etatActuel = EtatJeu::SHOWDOWN;
}

// Jouer le showdown
void PokerGameBridge::jouerShowdown() {
    std::cout << "\n--- SHOWDOWN ---" << std::endl;
    
    // Ici, on évaluera les mains et distribuera le pot
    
    std::cout << "Le joueur X remporte le pot de " << pot << " jetons!" << std::endl;
    
    // Préparer la prochaine main
    donneur = (donneur + 1) % nbJoueurs;
    etatActuel = EtatJeu::ATTENTE;
    
    // Mettre à jour l'affichage
    synchroniserAffichage();
}

// Actions des joueurs (seront appelées par les boutons)
void PokerGameBridge::actionFold(int joueur) {
    std::cout << "Joueur " << joueur + 1 << " FOLD" << std::endl;
}

void PokerGameBridge::actionCheck(int joueur) {
    std::cout << "Joueur " << joueur + 1 << " CHECK" << std::endl;
}

void PokerGameBridge::actionCall(int joueur) {
    std::cout << "Joueur " << joueur + 1 << " CALL" << std::endl;
}

void PokerGameBridge::actionRaise(int joueur, int montant) {
    std::cout << "Joueur " << joueur + 1 << " RAISE de " << montant << std::endl;
}