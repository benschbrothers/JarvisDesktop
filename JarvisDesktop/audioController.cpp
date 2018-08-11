
#include "audioController.h"


//###################### SET DEFAULT DEVICE ######################
char setDefaultDevice(IMMDeviceEnumerator* enumerator, EDataFlow dataFlowFilter, DWORD stateMaskFilter, LPWSTR setDevice)
{
	HRESULT hr;

	IMMDeviceCollection* devices;

	hr = enumerator->EnumAudioEndpoints(dataFlowFilter, stateMaskFilter, &devices);
	if (hr) { printHResult("EnumAudioEndpoints failed", hr); return 1; }

	UINT deviceCount;
	hr = devices->GetCount(&deviceCount);
	if (hr) { printHResult("IMMDeviceCollection->GetCount failed", hr); return 1; }

	printf("Found %d devices\n", deviceCount);

	//iterate all devices
	for (UINT i = 0; i < deviceCount; i++) {
		IMMDevice* device;
		hr = devices->Item(i, &device);
		if (hr) { printHResult("IMMDeviceCollection->Item failed", hr); return 1; }

		printf("  Device %d:\n", i + 1);

		LPWSTR devID;
		hr = device->GetId(&devID);

		// Found device -> set it
		if (wcscmp(devID, setDevice) == 0) {

			IPolicyConfigVista *pPolicyConfig;
			HRESULT hrn = CoCreateInstance(__uuidof(CPolicyConfigVistaClient), NULL, CLSCTX_ALL, __uuidof(IPolicyConfigVista), (LPVOID *)&pPolicyConfig);

			if (SUCCEEDED(hrn))
			{
				hrn = pPolicyConfig->SetDefaultEndpoint(devID, eConsole);
				hrn = pPolicyConfig->SetDefaultEndpoint(devID, eMultimedia);
				hrn = pPolicyConfig->SetDefaultEndpoint(devID, eCommunications);
				pPolicyConfig->Release();
			}

		}
		device->Release();
	}
	devices->Release();

	return 0;
}

//###################### GET DEFAULT DEVICE ######################
LPWSTR tryGetDefaultAudioEndpoint(IMMDeviceEnumerator* enumerator, EDataFlow dataFlow, ERole role)
{
	HRESULT hr;
	LPWSTR id;
	IMMDevice* device;

	hr = enumerator->GetDefaultAudioEndpoint(dataFlow, role, &device);
	if (hr) { printHResult("GetDefaultAudioEndpoint failed", hr); }

	printf("  DefaultDevice: ");

	hr = device->GetId(&id);
	if (hr) { printHResult("GetDefaultAudioEndpoint failed", hr); }

	IPropertyStore *pProps = NULL;

	hr = device->OpenPropertyStore(STGM_READ, &pProps);
	if (hr) { printHResult("GetDefaultAudioEndpoint failed", hr); }

	PROPVARIANT varName;

	PropVariantInit(&varName);

	hr = pProps->GetValue(PKEY_Device_FriendlyName, &varName);
	if (hr) { printHResult("GetDefaultAudioEndpoint failed", hr); }

	printf(" %S \n", varName.pwszVal);

	device->Release();

	return id;
}

//###################### GET DEVICE COUNT ######################
int getAudioEndpointCount(IMMDeviceEnumerator* enumerator) 
{
	HRESULT hr;
	IMMDeviceCollection* devices;

	hr = enumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &devices);
	if (hr) { printHResult("IMMDeviceCollection->GetCount failed", hr); }

	UINT deviceCount;

	hr = devices->GetCount(&deviceCount);
	if (hr) { printHResult("IMMDeviceCollection->GetCount failed", hr); }

	printf("Found %d devices\n", deviceCount);

	int result = deviceCount;

	devices->Release();

	return result;
}

//###################### GET ID OF DEVICE ######################
LPWSTR getAudioEndpointID(IMMDeviceEnumerator* enumerator, int num) 
{
	HRESULT hr;
	IMMDeviceCollection* devices;
	IMMDevice* device;

	hr = enumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &devices);
	if (hr) { printHResult("IMMDeviceCollection->GetCount failed", hr); }

	hr = devices->Item(num, &device);
	if (hr) { printHResult("IMMDeviceCollection->GetCount failed", hr); }

	LPWSTR devID;
	hr = device->GetId(&devID);
	if (hr) { printHResult("IMMDeviceCollection->GetCount failed", hr); }

	LPWSTR result;
	result = devID;

	devices->Release();
	device->Release();

	return result;
}

//###################### GET NAME OF DEVICE ######################
string getAudioEndpointName(IMMDeviceEnumerator* enumerator, int num) 
{
	HRESULT hr;
	IMMDevice* device;
	IMMDeviceCollection* devices;

	hr = enumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &devices);
	if (hr) { printHResult("IMMDeviceCollection->GetCount failed", hr); }

	hr = devices->Item(num, &device);
	if (hr) { printHResult("IMMDeviceCollection->GetCount failed", hr); }

	LPWSTR devID;
	hr = device->GetId(&devID);
	if (hr) { printHResult("GetDefaultAudioEndpoint failed", hr); }

	IPropertyStore *pProps = NULL;

	hr = device->OpenPropertyStore(STGM_READ, &pProps);
	if (hr) { printHResult("GetDefaultAudioEndpoint failed", hr); }

	PROPVARIANT varName;

	PropVariantInit(&varName);

	hr = pProps->GetValue(PKEY_Device_FriendlyName, &varName);
	if (hr) { printHResult("GetDefaultAudioEndpoint failed", hr); }

	printf(" %S \n", varName.pwszVal);
	string result;

	result = CW2A((LPCWSTR)varName.pwszVal);

	devices->Release();
	device->Release();
	return result;
}
