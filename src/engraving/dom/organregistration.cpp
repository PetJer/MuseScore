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
const std::array<String, MANUALS_NO> NAMING_NUMBER = {
    u"I",
    u"II",
    u"III",
    u"IV",
    u"V",
    u"VI",
};

const std::array<String, MANUALS_NO> NAMING_ENGLISH = {
    u"Great",
    u"Swell",
    u"Choir",
    u"Solo",
    u"Echo",
    u"Antiphonal",
};

const std::array<String, MANUALS_NO> NAMING_GERMAN = {
    u"Hauptwerk",
    u"Schwellwerk",
    u"Rückpositiv",
    u"Oberwerk",
    u"Brustwerk",
    u"",
};

const std::array<String, MANUALS_NO> NAMING_FRENCH = {
    u"Grand Orgue",
    u"Positif",
    u"Grand Choeur",
    u"Récit",
    u"Bombarde",
    u"",
};

static const std::array<String, MANUALS_NO> ManualNamingConvention(NamingConvention namingConvention)
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
    std::array<String, MANUALS_NO> manualNamingConvention = ManualNamingConvention(NamingConvention::FRENCH);

    switch(manualPedal) {
    case ManualPedal::VI:
        return manualNamingConvention[5];
    case ManualPedal::V:
        return manualNamingConvention[4];
    case ManualPedal::IV:
        return manualNamingConvention[3];
    case ManualPedal::III:
        return manualNamingConvention[2];
    case ManualPedal::II:
        return manualNamingConvention[1];
    case ManualPedal::I:
        return manualNamingConvention[0];
    case ManualPedal::PED:
        // Should change based on preference for short or long names
        return String(u"Ped.");
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
    m_organDisposition = std::map<ManualPedal, StringList> {
        {ManualPedal::II, {u"Copula major 8", u"Copula minor 4", u"Gozdna fl. 2"}},
        {ManualPedal::I, {u"Principal 8", u"Bordon 8", u"Oktava 4", u"Superoktava 2"}},
        {ManualPedal::PED, {u"Subbas 16", u"Bordon 8"}},
    };
    m_organCouplers = std::vector<std::pair<ManualPedal, ManualPedal>> {
        {ManualPedal::II, ManualPedal::I},
        {ManualPedal::II, ManualPedal::PED},
        {ManualPedal::I, ManualPedal::PED}
    };
    m_organPistons = StringList {
        u"Tutti", u"Forte", u"Piano"
    };

    // For testing
    m_stops = m_organDisposition;
    m_couplers = m_organCouplers;
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

    for (const auto &s : m_stops) {
        stops.append(ManualPedalToString(s.first) + u": " + s.second.join(SEPARATOR));
    }

    if (!stops.empty()) {
        registration.append(stops.join(u"\n"));
    }

    for (size_t i = 0; i < m_couplers.size(); i++) {
        couplers.append(
            ManualPedalToString(m_couplers[i].first) + u"/"
            + ManualPedalToString(m_couplers[i].second)
        );
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

/*
 * ADD UNDO REGISTRATION FUNCTION
*/

PropertyValue OrganRegistration::getProperty(Pid propertyId) const
{
    switch (propertyId) {
    case Pid::TEXT_STYLE:
        return TextStyleType::ORGAN_REGISTRATION;
    default:
        return TextBase::getProperty(propertyId);
    }
}

bool OrganRegistration::setProperty(Pid propertyId, const PropertyValue& v)
{
    setTextStyleType(TextStyleType::ORGAN_REGISTRATION);

    if (!TextBase::setProperty(propertyId, v)) {
        return false;
    }

    return true;
}

PropertyValue OrganRegistration::propertyDefault(Pid id) const
{
    switch (id) {
    case Pid::TEXT_STYLE:
        return TextStyleType::ORGAN_REGISTRATION;
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
