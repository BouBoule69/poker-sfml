#include "include/poker2.hpp"
#include <iostream>
#include <vector> //pour stocker les 52 cartes
#include <random> // pour std::random_device et std::mt19937
#include <algorithm> //pour le shuffle

using namespace std;

std::random_device rd;   // source d'aléatoire pour initialiser le générateur
std::mt19937 gen(rd());  // générateur Mersenne Twister


string afficherCarte(const Carte& c) {
    string couleurStr;
    switch(c.couleur) {
        case Couleur::Coeur:   couleurStr = "Coeur"; break;
        case Couleur::Pique:   couleurStr = "Pique"; break;
        case Couleur::Carreau: couleurStr = "Carreau"; break;
        case Couleur::Trefle:  couleurStr = "Trefle"; break;
    }

    string rangStr;
    switch(c.rang) {
        case Rang::Deux:  rangStr = "2"; break;
        case Rang::Trois: rangStr = "3"; break;
        case Rang::Quatre: rangStr = "4"; break;
        case Rang::Cinq:  rangStr = "5"; break;
        case Rang::Six:   rangStr = "6"; break;
        case Rang::Sept:  rangStr = "7"; break;
        case Rang::Huit:  rangStr = "8"; break;
        case Rang::Neuf:  rangStr = "9"; break;
        case Rang::Dix:   rangStr = "10"; break;
        case Rang::Valet: rangStr = "Valet"; break;
        case Rang::Dame:  rangStr = "Dame"; break;
        case Rang::Roi:   rangStr = "Roi"; break;
        case Rang::As:    rangStr = "As"; break;
    }

    return rangStr + " de " + couleurStr;
}



    // Fonction qui va faire le calcul de jetons restant à chaque joueur pendant les mises
    bool retirerJetons(JetonsJoueur& j, int montant) {
        // Vérifier si le joueur a assez de jetons
        if (j.total() < montant) {
            cout << "Pas assez de jetons !" << endl;
            return false;
        }
        
        int reste = montant;
        
        
        // Retirer avec les jetons de 50
        while (reste >= 50 && j.jeton50 > 0) {
            j.jeton50--;
            reste -= 50;
        }
        
        // Retirer avec les jetons de 25
        while (reste >= 25 && j.jeton25 > 0) {
            j.jeton25--;
            reste -= 25;
        }
        
        // Retirer avec les jetons de 10
        while (reste >= 10 && j.jeton10 > 0) {
            j.jeton10--;
            reste -= 10;
        }
        // Retirer avec les jetons de 5
        while (reste >= 5 && j.jeton5 > 0) {
            j.jeton5--;
            reste -= 5;
        }
        // Retirer avec les jetons de 2
        while (reste >= 2 && j.jeton2 > 0) {
            j.jeton2--;
            reste -= 2;
        }
        
        // Retirer avec les jetons de 1
        while (reste >= 1 && j.jeton1 > 0) {
            j.jeton1--;
            reste -= 1;
        }
        
        return true;  // Retrait réussi
    }

