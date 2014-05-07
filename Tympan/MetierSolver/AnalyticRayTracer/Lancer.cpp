/*
 * Copyright (C) <2012> <EDF-R&D> <FRANCE>
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "meteoLin.h"
#include "RayCourb.h"
#include "Lancer.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <array>
#include "Tympan/MetierSolver/AcousticRaytracer/Geometry/Latitude2DSampler.h"
#include "Tympan/MetierSolver/AcousticRaytracer/Geometry/Longitude2DSampler.h"
#include "Tympan/MetierSolver/AcousticRaytracer/Geometry/UniformSphericSampler.h"
#include "Tympan/MetierSolver/AcousticRaytracer/Geometry/UniformSphericSampler2.h"


Lancer::Lancer() : sources(NULL), recepteurs(NULL), _weather(NULL), h(0.001), TMax(3.0), temps(NULL), dmax(1000), nbRay(20)
{
    _weather = new meteoLin();
    initialAngleTheta = 0.0;                /*!<  angle de tir initial selon theta */
    finalAngleTheta = 360.0;                  /*!<  angle de tir final selon theta */
    initialAnglePhi = 0.0;                  /*!<  angle de tir initial selon phi */
    finalAnglePhi = 360.0;                    /*!<  angle de tir final selon phi */

    _launchType = 1;            /*!<  mode de lancer des rayons 1:horizontal / 2:vertical / 3:spheric / 4:file */
    wantOutFile = true;                   /*!<  true if outputfile wanted */

    init();
}


Lancer::Lancer(Lancer& L)
{
    sources = L.sources;
    recepteurs = L.recepteurs;
    _plan = L._plan;
    _weather = L._weather;
    h = L.h;
    TMax = L.TMax;
    temps = L.temps;
    dmax = L.dmax;
    nbRay = L.nbRay;
    MatRes = L.MatRes;

    initialAngleTheta = L.initialAngleTheta;
    finalAngleTheta = L.finalAngleTheta;
    initialAnglePhi = L.initialAnglePhi;
    finalAnglePhi = L.finalAnglePhi;

    _launchType = L._launchType;
    wantOutFile = L.wantOutFile;

    init();
}


//Lancer::Lancer(vector<vec3> sources, vector<vec3> recepteurs, vector<vec3*> plan, meteo* Meteo, decimal h, decimal TmpMax, vector<decimal> temps, decimal dmax, unsigned int nbRay) :
//    sources(sources), recepteurs(recepteurs), _plan(plan), _weather(Meteo), h(h), TMax(TmpMax), temps(temps), dmax(dmax), nbRay(nbRay)
//{
//    //RemplirMat();
//}

Lancer::~Lancer()
{
    purgeMatRes();
}

void Lancer::init()
{
    switch (_launchType)
    {
        case 1 : // Tir horizontal
            _sampler = new Latitude2DSampler(nbRay);
            dynamic_cast<Latitude2DSampler*>(_sampler)->setStartTheta(initialAngleTheta);
            dynamic_cast<Latitude2DSampler*>(_sampler)->setStartPhi(initialAnglePhi);
            dynamic_cast<Latitude2DSampler*>(_sampler)->setEndPhi(finalAnglePhi);
            break;
        case 2 : // Tir vertical
            _sampler = new Longitude2DSampler(nbRay);
            dynamic_cast<Longitude2DSampler*>(_sampler)->setStartTheta(initialAngleTheta);
            dynamic_cast<Longitude2DSampler*>(_sampler)->setEndTheta(finalAngleTheta);
            dynamic_cast<Longitude2DSampler*>(_sampler)->setStartPhi(initialAnglePhi);
            break;
        case 3 : // Tir sur une sphere
            _sampler = new UniformSphericSampler(nbRay);
            nbRay = dynamic_cast<UniformSphericSampler*>(_sampler)->getRealNbRays();
            break;
        case 4 : // Tir sur une sphere v2
            _sampler = new UniformSphericSampler2(nbRay);
            nbRay = dynamic_cast<UniformSphericSampler2*>(_sampler)->getRealNbRays();
            break;
        default :
            return; // do nothing
    }
}

