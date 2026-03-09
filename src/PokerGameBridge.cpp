#include "../include/PokerGameBridge.hpp"
#include <iostream>
#include <random>
#include <algorithm>

// ─────────────────────────────────────────────────────────────────────────────
// Générateur aléatoire (défini une seule fois ici, ou dans poker2.cpp)
// ─────────────────────────────────────────────────────────────────────────────
extern std::mt19937 gen;

// ═════════════════════════════════════════════════════════════════════════════
// CONSTRUCTEUR
// ═════════════════════════════════════════════════════════════════════════════
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
      petiteBlind(10),
      joueurCourant(0),
      joueurPetiteBlind(0),
      joueurGrosseBlind(0),
      enAttenteHumain(false),
      dernierRelanceur(-1),
      premierTourFait(false),
      etatActuel(EtatJeu::ATTENTE),
      compteurShowdown(0),
      showdownEnCours(false),
      compteurBot(0)
{
    std::cout << "Bridge de jeu initialisé avec " << nbJoueurs << " joueurs" << std::endl;

    // Jetons de départ
    for (int i = 0; i < nbJoueurs; ++i) {
        jetons[i].jeton1  = 14;
        jetons[i].jeton2  = 8;
        jetons[i].jeton5  = 6;
        jetons[i].jeton10 = 4;
        jetons[i].jeton25 = 2;
        jetons[i].jeton50 = 1;
    }

    // Créer le deck de 52 cartes
    for (int s = 0; s < 4; ++s)
        for (int r = 0; r < 13; ++r) {
            Carte c;
            c.couleur = static_cast<Couleur>(s);
            c.rang    = static_cast<Rang>(r);
            conteneur.push_back(c);
        }
    std::shuffle(conteneur.begin(), conteneur.end(), gen);

    synchroniserAffichage();
}

// ═════════════════════════════════════════════════════════════════════════════
// SYNCHRONISATION AFFICHAGE
// ═════════════════════════════════════════════════════════════════════════════
void PokerGameBridge::synchroniserAffichage() {
    for (int i = 0; i < nbJoueurs; i++)
        table.mettreAJourJetons(i, joueurElimine[i] ? -1 : jetons[i].total());

    if (!cartesCommunes.empty())
        table.mettreAJourCartesCommunes(cartesCommunes);

    for (int i = 0; i < nbJoueurs; i++)
        if (!joueurElimine[i] && !mainsJoueurs[i].empty())
            table.mettreAJourMainJoueur(i, mainsJoueurs[i]);
}

// ═════════════════════════════════════════════════════════════════════════════
// GESTION DU TOUR DE MISE  (privées)
// ═════════════════════════════════════════════════════════════════════════════

// Avance joueurCourant au prochain joueur actif (ignore éliminés et couchés)
void PokerGameBridge::passerAuJoueurSuivant() {
    int depart = joueurCourant;
    do {
        joueurCourant = (joueurCourant + 1) % nbJoueurs;
    } while (!joueurActif[joueurCourant] && joueurCourant != depart);
    table.setJoueurCourant(joueurCourant);
}

// Le tour se termine quand TOUS les joueurs actifs ont misé autant que miseActuelle
// OU quand il ne reste qu'un joueur actif
bool PokerGameBridge::tourDeMiseTermine() const {
    int actifs = 0;
    for (int i = 0; i < nbJoueurs; i++)
        if (joueurActif[i]) actifs++;

    if (actifs <= 1) return true;

    // Tout le monde doit avoir misé autant
    for (int i = 0; i < nbJoueurs; i++) {
        if (!joueurActif[i]) continue;
        if (jetons[i].total() > 0 && misesJoueurs[i].miseCeTour != miseActuelle)
            return false;
    }

    // ET on est revenu au dernierRelanceur (tour complet)
    return (joueurCourant == dernierRelanceur);
}

