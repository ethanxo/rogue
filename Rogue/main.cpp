#include "main.h"

#include <windows.h>
#include <iostream>
#include <string>
#include <fstream>
#include <chrono>
#include <thread>
#include <fstream>
#include <tchar.h>
#include <intrin.h>
#include <stdio.h>

using namespace std;

void console::prefixPrint(string text) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, 11);
    cout << "[ROGUE | EXO.BLACK] ";
    SetConsoleTextAttribute(hConsole, 7);
    cout << text;
}

void KillProcessById(DWORD pid) {
    HANDLE hnd;
    hnd = OpenProcess(SYNCHRONIZE | PROCESS_TERMINATE, TRUE, pid);
    TerminateProcess(hnd, 0);
}

string getFirstHddSerialNumber() {
    HANDLE h = CreateFileW(L"\\\\.\\PhysicalDrive0", 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
    if (h == INVALID_HANDLE_VALUE) return {};
    unique_ptr<std::remove_pointer<HANDLE>::type, void(*)(HANDLE)> hDevice{ h, [](HANDLE handle) {CloseHandle(handle); } };
    STORAGE_PROPERTY_QUERY storagePropertyQuery{};
    storagePropertyQuery.PropertyId = StorageDeviceProperty;
    storagePropertyQuery.QueryType = PropertyStandardQuery;
    STORAGE_DESCRIPTOR_HEADER storageDescriptorHeader{};
    DWORD dwBytesReturned = 0;
    if (!DeviceIoControl(hDevice.get(), IOCTL_STORAGE_QUERY_PROPERTY, &storagePropertyQuery, sizeof(STORAGE_PROPERTY_QUERY),
        &storageDescriptorHeader, sizeof(STORAGE_DESCRIPTOR_HEADER), &dwBytesReturned, NULL))
        return {};
    const DWORD dwOutBufferSize = storageDescriptorHeader.Size;
    std::unique_ptr<BYTE[]> pOutBuffer{ new BYTE[dwOutBufferSize]{} };
    if (!DeviceIoControl(hDevice.get(), IOCTL_STORAGE_QUERY_PROPERTY, &storagePropertyQuery, sizeof(STORAGE_PROPERTY_QUERY),
        pOutBuffer.get(), dwOutBufferSize, &dwBytesReturned, NULL))
        return {};
    STORAGE_DEVICE_DESCRIPTOR* pDeviceDescriptor = reinterpret_cast<STORAGE_DEVICE_DESCRIPTOR*>(pOutBuffer.get());
    const DWORD dwSerialNumberOffset = pDeviceDescriptor->SerialNumberOffset;
    if (dwSerialNumberOffset == 0) return {};
    const char* serialNumber = reinterpret_cast<const char*>(pOutBuffer.get() + dwSerialNumberOffset);
    return serialNumber;
}

void center() {
    cout << "                                  ";
}

int main() {
    SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
    SetConsoleTitle(_T("ROGUE | EXO.BLACK"));
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, 11);
    cout << "                                   _______    ______    ______   __    __  ________ \r\n"
        << "                                  |       \\  \/      \\  \/      \\ |  \\  |  \\|        \\\r\n"
        << "                                  | $$$$$$$\\|  $$$$$$\\|  $$$$$$\\| $$  | $$| $$$$$$$$\r\n"
        << "                                  | $$__| $$| $$  | $$| $$ __\\$$| $$  | $$| $$__    \r\n"
        << "                                  | $$    $$| $$  | $$| $$|    \\| $$  | $$| $$  \\   \r\n"
        << "                                  | $$$$$$$\\| $$  | $$| $$ \\$$$$| $$  | $$| $$$$$   \r\n"
        << "                                  | $$  | $$| $$__\/ $$| $$__| $$| $$__\/ $$| $$_____ \r\n"
        << "                                  | $$  | $$ \\$$    $$ \\$$    $$ \\$$    $$| $$     \\\r\n"
        << "                                   \\$$   \\$$  \\$$$$$$   \\$$$$$$   \\$$$$$$  \\$$$$$$$$\n\n";
    SetConsoleTextAttribute(hConsole, 7);

    if (true) {
        center(); console::prefixPrint("Your HWID is valid.\n");
        thread menu(guii::menu); 
        center(); console::prefixPrint("GUI thread started...\n");
        thread boot(guii::boot);
        center(); console::prefixPrint("Input thread started...\n");
        center(); console::prefixPrint("Recoil thread started...\n");
        menu.join(); boot.join();
        center(); console::prefixPrint("Synced threads...\n");
        center(); console::prefixPrint("Enjoy!\n");
        center();
    }
    for (;;) {
        Sleep(5);
        char* wnd_title;
        HWND hwnd = GetForegroundWindow(); 
        GetWindowText(hwnd, wnd_title, sizeof(wnd_title));
        cout << wnd_title;
    }
    return 0;
}
