#include "../include/TableDePoker.hpp"


TableDePoker::TableDePoker() {
    // Créer la fenêtre
    window.create(sf::VideoMode(LARGEUR, HAUTEUR), "Poker Texas Hold'em", sf::Style::Titlebar | sf::Style::Close);
    window.setFramerateLimit(60);
    
    // Charger la police (créons une police par défaut si aucune n'est trouvée)
    if (!police.loadFromFile("fonts/arial.ttf")) {
        // Si la police n'est pas trouvée, on utilise la police par défaut du système
        std::cout << "Attention: Police non trouvée, utilisation de la police par défaut" << std::endl;
        
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
     // Initialiser les vecteurs pour les cartes
     mainsJoueurs.resize(6);  // Pour 6 joueurs maximum
     cartes.clear();  // Pas de cartes communes au départ
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
    
    // DESSINER LE POT AU CENTRE
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
        // Cercle du joueur
        sf::CircleShape cercleJoueur(35);
        
        // Couleur selon les jetons
        if (jetonsJoueurs[i] >= 200) {
            cercleJoueur.setFillColor(sf::Color::Green);
        } else if (jetonsJoueurs[i] >= 100) {
            cercleJoueur.setFillColor(sf::Color::Blue);
        } else if (jetonsJoueurs[i] >= 50) {
            cercleJoueur.setFillColor(sf::Color::Yellow);
        } else {
            cercleJoueur.setFillColor(sf::Color::Red);
        }
        
        // Contour spécial pour le joueur courant
        if (i == joueurCourant) {
            cercleJoueur.setOutlineThickness(5);
            cercleJoueur.setOutlineColor(sf::Color::Yellow);
        } else {
            cercleJoueur.setOutlineThickness(3);
            cercleJoueur.setOutlineColor(sf::Color::White);
        }
        
        cercleJoueur.setPosition(positionsJoueurs[i].x, positionsJoueurs[i].y);
        window.draw(cercleJoueur);
        
        // Flèche pour le joueur courant
        if (i == joueurCourant) {
            sf::CircleShape fleche(10, 3);
            fleche.setFillColor(sf::Color::Yellow);
            fleche.setRotation(90);
            fleche.setPosition(positionsJoueurs[i].x + 10, positionsJoueurs[i].y - 20);
            window.draw(fleche);
        }
        
        // Rectangle blanc pour le numéro
        sf::RectangleShape numeroRect(sf::Vector2f(25, 25));
        numeroRect.setFillColor(sf::Color::White);
        numeroRect.setOutlineThickness(1);
        numeroRect.setOutlineColor(sf::Color::Black);
        numeroRect.setPosition(positionsJoueurs[i].x + 20, positionsJoueurs[i].y + 10);
        window.draw(numeroRect);
        
        // Rectangle gris pour le nom
        sf::RectangleShape nomRect(sf::Vector2f(50, 20));
        nomRect.setFillColor(sf::Color(100, 100, 100, 200));
        nomRect.setOutlineThickness(1);
        nomRect.setOutlineColor(sf::Color::White);
        nomRect.setPosition(positionsJoueurs[i].x - 10, positionsJoueurs[i].y - 25);
        window.draw(nomRect);
        
        // Rectangle pour les jetons
        sf::RectangleShape jetonsRect(sf::Vector2f(70, 20));
        if (jetonsJoueurs[i] >= 200) {
            jetonsRect.setFillColor(sf::Color(255, 215, 0, 200));
        } else if (jetonsJoueurs[i] >= 100) {
            jetonsRect.setFillColor(sf::Color(255, 255, 0, 200));
        } else {
            jetonsRect.setFillColor(sf::Color(255, 165, 0, 200));
        }
        jetonsRect.setOutlineThickness(1);
        jetonsRect.setOutlineColor(sf::Color::Black);
        jetonsRect.setPosition(positionsJoueurs[i].x - 15, positionsJoueurs[i].y - 5);
        window.draw(jetonsRect);
        
        // Traits indicateurs de jetons
        int nbTraits = jetonsJoueurs[i] / 50;
        if (nbTraits > 5) nbTraits = 5;
        for (int t = 0; t < nbTraits; t++) {
            sf::RectangleShape trait(sf::Vector2f(5, 10));
            trait.setFillColor(sf::Color::White);
            trait.setPosition(positionsJoueurs[i].x + 30 + t * 7, positionsJoueurs[i].y - 3);
            window.draw(trait);
        }
        
        // === PETITES CARTES DES JOUEURS ===
        if (i < static_cast<int>(mainsJoueurs.size()) && !mainsJoueurs[i].empty()) {
            for (int c = 0; c < static_cast<int>(mainsJoueurs[i].size()) && c < 2; c++) {
                // Carte agrandie : 42x56 au lieu de 30x40
                float carteW = 42.0f;
                float carteH = 56.0f;
                float carteX = positionsJoueurs[i].x + 5 + c * 36;
                float carteY = positionsJoueurs[i].y - 40;

                // Fond de la carte
                sf::RectangleShape petiteCarte(sf::Vector2f(carteW, carteH));
                petiteCarte.setFillColor(sf::Color::White);
                petiteCarte.setOutlineThickness(1);
                petiteCarte.setOutlineColor(sf::Color::Black);
                petiteCarte.setPosition(carteX, carteY);
                window.draw(petiteCarte);

                // Rang en haut à gauche (plus grand)
                sf::Text petitRang;
                petitRang.setFont(police);
                petitRang.setString(getSymboleCarte(mainsJoueurs[i][c]));
                petitRang.setCharacterSize(14);
                petitRang.setFillColor(getCouleurCarte(mainsJoueurs[i][c]));
                petitRang.setStyle(sf::Text::Bold);
                petitRang.setPosition(carteX + 3, carteY + 2);
                window.draw(petitRang);

                // Symbole de couleur au centre de la carte (taille 10)
                dessinerSymboleCouleur(window, mainsJoueurs[i][c].couleur,
                    sf::Vector2f(carteX + carteW / 2.0f, carteY + carteH / 2.0f + 4),
                    10,
                    getCouleurCarte(mainsJoueurs[i][c]));
            }
        }
        // === FIN DES PETITES CARTES ===
    }
}

