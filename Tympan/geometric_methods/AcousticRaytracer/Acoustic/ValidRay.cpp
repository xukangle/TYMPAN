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

#include "Engine/AcousticRaytracerConfiguration.h"
#include "Geometry/Cylindre.h"
#include "Geometry/Sphere.h"
#include "ValidRay.h"
#include "SpecularReflexion.h"
#include "Diffraction.h"
#include "DoNothing.h"

bool ValidRay::validRayWithDoNothingEvent(Ray *r, Intersection* inter)
{
	vec3 impact = r->position + r->direction * inter->t;
	DoNothing *newEvent = new DoNothing(impact, r->direction, inter->p);
	std::shared_ptr<Event> SPEv(newEvent);

    vec3 newDir;
    if (newEvent->getResponse(newDir))
    {
        r->position = impact;
        r->direction = newDir;
        r->direction.normalize();
        r->events.push_back(SPEv);
        return true;
	}

	return false;
}

bool ValidRay::validTriangleWithSpecularReflexion(Ray* r, Intersection* inter)
{
	vec3 impact = r->position + r->direction * inter->t;
    vec3 normale = inter->p->getNormal(impact);
    if (normale.dot(r->direction) > 0.) { return false; }

	if (AcousticRaytracerConfiguration::get()->UsePathDifValidation && !pathDiffValidationForReflection(r, impact)) // Validation sur la différence de marche due aux diffractions
	{
		return false;
	}

	SpecularReflexion* newEvent = new SpecularReflexion(impact, r->direction, inter->p);
	std::shared_ptr<Event> SPEv(newEvent);

    vec3 newDir;
    if (newEvent->getResponse(newDir))
    {
        r->position = r->position + r->direction * inter->t;
        r->direction = newDir;
        r->direction.normalize();
        r->events.push_back(SPEv);
        r->nbReflexion += 1;
        return true;
    }

    return false;
}

//Computes distance between impact and last event/source and adds it tout the cumulDistance
void ValidRay::computeCumulDistance(Ray *r, const vec3& impact)
{
	vec3 previousPos;
	if ( r->getEvents()->size() )
	{
		previousPos = r->getEvents()->back()->getPosition();
	}
	else
	{
		previousPos = r->getSource()->getPosition();
	}
	r->cumulDistance += previousPos.distance(impact);
}

//Compute the length between last reflexion/source and 
bool ValidRay::pathDiffValidationForReflection(Ray * r, const vec3& impact)
{
	vec3 lastReflexionPos = r->computeLocalOrigin( r->getLastPertinentEventOrSource(SPECULARREFLEXION));
	computeCumulDistance(r, impact);

	// Compute difference between :
	//    -  the distance between impact and the last event 
	//		and
	//    -  the distance between impact and the last reflexion
	decimal delta= r->cumulDistance - impact.distance(lastReflexionPos);

	//Add the difference to the cumulative delta
	r->cumulDelta += delta ;

	//Reset cumulDistance
	r->cumulDistance = 0.;

	return ( r->cumulDelta <= AcousticRaytracerConfiguration::get()->MaxPathDifference );
}

bool ValidRay::pathDiffValidationForDiffraction(Ray *r, const vec3& impact)
{
	vec3 lastReflexionPos = r->computeLocalOrigin( r->getLastPertinentEventOrSource(SPECULARREFLEXION) );
	computeCumulDistance(r, impact);

	// Compute difference between :
	//    -  the distance between impact and the last event 
	//		and
	//    -  the distance between impact and the last reflexion
	decimal delta= r->cumulDistance - impact.distance(lastReflexionPos);

	//Sum of cumulDelta and delta (Note: cumulDelta is not modified)
	decimal currentCumulDelta = r->cumulDelta + delta;

	return ( currentCumulDelta <= AcousticRaytracerConfiguration::get()->MaxPathDifference );
}

bool ValidRay::computeRealImpact(Ray *r, Intersection* inter, Cylindre *cylindre, vec3& impact)
{
	vec3 p1 = r->position;
	vec3 p2 = r->position + r->direction * inter->t * 2.;
	// Define second segment
	vec3 p3 = cylindre->getVertices()->at(cylindre->getLocalVertices()->at(0));
	vec3 p4 = cylindre->getVertices()->at(cylindre->getLocalVertices()->at(1));

	// shortest segment definition
	vec3 *pa = new vec3(), *pb = new vec3();
	decimal *mua = new decimal(), *mub = new decimal();
	bool res = LineLineIntersect(p1, p2, p3, p4, pa, pb, mua, mub);
   delete pa;
   delete pb;
   delete mua;
   delete mub;
	impact = *pb;

	return res;
}

