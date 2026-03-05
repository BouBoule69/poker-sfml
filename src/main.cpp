#include <iostream>
#include <chrono>
#include <thread>
#include "../include/TableDePoker.hpp"
#include "../include/PokerGameBridge.hpp"

int main() {
    std::cout << "=== POKER TEXAS HOLD'EM AVEC SFML ===" << std::endl;
    std::cout << "Cliquez sur les boutons pour tester - Les coordonnées s'afficheront" << std::endl;
    
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

        sf::Event event;
        while (table.getWindow().pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                table.fermer();
            }
            
            // Gérer les clics de souris
            // Remplacer la section des clics par :
            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2f souris = table.getPositionSouris();
                    
                    
                    if (table.boutonFoldClique(souris)) {
                        std::cout << "✅ Clic sur Fold" << std::endl;
                        jeu.actionFold(0);  // Pour l'instant, joueur 0, à améliorer plus tard
                    }
                    else if (table.boutonCheckClique(souris)) {
                        std::cout << "✅ Clic sur Check" << std::endl;
                        jeu.actionCheck(0);
                    }
                    else if (table.boutonCallClique(souris)) {
                        std::cout << "✅ Clic sur Call" << std::endl;
                        jeu.actionCall(0);
                    }
                    else if (table.boutonRaiseClique(souris)) {
                        std::cout << "✅ Clic sur Raise" << std::endl;
                        jeu.actionRaise(0, 40);
                    }
                    
                }
            }
        }
        
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