void TableDePoker::dessinerCartesCommunes() {
    for (size_t i = 0; i < cartesCommunes.size(); i++) {
        sf::RectangleShape carte(sf::Vector2f(80, 110));
        carte.setFillColor(sf::Color::White);
        carte.setOutlineThickness(2);
        carte.setOutlineColor(sf::Color::Black);
        carte.setPosition(350 + i * 90, 300);
        window.draw(carte);
        
        // Si nous avons des cartes à afficher
        if (i < cartes.size()) {
            const Carte& c = cartes[i];
            
            // RANG en haut à gauche
            sf::Text texteRang;
            texteRang.setFont(police);
            texteRang.setString(getSymboleCarte(c));
            texteRang.setCharacterSize(24);
            texteRang.setFillColor(getCouleurCarte(c));
            texteRang.setStyle(sf::Text::Bold);
            texteRang.setPosition(carte.getPosition().x + 10, carte.getPosition().y + 10);
            window.draw(texteRang);
            
           
            dessinerSymboleCouleur(window, c.couleur, 
                sf::Vector2f(carte.getPosition().x + 40, carte.getPosition().y + 55), 
                25,  // Taille un peu plus petite
                getCouleurCarte(c));
            
            // RANG en bas à droite (petit)
            sf::Text texteRangBas;
            texteRangBas.setFont(police);
            texteRangBas.setString(getSymboleCarte(c));
            texteRangBas.setCharacterSize(16);
            texteRangBas.setFillColor(getCouleurCarte(c));
            texteRangBas.setStyle(sf::Text::Bold);
            texteRangBas.setPosition(carte.getPosition().x + 55, carte.getPosition().y + 80);
            window.draw(texteRangBas);
        } else {
            // Dos de carte
            sf::RectangleShape dosCarte(sf::Vector2f(70, 100));
            dosCarte.setFillColor(sf::Color(50, 50, 150));
            dosCarte.setOutlineThickness(2);
            dosCarte.setOutlineColor(sf::Color::White);
            dosCarte.setPosition(carte.getPosition().x + 5, carte.getPosition().y + 5);
            window.draw(dosCarte);
            
            sf::CircleShape motif(10);
            motif.setFillColor(sf::Color::Red);
            motif.setPosition(carte.getPosition().x + 30, carte.getPosition().y + 45);
            window.draw(motif);
        }
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

void TableDePoker::mettreAJourCartesCommunes(const std::vector<Carte>& nouvellesCartes) {
    cartes = nouvellesCartes;
    std::cout << "Mise à jour des cartes communes (" << cartes.size() << " cartes)" << std::endl;
}

void TableDePoker::mettreAJourMainJoueur(int joueur, const std::vector<Carte>& main) {
    if (static_cast<int>(mainsJoueurs.size()) <= joueur) {
        mainsJoueurs.resize(joueur + 1);
    }
    mainsJoueurs[joueur] = main;
    std::cout << "Mise à jour de la main du joueur " << joueur + 1 << " (" << main.size() << " cartes)" << std::endl;
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

//fonction pour montrer les cartes
std::string TableDePoker::getSymboleCarte(const Carte& c) {
    // Retourner le symbole du rang
    switch(c.rang) {
        case Rang::Deux:  return "2";
        case Rang::Trois: return "3";
        case Rang::Quatre: return "4";
        case Rang::Cinq:  return "5";
        case Rang::Six:   return "6";
        case Rang::Sept:  return "7";
        case Rang::Huit:  return "8";
        case Rang::Neuf:  return "9";
        case Rang::Dix:   return "10";
        case Rang::Valet: return "J";
        case Rang::Dame:  return "Q";
        case Rang::Roi:   return "K";
        case Rang::As:    return "A";
        default: return "?";
    }
}

sf::Color TableDePoker::getCouleurCarte(const Carte& c) {
    // Retourner la couleur du texte selon l'enseigne
    switch(c.couleur) {
        case Couleur::Coeur:   
        case Couleur::Carreau: return sf::Color::Red;   // Rouge pour ♥ et ♦
        case Couleur::Pique:   
        case Couleur::Trefle:  return sf::Color::Black; // Noir pour ♠ et ♣
        default: return sf::Color::White;
    }
}

std::string TableDePoker::getSymboleCouleur(const Carte& c) {
    // Retourner le symbole de la COULEUR (♥ ♦ ♠ ♣)
    switch(c.couleur) {
        case Couleur::Coeur:   return "♥";
        case Couleur::Carreau: return "♦";
        case Couleur::Pique:   return "♠";
        case Couleur::Trefle:  return "♣";
        default: return "?";
    }
}

void TableDePoker::dessinerSymboleCouleur(sf::RenderWindow& window, Couleur couleur, sf::Vector2f position, float taille, sf::Color teinte) {
    switch(couleur) {
        case Couleur::Coeur: {
            // Cœur fidèle à l'image : deux gros lobes ronds en haut avec encoche, pointe effilée en bas
            float r = taille / 2.0f;

            // Lobe gauche — grand et rond
            sf::CircleShape cercle1(r * 0.72f);
            cercle1.setFillColor(teinte);
            cercle1.setPosition(position.x - r * 1.38f, position.y - r * 1.1f);
            window.draw(cercle1);

            // Lobe droit — symétrique
            sf::CircleShape cercle2(r * 0.72f);
            cercle2.setFillColor(teinte);
            cercle2.setPosition(position.x - r * 0.06f, position.y - r * 1.1f);
            window.draw(cercle2);

            // Corps : large en haut (couvrant les deux lobes en bas), pointe fine en bas
            sf::ConvexShape corps;
            corps.setPointCount(3);
            corps.setPoint(0, sf::Vector2f(position.x - r * 1.4f, position.y - r * 0.25f));
            corps.setPoint(1, sf::Vector2f(position.x + r * 1.4f, position.y - r * 0.25f));
            corps.setPoint(2, sf::Vector2f(position.x,             position.y + r * 1.5f));
            corps.setFillColor(teinte);
            window.draw(corps);

            // Pont pour combler le gap entre les cercles et le corps
            sf::RectangleShape pont(sf::Vector2f(r * 2.8f, r * 0.7f));
            pont.setFillColor(teinte);
            pont.setPosition(position.x - r * 1.4f, position.y - r * 0.7f);
            window.draw(pont);
            break;
        }
        
        case Couleur::Carreau: {
            // Dessiner un carreau (losange)
            sf::ConvexShape losange;
            losange.setPointCount(4);
            losange.setPoint(0, sf::Vector2f(position.x, position.y - taille/2));
            losange.setPoint(1, sf::Vector2f(position.x + taille/2, position.y));
            losange.setPoint(2, sf::Vector2f(position.x, position.y + taille/2));
            losange.setPoint(3, sf::Vector2f(position.x - taille/2, position.y));
            losange.setFillColor(teinte);
            window.draw(losange);
            break;
        }
        
        case Couleur::Pique: {
            // Pique : pointe fine en HAUT + deux lobes arrondis sur les côtés + tige+pied
            float r = taille / 2.0f;

            // Lobe gauche arrondi
            sf::CircleShape lobeGauche(r * 0.62f);
            lobeGauche.setFillColor(teinte);
            lobeGauche.setPosition(position.x - r * 1.3f, position.y - r * 0.7f);
            window.draw(lobeGauche);

            // Lobe droit arrondi (symétrique)
            sf::CircleShape lobeDroit(r * 0.62f);
            lobeDroit.setFillColor(teinte);
            lobeDroit.setPosition(position.x + r * 0.08f, position.y - r * 0.7f);
            window.draw(lobeDroit);

            // Triangle principal : pointe fine en haut, large en bas
            sf::ConvexShape triangle;
            triangle.setPointCount(3);
            triangle.setPoint(0, sf::Vector2f(position.x,            position.y - r * 1.8f)); // pointe haut
            triangle.setPoint(1, sf::Vector2f(position.x + r * 1.3f, position.y + r * 0.3f)); // bas droit
            triangle.setPoint(2, sf::Vector2f(position.x - r * 1.3f, position.y + r * 0.3f)); // bas gauche
            triangle.setFillColor(teinte);
            window.draw(triangle);

            // Pont pour combler gap entre lobes et triangle
            sf::RectangleShape pont(sf::Vector2f(r * 2.6f, r * 0.5f));
            pont.setFillColor(teinte);
            pont.setPosition(position.x - r * 1.3f, position.y - r * 0.35f);
            window.draw(pont);

            // Tige fine centrée
            sf::RectangleShape tige(sf::Vector2f(taille / 7.0f, taille / 2.5f));
            tige.setFillColor(teinte);
            tige.setPosition(position.x - taille / 14.0f, position.y + r * 0.3f);
            window.draw(tige);

            // Pied évasé
            sf::ConvexShape pied;
            pied.setPointCount(4);
            float py = position.y + r * 0.3f + taille / 2.5f;
            pied.setPoint(0, sf::Vector2f(position.x - taille / 14.0f, py));
            pied.setPoint(1, sf::Vector2f(position.x + taille / 14.0f, py));
            pied.setPoint(2, sf::Vector2f(position.x + taille / 3.5f,  py + taille / 8.0f));
            pied.setPoint(3, sf::Vector2f(position.x - taille / 3.5f,  py + taille / 8.0f));
            pied.setFillColor(teinte);
            window.draw(pied);
            break;
        }
        
        case Couleur::Trefle: {
            // Trèfle : 3 cercles égaux disposés comme sur une vraie carte
            // r = rayon de chaque cercle
            float r = taille / 3.0f;
            // Centre de référence = position (centre du symbole)
            // Cercle HAUT : centré au-dessus, horizontalement centré
            // setPosition = coin haut-gauche du bounding box = centre - r
            sf::CircleShape cercleHaut(r);
            cercleHaut.setFillColor(teinte);
            cercleHaut.setPosition(position.x - r, position.y - r * 2.2f);
            window.draw(cercleHaut);

            // Cercle BAS-GAUCHE : centré à gauche, niveau milieu
            sf::CircleShape cercleBasGauche(r);
            cercleBasGauche.setFillColor(teinte);
            cercleBasGauche.setPosition(position.x - r * 2.1f, position.y - r * 0.9f);
            window.draw(cercleBasGauche);

            // Cercle BAS-DROIT : symétrique au bas-gauche
            sf::CircleShape cercleBasDroit(r);
            cercleBasDroit.setFillColor(teinte);
            cercleBasDroit.setPosition(position.x + r * 0.1f, position.y - r * 0.9f);
            window.draw(cercleBasDroit);

            // Tige fine centrée
            sf::RectangleShape tige(sf::Vector2f(taille / 7.0f, taille / 2.5f));
            tige.setFillColor(teinte);
            tige.setPosition(position.x - taille / 14.0f, position.y + r * 0.9f);
            window.draw(tige);

            // Pied évasé au bas de la tige
            sf::ConvexShape pied;
            pied.setPointCount(4);
            float px = position.x;
            float py = position.y + r * 0.9f + taille / 2.5f;
            pied.setPoint(0, sf::Vector2f(px - taille / 14.0f, py));           // haut gauche
            pied.setPoint(1, sf::Vector2f(px + taille / 14.0f, py));           // haut droit
            pied.setPoint(2, sf::Vector2f(px + taille / 3.5f, py + taille / 8.0f)); // bas droit évasé
            pied.setPoint(3, sf::Vector2f(px - taille / 3.5f, py + taille / 8.0f)); // bas gauche évasé
            pied.setFillColor(teinte);
            window.draw(pied);
            break;
        }
    }
}