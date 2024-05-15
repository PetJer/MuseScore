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
    property string context: organRegistrationModel.context

    property NavigationSection notationViewNavigationSection: null
    property int navigationOrderStart: 0
    property int navigationOrderEnd: registrationNavPanel.order

    property int maxColumn: 0

    contentWidth: consoleItems.width
    contentHeight: consoleItems.height

    margins: 0

    showArrow: false

    signal elementRectChanged(var elementRect)

    function updatePosition() {
        const marginFromElement = 5
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

    function updateStops(row, col) {
        root.stops[row][col] = !root.stops[row][col]
        organRegistrationModel.setStops(root.stops)
    }

    function updateCouplers(index) {
        root.couplers[index] = !root.couplers[index]
        organRegistrationModel.setCouplers(root.couplers)
    }

    function updatePistons(index) {
        root.pistons[index] = !root.pistons[index]
        organRegistrationModel.setPistons(root.pistons)
    }

    function updateContext(newContext) {
        root.context = newContext
        organRegistrationModel.setContext(root.context)
    }

    function getOrganDisposition() {
        let organDispositionModel = []

        for (let [row, manualPedal] of root.organDisposition.entries()) {
            for (let [col, stop] of manualPedal.entries()) {
                organDispositionModel.push({
                    name: stop,
                    pos: root.manualPedals.length - row,
                    row: row,
                    col: col,
                    checked: root.stops[row][col]
                })
            }
            if (manualPedal.length > root.maxColumn) {
                root.maxColumn = manualPedal.length
            }
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

    // Can be more elegant?
    function resetRegistration() {
        let stops = [[]]

        for (let i = 0; i < root.stops.length; i++) {
            stops[i] = new Array(root.stops[i].length).fill(false)
        }

        organRegistrationModel.setStops(stops)
        organRegistrationModel.setCouplers(new Array(root.couplers.length).fill(false))
        organRegistrationModel.setPistons(new Array(root.pistons.length).fill(false))
        organRegistrationModel.setContext("");
        root.context = ""
    }

    GridLayout {
        id: consoleItems
        rows: root.manualPedals.length + 5
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

            Layout.row: 0
            Layout.leftMargin: 20
            Layout.topMargin: 20

            font: ui.theme.bodyBoldFont
            text: root.organName
        }

        Repeater {
            model: root.manualPedals
            StyledTextLabel {
                Layout.row: index + 1
                Layout.leftMargin: 20
                Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft
                text: modelData
            }
        }

        Repeater {
            width: parent.width

            model: getOrganDisposition()

            FlatButton {
                Layout.row: modelData.pos
                Layout.column: modelData.col + 1
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                Layout.rightMargin: (modelData.col + 1 === root.maxColumn) ? 20 : 0

                text: modelData.name

                // Misusing the button...
                accentButton: modelData.checked
                onClicked: updateStops(modelData.row, modelData.col)
            }
        }

        StyledTextLabel {
            id: couplersLabel

            Layout.row: root.manualPedals.length + 2
            Layout.leftMargin: 20
            Layout.topMargin: 10

            text: "Couplers" // Translatable
        }

        Repeater {
            model: getOrganCouplers()
            FlatButton {
                Layout.row: root.manualPedals.length + 2
                Layout.column: index + 1
                Layout.topMargin: 10
                Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft

                text: modelData.name

                accentButton: modelData.checked
                onClicked: updateCouplers(index)
            }
        }

        StyledTextLabel {
            id: pistonsLabel

            Layout.row: couplersLabel.Layout.row + 1
            Layout.leftMargin: 20
            Layout.topMargin: 10

            text: "Pistons" // Translatable
        }

        Repeater {
            model: getOrganPistons()
            FlatButton {
                Layout.row: couplersLabel.Layout.row + 1
                Layout.column: index + 1
                Layout.topMargin: 10
                Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft

                text: modelData.name

                accentButton: modelData.checked
                onClicked: updatePistons(index)
            }
        }

        StyledTextLabel {
            id: contextLabel

            Layout.row: pistonsLabel.Layout.row + 1
            Layout.leftMargin: 20

            text: "Context" // Translatable
        }

        RowLayout {
            Layout.row: pistonsLabel.Layout.row + 1
            Layout.column: 1
            Layout.topMargin: 10
            Layout.bottomMargin: 10

            TextInputField {
                id: capoTextField

                Layout.fillWidth: true

                currentText: root.context
                onTextChanged: updateContext(newTextValue)
            }
        }

        FlatButton {
            id: resetButton

            Layout.row: contextLabel.Layout.row + 1
            Layout.leftMargin: 20
            Layout.bottomMargin: 20

            text: "Reset" // Translatable
            onClicked: resetRegistration()
        }
    }
}
