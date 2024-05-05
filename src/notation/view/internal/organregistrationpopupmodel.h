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

#include "view/abstractelementpopupmodel.h"

namespace mu::notation {
class OrganRegistrationPopupModel : public AbstractElementPopupModel
{
    Q_OBJECT

    INJECT(context::IGlobalContext, globalContext)

    Q_PROPERTY(
        QVector<QStringList> stops READ stops WRITE setStops NOTIFY stopsChanged)
    Q_PROPERTY(QRectF staffPos READ staffPos CONSTANT)

public:
    explicit OrganRegistrationPopupModel(QObject* parent = nullptr);

    QRectF staffPos() const;

    QVector<QStringList> stops() const;

    Q_INVOKABLE void init() override;

public slots:
    void setStops(QVector<QStringList> stops);

signals:
    void stopsChanged(QVector<QStringList> stops);

private:
    void load();

    void setPopupStops(std::array<QStringList, 3> stops);
    std::array<QStringList, 3> m_stops;

};
} //namespace mu::notation

/*
// Organ
String m_organName;
QMap<ManualPedal, StringList> m_organDisposition;
std::vector<std::pair<ManualPedal, ManualPedal>> m_organCouplers;
StringList m_organPistons;

// Registration
QMap<ManualPedal, StringList> m_stops;
std::vector<std::pair<ManualPedal, ManualPedal>> m_couplers;
StringList m_pistons;
String m_context;
*/

#endif // MU_NOTATION_ORGANREGISTRATIONPOPUPMODEL_H
