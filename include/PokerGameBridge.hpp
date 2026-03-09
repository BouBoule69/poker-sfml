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
    
    // Variables du jeu
    std::vector<JetonsJoueur> jetons;
    std::vector<bool> joueurElimine;
    std::vector<MisesJoueur> misesJoueurs;
    std::vector<Carte> cartesCommunes;
    std::vector<std::vector<Carte>> mainsJoueurs;
    std::vector<Carte> conteneur;   // Le deck
    int pot;
    int donneur;
    int miseActuelle;
    std::vector<bool> joueurActif;
    int grosseBlind;
    int petiteBlind;

    int joueurCourant;
    int joueurPetiteBlind;
    int joueurGrosseBlind;

    // ── Tour de mise ──────────────────────────────────────────────
    // Indique qu'on attend que l'humain clique sur un bouton
    bool enAttenteHumain;
    // Dernier joueur qui a raise (ou grosse-blind au pré-flop) :
    // le tour se ferme quand joueurCourant revient à lui sans relance
    int dernierRelanceur;
    bool premierTourFait; 
    
    enum class EtatJeu {
        ATTENTE,
        PREFLOP,
        PREFLOP_MISE,   // ← tour de mise pré-flop en cours
        FLOP,
        FLOP_MISE,
        TURN,
        TURN_MISE,
        RIVER,
        RIVER_MISE,
        SHOWDOWN
    };

    EtatJeu etatActuel;

    // ── Méthodes privées ──────────────────────────────────────────
    void synchroniserAffichage();
    void passerAuJoueurSuivant();         // avance joueurCourant (ignore éliminés/couchés)
    bool tourDeMiseTermine() const;       // tout le monde a misé pareil ou foldé ?
    void jouerActionBot(int joueur);      // décision automatique pour les bots
    void demarrerTourDeMise(bool preFlop = false); // init d'un nouveau tour
    void verifierEliminations();
    int compteurShowdown;  // délai avant nouvelle main
    bool showdownEnCours;
    int compteurBot;  // délai entre chaque action de bot
    
public:
    PokerGameBridge(TableDePoker& tableRef, int nbJ);

    // Appelées depuis main() à chaque frame si enAttenteHumain == false
    void demarrerMain();
    void jouerPreFlop();
    void jouerFlop();
    void jouerTurn();
    void jouerRiver();
    void jouerShowdown();

    // Appelées par les boutons SFML (joueur humain)
    void actionFold(int joueur);
    void actionCheck(int joueur);
    void actionCall(int joueur);
    void actionRaise(int joueur, int montant);
    void actionAllIn(int joueur);

    // Appelée chaque frame depuis main() pour avancer le jeu automatiquement
    void mettreAJour();

    // Accesseur pour main()
    bool estEnAttenteHumain() const { return enAttenteHumain; }
    bool estEnAttente() const { return etatActuel == EtatJeu::ATTENTE; }
    int getMiseActuelle()  const { return miseActuelle; }
    int getStackJoueur(int i) const { return jetons[i].total(); }
};

#endif