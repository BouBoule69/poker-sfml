#include "../include/PokerGameBridge.hpp"
#include <iostream>
#include <thread>
#include <random>
#include <algorithm>

extern std::mt19937 gen;  // Déclaré dans poker2.cpp

// Constructeur
PokerGameBridge::PokerGameBridge(TableDePoker& tableRef, int nbJ) 
    : table(tableRef), 
      nbJoueurs(nbJ),
      jetons(nbJ),
      joueurElimine(nbJ, false),
      misesJoueurs(nbJ),
      cartesCommunes(),
      mainsJoueurs(nbJ),
      conteneur(),
      pot(0),
      donneur(0),
      miseActuelle(0),
      joueurActif(nbJ, true),
      grosseBlind(20),
      joueurCourant(0),
      petiteBlind(10),
      joueurPetiteBlind(0),
      joueurGrosseBlind(0),
      etatActuel(EtatJeu::ATTENTE)
{
    std::cout << "Bridge de jeu initialisé avec " << nbJoueurs << " joueurs" << std::endl;
    
    // Initialiser les jetons des joueurs
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
    
    // Mélanger le deck
    std::shuffle(conteneur.begin(), conteneur.end(), gen);
    
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
    if (mainsJoueurs.size() == static_cast<size_t>(nbJoueurs)) {
        for (int i = 0; i < nbJoueurs; i++) {
            if (!joueurElimine[i] && !mainsJoueurs[i].empty()) {
                table.mettreAJourMainJoueur(i, mainsJoueurs[i]);
            }
        }
    }
}

// Démarrer une nouvelle main
void PokerGameBridge::demarrerMain() {
    std::cout << "\n=== NOUVELLE MAIN ===" << std::endl;
    std::cout << "Donneur: joueur " << donneur + 1 << std::endl;
    
    // Vérifier que conteneur n'est pas vide
    if (conteneur.empty()) {
        std::cout << "ERREUR: Deck vide! Recréation du deck..." << std::endl;
        for (int s = 0; s < 4; ++s) {
            for (int r = 0; r < 13; ++r) {
                Carte c;
                c.couleur = static_cast<Couleur>(s);
                c.rang = static_cast<Rang>(r);
                conteneur.push_back(c);
            }
        }
        std::shuffle(conteneur.begin(), conteneur.end(), gen);
    }
    
    // Réinitialiser les variables pour cette main
    pot = 0;
    cartesCommunes.clear();
    miseActuelle = 0;
    
    // Réinitialiser les mises des joueurs
    for(int i = 0; i < nbJoueurs; ++i) {
        misesJoueurs[i].miseTotale = 0;
        misesJoueurs[i].miseCeTour = 0;
        joueurActif[i] = !joueurElimine[i];
    }
    
    // Vérifier que mainsJoueurs a la bonne taille
    if (mainsJoueurs.size() != static_cast<size_t>(nbJoueurs)) {
        mainsJoueurs.resize(nbJoueurs);
    }
    
    // Déterminer les blinds
    joueurPetiteBlind = (donneur + 1) % nbJoueurs;
    while(joueurElimine[joueurPetiteBlind]) {
        joueurPetiteBlind = (joueurPetiteBlind + 1) % nbJoueurs;
    }
    
    joueurGrosseBlind = (joueurPetiteBlind + 1) % nbJoueurs;
    while(joueurElimine[joueurGrosseBlind]) {
        joueurGrosseBlind = (joueurGrosseBlind + 1) % nbJoueurs;
    }
    
    std::cout << "Petite blind: joueur " << joueurPetiteBlind + 1 
              << " (" << petiteBlind << " jetons)" << std::endl;
    std::cout << "Grosse blind: joueur " << joueurGrosseBlind + 1 
              << " (" << grosseBlind << " jetons)" << std::endl;
    
    // Prendre les blinds
    if (jetons[joueurPetiteBlind].total() >= petiteBlind) {
        retirerJetons(jetons[joueurPetiteBlind], petiteBlind);
        pot += petiteBlind;
        misesJoueurs[joueurPetiteBlind].miseTotale = petiteBlind;
        misesJoueurs[joueurPetiteBlind].miseCeTour = petiteBlind;
    } else {
        std::cout << "Joueur " << joueurPetiteBlind + 1 << " n'a pas assez pour la blind!" << std::endl;
    }
    
    if (jetons[joueurGrosseBlind].total() >= grosseBlind) {
        retirerJetons(jetons[joueurGrosseBlind], grosseBlind);
        pot += grosseBlind;
        misesJoueurs[joueurGrosseBlind].miseTotale = grosseBlind;
        misesJoueurs[joueurGrosseBlind].miseCeTour = grosseBlind;
    } else {
        std::cout << "Joueur " << joueurGrosseBlind + 1 << " n'a pas assez pour la blind!" << std::endl;
    }

    // 🆕 AJOUTEZ ICI, après avoir pris les blinds
    table.setMontantPot(pot);
    miseActuelle = grosseBlind;
    
    // Distribuer les cartes
    std::cout << "Distribution des cartes..." << std::endl;
    std::shuffle(conteneur.begin(), conteneur.end(), gen);
    
    for(int i = 0; i < nbJoueurs; ++i) {
        if(!joueurElimine[i]) {
            mainsJoueurs[i].clear();
            
            if (conteneur.size() >= 2) {
                mainsJoueurs[i].push_back(conteneur.back());
                conteneur.pop_back();
                mainsJoueurs[i].push_back(conteneur.back());
                conteneur.pop_back();
                std::cout << "Joueur " << i + 1 << " reçoit 2 cartes" << std::endl;
            } else {
                std::cout << "ERREUR: Plus assez de cartes dans le deck!" << std::endl;
            }
        }
    }
    
    // Le premier joueur à parler est après la grosse blind
    joueurCourant = (joueurGrosseBlind + 1) % nbJoueurs;

    while(!joueurActif[joueurCourant]) {
        joueurCourant = (joueurCourant + 1) % nbJoueurs;
    }
    
    // 🆕 AJOUTEZ ICI - après la boucle, quand joueurCourant est correct
    table.setJoueurCourant(joueurCourant);

    std::cout << "Premier joueur à parler: joueur " << joueurCourant + 1 << std::endl;
    
    // Mettre à jour l'affichage
    synchroniserAffichage();
    
    // Passer à l'état PREFLOP
    etatActuel = EtatJeu::PREFLOP;
}

