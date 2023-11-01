#include "../Common/StorageDevice.hpp"

#include <WbemIdl.h>
#include <Windows.h>
#include <comdef.h>

#include <format>
#include <locale>
#include <memory>
#include <optional>
#include <stdexcept>

#pragma comment(lib, "wbemuuid.lib")


namespace sedmgr {

struct ComPtrDeleter {
    template <class T>
    void operator()(T ptr) const {
        if (ptr != nullptr) {
            ptr->Release();
        }
    }
};

struct CoUninitializer {
    ~CoUninitializer() { CoUninitialize(); }
};


template <class T>
using ComPtr = std::unique_ptr<T, ComPtrDeleter>;


std::vector<StorageDeviceLabel> EnumerateStorageDevices() {
    if (FAILED(CoInitializeEx(0, COINIT_MULTITHREADED))) {
        throw std::runtime_error("failed to initialize COM");
    }
    CoUninitializer coUninitalzier;

    if (FAILED(CoInitializeSecurity(nullptr, -1, nullptr, nullptr, RPC_C_AUTHN_LEVEL_DEFAULT, RPC_C_IMP_LEVEL_IMPERSONATE, nullptr, EOAC_NONE, nullptr))) {
        throw std::runtime_error("failed to set up COM security levels");
    }


    IWbemLocator* wbemLocatorPtr = nullptr;
    if (FAILED(CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER, IID_IWbemLocator, reinterpret_cast<void**>(&wbemLocatorPtr)))) {
        throw std::runtime_error("failed to create IWbemLocator");
    }
    const auto wbemLocator = ComPtr<IWbemLocator>(wbemLocatorPtr);


    IWbemServices* wbemServicesPtr = nullptr;
    if (FAILED(wbemLocator->ConnectServer(_bstr_t(LR"(ROOT\Microsoft\Windows\Storage)"), nullptr, nullptr, nullptr, 0, nullptr, nullptr, &wbemServicesPtr))) {
        throw std::runtime_error("failed to connect to WMI services");
    }
    const auto wbemServices = ComPtr<IWbemServices>(wbemServicesPtr);


    if (FAILED(CoSetProxyBlanket(
            wbemServices.get(),
            RPC_C_AUTHN_WINNT,
            RPC_C_AUTHZ_NONE,
            nullptr,
            RPC_C_AUTHN_LEVEL_CALL,
            RPC_C_IMP_LEVEL_IMPERSONATE,
            nullptr,
            EOAC_NONE))) {
        throw std::runtime_error("failed to set proxy blanket");
    }

    IEnumWbemClassObject* enumeratorPtr = nullptr;
    if (FAILED(wbemServices->ExecQuery(
            bstr_t("WQL"),
            bstr_t("SELECT * FROM MSFT_Disk"),
            WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
            NULL,
            &enumeratorPtr))) {
        throw std::runtime_error("failed to query disk drives");
    }
    const auto enumerator = ComPtr<IEnumWbemClassObject>(enumeratorPtr);

    ULONG uReturn = 0;

    std::vector<StorageDeviceLabel> devices;
    IWbemClassObject* pclsObj = nullptr;
    while (true)
    {
        if (0 != enumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn)) {
            break;
        }

        VARIANT vtProp;
        VariantInit(&vtProp);
        std::optional<std::string> path;
        std::optional<unsigned> busType;
        if (!FAILED(pclsObj->Get(L"BusType", 0, &vtProp, 0, 0))) {
            busType = vtProp.uiVal;
            VariantClear(&vtProp);
        }
        if (!FAILED(pclsObj->Get(L"Path", 0, &vtProp, 0, 0))) {
            const auto& facet = std::use_facet<std::codecvt<wchar_t, char, std::mbstate_t>>(std::locale());
            using InputChar = std::remove_pointer_t<BSTR>;
            std::basic_string<InputChar> input{ vtProp.bstrVal };
            std::string output(input.size() * facet.max_length(), '\0');
            const InputChar* fromNext;
            char* toNext;
            std::mbstate_t state;
            facet.out(state, input.data(), input.data() + input.size(), fromNext, output.data(), output.data() + output.size(), toNext);
            output.resize(toNext - output.data());
            path = output;
            VariantClear(&vtProp);
        }

        if (busType && path) {
            eStorageDeviceInterface interfaceType = [&] {
                switch (*busType) {
                    case 1: return eStorageDeviceInterface::SCSI;
                    case 3: return eStorageDeviceInterface::ATA;
                    case 11: return eStorageDeviceInterface::SATA;
                    case 12: return eStorageDeviceInterface::SD;
                    case 13: return eStorageDeviceInterface::MMC;
                    case 17: return eStorageDeviceInterface::NVME;
                    default: return eStorageDeviceInterface::OTHER;
                }
            }();
            devices.emplace_back(std::move(*path), interfaceType);
        }

        pclsObj->Release();
    }

    return devices;
}

} // namespace sedmgr