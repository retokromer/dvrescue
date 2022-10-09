import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import Qt.labs.settings 1.0
import Launcher 0.1
import FileUtils 1.0
import SettingsUtils 1.0
import QwtQuick2 1.0

Popup {
    id: aboutDialog
    property string version: ""
    property string buildDate: ""
    property string buildQtVersion: ""
    property string runtimeQtVersion: ""
    property string buildFFMpegVersion: ""
    property string runtimeFFMpegVersion: ""

    anchors.centerIn: parent
    contentWidth: layout.childrenRect.width
    contentHeight: layout.childrenRect.height

    ColumnLayout {
        id: layout
        Image {
            source: "/dvrescue.png"
            Layout.alignment: Qt.AlignHCenter
        }

        Text {
            text: "DVRescue"
            font.pixelSize: 25
            Layout.alignment: Qt.AlignHCenter
        }

        RowLayout {
            Layout.alignment: Qt.AlignHCenter
            Text {
                text: "Version"
                font.pixelSize: 20
            }

            Text {
                text: version
                font.pixelSize: 20
            }
        }

        Text {
            text: buildDate
            Layout.alignment: Qt.AlignHCenter
        }

        Text {
            text: "Copyright (c) 2019-2022, Moving Image Preservation of Puget Sound."
            Layout.alignment: Qt.AlignHCenter
        }

        Text {
            text: "Third party libraries Copyright (c) 2012-2020 MediaArea.net SARL"
            Layout.alignment: Qt.AlignHCenter
        }

        Text {
            text: "Build Qt version: " + buildQtVersion + ", runtime Qt version: " + runtimeQtVersion
            Layout.alignment: Qt.AlignHCenter
        }

        Text {
            text: "Build FFMpeg version: " + buildFFMpegVersion + ", runtime FFMpeg version: " + runtimeFFMpegVersion
            Layout.alignment: Qt.AlignHCenter
        }
    }
}