// Initialise un nouveau tour de mise pour une phase donnée
void PokerGameBridge::demarrerTourDeMise(bool preFlop) {
    compteurBot = 0;  // reset le délai au début de chaque tour
    // Remettre les mises du tour à zéro
    premierTourFait = false;
    for (int i = 0; i < nbJoueurs; i++)
        misesJoueurs[i].miseCeTour = 0;

    if (preFlop) {
        // Au pré-flop les blinds ont déjà misé : on repart des valeurs déjà en place
        misesJoueurs[joueurPetiteBlind].miseCeTour = petiteBlind;
        misesJoueurs[joueurGrosseBlind].miseCeTour = grosseBlind;
        miseActuelle = grosseBlind;
        // Le premier à parler est après la grosse blind
        joueurCourant = (joueurGrosseBlind + 1) % nbJoueurs;
        while (!joueurActif[joueurCourant])
            joueurCourant = (joueurCourant + 1) % nbJoueurs;
        // La grosse blind est le "dernier relanceur" : le tour se ferme
        // quand on revient sur elle (sauf si quelqu'un relance)
        dernierRelanceur = joueurGrosseBlind;
    } else {
        miseActuelle = 0;
    
        // La petite blind parle en premier après le flop/turn/river
        joueurCourant = joueurPetiteBlind;
        while (!joueurActif[joueurCourant])
            joueurCourant = (joueurCourant + 1) % nbJoueurs;
    
        // Le tour se ferme quand on revient au joueur qui a ouvert
        dernierRelanceur = joueurCourant;
    }

    table.setJoueurCourant(joueurCourant);
    enAttenteHumain = false;  // sera mis à true si c'est le tour de l'humain
}

