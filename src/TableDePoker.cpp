#include "../include/TableDePoker.hpp"

TableDePoker::TableDePoker() {
    // Créer la fenêtre
    window.create(sf::VideoMode(LARGEUR, HAUTEUR), "Poker Texas Hold'em", sf::Style::Titlebar | sf::Style::Close);
    window.setFramerateLimit(60);
    
    // Charger la police (créons une police par défaut si aucune n'est trouvée)
    if (!police.loadFromFile("fonts/arial.ttf")) {
        // Si la police n'est pas trouvée, on utilise la police par défaut du système
        std::cout << "Attention: Police non trouvée, utilisation de la police par défaut" << std::endl;
        // Sur Ubuntu, vous pouvez installer arial avec : sudo apt install msttcorefonts -qq
    }
    
    // Définir les positions des joueurs (autour de la table)
    // Format: (x, y) pour chaque joueur (de 1 à 6)
    positionsJoueurs = {
        sf::Vector2f(200, 600),  // Joueur 1 (bas gauche)
        sf::Vector2f(500, 650),  // Joueur 2 (bas milieu)
        sf::Vector2f(800, 600),  // Joueur 3 (bas droite)
        sf::Vector2f(900, 300),  // Joueur 4 (droite)
        sf::Vector2f(600, 150),  // Joueur 5 (haut)
        sf::Vector2f(200, 300)   // Joueur 6 (gauche)
    };
    
    // Initialiser les textes des joueurs
    for (int i = 0; i < 6; i++) {
        sf::Text texteJoueur;
        texteJoueur.setFont(police);
        texteJoueur.setString("Joueur " + std::to_string(i + 1));
        texteJoueur.setCharacterSize(18);
        texteJoueur.setFillColor(sf::Color::White);
        texteJoueur.setStyle(sf::Text::Bold);
        texteJoueur.setPosition(positionsJoueurs[i].x - 30, positionsJoueurs[i].y - 40);
        textesJoueurs.push_back(texteJoueur);
        
        sf::Text texteJeton;
        texteJeton.setFont(police);
        texteJeton.setString("1000");
        texteJeton.setCharacterSize(16);
        texteJeton.setFillColor(sf::Color::Yellow);
        texteJeton.setPosition(positionsJoueurs[i].x - 20, positionsJoueurs[i].y - 20);
        textesJetons.push_back(texteJeton);
    }
    
    // Initialiser les cartes communes (5 emplacements)
    for (int i = 0; i < 5; i++) {
        sf::RectangleShape carte(sf::Vector2f(80, 110));
        carte.setFillColor(sf::Color::White);
        carte.setOutlineThickness(2);
        carte.setOutlineColor(sf::Color::Black);
        carte.setPosition(400 + i * 90, 300);
        cartesCommunes.push_back(carte);
    }
    
    // NOUVEAU : Initialiser les jetons des joueurs à 100
      for (int i = 0; i < 6; i++) {
        jetonsJoueurs.push_back(100);
    }
    // Charger une texture pour la table (si disponible)
    // Pour l'instant, on dessinera simplement un cercle vert
}

bool TableDePoker::estOuvert() {
    return window.isOpen();
}

void TableDePoker::fermer() {
    window.close();
}

void TableDePoker::effacer() {
    window.clear(sf::Color(0, 100, 0)); // Vert foncé pour la table
}

void TableDePoker::afficher() {
    window.display();
}

void TableDePoker::gererEvenements() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            fermer();
        }
    }
}

void TableDePoker::dessinerTable() {
    // Dessiner la table (un grand ovale vert)
    sf::CircleShape table(350);
    table.setFillColor(sf::Color(34, 139, 34)); // Vert forêt
    table.setOutlineThickness(5);
    table.setOutlineColor(sf::Color(139, 69, 19)); // Brun
    table.setPosition(250, 150);
    window.draw(table);
    
    // Centre de la table (plus clair)
    sf::CircleShape centre(250);
    centre.setFillColor(sf::Color(50, 150, 50));
    centre.setPosition(350, 250);
    window.draw(centre);
}