//================================== Fonction pour le tour de mise==========================================================
   //================================== Fonction pour le tour de mise==========================================================
   void tourDeMise(int& potTotal, int& miseActuelle, vector<bool>& joueurActif, 
    vector<JetonsJoueur>& Jetons, vector<MisesJoueur>& misesJoueurs,
    int nbJoueurs, int premierJoueur, int grosseBlind, 
    bool preFlop, vector<int>* misesPrecedentes)  // PLUS DE = false et = nullptr
{
    // Réinitialiser les mises du tour
    for(int i = 0; i < nbJoueurs; ++i) {
        misesJoueurs[i].miseCeTour = 0;
    }

    // Si pré-flop, initialiser avec les blinds
    if(preFlop && misesPrecedentes != nullptr) {
        for(int i = 0; i < nbJoueurs; ++i) {
            misesJoueurs[i].miseCeTour = (*misesPrecedentes)[i];
        }
    }

    int joueurCourant = premierJoueur;
    int nbJoueursParles = 0;
    int nbJoueursActifs = 0;

    // Compter les joueurs actifs
    for(int i = 0; i < nbJoueurs; ++i) {
        if(joueurActif[i]) nbJoueursActifs++;
    }

    // Si un seul joueur actif, pas de mise
    if(nbJoueursActifs <= 1) return;

    bool tourEnCours = true;

    while(tourEnCours) {

        // ✓ Vérifier IMMÉDIATEMENT si tous sont all-in
        int joueursAvecJetons = 0;
        for(int i = 0; i < nbJoueurs; ++i) {
            if(joueurActif[i] && Jetons[i].total() > 0) {
                joueursAvecJetons++;
            }
        }
        
        if(joueursAvecJetons == 0) {
            cout << "Tous les joueurs sont all-in. Fin du tour de mise." << endl;
            break;  // Sortir immédiatement
        }


                // Dans la boucle while(tourEnCours), au début, ajoutez :
        //important, avant ici /tait static int iteration 
        int iteration = 0;
        iteration++;
        if(iteration > 100) {
            cout << "ERREUR: Boucle infinie detectee dans tourDeMise!" << endl;
            cout << "nbJoueursActifs = " << nbJoueursActifs << endl;
            cout << "joueurCourant = " << joueurCourant << endl;
            tourEnCours = false;
            break;
        }
        if(joueurActif[joueurCourant]) {
            // Si le joueur n'a plus de jetons (all-in), passer son tour
            if(Jetons[joueurCourant].total() == 0) {
                cout << "Joueur " << joueurCourant + 1 << " est all-in." << endl;
                nbJoueursParles++;
                joueurCourant = (joueurCourant + 1) % nbJoueurs;
                continue;
            }
            
            cout << "\nJoueur " << joueurCourant + 1 << " (" 
                << Jetons[joueurCourant].total() << " jetons restants)" << endl;
            cout << "Mise actuelle a egaler: " << miseActuelle << endl;
            cout << "Vous avez deja mise ce tour: " << misesJoueurs[joueurCourant].miseCeTour << endl;

            int choix = 0;

            while(choix < 1 || choix > 3) {
                cout << "1. Fold (se coucher)" << endl;

                // Check disponible SEULEMENT si miseActuelle == 0 ET que le joueur n'a rien misé
                if(miseActuelle == 0 && misesJoueurs[joueurCourant].miseCeTour == 0) {
                    cout << "2. Check (parole)" << endl;
                } else {
                    cout << "2. Call (suivre " << (miseActuelle - misesJoueurs[joueurCourant].miseCeTour) << " jetons)" << endl;
                }

                cout << "3. Raise (relancer)" << endl;
                cout << "Votre choix: ";
                cin >> choix;
                
                if(choix < 1 || choix > 3) {
                    cout << "ERREUR: Choix invalide ! Veuillez entrer 1, 2 ou 3." << endl << endl;
                    cin.clear();
                    cin.ignore(10000, '\n');
                }
            }

            if(choix == 1) {
                // FOLD
                joueurActif[joueurCourant] = false;
                nbJoueursActifs--;
                cout << "Joueur " << joueurCourant + 1 << " se couche." << endl;
            }
            else if(choix == 2) {
                // CALL ou CHECK
                if(miseActuelle == 0 && misesJoueurs[joueurCourant].miseCeTour == 0) {
                    // CHECK (seulement si personne n'a misé)
                    cout << "Joueur " << joueurCourant + 1 << " checke." << endl;
                } else {
                    // CALL (obligatoire si quelqu'un a relancé)
                    int aAjouter = miseActuelle - misesJoueurs[joueurCourant].miseCeTour;
                    int jetonsDisponibles = Jetons[joueurCourant].total();
                    
                    if(jetonsDisponibles < aAjouter) {
                        // ALL-IN : le joueur met tous ses jetons restants
                        int montantAllIn = jetonsDisponibles;
                        retirerJetons(Jetons[joueurCourant], montantAllIn);
                        potTotal += montantAllIn;
                        misesJoueurs[joueurCourant].miseCeTour += montantAllIn;
                        misesJoueurs[joueurCourant].miseTotale += montantAllIn;
                        
                        cout << "Joueur " << joueurCourant + 1 << " fait ALL-IN avec " << montantAllIn << " jetons !" << endl;
                    } else {
                        // CALL normal
                        if(retirerJetons(Jetons[joueurCourant], aAjouter)) {
                            potTotal += aAjouter;
                            misesJoueurs[joueurCourant].miseCeTour = miseActuelle;
                            misesJoueurs[joueurCourant].miseTotale += aAjouter;
                            cout << "Joueur " << joueurCourant + 1 << " suit avec " << aAjouter << " jetons." << endl;
                        }
                    }
                }
            }
            else if(choix == 3) {
                // RAISE
                int montantRelance;
                int minimum = (miseActuelle == 0) ? grosseBlind : miseActuelle + grosseBlind;
                int jetonsDisponibles = Jetons[joueurCourant].total();
                int dejaMise = misesJoueurs[joueurCourant].miseCeTour;
                int maxPossible = dejaMise + jetonsDisponibles;
                
                cout << "Montant de la relance (minimum " << minimum << ", maximum " << maxPossible << "): ";
                cin >> montantRelance;
                
                if(montantRelance > maxPossible) {
                    cout << "Vous n'avez pas assez de jetons ! Mise automatique en ALL-IN..." << endl;
                    montantRelance = maxPossible;
                }
                
                if(montantRelance >= minimum || montantRelance == maxPossible) {
                    int aAjouter = montantRelance - dejaMise;
                    
                    if(aAjouter == jetonsDisponibles) {
                        // ALL-IN
                        retirerJetons(Jetons[joueurCourant], jetonsDisponibles);
                        potTotal += jetonsDisponibles;
                        misesJoueurs[joueurCourant].miseCeTour = montantRelance;
                        misesJoueurs[joueurCourant].miseTotale += jetonsDisponibles;
                        miseActuelle = montantRelance;
                        
                        cout << "Joueur " << joueurCourant + 1 << " fait ALL-IN avec " << jetonsDisponibles << " jetons !" << endl;
                    } else {
                        // RAISE normal
                        if(retirerJetons(Jetons[joueurCourant], aAjouter)) {
                            potTotal += aAjouter;
                            misesJoueurs[joueurCourant].miseCeTour = montantRelance;
                            misesJoueurs[joueurCourant].miseTotale += aAjouter;
                            miseActuelle = montantRelance;
                            cout << "Joueur " << joueurCourant + 1 << " relance a " << montantRelance << " jetons." << endl;
                        }
                    }
                } else {
                    cout << "Relance trop faible !" << endl;
                    continue;
                }
            }

            nbJoueursParles++;
        }

        // Passer au joueur suivant (en sautant les joueurs inactifs/éliminés)
        do {
            joueurCourant = (joueurCourant + 1) % nbJoueurs;
        } while(!joueurActif[joueurCourant] && nbJoueursActifs > 0);

        // Conditions pour terminer le tour
        if(nbJoueursActifs <= 1) {
            tourEnCours = false;  // Un seul joueur reste
        }
        else {

            // Compter les joueurs qui peuvent encore miser
         /*  int joueursAvecJetons = 0;
            for(int i = 0; i < nbJoueurs; ++i) {
                if(joueurActif[i] && Jetons[i].total() > 0) {
                    joueursAvecJetons++;
                }
            }
            
            // Si personne ne peut plus miser (tous all-in), arrêter le tour
            if(joueursAvecJetons == 0) {
                tourEnCours = false;
                cout << "Tous les joueurs sont all-in. Fin du tour de mise." << endl;
                break;
            }
         */
            // Vérifier si tous les joueurs actifs ont égalé la mise actuelle
            bool tousEgale = true;
            int nbJoueursAyantParle = 0;
            
            for(int i = 0; i < nbJoueurs; ++i) {
                if(joueurActif[i]) {
                    nbJoueursAyantParle++;
                    // Un joueur actif qui peut encore miser n'a pas égalé la mise
                    if(Jetons[i].total() > 0 && misesJoueurs[i].miseCeTour != miseActuelle) {
                        tousEgale = false;
                        break;
                    }
                }
            }
            
            // Le tour se termine si tous les joueurs actifs ont égalé la mise 
            // ET que tous les joueurs ont eu au moins une chance de parler
            if(tousEgale && nbJoueursParles >= nbJoueursActifs) {
                tourEnCours = false;
            }
        }
    }

    
    cout << "\nPot total: " << potTotal << " jetons" << endl;


}

