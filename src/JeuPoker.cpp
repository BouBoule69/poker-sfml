#include "../include/JeuPoker.hpp"
#include <cstdlib>
#include <ctime>

JeuPoker::JeuPoker(TableDePoker& tableRef, int nbJ) : table(tableRef), nbJoueurs(nbJ) {
    // Initialiser les joueurs
    joueurElimine.resize(nbJoueurs, false);
    jetons.resize(nbJoueurs, 100); // 100 jetons chacun au départ
    donneur = 0;
    
    // Initialiser le générateur aléatoire
    std::srand(std::time(nullptr));
    
    std::cout << "Jeu de poker initialisé avec " << nbJoueurs << " joueurs" << std::endl;
}

void JeuPoker::mettreAJourAffichage() {
    // Mettre à jour les jetons sur l'interface
    for (int i = 0; i < nbJoueurs; i++) {
        table.mettreAJourJetons(i, jetons[i]);
    }
}

void JeuPoker::jouerMain() {
    std::cout << "\n=== DÉBUT D'UNE NOUVELLE MAIN ===" << std::endl;
    std::cout << "Donneur: joueur " << donneur + 1 << std::endl;
    
    // Identifier les blinds
    int petiteBlind = (donneur + 1) % nbJoueurs;
    int grosseBlind = (donneur + 2) % nbJoueurs;
    
    std::cout << "Petite blind: joueur " << petiteBlind + 1 << std::endl;
    std::cout << "Grosse blind: joueur " << grosseBlind + 1 << std::endl;
    
    // Simulation: les joueurs perdent des jetons aléatoirement
    for (int i = 0; i < nbJoueurs; i++) {
        if (!joueurElimine[i]) {
            // Perte aléatoire entre 0 et 20 jetons
            int perte = std::rand() % 20;
            jetons[i] -= perte;
            if (jetons[i] < 0) jetons[i] = 0;
            
            std::cout << "Joueur " << i + 1 << " perd " << perte << " jetons, reste: " << jetons[i] << std::endl;
        }
    }
    
    // Mettre à jour l'affichage
    mettreAJourAffichage();
    
    // Passer le donneur au suivant
    donneur = (donneur + 1) % nbJoueurs;
    
    std::cout << "=== FIN DE LA MAIN ===\n" << std::endl;
}

void JeuPoker::demarrerPartie() {
    std::cout << "Démarrage de la partie de poker!" << std::endl;
    
    bool continuer = true;
    int tour = 0;
    
    while (continuer && tour < 5) { // On limite à 5 tours pour le test
        std::cout << "\n--- Tour " << tour + 1 << " ---" << std::endl;
        jouerMain();
        
        // Vérifier si des joueurs sont éliminés
        int joueursRestants = 0;
        for (int i = 0; i < nbJoueurs; i++) {
            if (jetons[i] > 0) {
                joueursRestants++;
            } else {
                joueurElimine[i] = true;
            }
        }
        
        if (joueursRestants <= 1) {
            std::cout << "Partie terminée! Il ne reste qu'un joueur." << std::endl;
            continuer = false;
        }
        
        tour++;
    }
}