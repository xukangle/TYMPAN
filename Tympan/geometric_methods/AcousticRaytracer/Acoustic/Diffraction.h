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

#ifndef DIFFRACTION_H
#define DIFFRACTION_H

#include <cmath>

#include "Event.h"
#include "Geometry/Repere.h"
#include "Base.h"

class Cylindre;

/**
 * \brief Diffraction class Event
 */
class Diffraction : public Event
{
public:
	/// Constructor
    Diffraction(const vec3& position = vec3(0.0, 0.0, 0.0), const vec3& incomingDirection = vec3(0.0, 0.0, 0.0), Cylindre* c = NULL);
    /// Copy constructor
    Diffraction(const Diffraction& other);
    /// Destructor
	virtual ~Diffraction() {}

    virtual void setNbResponseLeft(int _nbResponseLeft) 
	{ 
		nbResponseLeft = static_cast<unsigned int>( std::floor( static_cast<decimal>(_nbResponseLeft-1) / M_2PI * angleOuverture + 0.5 ) );
		nbResponseLeft = nbResponseLeft >= 4 ? nbResponseLeft : 4; // Limite le nombre a 4 reponses minimum
		initialNbResponse = nbResponseLeft;
		computeDTheta(); 
	}

    /**
 	* \brief Computes the next response of the event in function of the number of responses left
 	*/ 
	virtual bool getResponse(vec3& r, bool force = false);

    /// Filter generated response (or not)
	bool (*responseValidator) (const vec3&, const vec3&, const vec3&, vec3 &);

	/// Get the angle of the next response around Keller's cone. (returns only angles that result in a response pointing outside the obstacle)
	void (*getTheta) (const decimal&, const decimal&, const decimal&, decimal&);

	/**
 	* \brief Set the aperture angle of the cone of Keller
 	*/  
    void setAngleOuverture(decimal angle) { angleOuverture = angle; computeDTheta(); }

    /**
 	* \brief Get the aperture angle of the cone of Keller
 	*/  
    decimal getAngleOuverture() { return angleOuverture; }

    /**
 	* \brief Get the angle between 2 consecutive responses
 	*/  
	decimal getDeltaTheta() const { return delta_theta; }
	
	/**
 	* \brief Get the angle between the incident ray and the diffraction edge
 	*/ 
    virtual double getAngle() { return angleArrive; }
    virtual bool generateTest(std::vector<vec3>& succededTest, std::vector<vec3>& failTest, unsigned int nbResponses);
    /// Get the local frame
	virtual const Repere& getRepere() const { return localRepere; }

#ifdef _ALLOW_TARGETING_
    virtual bool isAcceptableResponse(vec3& test);
    virtual bool generateResponse(std::vector<vec3>& responses, unsigned int nbResponses);
    virtual bool appendTarget(vec3 target, bool force = false);
#endif //_ALLOW_TARGETING_

protected:

	/**
 	* \brief Build local frame
 	*/ 
    void buildRepere();
    
    /**
 	* \brief Compute the angle between the incident ray and the diffraction edge
 	*/ 
    void computeAngle();

    /**
 	* \brief Compute the angle step between two responses in function of the aperture angle and the number of responses left
 	* 
 	*/ 
	void computeDTheta() 
	{
		//Angles should span the aperture angle of the diffraction edge
		delta_theta = angleOuverture / static_cast<decimal>(nbResponseLeft); 
	}

    Repere localRepere;				//!< Local frame

    decimal angleOuverture;			//!< Angle formed by the two faces of the diffraction edge
    decimal angleArrive;			//!< Incident ray angle

	decimal delta_theta;			//!< Angle step between two rays to send

	vec3 N1;						//!< Face 1 normal
	vec3 N2;						//!< Face 2 normal

};

#endif
