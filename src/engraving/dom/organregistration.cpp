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

#include "organregistration.h"

#include "types/typesconv.h"

#include "part.h"
#include "score.h"
#include "segment.h"

#include "undo.h"

using namespace mu;
using namespace mu::engraving;

/*
 * Settings for the future:
 * - Manual naming convention
 * - Short or long names
 * - Colon (other symbol?) after manual/pedal name
 * - items separators
 * - Show organ name / Label (invisible by default but visible if no registration is selected)
 * - show differences (will only show additions(+) and subtractions(-) of stops)
 * - Affect playback (a fun one for the future...)
 */

/*
 * Because I lack the knowledge of naming conventions around the world this may be wrong. Some questions for later me:
 * - namings do change just based on the number of manuals (or the organ makers design!)?
 * - are the names below even correct (from IV to VI)?
 * - Do pedal names change too?
 */
const std::array<String, MANUAL_PEDAL_NO> NAMING_NUMBER = {
    u"Pedal",
    u"I",
    u"II",
    u"III",
    u"IV",
    u"V",
    u"VI",
};

const std::array<String, MANUAL_PEDAL_NO> NAMING_ENGLISH = {
    u"Pedal",
    u"Great",
    u"Swell",
    u"Choir",
    u"Solo",
    u"Echo",
    u"Antiphonal",
};

const std::array<String, MANUAL_PEDAL_NO> NAMING_GERMAN = {
    u"Pedal",
    u"Hauptwerk",
    u"Schwellwerk",
    u"Rückpositiv",
    u"Oberwerk",
    u"Brustwerk",
    u"",
};

const std::array<String, MANUAL_PEDAL_NO> NAMING_FRENCH = {
    u"Pedal",
    u"Grand Orgue",
    u"Positif",
    u"Grand Choeur",
    u"Récit",
    u"Bombarde",
    u"",
};

static const std::array<String, MANUAL_PEDAL_NO> ManualNamingConvention(NamingConvention namingConvention)
{
    switch(namingConvention) {
    case NamingConvention::NUMBER:
        return NAMING_NUMBER;
    case NamingConvention::ENGLISH:
        return NAMING_ENGLISH;
    case NamingConvention::GERMAN:
        return NAMING_GERMAN;
    case NamingConvention::FRENCH:
        return NAMING_FRENCH;
    default:
        return NAMING_NUMBER;
    }
}

static const String ManualPedalToString(ManualPedal manualPedal)
{
    // There will be a setting as a parameter. Hardcoded for now
    std::array<String, MANUAL_PEDAL_NO> manualNamingConvention = ManualNamingConvention(NamingConvention::NUMBER);

    switch(manualPedal) {
    case ManualPedal::PED:
        return manualNamingConvention[0];
    case ManualPedal::I:
        return manualNamingConvention[1];
    case ManualPedal::II:
        return manualNamingConvention[2];
    case ManualPedal::III:
        return manualNamingConvention[3];
    case ManualPedal::IV:
        return manualNamingConvention[4];
    case ManualPedal::V:
        return manualNamingConvention[5];
    case ManualPedal::VI:
        return manualNamingConvention[6];
    }

    return String();
}

// Add maximum width
static const ElementStyle organRegistrationStyle {
    { Sid::organRegistrationPlacement, Pid::PLACEMENT },
    { Sid::organRegistrationMinDistance, Pid::MIN_DISTANCE },
    { Sid::organRegistrationFontSize, Pid::FONT_SIZE },
    { Sid::organRegistrationLineSpacing, Pid::TEXT_LINE_SPACING },
    { Sid::organRegistrationAlign, Pid::ALIGN },
    { Sid::organRegistrationFrameType, Pid::FRAME_TYPE },
    { Sid::organRegistrationFramePadding, Pid::FRAME_PADDING },
};

