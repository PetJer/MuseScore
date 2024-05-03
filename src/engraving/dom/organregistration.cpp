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

// #include "organorgandiagram.h"
#include "organregistration.h"

// #include "translation.h"
#include "types/typesconv.h"

#include "part.h"
#include "score.h"
#include "segment.h"
// #include "undo.h"

// #include "log.h"

using namespace mu;
using namespace mu::engraving;

// STYLE
static const ElementStyle organRegistrationStyle {
                                                 { Sid::organRegistrationPlacement, Pid::PLACEMENT },
                                                 { Sid::organRegistrationMinDistance, Pid::MIN_DISTANCE },
                                                 };

OrganRegistration::OrganRegistration(Segment* parent)
    : TextBase(ElementType::ORGAN_REGISTRATION, parent, TextStyleType::ORGAN_REGISTRATION, ElementFlag::MOVABLE | ElementFlag::ON_STAFF)
{
    initElementStyle(&organRegistrationStyle);
    m_registrationState = std::array<String, REGISTER_NO> { u"Register1", u"Register1", u"Register1" };
}

OrganRegistration::OrganRegistration(const OrganRegistration& r)
    : TextBase(r)
{
    m_registrationState = r.m_registrationState;
}

void OrganRegistration::setRegistrationState(std::array<String, REGISTER_NO> state)
{
    m_registrationState = state;
}

// void OrganRegistration::setOrganRegistration()
// {
//     const ElementStyle& styleMap = organRegistrationStyle;

//     setTextStyleType(TextStyleType::ORGAN_REGISTRATION);

//     initElementStyle(&styleMap);
// }

// void OrganRegistration::setRegistration(int idx, String registration)
// {
//     m_registrationState.at(idx) = registration;
// }

String OrganRegistration::createRegistrationText()
{
    String registration  = u"Organ registration";

    // for (size_t idx = 0; idx < m_registrationState.size(); idx++) {
    //     registration.append(u"<sym>harpPedalRaised</sym>");
    //     registration.append(u"<sym>harpPedalDivider</sym>");
    // }

    return registration;
}

void OrganRegistration::updateRegistrationText()
{
    undoChangeProperty(Pid::TEXT, createRegistrationText(), PropertyFlags::STYLED);
}

// void OrganRegistration::undoChangeOrganState(std::array<OrganPosition, ORGAN_STRING_NO> _organState)
// {
//     const std::list<EngravingObject*> links = linkList();
//     for (EngravingObject* obj : links) {
//         if (!obj || !obj->isOrganRegistration()) {
//             continue;
//         }

//         OrganRegistration* item = toOrganRegistration(obj);
//         Score* linkedScore = item->score();
//         if (!linkedScore) {
//             continue;
//         }

//         linkedScore->undo(new ChangeOrganOrganState(item, _organState));
//     }
// }

PropertyValue OrganRegistration::getProperty(Pid propertyId) const
{
    // return true;

    return TextBase::getProperty(propertyId);
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
    // return true;
    // switch (id) {
    // case Pid::TEXT_STYLE:
    //     return TextStyleType::ORGAN_REGISTRATION;
    // default:
    //     return TextBase::propertyDefault(id);
    // }
    return TextBase::propertyDefault(id);
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
