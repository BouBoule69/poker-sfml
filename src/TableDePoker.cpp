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
    
    // Initialiser les boutons
    actionsBoutons = {"Fold", "Check", "Call", "Raise"};
    couleursBoutons = {sf::Color::Red, sf::Color::Yellow, sf::Color::Green, sf::Color::Blue};
    texteCall = "Call";
    texteRaise = "Raise";
    
    for (int i = 0; i < 4; i++) {
        sf::RectangleShape bouton(sf::Vector2f(100, 40));
        bouton.setFillColor(couleursBoutons[i]);
        bouton.setOutlineThickness(2);
        bouton.setOutlineColor(sf::Color::Black);
        bouton.setPosition(300 + i * 120, 550);
        boutons.push_back(bouton);
        
        sf::RectangleShape texteRect(sf::Vector2f(80, 25));
        texteRect.setFillColor(sf::Color::White);
        texteRect.setOutlineThickness(1);
        texteRect.setOutlineColor(sf::Color::Black);
        texteRect.setPosition(310 + i * 120, 558);
        textesBoutons.push_back(texteRect);
    }
    joueurCourant = 0;  // Initialisation
    // Ajoutez cette ligne à la fin du constructeur, avant l'accolade fermante
    montantPot = 0;  // Initialisation du pot à 0
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
    
    // 🆕 DESSINER LE POT AU CENTRE
    // Rectangle de fond pour le pot
    sf::RectangleShape potFond(sf::Vector2f(150, 60));
    potFond.setFillColor(sf::Color(139, 69, 19, 200)); // Brun semi-transparent
    potFond.setOutlineThickness(3);
    potFond.setOutlineColor(sf::Color::White);
    potFond.setPosition(525, 450);
    window.draw(potFond);
    
    // Texte "POT"
    sf::Text textePot;
    textePot.setFont(police);
    textePot.setString("POT");
    textePot.setCharacterSize(24);
    textePot.setFillColor(sf::Color::White);
    textePot.setStyle(sf::Text::Bold);
    
    sf::FloatRect bounds = textePot.getLocalBounds();
    textePot.setPosition(
        525 + (150 - bounds.width) / 2,
        455 
    );
    window.draw(textePot);
    
    // Montant du pot
    sf::Text texteMontant;
    texteMontant.setFont(police);
    texteMontant.setString(std::to_string(montantPot));
    texteMontant.setCharacterSize(28);
    texteMontant.setFillColor(sf::Color::Yellow);
    texteMontant.setStyle(sf::Text::Bold);
    
    bounds = texteMontant.getLocalBounds();
    texteMontant.setPosition(
        525 + (150 - bounds.width) / 2,
        480
    );
    window.draw(texteMontant);
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
        
        // 🆕 Si c'est le joueur courant, ajouter un contour doré plus épais
        if (i == joueurCourant) {
            joueur.setOutlineThickness(5);
            joueur.setOutlineColor(sf::Color::Yellow);
        } else {
            joueur.setOutlineThickness(3);
            joueur.setOutlineColor(sf::Color::White);
        }
        
        joueur.setPosition(positionsJoueurs[i].x, positionsJoueurs[i].y);
        window.draw(joueur);
        
        // 🆕 Ajouter une petite flèche ou étoile pour le joueur courant
        if (i == joueurCourant) {
            sf::CircleShape etoile(10, 3); // Triangle
            etoile.setFillColor(sf::Color::Yellow);
            etoile.setRotation(90);
            etoile.setPosition(positionsJoueurs[i].x + 10, positionsJoueurs[i].y - 20);
            window.draw(etoile);
        }
        
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
        
        // Rectangle pour les jetons
        sf::RectangleShape jetonsRect(sf::Vector2f(70, 20));
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
        
        // Petit indicateur de jetons
        int nbTraits = jetonsJoueurs[i] / 50;
        if (nbTraits > 5) nbTraits = 5;
        
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
    for (int i = 0; i < 4; i++) {
        // Dessiner le bouton
        window.draw(boutons[i]);
        
        // Dessiner le rectangle blanc du texte
        window.draw(textesBoutons[i]);
        
        // Maintenant on peut dessiner le vrai texte avec la police
        sf::Text texteBouton;
        texteBouton.setFont(police);
        
        // Choisir le texte selon le bouton
        if (i == 2) { // Call
            texteBouton.setString(texteCall);
        } else if (i == 3) { // Raise
            texteBouton.setString(texteRaise);
        } else {
            texteBouton.setString(actionsBoutons[i]);
        }
        
        texteBouton.setCharacterSize(18);
        texteBouton.setFillColor(sf::Color::Black);
        texteBouton.setStyle(sf::Text::Bold);
        
        // Centrer le texte sur le bouton
        sf::FloatRect bounds = texteBouton.getLocalBounds();
        texteBouton.setPosition(
            boutons[i].getPosition().x + (boutons[i].getSize().x - bounds.width) / 2,
            boutons[i].getPosition().y + (boutons[i].getSize().y - bounds.height) / 2 - 5
        );
        
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

// ===== NOUVEAU CODE À AJOUTER (AVANT LA DERNIÈRE ACCCOLADE) =====

sf::Vector2f TableDePoker::getPositionSouris() {
    return sf::Vector2f(sf::Mouse::getPosition(window));
}

bool TableDePoker::boutonFoldClique(sf::Vector2f souris) {
    if (boutons.size() > 0) {
        bool clique = boutons[0].getGlobalBounds().contains(souris);
        std::cout << "Test Fold: bouton à (" << boutons[0].getPosition().x 
                  << "," << boutons[0].getPosition().y 
                  << "), souris à (" << souris.x << "," << souris.y 
                  << ") => " << (clique ? "OUI" : "NON") << std::endl;
        return clique;
    }
    return false;
}

bool TableDePoker::boutonCheckClique(sf::Vector2f souris) {
    if (boutons.size() > 1) {
        bool clique = boutons[1].getGlobalBounds().contains(souris);
        std::cout << "Test Check: bouton à (" << boutons[1].getPosition().x 
                  << "," << boutons[1].getPosition().y 
                  << "), souris à (" << souris.x << "," << souris.y 
                  << ") => " << (clique ? "OUI" : "NON") << std::endl;
        return clique;
    }
    return false;
}

bool TableDePoker::boutonCallClique(sf::Vector2f souris) {
    if (boutons.size() > 2) {
        bool clique = boutons[2].getGlobalBounds().contains(souris);
        std::cout << "Test Call: bouton à (" << boutons[2].getPosition().x 
                  << "," << boutons[2].getPosition().y 
                  << "), souris à (" << souris.x << "," << souris.y 
                  << ") => " << (clique ? "OUI" : "NON") << std::endl;
        return clique;
    }
    return false;
}

bool TableDePoker::boutonRaiseClique(sf::Vector2f souris) {
    if (boutons.size() > 3) {
        bool clique = boutons[3].getGlobalBounds().contains(souris);
        std::cout << "Test Raise: bouton à (" << boutons[3].getPosition().x 
                  << "," << boutons[3].getPosition().y 
                  << "), souris à (" << souris.x << "," << souris.y 
                  << ") => " << (clique ? "OUI" : "NON") << std::endl;
        return clique;
    }
    return false;
}

void TableDePoker::setBoutonCallTexte(const std::string& texte) {
    texteCall = texte;
}

void TableDePoker::setBoutonRaiseTexte(const std::string& texte) {
    texteRaise = texte;
}

// ===== FIN DU NOUVEAU CODE =====

void TableDePoker::setJoueurCourant(int joueur) {
    joueurCourant = joueur;
}

void TableDePoker::setMontantPot(int montant) {
    montantPot = montant;
}