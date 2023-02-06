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

string encodeCaesarCipher(string str, int shift)
{
    string temp = str;
    int length;
    length = (int)temp.length();
    for (int i = 0; i < length; i++)
    {
        if (isalpha(temp[i]))
        {
            for (int j = 0; j < shift; j++)
            {
                if (temp[i] == 'z')
                {
                    temp[i] = 'a';
                }
                else if (temp[i] == 'Z')
                {
                    temp[i] = 'A';
                }
                else
                {
                    temp[i]++;
                }
            }
        }
    }
    return temp;
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



    string hwidEnc = encodeCaesarCipher(getFirstHddSerialNumber(), 13);
    string hwid = hwidEnc.substr(hwidEnc.find_first_not_of(' '), (hwidEnc.find_last_not_of(' ') - hwidEnc.find_first_not_of(' ')) + 1);
    bool valid = false;
    string hwids[]
    {
        "JQ-JPP6L1PNQF41", // xo
        "76VR9CCNF", // cain
        "2V0220045336", // dance 291361016762073090
        "181008800460", // inured 262726913045299210
        "183915804850", // zach 196771793170726915
        "ZA1240SN0OYUWQ", // markten 245424555399577611
        "JQ-JPP6L5LF75E8", // baconboi666. 551221260798132224
        "19461R800107", // dr.p 455170364117483521
        "F3M8AO0XP74843E", // specialagentrandybeanz 162057261693272064
        "JQ-JPP4Z4YE9PRN", // plantyou 418868480251789312
        "183841801996", // fadedpanda 434527436319883274
        "JPP6L7EPSMG2", // mr.magnoliuh 353614238742216710
        "F599AR0ZO16684G", // dunkndrip 170666468436869120
        "M4M8C6X1", // dug 534957993297444874
        "975061CZX46F", // society 213696432828448771
        "JQ-JPP4W0IX2F3H", // heisenburg 364094436931665940
        "JQ-JPP3S6XAU9FI", // jon 153551019345707009
        "F2E5AK0U529051Z", // stevie 202247393704411136
        "F3CGAO0W938857I", // willy 199307241071837195
        "8RSN077918R701344492", // exi 151925993886580737
        "50026O724P09SS08", // rowdy 191391020888489984
        "JQ-JPP6L4FH71G6", // swiftyy 197855555023273984
        "0025_3858_91O3_05OQ", // sneeky_bassterd 619378961474125824
        "F21UAKNTP04302E", // wolfs F21UAKNTP04302E
        "JQ-JZP6A0Y2E552", // ethn 231984628037058561
        "F3CMAS0WO36121X", // bn0 564353215294996504
        "CAL42140000624440519", // niki 248176485041569794
        "50026O775N0P5POP", // ali 200656943834988553
        "M9NGJ872", // chris. 372468961297825792
        "M9NRZEPC", // bunnnyyyyyyy 220996148523958273
        "F3M2AO1X732148O", // aaassane 197769993411231745
        "50026O76825PPONQ", // adam 225997885374857217
        "JQ-JPNLI0934442", // premium_hub 590996930507702406
        "F41NAS0XN02957X", // rice pudding 363853301541896193
        "184189804175", // logan52s 215258068819247104
        "JQ-JPP3S4GFN97P", // jacobs.24 130727760741138433
        "M9N064AT", // justkrepto 225672176735485953
        "F33FAJOU441494S", // bess 371755236290854912
        "0000_0000_0100_0000_R4Q2_5POO_3N8P_5101.", // lil mqn 606454511434858500
        "JQ-JPP6L5QECKMU", // skull 349934936125341707
        "F598AR0Z858878S", // retarasdadfwetgf 186564791987470336
        "M6RDFIAI", // buddyy 301390756747411457
        "172182801294", // slim 246962071047110657
        "87XRHYENF", // rice pudding 363853301541896193
        "184898800625", // sizurpmits 237037995381882882
        "M9NATEWC", // sleepy 340931186253758475
        "J1Q21N16", // jarod 110523012352065536
        "L4W9C1WNF", // koba 293089468817342465
        "JQ-JPP6L0LQ9ARE", // pookie 668982421869428740
        "0023_0356_8000_24N6.", // change 224378343666089985
        "JSY1WDFP", // addison 333852979323076620
        "50026O778288Q6N3", // disagreed 491656664186093569
        "50026O77828S8SRO", // jonathan1 282964854690545666
        "F14PARNQ303918N", // mynameisjimmy 146140414842372097
        "M9N3FQT6", // hamish 279151032502583297
        "1835_0242_2016_0001_001O_444N_4442_099O." // ex 162941265195827200
    };
    for (int i = 0; i < (sizeof(hwids) / sizeof(hwids[0])); i++) {
        if (!valid) {
            int x = hwid.compare(hwids[i]);
            if (x == 0)
                valid = true;
        }
        if (valid)
            i = (sizeof(hwids) / sizeof(hwids[0]));
    }
    if (valid) {
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
    else {
        center(); console::prefixPrint("Your HWID is invalid.\n");
        center(); console::prefixPrint("Send this to xo to be validated: "); cout << "\"" << hwid << "\"\n";
        center(); console::prefixPrint("Exiting in 15 seconds...\n");
        center();
        Sleep(15000);
        DWORD pid = GetCurrentProcessId();
        KillProcessById(GetCurrentProcessId());
    }



    /*// set hwid ^
    string key;
    ifstream ifile("KEY.ROGUE");
    // set key ^
    // init auth
    if (ifile) {
        console::prefixPrint("File exists at keypath.");
        ifile >> key;
        cout << "\n";
        console::prefixPrint("Using stored key: ");
        cout << key << ".\n";
        bool login
        if (!valid) if invalid
        {
            console::prefixPrint("Invalid key, authentication failed.\n");
            console::prefixPrint("Exiting in 5 seconds...\n");
            Sleep(5000);
            DWORD pid = GetCurrentProcessId();
            handler::KillProcessById(pid);
        }
    }
    else {
        console::prefixPrint("File doesn't exist at key path.\n");
        console::prefixPrint("Enter your key to authenticate: ");
        std::cin >> key;
        cout << "\n";
        console::prefixPrint("Using key: ");
        cout << key << ".\n";
        bool valid = handler::keyloginregis(key);
        if (!valid)
        {
            console::prefixPrint("Invalid key, authentication failed.\n");
            console::prefixPrint("Exiting in 5 seconds...\n");
            Sleep(5000);
            DWORD pid = GetCurrentProcessId();
            handler::KillProcessById(pid);
        }
        // store key into file
    }
    thread menu(guii::menu);
    thread boot(guii::boot);
    menu.join();
    boot.join();*/
    for (;;) {
        Sleep(5);
        //char* wnd_title;
        //HWND hwnd = GetForegroundWindow(); 
        //GetWindowText(hwnd, wnd_title, sizeof(wnd_title));
        //cout << wnd_title;
    }
    return 0;
}