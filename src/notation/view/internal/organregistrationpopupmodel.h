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

    Q_PROPERTY(QString organName READ organName NOTIFY organNameChanged)
    Q_PROPERTY(QVector<QStringList> organDisposition READ organDisposition NOTIFY organDispositionChanged)
    Q_PROPERTY(QStringList manualPedals READ manualPedals NOTIFY manualPedalsChanged)
    Q_PROPERTY(QStringList organCouplers READ organCouplers NOTIFY organCouplersChanged)

    Q_PROPERTY(QVector<QVector<bool>> stops READ stops WRITE setStops NOTIFY stopsChanged)
    Q_PROPERTY(QVector<bool> couplers READ couplers WRITE setCouplers NOTIFY couplersChanged)

    Q_PROPERTY(QRectF staffPos READ staffPos CONSTANT)

public:
    explicit OrganRegistrationPopupModel(QObject* parent = nullptr);

    QRectF staffPos() const;

    QString organName() const { return m_organName; }
    QVector<QStringList> organDisposition() const;
    QStringList manualPedals() const { return m_manualPedals; }
    QStringList organCouplers() const { return m_organCouplers; }

    QVector<QVector<bool>> stops() const;
    QVector<bool> couplers() const { return m_couplers; }

    Q_INVOKABLE void init() override;

public slots:
    void setStops(QVector<QVector<bool>> stops);
    void setCouplers(QVector<bool> couplers);

signals:
    void organNameChanged(QString organName);
    void organDispositionChanged(QVector<QStringList> organDisposition);
    void manualPedalsChanged(QStringList manualPedals);
    void organCouplersChanged(QStringList organCouplers);

    void stopsChanged(QVector<QVector<bool>> stops);
    void couplersChanged(QVector<bool> couplers);

private:
    void load();

    // Organ
    QString m_organName;
    std::array<QStringList, engraving::MANUAL_PEDAL_NO> m_organDisposition;
    QStringList m_manualPedals;
    QStringList m_organCouplers;

    // QStringList m_organPistons;

    // Registration
    void setPopupStops(std::array<QVector<bool>, engraving::MANUAL_PEDAL_NO> stops);
    QMap<mu::engraving::ManualPedal, QVector<bool>> getPopupStops();
    std::array<QVector<bool>, engraving::MANUAL_PEDAL_NO> m_stops;

    void setPopupCouplers(QVector<bool> couplers);
    QVector<bool> getPopupCouplers();
    QVector<bool> m_couplers;

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
