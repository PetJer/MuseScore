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

#ifndef MU_NOTATION_ORGANREGISTRATIONPOPUPMODEL_H
#define MU_NOTATION_ORGANREGISTRATIONPOPUPMODEL_H

#include <QObject>

#include "context/iglobalcontext.h"

#include "engraving/dom/organregistration.h"
#include "engraving/dom/undo.h"

#include "view/abstractelementpopupmodel.h"

namespace mu::notation {
static constexpr int ORGAN_STRING_NO = 7;

class OrganRegistrationPopupModel : public AbstractElementPopupModel
{
    Q_OBJECT

    INJECT(context::IGlobalContext, globalContext)

    Q_PROPERTY(
        QVector<mu::notation::OrganRegistrationPopupModel::Position> pedalState READ pedalState WRITE setDiagramPedalState NOTIFY pedalStateChanged)
    Q_PROPERTY(QRectF staffPos READ staffPos CONSTANT)

public:
    enum class Position {
        FLAT,
        NATURAL,
        SHARP,

        UNSET
    };
    Q_ENUM(Position)

    explicit OrganRegistrationPopupModel(QObject* parent = nullptr);

    QRectF staffPos() const;

    QVector<Position> pedalState() const;

    Q_INVOKABLE void init() override;

public slots:
    void setDiagramPedalState(QVector<mu::notation::OrganRegistrationPopupModel::Position> pedalState);

signals:
    void pedalStateChanged(QVector<mu::notation::OrganRegistrationPopupModel::Position> pedalState);

private:
    void load();

    // Convert between mu::engraving::PedalPosition and internal qml safe Position enums
    void setPopupPedalState(std::array<mu::engraving::PedalPosition, ORGAN_STRING_NO> pos);

    void setPopupPedalState(std::array<OrganRegistrationPopupModel::Position, ORGAN_STRING_NO> pos);

    std::array<mu::engraving::PedalPosition, ORGAN_STRING_NO> getPopupPedalState();

    std::array<Position, ORGAN_STRING_NO> m_pedalState;
};
} //namespace mu::notation

#ifndef NO_QT_SUPPORT
Q_DECLARE_METATYPE(mu::notation::OrganRegistrationPopupModel::Position)
#endif

#endif // MU_NOTATION_ORGANREGISTRATIONPOPUPMODEL_H
