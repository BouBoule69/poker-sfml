#include <iostream>
#include <chrono>
#include <thread>
#include "../include/TableDePoker.hpp"
#include "../include/PokerGameBridge.hpp"

int main() {
    std::cout << "=== POKER TEXAS HOLD'EM AVEC SFML ===" << std::endl;
    
    // Créer la table
    TableDePoker table;
    int nbJoueurs = 6;
    
    // Créer le pont vers le jeu
    PokerGameBridge jeu(table, nbJoueurs);
    
    // Variables pour le déroulement du jeu
    int etape = 0;
    bool mainEnCours = false;
    
    while (table.estOuvert()) {
        // Gérer les événements SFML
        table.gererEvenements();
        
        // Effacer et redessiner
        table.effacer();
        table.dessinerTable();
        table.dessinerJoueurs(nbJoueurs);
        table.dessinerCartesCommunes();
        table.dessinerBoutons();
        table.afficher();
        
        // Simulation du déroulement d'une main (toutes les 200 frames)
        static int compteur = 0;
        compteur++;
        
        if (compteur % 200 == 0) {
            if (!mainEnCours) {
                jeu.demarrerMain();
                mainEnCours = true;
                etape = 0;
            } else {
                switch(etape) {
                    case 0:
                        jeu.jouerPreFlop();
                        etape++;
                        break;
                    case 1:
                        jeu.jouerFlop();
                        etape++;
                        break;
                    case 2:
                        jeu.jouerTurn();
                        etape++;
                        break;
                    case 3:
                        jeu.jouerRiver();
                        etape++;
                        break;
                    case 4:
                        jeu.jouerShowdown();
                        mainEnCours = false;
                        break;
                }
            }
        }
        
        // Petite pause
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    std::cout << "Fermeture du jeu" << std::endl;
    return 0;
}