void Lancer::purgeMatRes()
{
    // Nettoyage de la matrice resultat
    for (unsigned int i = 0; i < MatRes.size(); i++)
    {
        if (MatRes[i]) { delete [] MatRes[i]; }
        MatRes[i] = NULL;
    }
    MatRes.clear();
}

Step Lancer::EqRay(const Step& y0)                       // Fonction definissant le probleme a resoudre
{
    Step y;
    vec3 n;

    // calcul des variables de celerite et de son gradient, de la vitesse du vent et de sa derive
    vec3 Dc = vec3();
    decimal c = dynamic_cast<meteoLin*>(_weather)->cTemp(y0.pos, Dc);

    //map<pair<int, int>, decimal> Jv;
    const array< array<double, 3>, 3 >& Jv = dynamic_cast<meteoLin*>(_weather)->getJacobMatrix();
    vec3 v = dynamic_cast<meteoLin*>(_weather)->cWind(y0.pos);

    // definition de s (normale normalisee selon la celerite effective) et de Omega
    vec3 s = y0.norm;
    decimal omega = 1 - (v * s);

    // on calcule les coordonnees
    y.pos = ((s * (c * c / omega)) + v);    //(c * c / omega * s + v)

    // on calcule les normales
    //n.x = - omega / c * Dc.x - Jv[make_pair(1, 1)] * s.x - Jv[make_pair(1, 2)] * s.y - Jv[make_pair(1, 3)] * s.z;
    //n.y = - omega / c * Dc.y - Jv[make_pair(2, 1)] * s.x - Jv[make_pair(2, 2)] * s.y - Jv[make_pair(2, 3)] * s.z;
    //n.z = - omega / c * Dc.z - Jv[make_pair(3, 1)] * s.x - Jv[make_pair(3, 2)] * s.y - Jv[make_pair(3, 3)] * s.z;

    //n.x += (Jv[make_pair(1, 2)] - Jv[make_pair(2, 1)]) * s.y + (Jv[make_pair(1, 3)] - Jv[make_pair(3, 1)]) * s.z;
    //n.y += (Jv[make_pair(2, 3)] - Jv[make_pair(3, 2)]) * s.z + (Jv[make_pair(2, 1)] - Jv[make_pair(1, 2)]) * s.x;
    //n.z += (Jv[make_pair(3, 1)] - Jv[make_pair(1, 3)]) * s.x + (Jv[make_pair(3, 2)] - Jv[make_pair(2, 3)]) * s.y;

    n.x = - omega / c * Dc.x - Jv[0][0] * s.x - Jv[0][1] * s.y - Jv[0][2] * s.z;
    n.y = - omega / c * Dc.y - Jv[1][0] * s.x - Jv[1][1] * s.y - Jv[1][2] * s.z;
    n.z = - omega / c * Dc.z - Jv[2][0] * s.x - Jv[2][1] * s.y - Jv[2][2] * s.z;

    n.x += (Jv[0][1] - Jv[1][0]) * s.y + (Jv[0][2] - Jv[2][0]) * s.z;
    n.y += (Jv[1][2] - Jv[2][1]) * s.z + (Jv[1][0] - Jv[0][1]) * s.x;
    n.z += (Jv[2][0] - Jv[0][2]) * s.x + (Jv[2][1] - Jv[1][2]) * s.y;


    y.norm = n;

    return y;
}



