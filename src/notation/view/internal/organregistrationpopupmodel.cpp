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

    setPopupOrganName(registration->getOrganName());
    emit organNameChanged(organName());

    setPopupOrganDisposition(registration->getOrganDisposition());
    emit organDispositionChanged(organDisposition());

    setPopupStops(registration->getStops());
    emit stopsChanged(stops());
    return;
}


void OrganRegistrationPopupModel::setPopupOrganName(std::string organName)
{
    m_organName = organName;
}

void OrganRegistrationPopupModel::setPopupOrganDisposition(std::array<QStringList, engraving::MANUAL_PEDAL_NO> organDisposition)
{
    m_organDisposition = organDisposition;
}

void OrganRegistrationPopupModel::setPopupStops(std::array<QVector<bool>, engraving::MANUAL_PEDAL_NO> stops)
{
    m_stops = stops;
}


QString OrganRegistrationPopupModel::organName() const
{
    return QString::fromStdString(m_organName);
}

QVector<QStringList> OrganRegistrationPopupModel::organDisposition() const
{
    return QVector<QStringList>(m_organDisposition.begin(), m_organDisposition.end());
}

QVector<QVector<bool>> OrganRegistrationPopupModel::stops() const
{
    return QVector<QVector<bool>>(m_stops.begin(), m_stops.end());
}


void OrganRegistrationPopupModel::setOrganName(QString organName)
{
    std::string stdOrganName;
    stdOrganName = organName.toStdString();

    if (stdOrganName == m_organName) {
        return;
    }

    beginCommand();
    setPopupOrganName(stdOrganName);
    // toOrganRegistration(m_item)->undoChangeStops(getPopupStops());
    updateNotation();
    endCommand();
    emit organNameChanged(organName);
}

void OrganRegistrationPopupModel::setOrganDisposition(QVector<QStringList> organDisposition)
{
    std::array<QStringList, engraving::MANUAL_PEDAL_NO> stdOrganDisposition;
    for (int i = 0; i < engraving::MANUAL_PEDAL_NO; i++) {
        stdOrganDisposition[i] = organDisposition.at(i);
    }

    if (stdOrganDisposition == m_organDisposition) {
        return;
    }

    beginCommand();
    setPopupOrganDisposition(stdOrganDisposition);
    updateNotation();
    endCommand();
    emit organDispositionChanged(organDisposition);
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
    updateNotation();
    endCommand();
    emit stopsChanged(stops);
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
