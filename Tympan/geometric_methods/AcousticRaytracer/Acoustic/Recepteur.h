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

#ifndef RECEPTEUR_H
#define RECEPTEUR_H

#include "Geometry/Shape.h"
#include "Geometry/Sphere.h"
#include "Geometry/mathlib.h"
#include "Base.h"

/**
 * \brief Receptor inherits from a Sphere Shape
 */
class Recepteur : public Sphere
{
public:
	/// Default constructor
    Recepteur() {};
    /// Constructor
	Recepteur(const vec3& pos, const decimal& r) : Sphere(pos, r) 
	{
		name = "unknow receptor";
	}
	/// Copy constructor
    Recepteur(const Recepteur& other) : Sphere(other)
    {
        id = other.id;
    }
    /// Destructor
    virtual ~Recepteur() { }
    /// Return the Shape
    Shape* getShape() { return dynamic_cast<Shape*>(this); }

    /// Get the center of the bounding box
    vec3 getPosition() { return this->getBBox().centroid; }

    /// Get the Intersection between a ray and this shape
    bool intersectionRecepteur(vec3& origine, vec3& directeur, float tmax, Intersection& result);

    /// Get/Set identification
    unsigned int getId() { return id;}
    void setId(unsigned int _id) { id = _id; }

protected:
    unsigned int id;	//!< Shape identification
};

#endif
