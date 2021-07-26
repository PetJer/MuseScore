/*
 * SPDX-License-Identifier: GPL-3.0-only
 * MuseScore-CLA-applies
 *
 * MuseScore
 * Music Composition & Notation
 *
 * Copyright (C) 2021 MuseScore BVBA and others
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
#ifndef MU_PALETTE_EDITDRUMSETDIALOG_H
#define MU_PALETTE_EDITDRUMSETDIALOG_H

#include "ui_editdrumsetdialog.h"

#include "modularity/ioc.h"
#include "global/iinteractive.h"
#include "context/iglobalcontext.h"
#include "notation/inotationconfiguration.h"

#include "libmscore/drumset.h"

namespace Ms {
//---------------------------------------------------------
//   EditDrumsetDialog
//---------------------------------------------------------

class EditDrumsetDialog : public QDialog, private Ui::EditDrumsetDialog
{
    Q_OBJECT

    INJECT(Ms, mu::framework::IInteractive, interactive)
    INJECT(Ms, mu::context::IGlobalContext, globalContext)
    INJECT(Ms, mu::notation::INotationConfiguration, notationConfiguration)

public:
    EditDrumsetDialog(QWidget* parent = nullptr);
    EditDrumsetDialog(const EditDrumsetDialog& other);

    static int static_metaTypeId();

private slots:
    void bboxClicked(QAbstractButton* button);
    void itemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous);
    void nameChanged(const QString&);
    void shortcutChanged();
    void valueChanged();
    void load();
    void save();
    void customGboxToggled(bool);
    void customQuarterChanged(int);

private:
    void apply();
    void updatePitchesList();
    void refreshPitchesList();
    void updateExample();

    void fillCustomNoteheadsDataFromComboboxes(int pitch);
    void setCustomNoteheadsGUIEnabled(bool enabled);

    void setEnabledPitchControls(bool enable);
    void fillNoteheadsComboboxes(bool customGroup, int pitch);

    mu::notation::INotationPtr m_notation;
    mu::notation::ID m_partId;
    mu::notation::ID m_instrumentId;
    Drumset m_editedDrumset;
};
} // namespace Ms

Q_DECLARE_METATYPE(Ms::EditDrumsetDialog)

#endif // MU_PALETTE_EDITDRUMSETDIALOG_H
