/*
 * Copyright (C) <2012-2014> <EDF-R&D> <FRANCE>
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

#include "config.h"

namespace tympan
{

// Initialise a NULL.
LPSolverConfiguration SolverConfiguration::_pInstance = 0;

SolverConfiguration::SolverConfiguration()
{
    AtmosPressure=101300.;
    AtmosTemperature=20.;
    AtmosHygrometry=60.;
    AnalyticC0=340.;
    AnalyticGradC=0.;
    AnalyticGradV=0.;
    WindDirection=0.;

    RayTracingOrder=0;
    Discretization=2;
    NbRaysPerSource=10000;
    MaxLength=5000;
    SizeReceiver=2.;
    Accelerator=1; // XXX 3 for ANIME3D Solver?
    MaxTreeDepth=12;
    AngleDiffMin=5.;
    CylindreThick=0.3f;
    MaxProfondeur=10;
    UseSol=0;
    MaxReflexion=0;
    MaxDiffraction=2;
    DiffractionUseRandomSampler=0;
    NbRayWithDiffraction=0;
    DiffractionDropDownNbRays=1;
    DiffractionFilterRayAtCreation=0;
    UsePathDifValidation=0;
    MaxPathDifference=25.;
    DiffractionUseDistanceAsFilter=1;
    KeepDebugRay=0;
    UsePostFilters=1;

    EnableFullTargets=0;
    TargetsDensity=0.1f;

    InitialAngleTheta=0.;
    InitialAnglePhi=0.;
    AnalyticNbRay=20;
    AnalyticTMax=10.;
    AnalyticH=0.1;
    AnalyticDMax=3000;

    AnalyticTypeTransfo=1;

    NbThreads=4;

    UseMeteo=0;
    OverSampleD=0;
    UseFresnelArea=0;
    Anime3DSigma=0.;
    Anime3DForceC=1;

    DebugUseCloseEventSelector=1;
    DebugUseDiffractionAngleSelector=1;
    DebugUseDiffractionPathSelector=1;
    DebugUseFermatSelector=1;
    DebugUseFaceSelector=1;

}

LPSolverConfiguration SolverConfiguration::get()
{
    if (!_pInstance)
    {
        _pInstance = new SolverConfiguration();
    }
    return _pInstance;
}

SolverConfiguration::~SolverConfiguration()
{
}

}