// SPDX-License-Identifier: GPL-2.0-only
// Copyright (C) 2023 Bardia Moshiri <fakeshell@bardia.tech>

import QtQuick 2.12
import QtQuick.Controls 2.12
import org.droidian.fingerprint 1.0

ApplicationWindow {
    visible: true
    width: 640
    height: 480
    title: qsTr("FPD Interface")

    FPDInterface {
        id: fpdInterface
    }

    function formatFingerNames(fingerprints) {
        return fingerprints.map(function(finger) {
            return finger.replace(/[_-]/g, ' ');
        });
    }

    Column {
        anchors.fill: parent
        spacing: 10

        Button {
            text: "Enroll"
            width: parent.width
            onClicked: {
                enrollDialog.open();
            }
        }

        Button {
            text: "Remove"
            width: parent.width
            onClicked: {
                if (fingerList.currentText === "") {
                    messageLabel.text = "Please select a finger from the list."
                } else {
                    var fingerName = fingerList.currentText.replace(/ /g, '_').toLowerCase();
                    confirmationDialog.confirmationText = "Are you sure you want to remove fingerprint '" + fingerList.currentText + "'?";
                    confirmationDialog.open();
                }
            }
        }

        ComboBox {
            id: fingerList
            width: parent.width
            model: formatFingerNames(fpdInterface.fingerprints)
        }

        Label {
            id: messageLabel
            color: "red"
            width: parent.width
        }

        Label {
            id: enrollmentProgressLabel
            color: "blue"
            width: parent.width
        }

        Label {
            id: acquisitionInfoLabel
            color: "green"
            width: parent.width
        }

        Rectangle {
            height: 1
            color: "gray"
            width: parent.width
        }
    }

    Label {
        id: enrollmentProgressBigLabel
        font.pixelSize: 72
        color: "black"
        anchors.centerIn: parent
    }

    Dialog {
        id: enrollDialog
        title: "Enroll Finger"
        modal: true
        standardButtons: Dialog.Ok | Dialog.Cancel
        anchors.centerIn: parent

        TextField {
            id: fingerNameField
            width: parent.width
            placeholderText: "Enter finger name"
            focus: true
        }

        onAccepted: {
            if (fingerNameField.text === "") {
                messageLabel.text = "Please enter a finger name."
            } else {
                fpdInterface.enroll(fingerNameField.text);
                enrollmentGuideDialog.open();
            }
        }
    }

    Dialog {
        id: enrollmentGuideDialog
        title: "Enrollment Guide"
        modal: true
        standardButtons: Dialog.Ok
        anchors.centerIn: parent

        Text {
            text: "Take your finger on and off of the sensor until the enrollment reaches 100%"
            wrapMode: Text.Wrap
            anchors.fill: parent
        }
    }

    Dialog {
        id: confirmationDialog
        title: "Confirmation"
        modal: true
        standardButtons: Dialog.Yes | Dialog.No
        anchors.centerIn: parent

        property string confirmationText: ""

        Text {
            wrapMode: Text.Wrap
            anchors.fill: parent
            text: confirmationDialog.confirmationText
        }

        onAccepted: {
            var fingerName = fingerList.currentText.replace(/ /g, '_').toLowerCase();
            fpdInterface.remove(fingerName);
        }

        onRejected: {
            console.log("Fingerprint removal cancelled.");
        }
    }

    Dialog {
        id: aboutDialog
        title: "About"
        modal: true
        standardButtons: Dialog.Close
        anchors.centerIn: parent

        Column {
            Text {
                text: "Credits"
                font.bold: true
                font.pixelSize: 16
            }

            Text {
                text: "Bardia Moshiri"
            }

            Text {
                text: "Sailfish open project"
            }

            Text {
                text: ""
            }

            Text {
                text: "Legal"
                font.bold: true
                font.pixelSize: 16
            }

            Text {
                text: "© 2023 Bardia Moshiri, GPL-2.0-only"
            }
        }
    }

    Button {
        text: "About"
        width: parent.width
        onClicked: {
            aboutDialog.open()
        }
        anchors.bottom: parent.bottom
    }

    Connections {
        target: fpdInterface
        onFingerprintsChanged: {
            fingerList.model = formatFingerNames(fpdInterface.fingerprints)
        }
        onMessageChanged: messageLabel.text = msg
        onEnrollmentProgressChanged: {
            enrollmentProgressBigLabel.text = progress + "%"
        }
        onAcquisitionInfoChanged: {
            if (info === "FPACQUIRED_GOOD") {
                acquisitionInfoLabel.text = "Sensor read: Good";
                acquisitionInfoLabel.color = "green";
            } else if (info === "FPACQUIRED_PARTIAL") {
                acquisitionInfoLabel.text = "Sensor read: Partial";
                acquisitionInfoLabel.color = "orange";
            }
        }
    }
}