// Jouer le pré-flop
void PokerGameBridge::jouerPreFlop() {
    std::cout << "\n--- PRE-FLOP ---" << std::endl;
    std::cout << "Tour de mise pré-flop" << std::endl;
    
    // Passer au flop
    etatActuel = EtatJeu::FLOP;
}

// Jouer le flop
void PokerGameBridge::jouerFlop() {
    std::cout << "\n--- FLOP ---" << std::endl;
    
    // Ajouter 3 cartes communes
    if (conteneur.size() >= 3) {
        // Brûler une carte
        conteneur.pop_back();
        
        for (int i = 0; i < 3; i++) {
            cartesCommunes.push_back(conteneur.back());
            conteneur.pop_back();
        }
    } else {
        std::cout << "ERREUR: Pas assez de cartes pour le flop!" << std::endl;
    }
    
    // Mettre à jour l'affichage
    table.mettreAJourCartesCommunes(cartesCommunes);
    
    std::cout << "3 cartes retournées sur le flop" << std::endl;
    
    // Passer au turn
    etatActuel = EtatJeu::TURN;
}

// Jouer le turn
void PokerGameBridge::jouerTurn() {
    std::cout << "\n--- TURN ---" << std::endl;
    
    // Ajouter 1 carte
    if (conteneur.size() >= 2) {
        // Brûler une carte
        conteneur.pop_back();
        
        cartesCommunes.push_back(conteneur.back());
        conteneur.pop_back();
    } else {
        std::cout << "ERREUR: Pas assez de cartes pour le turn!" << std::endl;
    }
    
    // Mettre à jour l'affichage
    table.mettreAJourCartesCommunes(cartesCommunes);
    
    std::cout << "1 carte retournée (Turn)" << std::endl;
    
    // Passer à la river
    etatActuel = EtatJeu::RIVER;
}

// Jouer la river
void PokerGameBridge::jouerRiver() {
    std::cout << "\n--- RIVER ---" << std::endl;
    
    // Ajouter 1 carte
    if (conteneur.size() >= 2) {
        // Brûler une carte
        conteneur.pop_back();
        
        cartesCommunes.push_back(conteneur.back());
        conteneur.pop_back();
    } else {
        std::cout << "ERREUR: Pas assez de cartes pour la river!" << std::endl;
    }
    
    // Mettre à jour l'affichage
    table.mettreAJourCartesCommunes(cartesCommunes);
    
    std::cout << "1 carte retournée (River)" << std::endl;
    
    // Passer au showdown
    etatActuel = EtatJeu::SHOWDOWN;
}

// Jouer le showdown
void PokerGameBridge::jouerShowdown() {
    std::cout << "\n--- SHOWDOWN ---" << std::endl;
    
    std::cout << "Le joueur X remporte le pot de " << pot << " jetons!" << std::endl;
    
    // Distribuer le pot au premier joueur actif (simplifié)
    for (int i = 0; i < nbJoueurs; i++) {
        if (joueurActif[i]) {
            jetons[i].jeton1 += pot;
            std::cout << "Joueur " << i + 1 << " remporte " << pot << " jetons!" << std::endl;
            break;
        }
    }
    
    // 🆕 Remettre le pot à 0 après distribution
    pot = 0;
    table.setMontantPot(0);
    
    // Préparer la prochaine main
    donneur = (donneur + 1) % nbJoueurs;
    etatActuel = EtatJeu::ATTENTE;
    
    // Mettre à jour l'affichage
    synchroniserAffichage();
}