bool ValidRay::isRayClosestFromRidge(Ray *r, const vec3& impact, const vec3& realImpact)
{
	vec3 closestPoint;
	decimal length = r->computePertinentLength(realImpact, impact, closestPoint);
	decimal thick = r->getThickness(length, true);
	decimal closestDistance = realImpact.distance(closestPoint);

	return ( closestDistance <= ( thick / 2. ) );
}

bool ValidRay::validCylindreWithDiffraction(Ray* r, Intersection* inter)
{
    Cylindre* cylindre = (Cylindre*)(inter->p);

	vec3 impact = r->position + r->direction * inter->t;

        AcousticRaytracerConfiguration* config = AcousticRaytracerConfiguration::get();
// Test if creating a new diffraction is allowed (depends on path length difference)
	if ( config->UsePathDifValidation && !pathDiffValidationForDiffraction(r, impact) )
	{
		return false;
	}

// Compute real impact of the ray on the ridge
	// Define first segment
	vec3 realImpact;
	if ( !computeRealImpact(r, inter, cylindre, realImpact) ) { return false; }

// Valid creation of event using distance from the ridge (if needed)
	if ( config->DiffractionUseDistanceAsFilter && !isRayClosestFromRidge(r, impact, realImpact) )
	{
		return ValidRay::validRayWithDoNothingEvent(r, inter);
	}

// Create diffraction event
    vec3 from = realImpact - r->position;
    from.normalize();
	Diffraction* newEvent = new Diffraction(realImpact, from, (Cylindre*)(inter->p));
	std::shared_ptr<Event> SPEv(newEvent);

// define the number of rays to throw
	unsigned int diff_nb_rays = 0;
	if ( config->NbRayWithDiffraction > 0 )
	{
		diff_nb_rays = config->NbRayWithDiffraction;
	}
	else
	{
		diff_nb_rays = (unsigned int)(r->getSource()->getSampler()->computeDiffractionNbr((decimal)(M_PIDIV2 - newEvent->getAngle())));
	}

// Reduce (if needed) number of rays thrown depending on the diffraction order
	if ( config->DiffractionDropDownNbRays )
	{
		diff_nb_rays = (unsigned int)(diff_nb_rays / pow( static_cast<float>(2), static_cast<int>(r->nbDiffraction) ));
	}

	diff_nb_rays += 1; // DO NOT MOVE/CONCATENATE THIS OPERATION ON UPPER LINES !!!
    newEvent->setNbResponseLeft(diff_nb_rays); // Set the number of ray to throw

// Add the event to the list
    r->events.push_back(SPEv);
    r->nbDiffraction += 1;
    vec3 newDir;
    if (newEvent->getResponse(newDir))
    {
        r->position = realImpact;
        r->direction = newDir;
 
        return true;
    }

    return true;
}
#ifdef _ALLOW_TARGETING_
void ValidRay::appendDirectionToEvent(std::shared_ptr<Event> e, TargetManager& targets)
{
    std::vector<vec3> &ponctualTargets = targets.getTargets();

    std::vector<vec3> acceptableTargets;
    for (unsigned int i = 0; i < ponctualTargets.size(); i++)
    {
        vec3 response = ponctualTargets.at(i) - e->getPosition();
        response.normalize();
        if (e->isAcceptableResponse(response))
        {
            acceptableTargets.push_back(response);
        }
    }

    if (acceptableTargets.size() < static_cast<unsigned int>(e->getNbResponseLeft()))
    {
        for (unsigned int i = 0; i < acceptableTargets.size(); i++)
        {
            e->appendTarget(acceptableTargets.at(i), true);
        }
    }
    else
    {
        for (unsigned int i = 0; i < static_cast<unsigned int>(e->getNbResponseLeft()); i++)
        {
            int index = rand() % e->getNbResponseLeft();
            e->appendTarget(acceptableTargets.at(index), true);
        }
    }
}
#endif // _ALLOW_TARGETING_