//==========================================fin fonction tour de mise============================================

//========================================nouvelle fonction calculer les pots secondaires===============
void calculerPotsSecondaires(vector<Pot>& pots, const vector<MisesJoueur>& misesJoueurs, 
    const vector<bool>& joueurActif, int nbJoueurs) {

    pots.clear();

    // 1. Trouver tous les montants de mises différents
    vector<int> montantsUniques;
    for(int i = 0; i < nbJoueurs; ++i) {
        if(misesJoueurs[i].miseTotale > 0) {
            montantsUniques.push_back(misesJoueurs[i].miseTotale);
        }
    }

    if(montantsUniques.empty()) return;

    // Trier et enlever les doublons
    sort(montantsUniques.begin(), montantsUniques.end());
    montantsUniques.erase(unique(montantsUniques.begin(), montantsUniques.end()), montantsUniques.end());

    // 2. Pour chaque niveau, créer un pot
    int montantPrecedent = 0;
    for(int montant : montantsUniques) {
        Pot nouveauPot;
        nouveauPot.montant = 0;
        
        // Pour chaque joueur
        for(int i = 0; i < nbJoueurs; ++i) {
            int miseJoueur = misesJoueurs[i].miseTotale;
            
            if(miseJoueur > montantPrecedent) {
                // Ce joueur contribue à ce niveau
                int contribution = min(miseJoueur, montant) - montantPrecedent;
                nouveauPot.montant += contribution;
                
                // Seulement les joueurs actifs sont éligibles
                if(joueurActif[i]) {
                    nouveauPot.joueursEligibles.push_back(i);
                }
            }
        }
        
        // Ajouter le pot s'il n'est pas vide
        if(nouveauPot.montant > 0) {
            // Enlever les doublons dans joueursEligibles
            sort(nouveauPot.joueursEligibles.begin(), nouveauPot.joueursEligibles.end());
            nouveauPot.joueursEligibles.erase(
                unique(nouveauPot.joueursEligibles.begin(), nouveauPot.joueursEligibles.end()), 
                nouveauPot.joueursEligibles.end()
            );
            
            pots.push_back(nouveauPot);
        }
        
        montantPrecedent = montant;
    }

    // Vérification que la somme des pots = total des mises
    int totalMises = 0;
    for(int i = 0; i < nbJoueurs; ++i) {
        totalMises += misesJoueurs[i].miseTotale;
    }
    
    int totalPots = 0;
    for(const auto& p : pots) {
        totalPots += p.montant;
    }
    
    if(totalPots != totalMises) {
        cout << "ATTENTION: Total pots (" << totalPots 
             << ") != Total mises (" << totalMises << ")" << endl;
    }
    
    // Afficher les pots
    if(pots.size() > 0) {
        cout << "\n=== POTS CREES ===" << endl;
        for(size_t i = 0; i < pots.size(); ++i) {
            cout << "Pot " << i + 1 << ": " << pots[i].montant << " jetons" << endl;
            cout << "  Joueurs eligibles: ";
            for(int joueur : pots[i].joueursEligibles) {
                cout << joueur + 1 << " ";
            }
            cout << endl;
        }
        cout << "Total dans les pots: " << totalPots << " jetons" << endl;
    }
}

