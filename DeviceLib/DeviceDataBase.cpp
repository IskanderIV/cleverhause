// DeviceDataBase
// (c) Ivanov Aleksandr, 2018

#include "DeviceDataBase.h"

DeviceDataBase::DeviceDataBase(DeviceEepromManager* _eepromManager) {
	init(pEepromMngr);
}

DeviceDataBase::~DeviceDataBase() {
}

void DeviceDataBase::init(EepromManager* pEepromMngr) {
	_eepromMngr = pEepromMngr;
	_ack = 0.0;
	_radioError = false;
	
	if (_eepromMngr) {
		initFromEeprom();
	}
}

void DeviceDataBase::initFromEeprom() {
	// boardUID
	_boardUID = _eepromMngr->fetchBoardUID(eepr_deviceMin);
	
	// id
	_deviceId = _eepromMngr->fetchDeviceFloat(eepr_deviceMin);
	
	// min
	_min = _eepromMngr->fetchDeviceFloat(eepr_deviceMin);

	// max
	_max = _eepromMngr->fetchDeviceFloat(eepr_deviceMax);

	// discrete
	_discrete = _eepromMngr->fetchDeviceFloat(eepr_deviceDiscrete);

	// deviceCtrl
	_controlValue = _eepromMngr->fetchDeviceFloat(eepr_deviceCtrl);

	// deviceDigital
	_digital = _eepromMngr->fetchDeviceBool(eepr_deviceDigitalBool);

	// deviceAnalog
	_analog = _eepromMngr->fetchDeviceBool(eepr_deviceAnalogBool);

	// deviceAdjustable
	_adjustable = _eepromMngr->fetchDeviceBool(eepr_deviceAdjustableBool);

	// deviceRotatable
	_rotatable = _eepromMngr->fetchDeviceBool(eepr_deviceRotatableBool);

}

/******************
* Public interface*
*******************/

long DeviceDataBase::getBoardUID() {
	return _boardUID;
}

void DeviceDataBase::setBoardUID(long pBoardUID) {
	if (getBoardUID() != pBoardUID) {
		_eepromMngr->saveBoardUID(pBoardUID); //TODO �������� ��� ������
	}
	_boardUID = pBoardUID;
}

uint8_t DeviceDataBase::getDeviceId() {
	return _deviceId;
}

void DeviceDataBase::setDeviceID(uint8_t pDeviceId) {
	if (getDeviceId() != pDeviceId) {
		_eepromMngr->saveDeviceId(pDeviceId); //TODO �������� ��� ������
	}
	_deviceId = pDeviceId;
}

float DeviceDataBase::getDeviceAck() {
	return _deviceAck;
}

void DeviceDataBase::setDeviceAck(float pAck) {
	_deviceAck = pAck;
}

float DeviceDataBase::getDeviceMin() {
	return _min;
}

void DeviceDataBase::setDeviceMin(float pMin) {
	if (getDeviceMin() != pMin) {
		_eepromMngr->saveFloat(eepr_deviceMin, pMin); //TODO �������� ��� ������ � �������� eeprom
	}
	_min = pMin;
}

float DeviceDataBase::getDeviceMax() {
	return _max;
}

void DeviceDataBase::setDeviceMax(float pMax) {
	if (getDeviceMax() != pMax) {
		_eepromMngr->saveFloat(eepr_deviceMax, pMax); //TODO �������� ��� ������ � �������� eeprom
	}
	_max = pMax;
}

float DeviceDataBase::getDeviceDiscrete() {
	return _discrete;
}

void DeviceDataBase::setDeviceDiscrete(float pDiscrete) {
	if (getDeviceMax() != pDiscrete) {
		_eepromMngr->saveFloat(eepr_deviceDiscrete, pDiscrete); //TODO �������� ��� ������ � �������� eeprom
	}
	_discrete = pDiscrete;
}

float DeviceDataBase::getDeviceControlValue() {
	return _controlValue;
}

void DeviceDataBase::setDeviceControlValue(float pControlValue) {
	if (getDeviceControlValue() != pControlValue) {
		_eepromMngr->saveFloat(eepr_deviceCtrl, _pControlValue); //TODO �������� ��� ������ � �������� eeprom
	}
	_controlValue = pControlValue;
}

bool DeviceDataBase::getDeviceDigital() {
	return _digital;
}

void DeviceDataBase::setDeviceDigital(bool pDigital) {
	if (getDeviceDigital() != pDigital) {
		_eepromMngr->saveBool(eepr_deviceDigitalBool, pDigital);//TODO �������� ��� ������ � �������� eeprom
	}
	_digital = pDigital;
}

bool DeviceDataBase::getDeviceAnalog() {
	return _analog;
}

void DeviceDataBase::setDeviceAnalog(bool pAnalog) {
	if (getDeviceAnalog() != pAnalog) {
		_eepromMngr->saveBool(eepr_deviceAnalogBool, pAnalog);//TODO �������� ��� ������ � �������� eeprom
	}
	_analog = pAnalog;
}

bool DeviceDataBase::getDeviceAdj() {
	return _adjustable;
}

void DeviceDataBase::setDeviceAdj(bool pAdj) {
	if (isDeviceAdj() != pAdj) {
		_eepromMngr->saveBool(eepr_deviceAdjustableBool, pAdj);//TODO �������� ��� ������ � �������� eeprom
	}
	_adjustable = pAdj;
}

bool DeviceDataBase::getDeviceRotatable() {
	return _rotatable;
}

void DeviceDataBase::setDeviceRotatable(bool pRotatable) {
	if (isDeviceAdj() != pRotatable) {
		_eepromMngr->saveBool(eepr_deviceRotatableBool, pRotatable);//TODO �������� ��� ������ � �������� eeprom
	}
	_rotatable = pRotatable;
}

bool DeviceDataBase::getDeviceRFErr() {
	return _radioError;
}

void DeviceDataBase::setDeviceRFErr(bool pRadioError) {
	_radioError = pRadioError;
}

/*********************
*	Private interface*
**********************/