OrganRegistration::OrganRegistration(Segment* parent)
    : TextBase(ElementType::ORGAN_REGISTRATION, parent, TextStyleType::ORGAN_REGISTRATION, ElementFlag::MOVABLE | ElementFlag::ON_STAFF)
{
    // Manually creating our own organ as there is no creation process yet
    m_organName = "My Organ";
    m_organDisposition = QMap<ManualPedal, StringList> {
        {ManualPedal::PED, {u"Subbas 16", u"Bordon 8"}},
        {ManualPedal::I, {u"Principal 8", u"Bordon 8", u"Oktava 4", u"Superoktava 2"}},
        {ManualPedal::II, {u"Copula major 8", u"Copula minor 4", u"Gozdna fl. 2"}},
    };
    m_organCouplers = QVector<std::pair<ManualPedal, ManualPedal>> {
        {ManualPedal::II, ManualPedal::I},
        {ManualPedal::II, ManualPedal::PED},
        {ManualPedal::I, ManualPedal::PED}
    };
    m_organPistons = StringList {
        u"Tutti", u"Forte", u"Piano"
    };

    // For testing
    m_stops = QMap<ManualPedal, QVector<bool>> {
        {ManualPedal::PED, {false, true}},
        {ManualPedal::I, {true, false, true, false}},
        {ManualPedal::II, {true, false, true}},
    };

    m_couplers.fill(false, m_organCouplers.size());
    m_couplers[0] = true; // testing
    m_pistons = m_organPistons;
    m_context = "Open Swell";

    initElementStyle(&organRegistrationStyle);
}

OrganRegistration::OrganRegistration(const OrganRegistration& r)
    : TextBase(r)
{
    m_organName = r.m_organName;
    m_organDisposition = r.m_organDisposition;
    m_organCouplers = r.m_organCouplers;
    m_organPistons = r.m_organPistons;

    m_stops = r.m_stops;
    m_couplers = r.m_couplers;
    m_pistons = r.m_pistons;
    m_context = r.m_context;
}

String OrganRegistration::createRegistrationText()
{
    StringList registration, stops, couplers;

    // Show organ name (setting)
    if (false) {
        registration.append(m_organName);
    }

    for (auto mp = m_stops.cbegin(); mp != m_stops.cend(); mp++) {
        StringList _stops;
        for (int i = 0; i < m_stops[mp.key()].size(); i++) {
            if (m_stops[mp.key()][i] == true) {
                _stops.append(m_organDisposition[mp.key()][i]);
            }
        }
        if (!_stops.empty()) {
            stops.append(ManualPedalToString(mp.key()) + u": " + _stops.join(SEPARATOR));
        }
    }

    if (!stops.empty()) {
        registration.append(stops.join(u"\n"));
    }

    for (int i = 0; i < m_organCouplers.size(); i++) {
        if (m_couplers[i]) {
            couplers.append(
                ManualPedalToString(m_organCouplers[i].first) + u"/"
                + ManualPedalToString(m_organCouplers[i].second)
            );
        }
    }

    if (!couplers.empty()) {
        registration.append(couplers.join(SEPARATOR));
    }

    if (!m_pistons.empty()) {
        registration.append(m_pistons.join(SEPARATOR));
    }

    if (!m_context.empty()) {
        registration.append(m_context);
    }

    // If no registration is select it defaults to organ name (translatable string)
    return registration.empty() ? String(m_organName + u" Registration") : registration.join(u"\n");
}

void OrganRegistration::updateRegistrationText()
{
    undoChangeProperty(Pid::TEXT, createRegistrationText(), PropertyFlags::STYLED);
}

void OrganRegistration::undoChangeRegistration(QMap<ManualPedal, QVector<bool>> _stops, QVector<bool> _couplers)
{
    const std::list<EngravingObject*> links = linkList();
    for (EngravingObject* obj : links) {
        if (!obj || !obj->isOrganRegistration()) {
            continue;
        }

        OrganRegistration* item = toOrganRegistration(obj);
        Score* linkedScore = item->score();
        if (!linkedScore) {
            continue;
        }

        linkedScore->undo(new ChangeOrganRegistration(item, _stops, _couplers));
    }
}


void OrganRegistration::setStops(QMap<ManualPedal, QVector<bool>> stops)
{
    m_stops = stops;
}

void OrganRegistration::setCouplers(QVector<bool> couplers)
{
    m_couplers = couplers;
}


QString OrganRegistration::getOrganName() const
{
    return m_organName.toQString();
}