// ─────────────────────────────────────────────────────────────────────────────
// Décision automatique pour un bot (logique simple)
// ─────────────────────────────────────────────────────────────────────────────
void PokerGameBridge::jouerActionBot(int joueur) {
    int aCaller = miseActuelle - misesJoueurs[joueur].miseCeTour;
    int stack   = jetons[joueur].total();

    // ── Évaluer la force de la main du bot ──────────────────────
    int force = 0;
    if (!mainsJoueurs[joueur].empty()) {
        std::vector<Carte> toutesCartes = mainsJoueurs[joueur];
        for (const auto& c : cartesCommunes)
            toutesCartes.push_back(c);
        MainEvaluee main = evaluerMain(toutesCartes);
        force = main.force;
    }

    // ── Évaluer la force des 2 cartes en main (pré-flop) ────────
    bool mainFortePreFlop = false;
    if (cartesCommunes.empty() && mainsJoueurs[joueur].size() == 2) {
        int r1 = valeurRang(mainsJoueurs[joueur][0].rang);
        int r2 = valeurRang(mainsJoueurs[joueur][1].rang);
        // Paire ou 2 cartes hautes (10+)
        mainFortePreFlop = (r1 == r2) || (r1 >= 10 && r2 >= 10);
    }

    // ── Bluff aléatoire (15% de chance) ─────────────────────────
    std::uniform_int_distribution<int> dé(1, 100);
    bool bluff = (dé(gen) <= 15);

    // ── Décision ────────────────────────────────────────────────
    if (cartesCommunes.empty()) {
        // PRÉ-FLOP
        if (mainFortePreFlop || bluff) {
            if (force >= 2 || bluff) {
                // Raise
                int montantRaise = grosseBlind * 2;
                if (montantRaise <= stack) {
                    int nouvelleMise = miseActuelle + montantRaise;
                    int aVerser = nouvelleMise - misesJoueurs[joueur].miseCeTour;
                    aVerser = std::min(aVerser, stack);
                    retirerJetons(jetons[joueur], aVerser);
                    pot += aVerser;
                    misesJoueurs[joueur].miseCeTour  += aVerser;
                    misesJoueurs[joueur].miseTotale  += aVerser;
                    miseActuelle    = nouvelleMise;
                    dernierRelanceur = joueur;
                    table.setMontantPot(pot);
                    std::cout << "[BOT " << joueur+1 << "] RAISE +" << montantRaise
                              << (bluff ? " (BLUFF!)" : "") << std::endl;
                    return;
                }
            }
            // Call si pas assez pour raise
            int montant = std::min(aCaller, stack);
            retirerJetons(jetons[joueur], montant);
            pot += montant;
            misesJoueurs[joueur].miseCeTour  += montant;
            misesJoueurs[joueur].miseTotale  += montant;
            table.setMontantPot(pot);
            std::cout << "[BOT " << joueur+1 << "] CALL " << montant << std::endl;
        } else {
            // Fold si main faible et mise > 20% du stack
            if (aCaller > stack / 5) {
                std::cout << "[BOT " << joueur+1 << "] FOLD (main faible pré-flop)" << std::endl;
                joueurActif[joueur] = false;
            } else {
                // Call quand même si c'est pas cher
                int montant = std::min(aCaller, stack);
                retirerJetons(jetons[joueur], montant);
                pot += montant;
                misesJoueurs[joueur].miseCeTour  += montant;
                misesJoueurs[joueur].miseTotale  += montant;
                table.setMontantPot(pot);
                std::cout << "[BOT " << joueur+1 << "] CALL " << montant
                          << " (pas cher)" << std::endl;
            }
        }
    } else {
        // POST-FLOP (flop/turn/river)
        if (force >= 7 || bluff) {
            // Main très forte ou bluff → raise agressif
            int montantRaise = std::min(pot / 2, stack);  // mise de moitié du pot
            montantRaise = std::max(montantRaise, grosseBlind * 2);
            if (montantRaise <= stack && aCaller + montantRaise <= stack) {
                int nouvelleMise = miseActuelle + montantRaise;
                int aVerser = nouvelleMise - misesJoueurs[joueur].miseCeTour;
                aVerser = std::min(aVerser, stack);
                retirerJetons(jetons[joueur], aVerser);
                pot += aVerser;
                misesJoueurs[joueur].miseCeTour  += aVerser;
                misesJoueurs[joueur].miseTotale  += aVerser;
                miseActuelle     = nouvelleMise;
                dernierRelanceur = joueur;
                table.setMontantPot(pot);
                std::cout << "[BOT " << joueur+1 << "] RAISE +" << montantRaise
                          << " (force=" << force << (bluff ? ", BLUFF" : "") << ")" << std::endl;
                return;
            }
        }

        if (force >= 4) {
            // Brelan ou mieux → call toujours
            int montant = std::min(aCaller, stack);
            if (montant > 0) {
                retirerJetons(jetons[joueur], montant);
                pot += montant;
                misesJoueurs[joueur].miseCeTour  += montant;
                misesJoueurs[joueur].miseTotale  += montant;
                table.setMontantPot(pot);
            }
            std::cout << "[BOT " << joueur+1 << "] CALL " << montant
                      << " (force=" << force << ")" << std::endl;
        } else if (force >= 2) {
            // Paire/double paire → call si pas trop cher (< 40% du stack)
            if (aCaller <= stack * 4 / 10) {
                int montant = std::min(aCaller, stack);
                if (montant > 0) {
                    retirerJetons(jetons[joueur], montant);
                    pot += montant;
                    misesJoueurs[joueur].miseCeTour  += montant;
                    misesJoueurs[joueur].miseTotale  += montant;
                    table.setMontantPot(pot);
                }
                std::cout << "[BOT " << joueur+1 << "] CALL " << montant
                          << " (paire)" << std::endl;
            } else {
                std::cout << "[BOT " << joueur+1 << "] FOLD (paire mais trop cher)" << std::endl;
                joueurActif[joueur] = false;
            }
        } else {
            // Carte haute → fold si quelqu'un a misé
            if (aCaller > 0) {
                std::cout << "[BOT " << joueur+1 << "] FOLD (carte haute)" << std::endl;
                joueurActif[joueur] = false;
            } else {
                std::cout << "[BOT " << joueur+1 << "] CHECK (carte haute)" << std::endl;
            }
        }
    }
}

