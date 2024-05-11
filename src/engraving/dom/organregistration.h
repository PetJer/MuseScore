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

#include "textbase.h"

using namespace mu;

namespace mu::engraving {
enum class ManualPedal : char {
    PED,
    I,
    II,
    III,
    IV,
    V,
    VI,
};

enum class NamingConvention : char {
    NUMBER,
    ENGLISH,
    GERMAN,
    FRENCH
};

static constexpr int MANUAL_PEDAL_NO = 7;
// Will be a setting. Leaving it here for now
static const String SEPARATOR = u", ";

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

    QString getOrganName() const;
    std::array<QStringList, MANUAL_PEDAL_NO> getOrganDisposition() const;
    QStringList getManualPedals() const;
    QStringList getOrganCouplers() const;
    QStringList getOrganPistons() const;

    std::array<QVector<bool>, engraving::MANUAL_PEDAL_NO> getArrayStops() const;
    QMap<ManualPedal, QVector<bool>> getStops() const { return m_stops; }
    QVector<bool> getCouplers() const { return m_couplers; }
    QVector<bool> getPistons() const { return m_pistons; }
    String getContext() const { return m_context; }

    void setStops(QMap<ManualPedal, QVector<bool>> stops);
    void setCouplers(QVector<bool> couplers);
    void setPistons(QVector<bool> pistons);

    String createRegistrationText();
    void updateRegistrationText();

    void undoChangeRegistration(QMap<ManualPedal, QVector<bool>> _stops, QVector<bool> _couplers, QVector<bool> _pistons);

private:
    // Organ
    String m_organName;
    QMap<ManualPedal, StringList> m_organDisposition;
    QVector<std::pair<ManualPedal, ManualPedal>> m_organCouplers;
    StringList m_organPistons;

    // Registration
    QMap<ManualPedal, QVector<bool>> m_stops;
    QVector<bool> m_couplers;
    QVector<bool> m_pistons;
    String m_context;
};
} // namespace mu::engraving

#endif // MU_ENGRAVING_ORGANREGISTRATION_H