RayCourb Lancer::RK4(const Step& y0)
{
    /*
    Pour une source donnee, a chaque pas de temps :
      - on calcule le point suivant par l'algorithme de Runge-Kutta d'ordre 4,
      - on regarde si le rayon rencontre ou non une face (reflexion), si c'est le cas, on enregistre la position de la reflexion (indice) et on incremente le compteur du nombre de
        reflexion.

    Les calculs se finissent soit quand on depasse le temps d'execution maximal, soit lorsque l'on depasse la distance maximale fixes par l'utilisateur.

    La methode rend un rayon courbe.
    */

    decimal travel_length = 0.;

    RayCourb y;
    y.setSize(temps.size() + 10);

    /* on definit deux vectors : un  "actuel" et un "suivant" car la methode de Runge-Kutta est une methode de resolution explicite a un pas de temps */
    Step yAct(y0);
    Step ySuiv;

    Step k1, k2, k3, k4;                            // variables intermediaires pour la resolution de l'algorithme de Runge-Kutta
    int cpt_tps = 0;                                      // compteur de temps

    // Les premieres valeurs correspondent a notre source.
    y.etapes.push_back(yAct);

    // on remplit le reste avec notre fonction EqRay qui definit notre probleme
    decimal d2Max = dmax * dmax;

    while ((static_cast<unsigned int>(cpt_tps) < temps.size()) && (travel_length < dmax))
    {
        // On definit deux variables qui vont nous servir a garder en memoire les valeurs minimales du point d'intersection
        // Car on parcourt les objets dans l'ordre ou ils sont ranges mais il se peut qu'un objet A plus pres du point considere se trouve apres un objet B plus loin
        // et quand on boucle, si on sort des la premiere intersection, la reponse sera l'intersection avec B alors que dans la realite, c'est avec l'objet A qu'il y a une intersection.


        k1 = EqRay(yAct) * h;               // k1 = h * EqRay(yAct, Meteo);
        k2 = EqRay(yAct + k1 * 0.5) * h;        // k2 = h * EqRay(yAct + 0.5 * k1, Meteo);
        k3 = EqRay(yAct + k2 * 0.5) * h;        // k3 = h * EqRay(yAct + 0.5 * k2, Meteo);
        k4 = EqRay(yAct + k3) * h;          // k4 = h * EqRay(yAct + k3, Meteo);

        ySuiv = yAct + ((k1 + k2 * 2. + k3 * 2. + k4) * (1. / 6.));

        // Recherche des intersections
        intersection(cpt_tps, y, yAct, ySuiv);

        // on remplit maintenant notre rayon
        y.etapes.push_back(ySuiv);

        // on met a jour nos variables
        travel_length += ySuiv.pos.distance(yAct.pos); // distance parcourue par le rayon

        yAct = ySuiv;
        cpt_tps++;
    }

    return y;
}

void Lancer::RemplirMat()
{
    /* Methode pour remplir la matrice des resultats.*/

    assert(nbRay > 0);  // on verifie que l'on souhaite bien lancer au moins un rayon.
    vec3 grad, s0, n0;
    //map< pair<int, int>, decimal > jacob;
    Step y0;

    RayCourb* tab = NULL;

    for (unsigned int ns = 0; ns < sources.size(); ++ns)
    {
        vec3& source = sources[ns];
        y0.pos = source;
        tab = new RayCourb[nbRay];

        for (unsigned int k = 0; k < nbRay; ++k)
        {
            n0 = _sampler->getSample();

            s0 = n0 / (dynamic_cast<meteoLin*>(_weather)->cTemp(source, grad) + (dynamic_cast<meteoLin*>(_weather)->cWind(source) * n0));
            y0.norm = s0;

            // on resoud l'equation par la methode de runge-kutta d'ordre 4
            tab[k] = RK4(y0);
        }

        MatRes.push_back(tab);
        tab = NULL;
    }
    assert(MatRes.size() == sources.size());

    if (wantOutFile) { save(); }
}


void Lancer::createTemps()
{
    decimal T = 0.0;
    do
    {
        temps.push_back(T);
        T += h;
    }
    while (T <= TMax);
}