//========================================fin nouvelle calculer les pots secondaires===============

//=============================================fonctions pour les regles  du jeu=================================

// Fonction pour obtenir la valeur numérique d'un rang
int valeurRang(Rang r) {
    switch(r) {
        case Rang::Deux:  return 2;
        case Rang::Trois: return 3;
        case Rang::Quatre: return 4;
        case Rang::Cinq:  return 5;
        case Rang::Six:   return 6;
        case Rang::Sept:  return 7;
        case Rang::Huit:  return 8;
        case Rang::Neuf:  return 9;
        case Rang::Dix:   return 10;
        case Rang::Valet: return 11;
        case Rang::Dame:  return 12;
        case Rang::Roi:   return 13;
        case Rang::As:    return 14;
    }
    return 0;
}


// Fonction pour évaluer une main de 7 cartes (2 cartes du joueur + 5 communes)
MainEvaluee evaluerMain(const vector<Carte>& cartes) {
    MainEvaluee result;
    result.force = 1; // Par défaut: carte haute
    
    // Compter les occurrences de chaque rang
    vector<int> compteur(15, 0); // Index 2 à 14 pour les rangs
    vector<int> compteurCouleur(4, 0); // Pour les couleurs
    
    for(const auto& c : cartes) {
        compteur[valeurRang(c.rang)]++;
        compteurCouleur[static_cast<int>(c.couleur)]++;
    }
    
    // Vérifier s'il y a une couleur (5 cartes de même couleur)
    bool aFlush = false;
    Couleur couleurFlush;
    for(int i = 0; i < 4; ++i) {
        if(compteurCouleur[i] >= 5) {
            aFlush = true;
            couleurFlush = static_cast<Couleur>(i);
            break;
        }
    }
    
    // Vérifier les suites
    bool aSuite = false;
    int hauteurSuite = 0;
    
    // Suite normale (5 cartes consécutives)
    for(int i = 14; i >= 6; --i) {
        if(compteur[i] >= 1 && compteur[i-1] >= 1 && compteur[i-2] >= 1 && 
           compteur[i-3] >= 1 && compteur[i-4] >= 1) {
            aSuite = true;
            hauteurSuite = i;
            break;
        }
    }
    
    // Suite spéciale A-2-3-4-5
    if(!aSuite && compteur[14] >= 1 && compteur[2] >= 1 && compteur[3] >= 1 && 
       compteur[4] >= 1 && compteur[5] >= 1) {
        aSuite = true;
        hauteurSuite = 5; // La suite se termine par le 5
    }
    
    // Quinte Flush Royale ou Quinte Flush
    if(aFlush && aSuite) {
        // Vérifier si les cartes de la suite sont de la bonne couleur
        vector<Carte> cartesCouleur;
        for(const auto& c : cartes) {
            if(c.couleur == couleurFlush) {
                cartesCouleur.push_back(c);
            }
        }
        
        // Vérifier la suite dans ces cartes
        vector<int> valeursFlush;
        for(const auto& c : cartesCouleur) {
            valeursFlush.push_back(valeurRang(c.rang));
        }
        sort(valeursFlush.rbegin(), valeursFlush.rend());
        
        bool suiteDansCouleur = false;
        for(int i = 14; i >= 6; --i) {
            bool trouve = true;
            for(int j = 0; j < 5; ++j) {
                if(find(valeursFlush.begin(), valeursFlush.end(), i-j) == valeursFlush.end()) {
                    trouve = false;
                    break;
                }
            }
            if(trouve) {
                suiteDansCouleur = true;
                hauteurSuite = i;
                break;
            }
        }
        
        if(suiteDansCouleur) {
            if(hauteurSuite == 14) {
                result.force = 10; // Quinte Flush Royale
            } else {
                result.force = 9; // Quinte Flush
            }
            result.valeurs.push_back(hauteurSuite);
            return result;
        }
    }
    
    // Compter paires, brelans, carrés
    vector<int> carres, brelans, paires;
    for(int i = 14; i >= 2; --i) {
        if(compteur[i] == 4) carres.push_back(i);
        else if(compteur[i] == 3) brelans.push_back(i);
        else if(compteur[i] == 2) paires.push_back(i);
    }
    
    // Carré
    if(!carres.empty()) {
        result.force = 8;
        result.valeurs.push_back(carres[0]);
        // Ajouter le meilleur kicker
        for(int i = 14; i >= 2; --i) {
            if(i != carres[0] && compteur[i] > 0) {
                result.valeurs.push_back(i);
                break;
            }
        }
        return result;
    }
    
    // Full (Brelan + Paire)
    if(!brelans.empty() && (!paires.empty() || brelans.size() >= 2)) {
        result.force = 7;
        result.valeurs.push_back(brelans[0]);
        if(brelans.size() >= 2) {
            result.valeurs.push_back(brelans[1]);
        } else {
            result.valeurs.push_back(paires[0]);
        }
        return result;
    }
    
    // Couleur
    if(aFlush) {
        result.force = 6;
        vector<int> valeursFlush;
        for(const auto& c : cartes) {
            if(c.couleur == couleurFlush) {
                valeursFlush.push_back(valeurRang(c.rang));
            }
        }
        sort(valeursFlush.rbegin(), valeursFlush.rend());
        for(int i = 0; i < 5 && i < static_cast<int>(valeursFlush.size()); ++i) {
            result.valeurs.push_back(valeursFlush[i]);
        }
        return result;
    }
    
    // Suite
    if(aSuite) {
        result.force = 5;
        result.valeurs.push_back(hauteurSuite);
        return result;
    }
    
    // Brelan
    if(!brelans.empty()) {
        result.force = 4;
        result.valeurs.push_back(brelans[0]);
        // Ajouter les 2 meilleurs kickers
        int kickersAjoutes = 0;
        for(int i = 14; i >= 2 && kickersAjoutes < 2; --i) {
            if(i != brelans[0] && compteur[i] > 0) {
                result.valeurs.push_back(i);
                kickersAjoutes++;
            }
        }
        return result;
    }
    
    // Double paire
    if(paires.size() >= 2) {
        result.force = 3;
        result.valeurs.push_back(paires[0]);
        result.valeurs.push_back(paires[1]);
        // Ajouter le meilleur kicker
        for(int i = 14; i >= 2; --i) {
            if(i != paires[0] && i != paires[1] && compteur[i] > 0) {
                result.valeurs.push_back(i);
                break;
            }
        }
        return result;
    }
    
    // Paire
    if(!paires.empty()) {
        result.force = 2;
        result.valeurs.push_back(paires[0]);
        // Ajouter les 3 meilleurs kickers
        int kickersAjoutes = 0;
        for(int i = 14; i >= 2 && kickersAjoutes < 3; --i) {
            if(i != paires[0] && compteur[i] > 0) {
                result.valeurs.push_back(i);
                kickersAjoutes++;
            }
        }
        return result;
    }
    
    // Carte haute
    result.force = 1;
    for(int i = 14; i >= 2; --i) {
        if(compteur[i] > 0) {
            result.valeurs.push_back(i);
            if(result.valeurs.size() >= 5) break;
        }
    }
    
    return result;
}

//=================================Fin fonction pour les regles=========================================================

// Fonction pour comparer deux mains
bool mainEstMeilleure(const MainEvaluee& m1, const MainEvaluee& m2) {
    if(m1.force != m2.force) {
        return m1.force > m2.force;
    }
    
    // Même force, comparer les valeurs
    for(size_t i = 0; i < min(m1.valeurs.size(), m2.valeurs.size()); ++i) {
        if(m1.valeurs[i] != m2.valeurs[i]) {
            return m1.valeurs[i] > m2.valeurs[i];
        }
    }
    
    return false; // Égalité
}

// Fonction pour afficher le nom de la main
string nomMain(int force) {
    switch(force) {
        case 10: return "Quinte Flush Royale";
        case 9:  return "Quinte Flush";
        case 8:  return "Carre";
        case 7:  return "Full";
        case 6:  return "Couleur";
        case 5:  return "Suite";
        case 4:  return "Brelan";
        case 3:  return "Double Paire";
        case 2:  return "Paire";
        case 1:  return "Carte Haute";
        default: return "Inconnu";
    }
}
