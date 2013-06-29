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
 
#ifndef __RAYCOURB_H
#define __RAYCOURB_H

#include <cmath>
#include <cassert>
#include <cstdlib>
#include <vector>
#include <map>
#include <iostream>

#include "meteo.h"

using namespace std;

typedef double R;
class R3;

class RayCourb
{
public:
    // donnees membres
    vector<R3> coord;                            /*!< vecteur de R3 contenant les coordonnees des points du rayon */
    vector<R3> normale;                          /*!< vecteur de R3 contenant les coordonnees des normales du rayon */
    int nbReflex;                                /*!< nombre de reflexions */
    vector<int> position;                        /*!< liste des indices des points ou il y a une reflexion (le numero du pas de temps) */
    map<int, int> rencontre;                     /*!< couple (pas de temps, numero de la face rencontree) */

    /*!
     *  \brief Constructeur
     *
     *  Constructeurs de la classe RayCourb par defaut, par passage d'arguments et par copie
     *
     */
    RayCourb();
    RayCourb(const R3& a);
    RayCourb(const RayCourb& r);

    /*!
     *  \brief Destructeur
     *
     *  Destructeur de la classe RayCourb
     */
    ~RayCourb();

    /*!
     * \fn void purge()
     * \brief Nettoie les tableaux en desallouant proprement les pointeurs
     */
    void purge();

    /*!
    * \fn void setSize(const unsigned int taille)
    * \brief Reservation de la taille des vecteurs
    */
    void setSize(const unsigned int taille) { coord.reserve(taille); normale.reserve(taille); }

    /*!
    * \fn RayCourb& operator = (RayCourb & P)
    * \brief Operateur d'affectation
    */
    RayCourb& operator= (const RayCourb& P);

    /*!
    * \fn RayCourb& operator = (RayCourb & P)
    * \brief Operateur mathematiques
    */
    RayCourb operator + (const RayCourb& P)const;
    RayCourb operator * (R c)const;
    RayCourb operator / (R c)const;

    /*!
    * \fn vector<R3> operator[](int i)
    * \brief Operateur tableau
    */
    vector<R3> operator[](int i);
};

RayCourb operator*(const R& c, const RayCourb& P);

R cLin(const R3& P, const meteo& Meteo, R3& grad);

R3 vent(const R3& P, const meteo& Meteo, map<pair<int, int>, R> &jacob);

#endif //__RAYCOURB_H