vec3 Lancer::valideIntersection(const vec3& S, const vec3& R, const vec3* A, int& reflexion, const vec3& nExt_plan, const vec3& SR)
{

    /*
     Calcule l'existence d'un trajet SR et d'une face convexe definie par un ensemble de 3 sommets Ai

     S : Coordonnees de la source. ATTENTION : la source ne doit pas se trouver dans le plan definit par A.
     R : Coordonnees du recepteur
     SR : vecteur Source-Recepteur

     A : Tableau contenant les 3 sommets de la face (tous les sommets doivent etre coplanaires)
         Les sommets sont ordonnes ciculairement dans le sens trigonometrique (de sorte que la normale au plan soit la normale exterieure)

     nExt_plan : normale exterieure au plan considere.

     reflexion : variable test pour savoir si on a ou non trouve une intersection entre la droite SR et le plan A.
    */

    vec3 I;
    int test = 0;
    reflexion = 2;
    vec3 u = SR / SR.length(); // u : Vecteur directeur du trajet SR

    // Premier test : on calcule u.n
    // Si le produit scalaire est positif ou nul, on ne rencontrera pas la face (on est respectivement en sens inverse ou parallele).

    double tmp = u * nExt_plan; //u.mult(nExt_plan);

    if (tmp >= 0)
    {
        test = 0;
        reflexion = 0;
        return I;
    }

    // Deuxieme test : on regarde si le rayon va dans la direction du plan

    vec3 A1 = A[0];
    vec3 A2 = A[1];
    vec3 A3 = A[2];

    vec3 SA1(S, A1);
    vec3 SA2(S, A2);
    vec3 SA3(S, A3);

    double uu1 = u * SA1;
    double uu2 = u * SA2;
    double uu3 = u * SA3;

    if ((uu1 < 0) && (uu2 < 0) && (uu3 < 0))
    {
        test = 0;
        reflexion = 0;
        return I;
    }

    // Troisieme test : on regarde si le produit scalaire entre u et chaque normale des 3 faces du tetraedre SA1A2A3 est negatif, c'est-a-dire si notre rayon entre dans le tetraedre.

    double u1 = u * (SA1 ^ SA2);
    double u2 = u * (SA2 ^ SA3);
    double u3 = u * (SA3 ^ SA1);

    if ((u1 >= 0) || (u2 >= 0) || (u3 >= 0))
    {
        test = 0;
        reflexion = 0;
        return I;
    }

    test = 1; // on est dans la bonne direction

    // Dernier test : on regarde si on arrive sur la face

    double l = SA1 * nExt_plan;
    l = l / tmp;

    // On calcule la longueur l du trajet SI ou I est le point d'impact sur la face
    // si SR<SI alors le trajet ne rencontre pas la face, il s'arrete avant.

    if (test != 0)
    {
        if (l > SR.length())
        {
            // Le trajet ne rencontre pas la face, il s'arrete avant ...
            reflexion = 0;
            return I;
        }
        else
        {
            // Le point d'intersection a pour coordonnees S+l*u
            reflexion = 1;
            I = S + (u * l);
        }
    }

    return I;
}

void Lancer::intersection(const unsigned int& timer, RayCourb& current, Step& Y_t0, Step& Y_t1)
{
    vec3 Imin(Y_t0.pos), n, nmin;
    decimal Dmin = 0.;
    int r, rmin;
    bool premiere = 1;                                    // variable boolenne pour savoir si on a deja rencontre un objet ou pas
    int intersec = 2;                                     // variable-test pour savoir s'il existe ou non une intersection
    vec3 I;                                                 // point d'intersection

    // reflexions: on boucle sur tous les objets existants
    for (r = 0; static_cast<unsigned int>(r) < _plan.size(); ++r)
    {
        // on determine les vecteurs directeurs u et v du plan plan[r]
        vec3 u(_plan[r][0], _plan[r][1]);
        vec3 v(_plan[r][0], _plan[r][2]);

        // on determine la normale exterieure n a ce plan
        n = u ^ v;
        n = n / n.length();

        // on calcule le point d'intersection entre le rayon et l'objet
        I = valideIntersection(Y_t0.pos, Y_t1.pos, _plan[r], intersec, n, Y_t0.norm);

        if (intersec == 1) // si on a trouve un point d'intersection entre notre rayon et notre face r
        {
            decimal DI = sqrt((Y_t0.pos.x - I.x) * (Y_t0.pos.x - I.x) + (Y_t0.pos.y - I.y) * (Y_t0.pos.y - I.y) + (Y_t0.pos.z - I.z) * (Y_t0.pos.z - I.z));

            if (premiere == 0) // si ce n'est pas la premiere reflexion que l'on rencontre
            {
                if (DI < Dmin) // si cette condition est verifiee, alors le I trouve est plus proche que l'ancien, cad que c'est celui-ci que notre rayon va rencontrer
                {
                    Imin = I;
                    Dmin = DI;
                    rmin = r;
                    nmin = n;
                }
            }
            else
            {
                Imin = I;
                Dmin = DI;
                rmin = r;
                nmin = n;

                premiere = 0;     // on a une reflexion
            }
        }

        intersec = 2;
    }

    if ((Imin.x != Y_t0.pos.x) || (Imin.y != Y_t0.pos.y) || (Imin.z != Y_t0.pos.z))   // si c'est le cas, cela signifie que notre rayon rencontre quelque chose
    {
        // on incremente le nombre de reflexions
        current.nbReflex++;

        // on garde en memoire le pas de temps pour lequel il y a une reflexion
        current.position.push_back(timer);

        // la "source" du rayon suivant est notre point d'intersection
        Y_t1.pos = Imin;

        // on calcule le cosinus de l'angle forme par le rayon qui arrive et la normale n
        decimal cos_angle = (-nmin) * (Y_t0.norm / Y_t0.norm.length());

        // enfin, on calcule la normale du rayon reflechi
        Y_t1.norm = Y_t0.norm + nmin * Y_t0.norm.length() * cos_angle * 2.;

        current.rencontre.insert(pair<int, int>(timer, rmin));
    }
}