// Action FOLD
void PokerGameBridge::actionFold(int joueur) {
    std::cout << "Joueur " << joueur + 1 << " FOLD - Se couche" << std::endl;
    joueurActif[joueur] = false;
    
    // Passer au joueur suivant
    int nbActifs = 0;
    for(int i = 0; i < nbJoueurs; ++i) {
        if(joueurActif[i]) nbActifs++;
    }
    
    if(nbActifs > 0) {
        do {
            joueurCourant = (joueurCourant + 1) % nbJoueurs;
        } while(!joueurActif[joueurCourant]);
        std::cout << "Au tour du joueur " << joueurCourant + 1 << std::endl;
         // 🆕 AJOUTEZ CETTE LIGNE
         table.setJoueurCourant(joueurCourant);
    } else {
        std::cout << "Plus de joueurs actifs!" << std::endl;
    }
    
    synchroniserAffichage();
}

// Action CHECK
void PokerGameBridge::actionCheck(int joueur) {
    std::cout << "Joueur " << joueur + 1 << " CHECK - Parole" << std::endl;
    
    // Vérifier si c'est possible
    if (miseActuelle == misesJoueurs[joueur].miseCeTour) {
        std::cout << "Check valide" << std::endl;
        
        // Passer au joueur suivant
        do {
            joueurCourant = (joueurCourant + 1) % nbJoueurs;
        } while(!joueurActif[joueurCourant] && nbJoueurs > 0);
        
        std::cout << "Au tour du joueur " << joueurCourant + 1 << std::endl;
         // 🆕 AJOUTEZ CETTE LIGNE
         table.setJoueurCourant(joueurCourant);
    } else {
        std::cout << "Check impossible, il faut suivre ou se coucher" << std::endl;
    }
    
    synchroniserAffichage();
}

// Action CALL
void PokerGameBridge::actionCall(int joueur) {
    std::cout << "Joueur " << joueur + 1 << " CALL - Suit" << std::endl;
    
    int aCaller = miseActuelle - misesJoueurs[joueur].miseCeTour;
    
    if (aCaller <= 0) {
        std::cout << "Rien à caller, mise déjà égale" << std::endl;
    } else if (aCaller <= jetons[joueur].total()) {
        retirerJetons(jetons[joueur], aCaller);
        pot += aCaller;
        misesJoueurs[joueur].miseCeTour += aCaller;
        misesJoueurs[joueur].miseTotale += aCaller;
        
        std::cout << "Joueur " << joueur + 1 << " call " << aCaller << " jetons" << std::endl;
        std::cout << "Pot total: " << pot << std::endl;
        // 🆕 AJOUTEZ ICI
         table.setMontantPot(pot);
    } else {
        int allIn = jetons[joueur].total();
        std::cout << "Joueur " << joueur + 1 << " ALL-IN avec " << allIn << " jetons!" << std::endl;
        retirerJetons(jetons[joueur], allIn);
        pot += allIn;
        misesJoueurs[joueur].miseCeTour += allIn;
        misesJoueurs[joueur].miseTotale += allIn;
    }
    
    // Passer au joueur suivant
    do {
        joueurCourant = (joueurCourant + 1) % nbJoueurs;
    } while(!joueurActif[joueurCourant] && nbJoueurs > 0);
    
    std::cout << "Au tour du joueur " << joueurCourant + 1 << std::endl;
     // 🆕 AJOUTEZ CETTE LIGNE
     table.setJoueurCourant(joueurCourant);
    
    synchroniserAffichage();
}

// Action RAISE
void PokerGameBridge::actionRaise(int joueur, int montant) {
    std::cout << "Joueur " << joueur + 1 << " RAISE de " << montant << std::endl;
    
    int miseTotale = misesJoueurs[joueur].miseCeTour + montant;
    
    if (miseTotale <= jetons[joueur].total() + misesJoueurs[joueur].miseCeTour) {
        int aAjouter = miseTotale - misesJoueurs[joueur].miseCeTour;
        
        retirerJetons(jetons[joueur], aAjouter);
        pot += aAjouter;
        misesJoueurs[joueur].miseCeTour = miseTotale;
        misesJoueurs[joueur].miseTotale += aAjouter;
        miseActuelle = miseTotale;
        
        std::cout << "Nouvelle mise: " << miseActuelle << std::endl;
        std::cout << "Pot: " << pot << std::endl;
         // 🟢 AJOUTEZ table.setMontantPot(pot) JUSTE ICI
        table.setMontantPot(pot);
        
        // Passer au joueur suivant
        do {
            joueurCourant = (joueurCourant + 1) % nbJoueurs;
        } while(!joueurActif[joueurCourant] && nbJoueurs > 0);
        
        std::cout << "Au tour du joueur " << joueurCourant + 1 << std::endl;
         // 🆕 AJOUTEZ CETTE LIGNE
         table.setJoueurCourant(joueurCourant);
    } else {
        std::cout << "Pas assez de jetons pour raise" << std::endl;
    }
    
    synchroniserAffichage();
}