void TableDePoker::dessinerJoueurs(int nbJoueurs) {
    for (int i = 0; i < nbJoueurs && i < 6; i++) {
        // Cercle du joueur - la couleur change selon le nombre de jetons
        sf::CircleShape joueur(35);
        
        // Choisir la couleur selon le nombre de jetons
        if (jetonsJoueurs[i] >= 200) {
            joueur.setFillColor(sf::Color::Green);  // Riche !
        } else if (jetonsJoueurs[i] >= 100) {
            joueur.setFillColor(sf::Color::Blue);   // Moyen
        } else if (jetonsJoueurs[i] >= 50) {
            joueur.setFillColor(sf::Color::Yellow); // Faible
        } else {
            joueur.setFillColor(sf::Color::Red);    // Presque ruiné
        }
        
        joueur.setOutlineThickness(3);
        joueur.setOutlineColor(sf::Color::White);
        joueur.setPosition(positionsJoueurs[i].x, positionsJoueurs[i].y);
        window.draw(joueur);
        
        // Rectangle pour le numéro du joueur
        sf::RectangleShape numeroRect(sf::Vector2f(25, 25));
        numeroRect.setFillColor(sf::Color::White);
        numeroRect.setOutlineThickness(1);
        numeroRect.setOutlineColor(sf::Color::Black);
        numeroRect.setPosition(positionsJoueurs[i].x + 20, positionsJoueurs[i].y + 10);
        window.draw(numeroRect);
        
        // Rectangle pour le nom
        sf::RectangleShape nomRect(sf::Vector2f(50, 20));
        nomRect.setFillColor(sf::Color(100, 100, 100, 200));
        nomRect.setOutlineThickness(1);
        nomRect.setOutlineColor(sf::Color::White);
        nomRect.setPosition(positionsJoueurs[i].x - 10, positionsJoueurs[i].y - 25);
        window.draw(nomRect);
        
        // Rectangle pour les jetons - avec couleur variable
        sf::RectangleShape jetonsRect(sf::Vector2f(70, 20));
        
        // Changer la couleur du rectangle des jetons selon le montant
        if (jetonsJoueurs[i] >= 200) {
            jetonsRect.setFillColor(sf::Color(255, 215, 0, 200)); // Or
        } else if (jetonsJoueurs[i] >= 100) {
            jetonsRect.setFillColor(sf::Color(255, 255, 0, 200)); // Jaune
        } else {
            jetonsRect.setFillColor(sf::Color(255, 165, 0, 200)); // Orange
        }
        
        jetonsRect.setOutlineThickness(1);
        jetonsRect.setOutlineColor(sf::Color::Black);
        jetonsRect.setPosition(positionsJoueurs[i].x - 15, positionsJoueurs[i].y - 5);
        window.draw(jetonsRect);
        
        // Petit indicateur de jetons (plus il y a de traits, plus il a de jetons)
        int nbTraits = jetonsJoueurs[i] / 50; // 1 trait par 50 jetons
        if (nbTraits > 5) nbTraits = 5; // Maximum 5 traits
        
        for (int t = 0; t < nbTraits; t++) {
            sf::RectangleShape trait(sf::Vector2f(5, 10));
            trait.setFillColor(sf::Color::White);
            trait.setPosition(positionsJoueurs[i].x + 30 + t * 7, positionsJoueurs[i].y - 3);
            window.draw(trait);
        }
    }
}

void TableDePoker::dessinerCartesCommunes() {
    for (auto& carte : cartesCommunes) {
        window.draw(carte);
        
        // Dessiner un petit texte "?" sur chaque carte
        sf::Text texteCarte;
        texteCarte.setFont(police);
        texteCarte.setString("?");
        texteCarte.setCharacterSize(30);
        texteCarte.setFillColor(sf::Color::Black);
        texteCarte.setPosition(carte.getPosition().x + 35, carte.getPosition().y + 35);
        window.draw(texteCarte);
    }
}

void TableDePoker::dessinerBoutons() {
    // Boutons d'action (en bas)
    std::vector<std::string> actions = {"Fold", "Check", "Call", "Raise"};
    std::vector<sf::Color> couleurs = {sf::Color::Red, sf::Color::Yellow, sf::Color::Green, sf::Color::Blue};
    
    for (int i = 0; i < 4; i++) {
        sf::RectangleShape bouton(sf::Vector2f(100, 50));
        bouton.setFillColor(couleurs[i]);
        bouton.setOutlineThickness(2);
        bouton.setOutlineColor(sf::Color::Black);
        bouton.setPosition(300 + i * 150, 700);
        window.draw(bouton);
        
        sf::Text texteBouton;
        texteBouton.setFont(police);
        texteBouton.setString(actions[i]);
        texteBouton.setCharacterSize(20);
        texteBouton.setFillColor(sf::Color::Black);
        texteBouton.setPosition(330 + i * 150, 715);
        window.draw(texteBouton);
    }
}

 void TableDePoker::mettreAJourCartesCommunes(const std::vector<Carte>& cartes) {
    // Cette fonction sera implémentée plus tard
    // Pour l'instant, elle ne fait rien
    std::cout << "Mise à jour des cartes communes (" << cartes.size() << " cartes)" << std::endl;
}

void TableDePoker::mettreAJourMainJoueur(int joueur, const std::vector<Carte>& main) {
    // Cette fonction sera implémentée plus tard
    // Pour l'instant, elle ne fait rien
    std::cout << "Mise à jour de la main du joueur " << joueur << " (" << main.size() << " cartes)" << std::endl;
}

void TableDePoker::mettreAJourJetons(int joueur, int nouveauxJetons) {
    if (joueur >= 0 && joueur < static_cast<int>(jetonsJoueurs.size())) {
        jetonsJoueurs[joueur] = nouveauxJetons;
        std::cout << "Joueur " << joueur + 1 << " a maintenant " << nouveauxJetons << " jetons" << std::endl;
    }
}