// ═════════════════════════════════════════════════════════════════════════════
// mettreAJour() — appelé chaque frame depuis main()
// Avance la logique du jeu quand ce n'est pas le tour de l'humain
// ═════════════════════════════════════════════════════════════════════════════
void PokerGameBridge::mettreAJour() {

    // ── Délai du showdown ────────────────────────────────────────
    if (showdownEnCours) {
        compteurShowdown++;
        if (compteurShowdown >= 180) {
            showdownEnCours = false;
            table.setShowdown(false);
            verifierEliminations();
            etatActuel = EtatJeu::ATTENTE;
            synchroniserAffichage();
        }
        return;
    }

    if (enAttenteHumain) return;

    bool tourMise = (etatActuel == EtatJeu::PREFLOP_MISE ||
                     etatActuel == EtatJeu::FLOP_MISE    ||
                     etatActuel == EtatJeu::TURN_MISE    ||
                     etatActuel == EtatJeu::RIVER_MISE);

    if (!tourMise) return;

    // ── Délai entre chaque action (bot ou vérification) ─────────
    compteurBot++;
    if (compteurBot < 60) return;  // attendre ~1 seconde
    compteurBot = 0;

    // ── Vérifier si le tour de mise est terminé ──────────────────
    if (tourDeMiseTermine()) {
        if (!premierTourFait) {
            premierTourFait = true;
        } else {
            premierTourFait = false;
            synchroniserAffichage();
            switch (etatActuel) {
                case EtatJeu::PREFLOP_MISE: jouerFlop();     break;
                case EtatJeu::FLOP_MISE:    jouerTurn();     break;
                case EtatJeu::TURN_MISE:    jouerRiver();    break;
                case EtatJeu::RIVER_MISE:   jouerShowdown(); break;
                default: break;
            }
            return;
        }
    }

    // ── Joueur humain à 0 jetons → passer automatiquement ───────
    if (joueurCourant == 0 && jetons[0].total() == 0) {
        std::cout << "[HUMAIN] All-in, tour passé automatiquement" << std::endl;
        passerAuJoueurSuivant();
        return;
    }

    // ── Tour de l'humain ─────────────────────────────────────────
    if (joueurCourant == 0) {
        enAttenteHumain = true;
        int aCaller = miseActuelle - misesJoueurs[0].miseCeTour;
        table.setBoutonCallTexte(aCaller <= 0 ? "Check" : "Call " + std::to_string(aCaller));
        std::cout << "[HUMAIN] À vous ! (à payer : " << aCaller << ")" << std::endl;
        return;
    }

    // ── Action du bot ────────────────────────────────────────────
    jouerActionBot(joueurCourant);
    passerAuJoueurSuivant();
    synchroniserAffichage();
}
// ═════════════════════════════════════════════════════════════════════════════
// PHASES DU JEU
// ═════════════════════════════════════════════════════════════════════════════

