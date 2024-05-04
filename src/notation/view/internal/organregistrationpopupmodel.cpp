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
    // setPopupPedalState(diagram->getPedalState());
    // emit pedalStateChanged(pedalState());
    return;
}

std::array<mu::engraving::PedalPosition, ORGAN_STRING_NO> OrganRegistrationPopupModel::getPopupPedalState()
{
    std::array<mu::engraving::PedalPosition, ORGAN_STRING_NO> posArr;
    for (int i = 0; i < ORGAN_STRING_NO; i++) {
        switch (m_pedalState.at(i)) {
        case OrganRegistrationPopupModel::Position::FLAT:
            posArr[i] = mu::engraving::PedalPosition::FLAT;
            break;
        case OrganRegistrationPopupModel::Position::NATURAL:
            posArr[i] = mu::engraving::PedalPosition::NATURAL;
            break;
        case OrganRegistrationPopupModel::Position::SHARP:
            posArr[i] = mu::engraving::PedalPosition::SHARP;
            break;
        case OrganRegistrationPopupModel::Position::UNSET:
            posArr[i] = mu::engraving::PedalPosition::UNSET;
            break;
        default:
            posArr[i] = mu::engraving::PedalPosition::UNSET;
            break;
        }
    }

    return posArr;
}

void OrganRegistrationPopupModel::setPopupPedalState(std::array<OrganRegistrationPopupModel::Position, ORGAN_STRING_NO> pos)
{
    m_pedalState = pos;
}

void OrganRegistrationPopupModel::setPopupPedalState(std::array<mu::engraving::PedalPosition, ORGAN_STRING_NO> pos)
{
    std::array<OrganRegistrationPopupModel::Position, ORGAN_STRING_NO> posArr;
    for (int i = 0; i < ORGAN_STRING_NO; i++) {
        switch (pos.at(i)) {
        case mu::engraving::PedalPosition::FLAT:
            posArr[i] = OrganRegistrationPopupModel::Position::FLAT;
            break;
        case mu::engraving::PedalPosition::NATURAL:
            posArr[i] = OrganRegistrationPopupModel::Position::NATURAL;
            break;
        case mu::engraving::PedalPosition::SHARP:
            posArr[i] = OrganRegistrationPopupModel::Position::SHARP;
            break;
        case mu::engraving::PedalPosition::UNSET:
            posArr[i] = OrganRegistrationPopupModel::Position::UNSET;
            break;
        default:
            posArr[i] = OrganRegistrationPopupModel::Position::UNSET;
            break;
        }
    }

    m_pedalState = posArr;
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

QVector<OrganRegistrationPopupModel::Position> OrganRegistrationPopupModel::pedalState() const
{
    return QVector<OrganRegistrationPopupModel::Position>(m_pedalState.begin(), m_pedalState.end());
}

void OrganRegistrationPopupModel::setDiagramPedalState(QVector<Position> pedalState)
{
    std::array<OrganRegistrationPopupModel::Position, ORGAN_STRING_NO> stdPedalState;
    for (int i = 0; i < ORGAN_STRING_NO; i++) {
        stdPedalState[i] = pedalState.at(i);
    }

    if (stdPedalState == m_pedalState) {
        return;
    }

    beginCommand();
    setPopupPedalState(stdPedalState);
    // engraving::toOrganRegistration(m_item)->undoChangePedalState(getPopupPedalState());
    updateNotation();
    endCommand();
    emit pedalStateChanged(pedalState);
}
