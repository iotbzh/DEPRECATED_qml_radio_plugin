import QtQuick 2.0
import QtQuick.Window 2.0
import QtQuick.Controls 1.4
import radio 1.0

Window {

	id: radio_window
	visible: true
	width: 320
	height: 300

	Rectangle {
		anchors.left: parent.left
		anchors.top: parent.top
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.margins: 20

		Label {
			text: "AM/FM Radio"
			font.pixelSize: 18
			font.bold: true
			anchors.centerIn: parent
        		y: 0
		}

		Text {
			text: radio.status
			y: 20
		}

		Button {
			text: "Turn on radio"
			onClicked: radio.play()
			y: 80
		}
		Button {
			text: "Turn off radio"
			onClicked: radio.stop()
			y: 110
		} 

		Label {
			text: "Mode"
			y: 160
		}
		ComboBox {
			currentIndex: 0
			model: [ "FM", "AM" ]
			y: 180
			onActivated: {
				if (currentIndex === 0) radio.mode = Radio.FM;
				                   else radio.mode = Radio.AM;
			}
		}

		Label {
			text: "Frequency"
			y: 220
		}
		SpinBox {
			minimumValue: 60.0
			maximumValue: 150.0
			value: 100.0
			stepSize: 0.1
			decimals: 2
			y: 240
			onEditingFinished: radio.freq = value
		}

		Label {
			text: "Mute"
			x: 200
			y: 220
		}
		Switch {
			checked: false
			x: 200
			y: 240
			onClicked: radio.mute = checked
		}
	}

	Radio {
		id: radio
		onPlaying: console.log("Playing radio")
		onStopped: console.log("Stopped radio")
		onModeChanged: console.log("Radio mode changed")
		onFreqChanged: console.log("Radio frequency changed")
		onMuteChanged: console.log("Radio mute state changed")
	}

}
