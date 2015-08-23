import QtQuick 2.0
import Ubuntu.Components 1.1
import ImpressRemote 1.0
/*!
    \brief MainView of LibreOffice Impress Assistant.
*/

MainView {
    // objectName for functional testing purposes (autopilot-qt5)
    objectName: "mainView"

    // Note! applicationName needs to match the "name" field of the click manifest
    applicationName: "impressremote.e"

    /*
     This property enables the application to change orientation
     when the device is rotated. The default is false.
    */
    //automaticOrientation: true

    // Removes the old toolbar and enables new features of the new header.
    useDeprecatedToolbar: false

    width: units.gu(100)
    height: units.gu(75)

    backgroundColor: "#1B676B"

    PageStack {
        id: stack
        Component.onCompleted: push(home)

        Impress {
            id: impress
            onConnectedToImpressServer: {
                console.debug("connected to server socket")
            }

            onPaired: {
                console.debug("paired")
                stack.push(dashboard)
                start()
            }

            onReportPageNumberGot: {
                note_title.text = "Note " + cur
                // note.text = impress.getNote(cur)
            }
        }

        Page {
            id: home
            title: i18n.tr("ImpressRemote")
            visible: false

            Column {
                spacing: units.gu(1)
                anchors {
                    margins: units.gu(2)
                    fill: parent
                }

                TextInput {
                    id: ip
                    text: "192.168.1.114"
                }

                Button {
                    id: connect_button
                    width: parent.width

                    text: i18n.tr("Connect")

                    onClicked: {
                        console.debug("try to connect " + ip.text)
                        impress.hostAddr = ip.text
                        impress.connect_server()

                        connect_button.text = i18n.tr("Connecting")

                    }
                }
            }
        }

        Page {
            id: dashboard
            visible: false

            Rectangle {
                id: note_container
                anchors.top: parent.top
                width: parent.width
                height: parent.height * 0.8
                color: "#1B676B"

                Label {
                    id: note_title
                    anchors.top: parent.top
                    anchors.bottom: line.top
                    anchors.left: line.left
                    anchors.margins: units.gu(2)
                    anchors.topMargin: units.gu(4)
                    fontSize: "large"
                    color: "#222222"
                    text: "Note"
                }

                Rectangle {
                    id: line
                    anchors.top: parent.top
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.topMargin: units.gu(8)
                    anchors.leftMargin: units.gu(2)
                    anchors.rightMargin: units.gu(2)
                    height: units.dp(1)
                    color: "#000000"
                }

                Text {
                    id: note
                    anchors.top: line.bottom
                    anchors.left: line.left
                    anchors.right: line.right
                    anchors.bottom: note_container.bottom
                    anchors.margins: units.gu(2)
                    text: "Have fun!"
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {

                    }
                }
            }

            Rectangle {
                id: prev
                anchors.top: note_container.bottom
                anchors.bottom: parent.bottom
                anchors.left: parent.left
                width: parent.width * (1 - 0.618)
                color: "#519548"

                Image {
                    anchors.centerIn: parent
                    height: parent.height / 3
                    fillMode: Image.PreserveAspectFit
                    source: "left_arrow.png"
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        note_title.text = "prev"
                        console.debug("prev page")
                        impress.prev_page()
                    }
                }
            }

            Rectangle {
                id: next
                anchors.top: note_container.bottom
                anchors.bottom: parent.bottom
                anchors.left: prev.right
                anchors.right: parent.right
                color: "#88C425"

                Image {
                    anchors.centerIn: parent
                    height: parent.height / 3
                    fillMode: Image.PreserveAspectFit
                    source: "right_arrow.png"
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        note_title.text = "next"
                        console.debug("next page")
                        impress.next_page()
                    }
                }
            }
        }
    }
}