decimal Lancer::distance_max()
{
    decimal result = 0;

    for (unsigned int ns = 0; ns < sources.size(); ++ns)
    {
        for (unsigned int nr = 0; nr < recepteurs.size(); ++nr)
        {
            result = max(result, sqrt((recepteurs[nr].x - sources[ns].x) * (recepteurs[nr].x - sources[ns].x) + (recepteurs[nr].y - sources[ns].y) * (recepteurs[nr].y - sources[ns].y) + (recepteurs[nr].z - sources[ns].z) * (recepteurs[nr].z - sources[ns].z)));
        }
    }

    return result * 1.2; // multiplie par 1.2 pour esperer que le recepteur est dans la zone de deformation
}

void Lancer::loadRayFile(vector<vec3>& tableau_norm)
{
    ifstream para(ray_fileName.c_str()) ;

    // Lecture du nombre de rayons
    para >> nbRay;

    for (unsigned int i = 0 ; i < nbRay ; i++)
    {
        // Recuperation des angles de depart
        unsigned int numRay, nbEvent; // numero du rayon, nombre d'evenement sur le rayon
        char c1, c2, c3;
        decimal anglePhi, angleTheta;

        para >> numRay >> c1 >> anglePhi >> c2 >> angleTheta;
        vec3 angles(anglePhi, angleTheta, 0);
        tableau_norm.push_back(angles);

        // Boucle sur les evenements des rayons (ces valeurs ne sont pas utilisee - uniquement lecture
        para >> nbEvent;
        double x, y, z, bidon;

        for (unsigned int j = 0; j < nbEvent; j++)
        {
            para >> x >> c1 >> y >> c2 >> z >> c3 >> bidon;
        }
    }

    para.close();
}

decimal angle_depart(const decimal& a, const decimal& c, const decimal& d, const decimal& h)
{
    /*La fonction calcule l'angle avec lequel on doit lancer notre rayon pour qu'il parcoure, en ligne droite, une distance minimale d
      a : gradient de celerite
      c : celerite au sol
      d : distance minimale a parcourir
      h : hauteur de la source
    */

    double a2 = a * a;
    double d2 = d * d;
    double c2 = c * c;
    double h2 = h * h;

    return (2 * atan((-4 * a * c * d + sqrt(16 * a2 * c2 * d2 + 4 * (a2 * d2 + a2 * h2 - 2 * c * h * a) * (a2 * d2 + a2 * h2 - 2 * c * h * a))) / (2 * (a2 * d2 + a2 * h2 - 2 * c * h * a))));
}


void Lancer::save()
{
    // on sauvegarde nos resultats dans un fichier .txt.
    ostringstream nom_var;
    nom_var << "MatRes_C" << dynamic_cast<meteoLin*>(_weather)->getGradC()
            << "_V" << dynamic_cast<meteoLin*>(_weather)->getGradV()
            << "_D" << dmax
            << ".txt" << ends;

    ofstream fic_out(nom_var.str().c_str());

    for (unsigned int i = 0; i < MatRes.size(); i++)
    {
        for (unsigned int j = 0; j < nbRay; ++j)
        {
            for (unsigned int k = 0; k < MatRes[i][j].etapes.size(); ++k)
            {
                fic_out << MatRes[i][j].etapes[k].pos.x << " "
                        << MatRes[i][j].etapes[k].pos.y << " "
                        << MatRes[i][j].etapes[k].pos.z << endl;
            }

            fic_out << endl << endl;
        }
    }
}
