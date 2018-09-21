// RFManager
// (c) Ivanov Aleksandr, 2018

#include "RFManager.h"
#include "EepromManager.h"
#include "DataBase.h"

RFManager::RFManager(DataBase* pDataBase) {
	// blink = new bool[MAX_DEVICES];
	// for (uint8_t i = 0; i < MAX_DEVICES; i++) {
		// if (i%2) {
			// blink[i] = false;
		// } else {
			// blink[i] = true;
		// }		
	// }
	init(pDataBase);
	Serial.println("RFManager()!");//TEST
}

RFManager::~RFManager() {
	delete _driver;
	delete _radioMngr;
}

void RFManager::init(DataBase* pDataBase) {
	_dataBase = pDataBase;
	_driver = new RH_ASK(RADIO_FREG, RADIO_RX_PIN, RADIO_TX_PIN);//RH_Serial(Serial3);
	_radioMngr = new RHReliableDatagram(*_driver, BAZA_ADDRESS);
	_initError = !_radioMngr->init();
	_radioMngr->setThisAddress(BAZA_ADDRESS);
	_radioMngr->setHeaderFrom(BAZA_ADDRESS);//?????? why is this needed
}

/****************
* public methods*
*****************/

bool RFManager::searchDevices(){
	//Serial.println("RFManager::searchDevice() Begin");
	if (_dataBase->getDeviceCount() >= _dataBase->getMaxDevices()) {
		Serial.println(F("max count of devices"));
		//TODO send message to controller to display about this situation
		return false;
	}
	if (_radioMngr->available()) {
        Serial.println(F("BAZA: got something by RF!!"));//TEST
		//input buffer
        uint8_t from;
        uint8_t len = sizeof(buf);
        if (_radioMngr->recvfromAck(dataInfoUnion.byteBuffer, &len, &from)) {
			Serial.println(String(F("FROM = ")) + from + "; _uniqID = " + dataInfoUnion.dataInfo._uniqID);//TEST
			Serial.println(String(F("FROM = ")) + from + "; _deviceID = " + dataInfoUnion.dataInfo._deviceID);//TEST
			Serial.println(String(F("FROM = ")) + from + "; len = " + len);//TEST
			
			if (!isDeviceKnown(from)) {
				Serial.println(F("BAZA: No such device found"));//TEST
				uint8_t newDeviceId = _dataBase->generateId();
				Serial.println(String(F("BAZA: new device number = ")) + newDeviceId);//TEST
				prepareDataForKnowingTransmit(newDeviceId);
				// Send a reply back to the originator client
				if (_radioMngr->sendtoWait(dataInfoUnion.byteBuffer, sizeof(DataInfo), from)) {
					// /////// TODO �� �������� ����� ����� ��������
				// ������������ ��� ���� ����� �������������� � ������� � �������
					// Good transmit and Ack. Add this device to eeprom memory and to the dataBase
					// registerNewDevice(newDeviceId);
					// Serial.println(F("RFManager::searchDevice() Good transmit and Ack I don't know you End"));
					// return true;					
				} else {
					// Bad transmit or Ack. No connection between us. Radio Error of this Device should be true!
					Serial.println(F("BAZA: sendtoWait of unknown failed"));
				}
				registerNewDevice(newDeviceId);
				Serial.println(F("RFManager::searchDevice() Good transmit and Ack I don't know you End"));
				return true;
			} else {
				if (_radioMngr->sendtoWait(dataInfoUnion.byteBuffer, sizeof(DataInfo), from)) {
					// Good transmit and Ack. Add this device to eeprom memory and to the dataBase
					Serial.println(F("RFManager::searchDevice() Good transmit and Ack. Known device. End"));
					return true;					
				} else {
					// Bad transmit or Ack. No connection between us. Radio Error of this Device should be true!
					Serial.println(F("BAZA: sendtoWait 'I know him' failed"));
				}
			}
		}
	}
	//Serial.println("RFManager::searchDevice() No results End");
	return false;
}