void PokerGameBridge::demarrerMain() {
    verifierEliminations(); 
    std::cout << "\n=== NOUVELLE MAIN === (donneur: joueur " << donneur + 1 << ")" << std::endl;

    // Remettre le deck si vide
    if (conteneur.size() < 20) {
        conteneur.clear();
        for (int s = 0; s < 4; ++s)
            for (int r = 0; r < 13; ++r) {
                Carte c;
                c.couleur = static_cast<Couleur>(s);
                c.rang    = static_cast<Rang>(r);
                conteneur.push_back(c);
            }
        std::shuffle(conteneur.begin(), conteneur.end(), gen);
    }

    // Réinitialiser la main
    pot = 0;
    cartesCommunes.clear();
    miseActuelle = 0;
    table.mettreAJourCartesCommunes(cartesCommunes);

    for (int i = 0; i < nbJoueurs; ++i) {
        misesJoueurs[i] = {0, 0};
        joueurActif[i]  = !joueurElimine[i];
    }

    // Blinds
    joueurPetiteBlind = (donneur + 1) % nbJoueurs;
    while (joueurElimine[joueurPetiteBlind])
        joueurPetiteBlind = (joueurPetiteBlind + 1) % nbJoueurs;

    joueurGrosseBlind = (joueurPetiteBlind + 1) % nbJoueurs;
    while (joueurElimine[joueurGrosseBlind])
        joueurGrosseBlind = (joueurGrosseBlind + 1) % nbJoueurs;

    std::cout << "Petite blind: J" << joueurPetiteBlind + 1
              << "  Grosse blind: J" << joueurGrosseBlind + 1 << std::endl;

    // Prélever les blinds
    auto preleverBlind = [&](int joueur, int montant) {
        int m = std::min(montant, jetons[joueur].total());
        retirerJetons(jetons[joueur], m);
        pot += m;
        misesJoueurs[joueur].miseTotale = m;
        misesJoueurs[joueur].miseCeTour = m;
    };
    preleverBlind(joueurPetiteBlind, petiteBlind);
    preleverBlind(joueurGrosseBlind, grosseBlind);
    table.setMontantPot(pot);
    // Afficher qui est petite/grosse blind dans la console
    std::cout << ">>> Petite blind : Joueur " << joueurPetiteBlind + 1
    << " | Grosse blind : Joueur " << joueurGrosseBlind + 1 << std::endl;
    // Mettre à jour les textes des boutons pour indiquer les blinds
    table.setBoutonCallTexte("Call " + std::to_string(grosseBlind));

    // Distribuer les cartes
    std::shuffle(conteneur.begin(), conteneur.end(), gen);
    for (int i = 0; i < nbJoueurs; ++i) {
        mainsJoueurs[i].clear();
        if (!joueurElimine[i] && conteneur.size() >= 2) {
            mainsJoueurs[i].push_back(conteneur.back()); conteneur.pop_back();
            mainsJoueurs[i].push_back(conteneur.back()); conteneur.pop_back();
        }
    }

    synchroniserAffichage();
    etatActuel = EtatJeu::PREFLOP;
}

void PokerGameBridge::jouerPreFlop() {
    std::cout << "\n--- PRÉ-FLOP ---" << std::endl;
    etatActuel = EtatJeu::PREFLOP_MISE;
    demarrerTourDeMise(true);   // true = pré-flop (blinds déjà mises)
}

void PokerGameBridge::jouerFlop() {
    std::cout << "\n--- FLOP ---" << std::endl;
    if (conteneur.size() >= 4) {
        conteneur.pop_back(); // brûler
        for (int i = 0; i < 3; i++) {
            cartesCommunes.push_back(conteneur.back());
            conteneur.pop_back();
        }
    }
    table.mettreAJourCartesCommunes(cartesCommunes);
    etatActuel = EtatJeu::FLOP_MISE;
    demarrerTourDeMise(false);
}

void PokerGameBridge::jouerTurn() {
    std::cout << "\n--- TURN ---" << std::endl;
    if (conteneur.size() >= 2) {
        conteneur.pop_back(); // brûler
        cartesCommunes.push_back(conteneur.back());
        conteneur.pop_back();
    }
    table.mettreAJourCartesCommunes(cartesCommunes);
    etatActuel = EtatJeu::TURN_MISE;
    demarrerTourDeMise(false);
}

