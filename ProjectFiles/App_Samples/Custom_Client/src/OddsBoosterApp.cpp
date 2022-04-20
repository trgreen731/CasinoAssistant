#ifndef UNICODE
#define UNICODE
#endif

#include <winsock2.h>
#include <windows.h>
#include <bluetoothapis.h>
#include <bthsdpdef.h>
#include <ws2bth.h>

#include <iostream>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char** argv){

	//create the search parameters for the bluetooth device
	BLUETOOTH_DEVICE_SEARCH_PARAMS params;
	params.dwSize = sizeof(BLUETOOTH_DEVICE_SEARCH_PARAMS);
	params.fReturnAuthenticated = true;
	params.fReturnRemembered = true;
	params.fReturnUnknown = true;
	params.fReturnConnected = true;
	params.fIssueInquiry = true;
	params.cTimeoutMultiplier = 10;
	params.hRadio = nullptr;

	BLUETOOTH_DEVICE_INFO device;

	//search for some devices
	HBLUETOOTH_DEVICE_FIND dev_handle;
	dev_handle = BluetoothFindFirstDevice(&params, &device);
	std::cout << "First Device Address: " << device.Address.ullLong << std::endl;
	bool next_succ = BluetoothFindNextDevice(&params, &device);
	if(next_succ){
		std::cout << "Next Device Address: " << device.Address.ullLong << std::endl;
	} else{
		std::cout << "Error: " << GetLastError() << std::endl;
	}
	BluetoothFindDeviceClose(dev_handle);
}
