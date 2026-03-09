#include <iostream>
#include <chrono>
#include <thread>
#include "../include/TableDePoker.hpp"
#include "../include/PokerGameBridge.hpp"

int main() {
    std::cout << "=== POKER TEXAS HOLD'EM ===" << std::endl;

    TableDePoker table;
    int nbJoueurs = 6;
    PokerGameBridge jeu(table, nbJoueurs);

    // Démarrer la première main tout de suite
    jeu.demarrerMain();
    jeu.jouerPreFlop();

    static int compteur = 0;

    while (table.estOuvert()) {

        // ── Événements SFML ────────────────────────────────────────
        sf::Event event;
        while (table.getWindow().pollEvent(event)) {

            if (event.type == sf::Event::Closed)
                table.fermer();

            // ── Saisie clavier pour le panneau raise ────────────────
            if (jeu.estEnAttenteHumain())
                table.gererSaisieRaise(event);

            // ── Clics souris ────────────────────────────────────────
            if (event.type == sf::Event::MouseButtonPressed &&
                event.mouseButton.button == sf::Mouse::Left) {

                sf::Vector2f souris = table.getPositionSouris();

                // CAS 1 : le panneau de raise est ouvert
                if (table.panneauRaiseEstVisible()) {

                    if (table.boutonRaiseConfirmerClique(souris)) {
                        int montant = table.getMontantRaiseSaisi();
                        table.cacherPanneauRaise();
                        jeu.actionRaise(0, montant);
                    }
                    else if (table.boutonRaiseAllInClique(souris)) {
                        table.cacherPanneauRaise();
                        jeu.actionAllIn(0);
                    }
                }

                // CAS 2 : boutons normaux (fold/check/call/raise)
                else {

                    if (table.boutonFoldClique(souris)) {
                        std::cout << "✅ Clic Fold" << std::endl;
                        jeu.actionFold(0);
                    }
                    else if (table.boutonCheckClique(souris)) {
                        std::cout << "✅ Clic Check/Call" << std::endl;
                        jeu.actionCall(0);
                    }
                    else if (table.boutonCallClique(souris)) {
                        std::cout << "✅ Clic Call" << std::endl;
                        jeu.actionCall(0);
                    }
                    else if (table.boutonRaiseClique(souris)) {
                        std::cout << "✅ Clic Raise" << std::endl;
                        // Calculer min/max et afficher le panneau
                        int miseMin = std::max(jeu.getMiseActuelle() * 2, 20); // minimum = double ou grosse blind
                        int miseMax = jeu.getStackJoueur(0);
                        table.afficherPanneauRaise(miseMin, miseMax);
                        table.afficherPanneauRaise(miseMin, miseMax);
                        // On n'appelle PAS actionRaise ici,
                        // on attend la confirmation dans le panneau
                    }
                }
            }
        }

        // ── Avancer la logique du jeu (bots) ───────────────────────
        compteur++;
        jeu.mettreAJour();

        // ── Relancer une nouvelle main après le showdown ────────────
        if (jeu.estEnAttente())  {
            jeu.demarrerMain();
            jeu.jouerPreFlop();
        }
        

        // ── Rendu ───────────────────────────────────────────────────
        table.effacer();
        table.dessinerTable();
        table.dessinerJoueurs(nbJoueurs);
        table.dessinerCartesCommunes();
        table.dessinerBoutons();   // dessine aussi le panneau raise si visible
        table.afficher();

        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }

    std::cout << "Fermeture du jeu" << std::endl;
    return 0;
}