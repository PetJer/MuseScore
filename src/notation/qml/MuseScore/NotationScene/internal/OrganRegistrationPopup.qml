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
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import Muse.Ui 1.0
import Muse.UiComponents 1.0
import MuseScore.NotationScene 1.0

StyledPopupView {
    id: root

    property QtObject model: organRegistrationModel

    property string organName: organRegistrationModel.organName
    property variant organDisposition: organRegistrationModel.organDisposition
    property variant manualPedals: organRegistrationModel.manualPedals
    property variant organCouplers: organRegistrationModel.organCouplers
    property variant organPistons: organRegistrationModel.organPistons

    property variant stops: organRegistrationModel.stops
    property variant couplers: organRegistrationModel.couplers
    property variant pistons: organRegistrationModel.pistons

    property NavigationSection notationViewNavigationSection: null
    property int navigationOrderStart: 0
    property int navigationOrderEnd: registrationNavPanel.order

    contentWidth: consoleItems.width
    contentHeight: consoleItems.height

    margins: 0

    showArrow: false

    signal elementRectChanged(var elementRect)

    function updatePosition() {
        const marginFromElement = 12
        var popupHeight = root.contentHeight + root.padding * 2

        // Above registration
        let yUp = Math.min(-popupHeight - marginFromElement,
                           (organRegistrationModel.staffPos.y - root.parent.y) - contentHeight - marginFromElement)
        let yDown = Math.max(root.parent.height + marginFromElement,
                             (organRegistrationModel.staffPos.y - root.parent.y) + organRegistrationModel.staffPos.height + marginFromElement)

        // not enough room on window to open above so open below stave
        let opensUp = true
        let globPos = root.parent.mapToItem(ui.rootItem, Qt.point(root.x, yUp))
        if (globPos.y < 0) {
            opensUp = false;
            globPos = root.parent.mapToItem(ui.rootItem, Qt.point(root.x, yDown))
        }

        // not enough room below stave to open so open above
        if (globPos + contentHeight > ui.rootItem.height) {
            opensUp = true;
        }

        setOpensUpward(opensUp)
        root.y = opensUp ? yUp : yDown
    }

    function updateStops(pos, col) {
        root.stops[pos][col] = !root.stops[pos][col]
        organRegistrationModel.setStops(root.stops)
    }

    function updateCouplers(index) {
        root.couplers[index] = !root.couplers[index]
        organRegistrationModel.setCouplers(root.couplers)
    }

    function updatePistons(index) {
        root.pistons[index] = !root.pistons[index]
        organRegistrationModel.setPistons(root.pistons)
        console.log(root.pistons)
    }

    function getOrganDisposition() {
        let organDispositionModel = []
        let ids = [
            viManual,
            vManual,
            ivManual,
            iiiManual,
            iiManual,
            iManual,
            ped
        ]

        let pos = 0
        for (let manualPedal of root.organDisposition) {
            for (let [col, stop] of manualPedal.entries()) {
                // { name: "Copula minor 4", pos: 0, col: 1, btnGroup: iiManual }
                organDispositionModel.push(
                    {name: stop, pos: pos, col: col, btnGroup: ids[pos], checked: root.stops[pos][col]}
                )
            }
            pos++;
        }

        return organDispositionModel
    }

    function getOrganCouplers() {
        let organCouplersModel = []

        for (let [index, coupler] of root.organCouplers.entries()) {
            organCouplersModel.push(
                {name: coupler, checked: root.couplers[index]}
            )
        }

        return organCouplersModel
    }

    function getOrganPistons() {
        let organPistonsModel = []

        for (let [index, piston] of root.organPistons.entries()) {
            organPistonsModel.push(
                {name: piston, checked: root.pistons[index]}
            )
        }

        return organPistonsModel
    }

    GridLayout {
        id: consoleItems
        rows: 5
        flow: GridLayout.TopToBottom
        columnSpacing: 10
        rowSpacing: 10

        OrganRegistrationPopupModel {
            id: organRegistrationModel

            onItemRectChanged: function(rect) {
                root.elementRectChanged(rect)
            }
        }

        Component.onCompleted: {
            organRegistrationModel.init()
        }

        NavigationPanel {
            id: registrationNavPanel
            name: "registrationSettings"
            direction: NavigationPanel.Vertical
            section: root.notationViewNavigationSection
            order: root.navigationOrderStart
            // accessible.name: qsTrc("notation", "Stops settings buttons")
        }

        StyledTextLabel {
            id: organName

            Layout.leftMargin: 10
            Layout.topMargin: 10

            font: ui.theme.bodyBoldFont
            text: root.organName
        }

        Repeater {
            model: root.manualPedals
            StyledTextLabel {
                Layout.row: index + 1
                Layout.leftMargin: 10
                Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft
                text: modelData
            }
        }

        ButtonGroup {
            id: viManual
        }

        ButtonGroup {
            id: vManual
        }

        ButtonGroup {
            id: ivManual
        }

        ButtonGroup {
            id: iiiManual
        }

        ButtonGroup {
            id: iiManual
        }

        ButtonGroup {
            id: iManual
        }

        ButtonGroup {
            id: ped
        }

        Repeater {
            width: parent.width

            model: getOrganDisposition()

            // Button toggle in the future
            CheckBox {
                Layout.row: modelData.pos + 1
                Layout.column: modelData.col + 1
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                // Layout.rightMargin: (modelData.col === 8) ? 30 : 0

                ButtonGroup.group: modelData.btnGroup
                text: modelData.name

                checked: modelData.checked
                onClicked: updateStops(modelData.pos, modelData.col)
            }
        }

        StyledTextLabel {
            id: couplersLabel

            Layout.row: root.organDisposition.length + 2
            Layout.leftMargin: 10

            text: "Couplers" // Translatable
        }

        Repeater {
            model: getOrganCouplers()
            CheckBox {
                Layout.row: root.organDisposition.length + 2
                Layout.column: index + 1
                Layout.topMargin: 10
                Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft

                text: modelData.name

                checked: modelData.checked
                onClicked: updateCouplers(index)
            }
        }

        StyledTextLabel {
            id: pistonsLabel

            Layout.row: root.organDisposition.length + 3
            Layout.leftMargin: 10

            text: "Pistons" // Translatable
        }

        Repeater {
            model: getOrganPistons()
            CheckBox {
                Layout.row: root.organDisposition.length + 3
                Layout.column: index + 1
                Layout.topMargin: 10
                Layout.bottomMargin: 10
                Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft

                text: modelData.name

                checked: modelData.checked
                onClicked: updatePistons(index)
            }
        }
    }
}