void RFManager::processDeveices(){
	Serial.println(F("RFManager::processDeveices() Begin"));
	const int maxDevices = _dataBase->getMaxDevices();
	uint8_t knownDeviceIds[maxDevices];
	memset(knownDeviceIds, 0, maxDevices);
	_dataBase->fetchIds(knownDeviceIds);
	uint8_t deviceId;
	uint8_t len = sizeof(buf);
	// uint8_t len = sizeof(DataInfo);
	Serial.println(String("len ") + len);//TEST
	for (int i = 0; i < maxDevices; i++) {
		deviceId = knownDeviceIds[i];		
		if (deviceId == 0) break;
		Serial.println(String(F("RFManager::processDeveices:deviceId ")) + deviceId);//TEST
		prepareDataForWorkingTransmit(deviceId);
		
		if (_radioMngr->sendtoWait(dataInfoUnion.byteBuffer, sizeof(DataInfo), deviceId)) {
			// Now wait for a reply from Device
			// ------------ experimental code -----------//
			bool dataRecieved = false;
			unsigned long currentMillis = millis();
			unsigned long previousMillis = currentMillis;	
	
			while (currentMillis - previousMillis < 2000) {
				currentMillis = millis();
				if (_radioMngr->available()) {
					Serial.println(F("DEVICE: got something by RF!"));//TEST
					uint8_t from;
					if (_radioMngr->recvfromAck(dataInfoUnion.byteBuffer, &len, &from)) {
						Serial.print(F("Get data from Device 0x")); Serial.println(from);
						if (isDeviceKnown(dataInfoUnion.dataInfo._deviceID)) {
							saveDeviceData(deviceId);
							Serial.print("Device #");
							Serial.print(deviceId);
							Serial.println(" is SWITCHED ON");
							dataRecieved = true;
							break;
						}
					}
				}
			}
			if (!dataRecieved) {
				Serial.println(String(F("Can't receive ack (timeout) from = ")) + deviceId);
			}
			// ------------ end of experimental code -----------//
			// ------------ old code -----------//
			// if (_radioMngr->recvfromAckTimeout(dataInfoUnion.byteBuffer, &len, 2000, &from)) {
				// Serial.print(F("Get data from Device 0x")); Serial.println(from);
				// if (isDeviceKnown(dataInfoUnion.dataInfo._deviceID)) {
					// saveDeviceData(deviceId);
					// Serial.print("Device #");
					// Serial.print(deviceId);
					// Serial.println(" is SWITCHED ON");
					// continue;
				// }
			// } else {
				// Serial.println(String("Can't receive ack (timeout) from = ") + deviceId);
			// }
			// ------------ end of old code -----------//
		} else {// end if sendtoWait
			Serial.println(String(F("Can't send to wait to = ")) + deviceId);
			Serial.print(F("Device #"));
			Serial.print(deviceId);
			Serial.println(F(" is maybe SWITCHED OFF"));
			_dataBase->setDeviceRFErr(deviceId, true);
		}		
	} 	// end for
	Serial.println(F("RFManager::processDeveices() End"));
}

bool RFManager::hasInitError() {
	return _initError;
}

/*****************
* private methods*
******************/

bool RFManager::isDeviceKnown(uint8_t from_id) {
	Serial.println(String(F("BAZA: from = ")) + String(from_id));//TEST
	bool existedDevice = _dataBase->isDeviceExist(from_id);
	if (existedDevice && dataInfoUnion.dataInfo._uniqID == getUniqID()) {
		Serial.println(String(F("BAZA: such device with id=")) + from_id + F(" is exist"));//TEST
		return true;
	}
	return false;
}

long RFManager::getUniqID() {
	return _dataBase->getUniqBaseID();
	
}

void RFManager::prepareDataForKnowingTransmit(uint8_t pDeviceId) {
	dataInfoUnion.dataInfo._uniqID = getUniqID();
	dataInfoUnion.dataInfo._deviceID = pDeviceId;
}

void RFManager::prepareDataForWorkingTransmit(uint8_t pDeviceId) {
	dataInfoUnion.dataInfo._uniqID = getUniqID();
	dataInfoUnion.dataInfo._deviceID = pDeviceId;
	dataInfoUnion.dataInfo._deviceControl = _dataBase->getDeviceControlValue(pDeviceId);
	
	// dataInfoUnion.dataInfo._deviceControl = blink[pDeviceId] ? 1.0 : 0.0; //STUB
	Serial.println(String(F("Control value = ")) + dataInfoUnion.dataInfo._deviceControl);	
	// blink[pDeviceId] = !blink[pDeviceId]; //STUB
	
}

void RFManager::saveDeviceData(uint8_t pDeviceId) {
	_dataBase->setDeviceAck(pDeviceId, dataInfoUnion.dataInfo._deviceAck);
	_dataBase->setDeviceRFErr(pDeviceId, itob(dataInfoUnion.dataInfo._radioError));
	Serial.println(String(F("passedDeviceAck = ")) + String(dataInfoUnion.dataInfo._deviceAck));
	Serial.println(String(F("passedDeviceAdj = ")) + String(dataInfoUnion.dataInfo._adjustable));
}

void RFManager::registerNewDevice(uint8_t pDeviceId) {
	_dataBase->addDeviceInfo(pDeviceId);
	_dataBase->setDeviceMin(pDeviceId, dataInfoUnion.dataInfo._min);
	_dataBase->setDeviceMax(pDeviceId, dataInfoUnion.dataInfo._max);
	_dataBase->setDeviceDiscrete(pDeviceId, dataInfoUnion.dataInfo._discrete);
	_dataBase->setDeviceDigital(pDeviceId, itob(dataInfoUnion.dataInfo._digital));
	_dataBase->setDeviceAnalog(pDeviceId, itob(dataInfoUnion.dataInfo._analog));
	_dataBase->setDeviceAdj(pDeviceId, itob(dataInfoUnion.dataInfo._adjustable));
	_dataBase->setDeviceRotatable(pDeviceId, itob(dataInfoUnion.dataInfo._rotatable));
	_dataBase->setDeviceRFErr(pDeviceId, itob(dataInfoUnion.dataInfo._radioError));
	_dataBase->saveDevicesIdsToEeprom();
}