std::array<QStringList, MANUAL_PEDAL_NO> OrganRegistration::getOrganDisposition() const
{
    std::array<QStringList, MANUAL_PEDAL_NO> disposition;
    for (auto s = m_organDisposition.cbegin(); s != m_organDisposition.cend(); ++s) {
        disposition[static_cast<int>(s.key())] = s.value().toQStringList();
    }

    return disposition;
}

QStringList OrganRegistration::getManualPedals() const
{
    QStringList manualPedals;
    for (auto const mp : m_organDisposition.keys()) {
        manualPedals.append(ManualPedalToString(mp));
    }

    return manualPedals;
}

QStringList OrganRegistration::getOrganCouplers() const
{
    QStringList organCouplers;
    for (auto const oc : m_organCouplers) {
        organCouplers.append(
            ManualPedalToString(oc.first) + u"/"
            + ManualPedalToString(oc.second)
        );
    }

    return organCouplers;
}


std::array<QVector<bool>, engraving::MANUAL_PEDAL_NO> OrganRegistration::getArrayStops() const
{
    std::array<QVector<bool>, engraving::MANUAL_PEDAL_NO> stops;
    for (auto s = m_stops.cbegin(); s != m_stops.cend(); ++s) {
        stops[static_cast<int>(s.key())] = s.value();
    }

    return stops;
}

// Surely you can get this values from styledef... Hardcoded for now
PropertyValue OrganRegistration::getProperty(Pid propertyId) const
{
    switch (propertyId) {
    case Pid::TEXT_STYLE:
        return TextStyleType::ORGAN_REGISTRATION;
    case Pid::FONT_SIZE:
        return 8.0;
    case Pid::TEXT_LINE_SPACING:
        return 0.8;
    case Pid::ALIGN:
        return Align(AlignH::LEFT, AlignV::VCENTER);
    case Pid::FRAME_TYPE:
        return int(FrameType::SQUARE);
    case Pid::FRAME_PADDING:
        return 1.0;
    default:
        return TextBase::propertyDefault(propertyId);
    }
}

bool OrganRegistration::setProperty(Pid propertyId, const PropertyValue& v)
{
    switch (propertyId) {
    case Pid::TEXT_STYLE:
        setTextStyleType(TextStyleType::ORGAN_REGISTRATION);
        break;
    case Pid::FONT_SIZE:
        setSize(8.0);
        break;
    case Pid::TEXT_LINE_SPACING:
        setTextLineSpacing(0.8);
        break;
    case Pid::ALIGN:
        setAlign(Align(AlignH::LEFT, AlignV::VCENTER));
        break;
    case Pid::FRAME_TYPE:
        setFrameType(FrameType::SQUARE);
        break;
    case Pid::FRAME_PADDING:
        setPaddingWidth(Spatium(1.0));
        break;
    default:
        if (!TextBase::setProperty(propertyId, v)) {
            return false;
        }
    }

    return true;
}

PropertyValue OrganRegistration::propertyDefault(Pid id) const
{
    switch (id) {
    case Pid::TEXT_STYLE:
        return TextStyleType::ORGAN_REGISTRATION;
    case Pid::FONT_SIZE:
        return 8.0;
    case Pid::TEXT_LINE_SPACING:
        return 0.8;
    case Pid::ALIGN:
        return Align(AlignH::LEFT, AlignV::VCENTER);
    case Pid::FRAME_TYPE:
        return int(FrameType::SQUARE);
    case Pid::FRAME_PADDING:
        return 1.0;
    default:
        return TextBase::propertyDefault(id);
    }
}

String OrganRegistration::accessibleInfo() const
{
    String rez = score() ? score()->getTextStyleUserName(textStyleType()).translated() : TConv::translatedUserName(textStyleType());
    String s;

    s.append(u"ACCESSIBLE ORGAN REGISTRATION");

    return String(u"%1: %2").arg(rez, s);
}

String OrganRegistration::screenReaderInfo() const
{
    String rez = score() ? score()->getTextStyleUserName(textStyleType()).translated() : TConv::translatedUserName(textStyleType());
    String s;

    s.append(u"SCREEN READER ORGAN REGISTRATION");

    return String(u"%1: %2").arg(rez, s);
}
