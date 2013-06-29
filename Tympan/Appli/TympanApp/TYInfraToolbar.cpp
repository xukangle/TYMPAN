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
 
/**
 * \file TYInfraToolbar.cpp
 * \brief Barre d'outil infastructure
 *
 *
 */




#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHTympanApp.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER

#include <qtoolbutton.h>
#include <qbuttongroup.h>

#include "Tympan/Tools/OLocalizator.h"
#include "Tympan/MetierSolver/DataManagerCore/TYPreferenceManager.h"


#define TR(id) OLocalizator::getString("TYInfraToolbar", (id))
#define IMG(id) OLocalizator::getPicture("TYInfraToolbar", (id))


TYInfraToolbar::TYInfraToolbar(QButtonGroup* pBtnGroup, QMainWindow* parent /*= 0*/, QString title /*= ""*/):
    QToolBar(title, parent)
{
    Q_CHECK_PTR(pBtnGroup);

    _toolButtonSrcPonct = new QToolButton(this);
    _toolButtonSrcPonct->setIcon(QPixmap(IMG("id_icon_srcponct")));
    _toolButtonSrcPonct->setText(TR("id_menuitem_srcponct"));
    _toolButtonSrcPonct->setToolTip(TR("id_menuitem_srcponct"));
    _toolButtonSrcPonct->setStatusTip(TR("id_menuitem_srcponct"));
    _toolButtonSrcPonct->setCheckable(true);
    addWidget(_toolButtonSrcPonct);
    pBtnGroup->addButton(_toolButtonSrcPonct, TYSiteModelerFrame::SrcPonctMode);

    addSeparator();

    _toolButtonRoute = new QToolButton(this);
    _toolButtonRoute->setIcon(QPixmap(IMG("id_icon_route")));
    _toolButtonRoute->setText(TR("id_menuitem_route"));
    _toolButtonRoute->setToolTip(TR("id_menuitem_route"));
    _toolButtonRoute->setStatusTip(TR("id_menuitem_route"));
    _toolButtonRoute->setCheckable(true);
    addWidget(_toolButtonRoute);
    pBtnGroup->addButton(_toolButtonRoute, TYSiteModelerFrame::RouteMode);

    //Ajout du nouveau bouton pour les route en deblai/remblai
    _toolButtonRouteDeblai = new QToolButton(this);
    _toolButtonRouteDeblai->setIcon(QPixmap(IMG("id_icon_route_deblai")));
    _toolButtonRouteDeblai->setText(TR("id_menuitem_route_deblai"));
    _toolButtonRouteDeblai->setToolTip(TR("id_menuitem_route_deblai"));
    _toolButtonRouteDeblai->setStatusTip(TR("id_menuitem_route_deblai"));
    _toolButtonRouteDeblai->setCheckable(true);
    addWidget(_toolButtonRouteDeblai);
    pBtnGroup->addButton(_toolButtonRouteDeblai, TYSiteModelerFrame::RouteDeblaiMode);

    _toolButtonReseau = new QToolButton(this);
    _toolButtonReseau->setIcon(QPixmap(IMG("id_icon_reseau")));
    _toolButtonReseau->setText(TR("id_menuitem_reseau"));
    _toolButtonReseau->setToolTip(TR("id_menuitem_reseau"));
    _toolButtonReseau->setStatusTip(TR("id_menuitem_reseau"));
    _toolButtonReseau->setCheckable(true);
    addWidget(_toolButtonReseau);
    pBtnGroup->addButton(_toolButtonReseau, TYSiteModelerFrame::ReseauTransportMode);

    _toolButtonEtage = new QToolButton(this);
    _toolButtonEtage->setIcon(QPixmap(IMG("id_icon_etage")));
    _toolButtonEtage->setText(TR("id_menuitem_etage"));
    _toolButtonEtage->setToolTip(TR("id_menuitem_etage"));
    _toolButtonEtage->setStatusTip(TR("id_menuitem_etage"));
    _toolButtonEtage->setCheckable(true);
    addWidget(_toolButtonEtage);
    pBtnGroup->addButton(_toolButtonEtage, TYSiteModelerFrame::EtageMode);

    _toolButtonSilos = new QToolButton(this);
    _toolButtonSilos->setIcon(QPixmap(IMG("id_icon_silos")));
    _toolButtonSilos->setText(TR("id_menuitem_silos"));
    _toolButtonSilos->setToolTip(TR("id_menuitem_silos"));
    _toolButtonSilos->setStatusTip(TR("id_menuitem_silos"));
    _toolButtonSilos->setCheckable(true);
    addWidget(_toolButtonSilos);
    pBtnGroup->addButton(_toolButtonSilos, TYSiteModelerFrame::SilosMode);

    addSeparator();

    /*_toolButtonMachine = new QToolButton(this);
    _toolButtonMachine->setIcon(QPixmap( IMG("id_icon_machine") ));
    _toolButtonMachine->setText(TR("id_menuitem_machine"));
    _toolButtonMachine->setToolTip(TR("id_menuitem_machine"));
    _toolButtonMachine->setStatusTip(TR("id_menuitem_machine"));
    _toolButtonMachine->setCheckable(true);
    pBtnGroup->addButton(_toolButtonMachine, TYSiteModelerFrame::MachineMode);
    addWidget(_toolButtonMachine);
    _toolButtonMachine->setEnabled(false);

    _toolButtonBatiment = new QToolButton(this);
    _toolButtonBatiment->setIcon(QPixmap( IMG("id_icon_batiment") ));
    _toolButtonBatiment->setText(TR("id_menuitem_batiment"));
    _toolButtonBatiment->setToolTip(TR("id_menuitem_batiment"));
    _toolButtonBatiment->setStatusTip(TR("id_menuitem_batiment"));
    _toolButtonBatiment->setCheckable(true);
    pBtnGroup->addButton(_toolButtonBatiment, TYSiteModelerFrame::BatimentMode);
    addWidget(_toolButtonBatiment);
    _toolButtonBatiment->setEnabled(false);

    _toolButtonEcran = new QToolButton(this);
    _toolButtonEcran->setIcon(QPixmap( IMG("id_icon_ecran") ));
    _toolButtonEcran->setText(TR("id_menuitem_ecran"));
    _toolButtonEcran->setToolTip(TR("id_menuitem_ecran"));
    _toolButtonEcran->setStatusTip(TR("id_menuitem_ecran"));
    _toolButtonEcran->setCheckable(true);
    pBtnGroup->addButton(_toolButtonEcran, TYSiteModelerFrame::EcranMode);
    addWidget(_toolButtonEcran);
    _toolButtonEcran->setEnabled(false);

    _toolButtonSite = new QToolButton(this);
    _toolButtonSite->setIcon(QPixmap( IMG("id_icon_site") ));
    _toolButtonSite->setText(TR("id_menuitem_site"));
    _toolButtonSite->setToolTip(TR("id_menuitem_site"));
    _toolButtonSite->setStatusTip(TR("id_menuitem_site"));
    _toolButtonSite->setCheckable(true);
    pBtnGroup->addButton(_toolButtonSite, TYSiteModelerFrame::SiteMode);
    addWidget(_toolButtonSite);
    _toolButtonSite->setEnabled(false);*/
}

TYInfraToolbar::~TYInfraToolbar()
{
}

void TYInfraToolbar::activeButtons(bool active)
{
    _toolButtonSrcPonct->setEnabled(active);
    _toolButtonRoute->setEnabled(active);
    _toolButtonRouteDeblai->setEnabled(active);
    _toolButtonReseau->setEnabled(active);
    _toolButtonEtage->setEnabled(active);
    _toolButtonSilos->setEnabled(active);

    // Desactivation de tout acces a la base de donnees - DT 07/03/2005 -
    // Les boutons suivants ont ete desactives
    /*_toolButtonMachine->setEnabled(false);
    _toolButtonBatiment->setEnabled(false);
    _toolButtonEcran->setEnabled(false);
    _toolButtonSite->setEnabled(false);*/
}