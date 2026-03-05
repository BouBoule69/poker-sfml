#ifndef TABLE_DE_POKER_HPP
#define TABLE_DE_POKER_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <iostream>
#include "poker2.hpp"


class TableDePoker {
private:
    sf::RenderWindow window;
    sf::Font police;
    sf::Texture textureTable;
    sf::Sprite spriteTable;
    
    // Dimensions de la fenêtre
    const int LARGEUR = 1200;
    const int HAUTEUR = 800;
    
    // Positions pour les différents éléments
    std::vector<sf::Vector2f> positionsJoueurs;
    std::vector<sf::Text> textesJoueurs;
    std::vector<sf::Text> textesJetons;
    
    // Cartes communes
    std::vector<sf::RectangleShape> cartesCommunes;
    std::vector<int> jetonsJoueurs;  // NOUVEAU : pour stocker les jetons de chaque joueur
    
    // Pour les boutons
    std::vector<sf::RectangleShape> boutons;
    std::vector<sf::RectangleShape> textesBoutons;
    std::vector<std::string> actionsBoutons;
    std::vector<sf::Color> couleursBoutons;
    
    // Texte spécial pour Call et Raise
    std::string texteCall;
    std::string texteRaise;
    int joueurCourant;  // Pour savoir quel joueur doit jouer
    int montantPot;  // Pour stocker le montant du pot

public:
    TableDePoker();
    bool estOuvert();
    void fermer();
    void effacer();
    void afficher();
    void gererEvenements();
    
    // Méthodes pour dessiner les éléments
    void dessinerTable();
    void dessinerJoueurs(int nbJoueurs);
    void dessinerCartesCommunes();
    void dessinerBoutons();
    
    // Nouvelles méthodes pour mettre à jour l'affichage
    void mettreAJourCartesCommunes(const std::vector<Carte>& cartes);
    void mettreAJourMainJoueur(int joueur, const std::vector<Carte>& main);

    // NOUVELLE méthode pour mettre à jour les jetons d'un joueur
    void mettreAJourJetons(int joueur, int nouveauxJetons);

    void setJoueurCourant(int joueur);
    void setMontantPot(int montant);
    
    // Nouvelles méthodes pour gérer les clics
    bool boutonFoldClique(sf::Vector2f souris);
    bool boutonCheckClique(sf::Vector2f souris);
    bool boutonCallClique(sf::Vector2f souris);
    bool boutonRaiseClique(sf::Vector2f souris);
    
    // Pour obtenir la position de la souris
    sf::Vector2f getPositionSouris();
    
    // Pour mettre à jour le texte des boutons
    void setBoutonCallTexte(const std::string& texte);
    void setBoutonRaiseTexte(const std::string& texte);

    // Getters
    sf::RenderWindow& getWindow() { return window; }
};

#endif