void PokerGameBridge::jouerRiver() {
    std::cout << "\n--- RIVER ---" << std::endl;
    if (conteneur.size() >= 2) {
        conteneur.pop_back(); // brûler
        cartesCommunes.push_back(conteneur.back());
        conteneur.pop_back();
    }
    table.mettreAJourCartesCommunes(cartesCommunes);
    etatActuel = EtatJeu::RIVER_MISE;
    demarrerTourDeMise(false);
}
void PokerGameBridge::verifierEliminations() {
    for (int i = 0; i < nbJoueurs; i++) {
        // Éliminé seulement si 0 jetons APRÈS le showdown
        // (pas pendant la main où il peut être all-in)
        if (!joueurElimine[i] && jetons[i].total() == 0) {
            joueurElimine[i] = true;
            joueurActif[i]   = false;
            std::cout << ">>> Joueur " << i+1 << " est éliminé !" << std::endl;
            // Marqueur négatif pour l'affichage
            table.mettreAJourJetons(i, -1);
        }
    }
}
void PokerGameBridge::jouerShowdown() {
    std::cout << "\n--- SHOWDOWN ---" << std::endl;
    table.setShowdown(true);
    table.setJoueursActifs(joueurActif);  // ← passer qui est encore dans la main
    // Forcer l'envoi des cartes de TOUS les joueurs actifs à l'affichage
    for (int i = 0; i < nbJoueurs; i++) {
        if (joueurActif[i] && !mainsJoueurs[i].empty()) {
            table.mettreAJourMainJoueur(i, mainsJoueurs[i]);
            std::cout << "Révèle cartes joueur " << i+1 << std::endl;
        }
    }
    // ── 1. Calculer les pots secondaires ────────────────────────
    std::vector<Pot> pots;
    calculerPotsSecondaires(pots, misesJoueurs, joueurActif, nbJoueurs);

    // Si pas de pots calculés, mettre le pot principal
    if (pots.empty()) {
        Pot potPrincipal;
        potPrincipal.montant = pot;
        for (int i = 0; i < nbJoueurs; i++)
            if (joueurActif[i])
                potPrincipal.joueursEligibles.push_back(i);
        pots.push_back(potPrincipal);
    }

    // ── 2. Évaluer la main de chaque joueur actif ───────────────
    std::vector<MainEvaluee> mains(nbJoueurs);
    for (int i = 0; i < nbJoueurs; i++) {
        if (!joueurActif[i] || mainsJoueurs[i].size() < 2) continue;
        std::vector<Carte> toutesCartes = mainsJoueurs[i];
        for (const auto& c : cartesCommunes)
            toutesCartes.push_back(c);
        mains[i] = evaluerMain(toutesCartes);
        std::cout << "Joueur " << i+1 << " : " << nomMain(mains[i].force) << std::endl;
    }

    // ── 3. Distribuer chaque pot ─────────────────────────────────
    for (auto& p : pots) {
        if (p.joueursEligibles.empty()) continue;

        // Trouver le(s) gagnant(s) parmi les éligibles
        int indexGagnant = -1;
        MainEvaluee meilleureMain;

        for (int i : p.joueursEligibles) {
            if (!joueurActif[i]) continue;
            if (indexGagnant == -1 || mainEstMeilleure(mains[i], meilleureMain)) {
                meilleureMain = mains[i];
                indexGagnant  = i;
            }
        }

        // Chercher les égalités (split pot)
        std::vector<int> gagnants;
        for (int i : p.joueursEligibles) {
            if (!joueurActif[i]) continue;
            if (!mainEstMeilleure(meilleureMain, mains[i]) &&
                !mainEstMeilleure(mains[i], meilleureMain)) {
                gagnants.push_back(i);
            }
        }

        if (gagnants.empty() && indexGagnant != -1)
            gagnants.push_back(indexGagnant);

        // Distribuer le pot
        int partChacun = p.montant / gagnants.size();
        int reste      = p.montant % gagnants.size(); // jetons impairs

        for (int g : gagnants) {
            jetons[g].jeton1 += partChacun;
            std::cout << ">>> Joueur " << g+1 << " remporte " << partChacun
                      << " jetons (" << nomMain(mains[g].force) << ")" << std::endl;
        }
        // Le reste va au premier gagnant (règle standard)
        if (reste > 0) {
            jetons[gagnants[0]].jeton1 += reste;
            std::cout << ">>> Joueur " << gagnants[0]+1
                      << " reçoit " << reste << " jeton(s) impair(s)" << std::endl;
        }
    }

    pot = 0;
    table.setMontantPot(0);
    donneur = (donneur + 1) % nbJoueurs;
    while (joueurElimine[donneur])
        donneur = (donneur + 1) % nbJoueurs;

    showdownEnCours  = true;
    compteurShowdown = 0;
    etatActuel       = EtatJeu::SHOWDOWN;
    // PAS de verifierEliminations() ici
    // PAS de setShowdown(false) ici
    // PAS de synchroniserAffichage() ici
}

