#ifndef POKER2_HPP
#define POKER2_HPP

#include <vector>
#include <string>
#include <algorithm>
#include <iostream>
using namespace std;

// Énumérations
enum class Couleur { Coeur, Pique, Carreau, Trefle };
enum class Rang { Deux, Trois, Quatre, Cinq, Six, Sept, Huit, Neuf, Dix, Valet, Dame, Roi, As };

// Structures
struct Carte {
    Couleur couleur;
    Rang rang;
};

struct JetonsJoueur {
    int jeton1 = 0;
    int jeton2 = 0;
    int jeton5 = 0;
    int jeton10 = 0;
    int jeton25 = 0;
    int jeton50 = 0;
    
    int total() const {
        return jeton1*1 + jeton2*2 + jeton5*5 + jeton10*10 + jeton25*25 + jeton50*50;
    }
};

struct MisesJoueur {
    int miseTotale = 0;
    int miseCeTour = 0;
};

struct Pot {
    int montant = 0;
    vector<int> joueursEligibles;
};

struct MainEvaluee {
    int force;
    vector<int> valeurs;
};

// Déclarations des fonctions
string afficherCarte(const Carte& c);
bool retirerJetons(JetonsJoueur& j, int montant);
void tourDeMise(int& potTotal, int& miseActuelle, vector<bool>& joueurActif, 
                vector<JetonsJoueur>& Jetons, vector<MisesJoueur>& misesJoueurs,
                int nbJoueurs, int premierJoueur, int grosseBlind, 
                bool preFlop = false, vector<int>* misesPrecedentes = nullptr);
void calculerPotsSecondaires(vector<Pot>& pots, const vector<MisesJoueur>& misesJoueurs, 
                            const vector<bool>& joueurActif, int nbJoueurs);
MainEvaluee evaluerMain(const vector<Carte>& cartes);
bool mainEstMeilleure(const MainEvaluee& m1, const MainEvaluee& m2);
string nomMain(int force);
int valeurRang(Rang r);

#endif