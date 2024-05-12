/*
 * SPDX-License-Identifier: GPL-3.0-only
 * MuseScore-Studio-CLA-applies
 *
 * MuseScore Studio
 * Music Composition & Notation
 *
 * Copyright (C) 2022 MuseScore Limited
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "organregistrationpopupmodel.h"
#include "engraving/dom/stafflines.h"

using namespace mu::notation;

OrganRegistrationPopupModel::OrganRegistrationPopupModel(QObject* parent)
    : AbstractElementPopupModel(PopupModelType::TYPE_ORGAN_REGISTRATION, parent)
{
}

void OrganRegistrationPopupModel::init()
{
    AbstractElementPopupModel::init();

    connect(this, &AbstractElementPopupModel::dataChanged, [this]() {
        load();
    });

    load();
}

void OrganRegistrationPopupModel::load()
{
    mu::engraving::OrganRegistration* registration = m_item && m_item->isOrganRegistration() ? toOrganRegistration(m_item) : nullptr;
    if (!registration) {
        return;
    }

    m_organName = registration->getOrganName();
    emit organNameChanged(organName());

    m_organDisposition = registration->getOrganDisposition();
    emit organDispositionChanged(organDisposition());

    m_manualPedals = registration->getManualPedals();
    emit manualPedalsChanged(manualPedals());

    m_organCouplers = registration->getOrganCouplers();
    emit organCouplersChanged(organCouplers());

    m_organPistons = registration->getOrganPistons();
    emit organPistonsChanged(organPistons());


    setPopupStops(registration->getArrayStops());
    emit stopsChanged(stops());

    setPopupCouplers(registration->getCouplers());
    emit couplersChanged(couplers());

    setPopupPistons(registration->getPistons());
    emit pistonsChanged(pistons());

    setPopupContext(registration->getContext());
    emit contextChanged(context());

    return;
}


QMap<mu::engraving::ManualPedal, QVector<bool>> OrganRegistrationPopupModel::getPopupStops()
{
    QMap<mu::engraving::ManualPedal, QVector<bool>> mapStops;
    for (size_t i = 0; i < engraving::MANUAL_PEDAL_NO; i++) {
        if (!m_stops[i].empty()) {
            mapStops[mu::engraving::ManualPedal(i)] = m_stops[i];
        }
    }

    return mapStops;
}

QVector<bool> OrganRegistrationPopupModel::getPopupCouplers() {
    return m_couplers;
}

QVector<bool> OrganRegistrationPopupModel::getPopupPistons() {
    return m_pistons;
}

QString OrganRegistrationPopupModel::getPopupContext() {
    return m_context;
}


void OrganRegistrationPopupModel::setPopupStops(std::array<QVector<bool>, engraving::MANUAL_PEDAL_NO> stops)
{
    m_stops = stops;
}

void OrganRegistrationPopupModel::setPopupCouplers(QVector<bool> couplers)
{
    m_couplers = couplers;
}

void OrganRegistrationPopupModel::setPopupPistons(QVector<bool> pistons)
{
    m_pistons = pistons;
}

void OrganRegistrationPopupModel::setPopupContext(QString context)
{
    m_context = context;
}


QVector<QStringList> OrganRegistrationPopupModel::organDisposition() const
{
    return QVector<QStringList>(m_organDisposition.begin(), m_organDisposition.end());
}

QVector<QVector<bool>> OrganRegistrationPopupModel::stops() const
{
    return QVector<QVector<bool>>(m_stops.begin(), m_stops.end());
}

void OrganRegistrationPopupModel::setStops(QVector<QVector<bool>> stops)
{
    std::array<QVector<bool>, engraving::MANUAL_PEDAL_NO> stdStops;
    for (int i = 0; i < engraving::MANUAL_PEDAL_NO; i++) {
        stdStops[i] = stops.at(i);
    }

    if (stdStops == m_stops) {
        return;
    }

    beginCommand();
    setPopupStops(stdStops);
    toOrganRegistration(m_item)->undoChangeRegistration(getPopupStops(), getPopupCouplers(), getPopupPistons(), getPopupContext());
    updateNotation();
    endCommand();
    emit stopsChanged(stops);
}

void OrganRegistrationPopupModel::setCouplers(QVector<bool> couplers)
{
    if (couplers == m_couplers) {
        return;
    }

    beginCommand();
    setPopupCouplers(couplers);
    toOrganRegistration(m_item)->undoChangeRegistration(getPopupStops(), getPopupCouplers(), getPopupPistons(), getPopupContext());
    updateNotation();
    endCommand();
    emit couplersChanged(couplers);
}

void OrganRegistrationPopupModel::setPistons(QVector<bool> pistons)
{
    if (pistons == m_pistons) {
        return;
    }

    beginCommand();
    setPopupPistons(pistons);
    toOrganRegistration(m_item)->undoChangeRegistration(getPopupStops(), getPopupCouplers(), getPopupPistons(), getPopupContext());
    updateNotation();
    endCommand();
    emit pistonsChanged(pistons);
}

void OrganRegistrationPopupModel::setContext(QString context)
{
    if (context == m_context) {
        return;
    }

    beginCommand();
    setPopupContext(context);
    toOrganRegistration(m_item)->undoChangeRegistration(getPopupStops(), getPopupCouplers(), getPopupPistons(), getPopupContext());
    updateNotation();
    endCommand();
    emit contextChanged(context);
}

QRectF OrganRegistrationPopupModel::staffPos() const
{
    // Just need top & bottom y.  Don't need x pos
    Measure* measure = m_item->findMeasure();
    if (!measure->system()) {
        return QRectF();
    }

    auto organIdxList = m_item->part()->staveIdxList();
    std::list<engraving::StaffLines*> staves;
    for (auto idx : organIdxList) {
        staves.push_back(measure->staffLines(idx));
    }

    if (staves.size() > 0) {
        engraving::StaffLines* topStaff = staves.front();
        engraving::StaffLines* bottomStaff = staves.back();
        muse::RectF staffRect
            = muse::RectF(measure->canvasBoundingRect().x(),
                          topStaff->canvasBoundingRect().y(),
                          measure->canvasBoundingRect().width(),
                          bottomStaff->canvasBoundingRect().bottomLeft().y() - topStaff->canvasBoundingRect().topLeft().y());

        return fromLogical(staffRect).toQRectF();
    }

    return QRectF();
}