// ═════════════════════════════════════════════════════════════════════════════
// ACTIONS HUMAIN
// (Toutes vérifient que c'est bien le tour de l'humain avant d'agir)
// ═════════════════════════════════════════════════════════════════════════════

void PokerGameBridge::actionFold(int joueur) {
    if (!enAttenteHumain || joueurCourant != joueur) return;

    std::cout << "Joueur " << joueur + 1 << " FOLD" << std::endl;
    joueurActif[joueur] = false;
    enAttenteHumain = false;
    passerAuJoueurSuivant();
    synchroniserAffichage();
}

void PokerGameBridge::actionCheck(int joueur) {
    if (!enAttenteHumain || joueurCourant != joueur) return;

    int aCaller = miseActuelle - misesJoueurs[joueur].miseCeTour;
    if (aCaller > 0) {
        std::cout << "Check impossible ! Il faut call " << aCaller << " ou folder." << std::endl;
        return;
    }

    std::cout << "Joueur " << joueur + 1 << " CHECK" << std::endl;
    enAttenteHumain = false;
    passerAuJoueurSuivant();
    synchroniserAffichage();
}

void PokerGameBridge::actionCall(int joueur) {
    if (!enAttenteHumain || joueurCourant != joueur) return;

    int aCaller = miseActuelle - misesJoueurs[joueur].miseCeTour;

    if (aCaller <= 0) {
        // Équivalent à un check
        actionCheck(joueur);
        return;
    }

    int montant = std::min(aCaller, jetons[joueur].total());
    retirerJetons(jetons[joueur], montant);
    pot += montant;
    misesJoueurs[joueur].miseCeTour  += montant;
    misesJoueurs[joueur].miseTotale  += montant;
    table.setMontantPot(pot);

    std::cout << "Joueur " << joueur + 1 << " CALL " << montant
              << "  (pot=" << pot << ")" << std::endl;

    enAttenteHumain = false;
    passerAuJoueurSuivant();
    synchroniserAffichage();
}

void PokerGameBridge::actionRaise(int joueur, int montant) {
    if (!enAttenteHumain || joueurCourant != joueur) return;

    // montant = supplément AU-DESSUS de la mise actuelle
    int nouvelleMise = miseActuelle + montant;
    int aVerser = nouvelleMise - misesJoueurs[joueur].miseCeTour;

    if (aVerser > jetons[joueur].total()) {
        std::cout << "Pas assez de jetons pour raise " << montant << std::endl;
        return;
    }

    retirerJetons(jetons[joueur], aVerser);
    pot += aVerser;
    misesJoueurs[joueur].miseCeTour  = nouvelleMise;
    misesJoueurs[joueur].miseTotale += aVerser;
    miseActuelle     = nouvelleMise;
    dernierRelanceur = joueur;   // le tour doit refaire le tour complet
    table.setMontantPot(pot);

    std::cout << "Joueur " << joueur + 1 << " RAISE +" << montant
              << " (nouvelle mise=" << miseActuelle << ", pot=" << pot << ")" << std::endl;

    enAttenteHumain = false;
    passerAuJoueurSuivant();
    synchroniserAffichage();
}
void PokerGameBridge::actionAllIn(int joueur) {
    if (!enAttenteHumain || joueurCourant != joueur) return;

    int stack = jetons[joueur].total();
    if (stack <= 0) return;

    int nouvelleMise = misesJoueurs[joueur].miseCeTour + stack;
    retirerJetons(jetons[joueur], stack);
    pot += stack;
    misesJoueurs[joueur].miseCeTour  = nouvelleMise;
    misesJoueurs[joueur].miseTotale += stack;

    if (nouvelleMise > miseActuelle) {
        miseActuelle     = nouvelleMise;
        dernierRelanceur = joueur;
    }

    table.setMontantPot(pot);
    std::cout << "Joueur " << joueur+1 << " ALL-IN avec " << stack
              << " jetons !" << std::endl;

    enAttenteHumain = false;
    passerAuJoueurSuivant();
    synchroniserAffichage();
}