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

#ifndef MU_ENGRAVING_ORGANREGISTRATION_H
#define MU_ENGRAVING_ORGANREGISTRATION_H

// #include "pitchspelling.h"
#include "textbase.h"

using namespace mu;

namespace mu::engraving {
// enum class OrganPosition : char {
//     FLAT,
//     NATURAL,
//     SHARP,

    //     UNSET                   // Only used in setDiagramText to represent the beginning of a score
    // };

    // // Use for indexes of _organState
    // enum OrganStringType : char {
    //     H, I, J, K, L, M, N
    // };

    // Not logical but cool for now
    static constexpr int REGISTER_NO = 3;

class OrganRegistration final : public TextBase
{
    OBJECT_ALLOCATOR(engraving, OrganRegistration)
    DECLARE_CLASSOF(ElementType::ORGAN_REGISTRATION)

public:
    OrganRegistration(Segment* parent);
    OrganRegistration(const OrganRegistration& h);

    OrganRegistration* clone() const override { return new OrganRegistration(*this); }
    bool isEditable() const override { return false; }

    Segment* segment() const { return (Segment*)explicitParent(); }
    Measure* measure() const { return (Measure*)explicitParent()->explicitParent(); }

    PropertyValue getProperty(Pid propertyId) const override;
    bool setProperty(Pid propertyId, const PropertyValue& v) override;
    PropertyValue propertyDefault(Pid id) const override;

    String accessibleInfo() const override;
    String screenReaderInfo() const override;

    void setOrganRegistration(); // organregistration.cpp

    // std::array<OrganPosition, ORGAN_STRING_NO> getOrganState() const { return m_organState; } // organregistration.cpp
    // void setOrganState(OrganPosition, ORGAN_STRING_NO> state); // organregistration.cpp
    std::array<String, REGISTER_NO> getRegistrationState() const { return m_registrationState; }
    void setRegistrationState(std::array<String, REGISTER_NO> state);

    // void setOrgan(OrganStringType harpString, OrganPosition organ); // organregistration.cpp
    void setRegistration(int idx, String registration); // organregistration.cpp

    String createRegistrationText(); // organregistration.cpp
    void updateRegistrationText(); // for now - tlayout.cpp and singlelayout.cpp

    // void undoChangeOrganState(std::array<OrganPosition, ORGAN_STRING_NO> _organState); // organregistration.cpp
    void undoChangeRegistrationState(std::array<String, REGISTER_NO> _registrationState);

private:
    // std::array<OrganPosition, ORGAN_STRING_NO> m_organState; // organregistration.cpp
    std::array<String, REGISTER_NO> m_registrationState;
};
} // namespace mu::engraving


#endif // MU_ENGRAVING_ORGANREGISTRATION_H
