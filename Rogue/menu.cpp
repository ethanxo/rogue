// dear imgui - standalone example application for DirectX 11
// If you are new to dear imgui, see examples/README.txt and documentation at the top of imgui.cpp.

#pragma comment(lib, "dwmapi.lib")

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_win32.h"
#include "ImGui/imgui_impl_dx11.h"
#include <d3d11.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <tchar.h>
#include "main.h"
#include <iostream>
#include <vector>
#include "Dwmapi.h"
#include <string>
#include <chrono>
#include <thread>

// Data
static ID3D11Device* g_pd3dDevice = NULL;
static ID3D11DeviceContext* g_pd3dDeviceContext = NULL;
static IDXGISwapChain* g_pSwapChain = NULL;
static ID3D11RenderTargetView* g_mainRenderTargetView = NULL;

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

struct {
    int ctrl_amtx = 100;
    int ctrl_amty = 100;
    float sens = 0.3;
    bool adsonly = true;
    bool recoilenabled = true;
    int smoothing = 10;
    int selectedWeapon = 0;
    int selectedSight = 0;
    int selectedAttachment = 0;
    float sightMulitplier = 1.0;
    float attachmentMulitplier = 1.0;
} Settings;

const char* weapons[]{ "Assault Rifle *BETA*", "LR-300", "MP5A4", "Custom SMG", "Thompson", "Python", "Semi-Automatic Rifle", "M39 Rifle", "M92 Pistol", "Semi-Automatic Pistol", "M249 *BETA*" };
const char* sights[]{ "<NONE>", "Handmade Sight", "Holosight", "8x Sight", "16x Sight" };
const char* attachments[]{ "<NONE>", "Silencer", "Muzzle Brake" };

namespace Tbls {
    // slpTime = 1000 / (RPM / 60) : https://rustlabs.com/

    struct {
        float offset_x[30] = { -171.692106458876, 26.4533949487003, -272.930522023896, -209.861566990982, -0.881442317256221, 82.4909445680218, 145.130569552198, 187.037638676019,208.211857595561, 208.653285179609, 188.362098034517, 147.338325594678, 85.5819089913074, 4.58099223949291, -74.1648379667306, -136.711538573842, -183.082362751902, -213.278429007825, -227.298206753201, -225.143491485972, -206.81251714259, -172.306696573893, -121.624911272967, -54.7683386155117, 59.2806309829524, 178.818997041679, 240.299436781142, 243.633882499036, 188.820980213307, 75.8620250372239 };
        float offset_y[30] = { 247.400160189028, 227.189446672981, 206.48279307801, 185.280199404116, 163.581636216905, 141.387280122733, 118.69701338403, 95.5106005256559, 71.8285124978901, 47.6502783504533, 48.9590491777585, 90.57292231807, 123.518837822808, 146.882916674202, 160.665276609821, 164.866506317515, 159.485192946446, 144.523573510441, 119.978822307822, 85.8532940899884, 42.1460469563802, 24.5492252302759, 68.6563091513551, 103.394308207948, 126.660900351442, 138.458675808377, 138.785515302493, 127.642596209491, 105.02921210395, 70.9458339361497 };
        float slpTime = 133.333333333;
        int mag = 30;
    } AK;

    struct {
        float offset_x[30] = { -12.1438475670638, -30.6899082145573, -49.9095188246833, -71.070083865413, -77.9235215834629, -70.3153639663885, -48.2455595040027, -11.0308623608248, 45.1658390186451, 85.3424896428615, 99.5471448074153, 87.7797530021197, 50.0403068683766, -2.16488163044423, -27.5313923204387, -45.9716312679244, -60.08455046901, -69.8704925584204, -75.3290859269507, -76.4606617115162, -73.2648519822109, -65.7421865580994, -53.8920178825473, -37.714875774619, -16.7375729407793, 66.5107774145809, 151.244769861669, 178.811432402811, 149.209778985859, 62.4405749050188 };
        float offset_y[30] = { 124.065184298857, 153.530188548712, 163.805425902944, 154.890896361551, 126.993391248915, 102.313359578451, 87.9411344174986, 74.811393831983, 62.924020084334, 52.2791309121214, 42.8764908402054, 34.7164530812958, 27.9829237196181, 24.4671621440369, 21.979131816346, 19.7774392587167, 17.8622022087191, 16.2335384039231, 14.8912123691889, 13.8354595796562, 13.0660445601852, 12.5832027859158, 12.3866987817081, 12.4766502851321, 12.8532927713276, 13.516155290015, 14.4655910539038, 15.7014823254244, 17.2237113670067, 19.0325136537905 };
        float slpTime = 120.0;
        int mag = 30;
    } LR;

    struct {
        float offset_x[30] = { 0, 0, 0, 63.0108883351419, 145.297366895793, 161.981111691322, 113.062152156123, 4.09520702597536, -89.3520425867151, -122.979776358899, -97.4948906604155, -18.4947767375428, 38.2433702916275, 68.6359405517578, 82.9906816835757, 81.3077408590434, 63.5867648654514, 29.8284012594341, -7.06413645803193, -26.5838187417866, -42.0157114664714, -53.6628122682925, -61.5245913281853, -65.6016962027844, -65.8935087698477, -62.4005882828324, -55.1223754882813, -44.0594443568477, -29.2113533726445, -10.5781025356717 };
        float offset_y[30] = { 107.263974201532, 144.861818831644, 163.978029180456, 164.612664116753, 146.76570892334, 110.437251903393, 64.7276419180411, 53.2149091178988, 47.5214734489535, 42.1376875889154, 37.0635515377845, 32.2991830331308, 27.8444643373843, 23.6991599754051, 19.8638586350429, 16.338089366018, 13.1218521683304, 10.2156179922598, 7.61868041238667, 5.33151037899064, 3.35422562964169, 1.68635521406009, 0.328252344955633, -0.720318452811536, 1.45888622896171, 1.88815740891445, -2.00766104239005, 1.81716165424865, 1.3177188826196, -0.508155351803627 };
        float slpTime = 100.0;
        int mag = 30;
    } MP5;

    struct {
        float offset_x[24] = { -80.4017502584575, -39.1243637343984, -2.35100587209066, 36.0607732961207, 60.3522213152897, 66.6770762131538, 55.0351724212552, 25.4267233389395, -18.3111502800459, -52.1476401223077, -66.8617144778923, -62.4359555450487, -38.8705288922345, 1.4809399475286, 36.6321390057787, 56.7895670731862, 62.1397940464962, 52.6830737973437, 28.190678302153, -5.69931518884353, -27.3304073898881, -36.3971421748032, -33.4065987004174, -18.35897564888 };
        float offset_y[24] = { 161.161466881081, 159.814519646727, 155.4756105682, 148.144739645499, 137.821951030213, 124.507185853558, 108.200591287495, 92.8173536135827, 84.4745871461468, 76.9246654745973, 69.9434162657938, 63.5301330943167, 57.6856401231554, 52.4092309268904, 47.7013764558015, 43.5619589723187, 39.9906252637322, 36.9879640178916, 34.5536220220872, 32.687481538749, 31.3900135181568,30.6607470100309, 30.4997997519411, 30.9071717438874 };
        float slpTime = 100.0;
        int mag = 24;
    } CUSTOM;

    struct {
        float offset_x[20] = { -91.3521537074336, -33.5026817557252, 20.6930696228404, 65.5462786003395, 79.8284381995966, 63.064138830444, 5.3503447403142, -44.7148305398447, -76.9274709401307, -76.0667467558825, -42.1324482670537, 16.0291901341191, 57.9482648107741, 74.1816560427348, 64.8283222575247, 29.8881972277606, -16.2409485122304, -39.7798014275822, -42.4185119293354, -24.2223066312295 };
        float offset_y[20] = { 193.327871369727, 190.526570802853, 182.555222216948, 169.413707874439, 151.10210136131, 127.620579283914, 108.129477795259, 96.7669781343437, 86.3860565939067, 76.9876550745081, 68.571420363438, 61.1372347231264, 54.6852158911434, 49.2153638674889, 44.727560914593, 41.2221602451654, 38.6988086464964, 37.157623856156, 36.5987236117139, 37.0215192253207 };
        float slpTime = 129.870129870;
        int mag = 20;
    } TOMMY;

    struct {
        float offset_y = 720;
        //float slpTime = 150.0;
        float slpTime = 25.0;
        int mag = 1;
    } PYTHON;

    struct {
        float offset_y = 409;
        //float slpTime = 174.927113703;  FIX
        float slpTime = 87.0;
        int mag = 1;
    } SAR;

    struct {
        float offset_y = 300;
        //float slpTime = 200;  FIX
        float slpTime = 25.0;
        int mag = 1;     
    } M39;

    struct {
        float offset_y = 334;
        //float slpTime = 150; 
        float slpTime = 50.0;
        int mag = 1;
    } M92;

    struct {
        float offset_y = 346;
        //float slpTime = 150;  FIX
        float slpTime = 25.0;
        int mag = 1;
    } P2;

    struct {
        float offset_y = 365;
        float slpTime = 120;
        int mag = 100;
    } M249;
}

static std::chrono::time_point<std::chrono::steady_clock> get_time() {
    return std::chrono::high_resolution_clock::now();
}

static void accurate_sleep(int64_t wanted_sleep_us, std::chrono::time_point<std::chrono::steady_clock> start) {
    auto truncated = (wanted_sleep_us - std::chrono::duration_cast<std::chrono::microseconds>(get_time() - start).count()) / 1000;
    while (std::chrono::duration_cast<std::chrono::microseconds>(get_time() - start).count() < wanted_sleep_us)
    {
        if (truncated)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(truncated));
            truncated = 0;
        }
    }
}

void comp(float slpTime, float offset_x, float offset_y, int smoothing) {
    std::chrono::time_point<std::chrono::steady_clock> start = get_time();
    for (int i = 0; i < smoothing; i++) {
        mouse_event(1, (offset_x / smoothing), (offset_y / smoothing), 0, 0);
        accurate_sleep((slpTime * 1000) / smoothing, start);
        start = get_time();
    }
    start = get_time();
    mouse_event(1, (int)offset_x % smoothing, (int)offset_y % smoothing, 0, 0);
    //accurate_sleep(((int)slpTime * 1000) % smoothing, start);
}

void rec() {
    for (;;) {
        int i = 0;
        if (Settings.recoilenabled) {
            while (GetAsyncKeyState(0x01) & 0x8000 && GetAsyncKeyState(0x02) & 0x8000) {
                //float hipfiremult;
                switch (Settings.selectedSight) {
                case 0:
                    Settings.sightMulitplier = 1.0;
                    break;
                case 1:
                    Settings.sightMulitplier = 0.8;
                    break;
                case 2:
                    Settings.sightMulitplier = 1.18605;
                    break;
                case 3:
                    Settings.sightMulitplier = 3.83721;
                    break;
                case 4:
                    Settings.sightMulitplier = 7.65116;
                    break;
                }
                switch (Settings.selectedAttachment) {
                case 0:
                    Settings.attachmentMulitplier = 1.0;
                    break;
                case 1:
                    Settings.attachmentMulitplier = 0.8;
                    break;
                case 2:
                    Settings.attachmentMulitplier = 0.5;
                    break;
                }
                float mult = Settings.attachmentMulitplier * Settings.sightMulitplier;// *hipfiremult;
                float xcont = (float)Settings.ctrl_amtx / 100;
                float ycont = (float)Settings.ctrl_amty / 100;
                switch (Settings.selectedWeapon) {
                case 0:
                    if (i < Tbls::AK.mag)
                        comp(Tbls::AK.slpTime - 4.6 /*tune small*/, (Tbls::AK.offset_x[i] / (Settings.sens / 0.1)) * mult * xcont, (Tbls::AK.offset_y[i] / (Settings.sens / 0.1)) * mult * ycont, Settings.smoothing);
                    break;
                case 1:
                    if (i < Tbls::LR.mag)
                        comp(Tbls::LR.slpTime - 7, (Tbls::LR.offset_x[i] / (Settings.sens / 0.1)) * mult * xcont, (Tbls::LR.offset_y[i] / (Settings.sens / 0.1)) * mult * ycont, Settings.smoothing);
                    break;
                case 2:
                    if (i < Tbls::MP5.mag)
                        comp(Tbls::MP5.slpTime - 7, (Tbls::MP5.offset_x[i] / (Settings.sens / 0.1)) * mult * xcont, (Tbls::MP5.offset_y[i] / (Settings.sens / 0.1)) * mult * ycont, Settings.smoothing);
                    break;
                case 3: 
                    if (i < Tbls::CUSTOM.mag) {
                        if (Settings.sightMulitplier == 1.0)
                            comp(Tbls::CUSTOM.slpTime - 6.5, 0.95 * (Tbls::CUSTOM.offset_x[i] / (Settings.sens / 0.1)) * mult * xcont, 0.85 * (Tbls::CUSTOM.offset_y[i] / (Settings.sens / 0.1)) * mult * ycont, Settings.smoothing);
                        else
                            comp(Tbls::CUSTOM.slpTime - 6.5, (Tbls::CUSTOM.offset_x[i] / (Settings.sens / 0.1)) * mult * xcont, (Tbls::CUSTOM.offset_y[i] / (Settings.sens / 0.1)) * mult * ycont, Settings.smoothing);
                    }
                    break;
                case 4:
                    if (i < Tbls::TOMMY.mag) {
                        if (Settings.sightMulitplier == 1.0)
                            comp(Tbls::TOMMY.slpTime - 6.5, 0.95 * (Tbls::TOMMY.offset_x[i] / (Settings.sens / 0.1)) * mult * xcont, 0.85 * (Tbls::TOMMY.offset_y[i] / (Settings.sens / 0.1)) * mult * ycont, Settings.smoothing);
                        else
                            comp(Tbls::TOMMY.slpTime - 6.5, (Tbls::TOMMY.offset_x[i] / (Settings.sens / 0.1)) * mult * xcont, (Tbls::TOMMY.offset_y[i] / (Settings.sens / 0.1)) * mult * ycont, Settings.smoothing);
                    }
                    break;
                case 5:
                    if (i < Tbls::PYTHON.mag) {
                        if (GetAsyncKeyState(0xA2))
                            comp(Tbls::PYTHON.slpTime, 0, (Tbls::PYTHON.offset_y / (Settings.sens / 0.1)) * mult * ycont * 0.5, Settings.smoothing);
                        else
                            comp(Tbls::PYTHON.slpTime, 0, (Tbls::PYTHON.offset_y / (Settings.sens / 0.1)) * mult * ycont, Settings.smoothing);
                    }
                    break;
                case 6:
                    if (i < Tbls::SAR.mag) {
                        if (GetAsyncKeyState(0xA2))
                            comp(Tbls::SAR.slpTime, 0, (Tbls::SAR.offset_y / (Settings.sens / 0.1)) * mult * ycont * 0.5, Settings.smoothing);
                        else
                            comp(Tbls::SAR.slpTime, 0, (Tbls::SAR.offset_y / (Settings.sens / 0.1)) * mult * ycont, Settings.smoothing);
                    }
                    break;
                case 7:
                    if (i < Tbls::M39.mag) {
                        if (GetAsyncKeyState(0xA2))
                            comp(Tbls::M39.slpTime, 0, (Tbls::M39.offset_y / (Settings.sens / 0.1)) * mult * ycont * 0.5, Settings.smoothing);
                        else
                            comp(Tbls::M39.slpTime, 0, (Tbls::M39.offset_y / (Settings.sens / 0.1)) * mult * ycont, Settings.smoothing);
                    }
                    break;
                case 8:
                    if (i < Tbls::M92.mag) {
                        if (GetAsyncKeyState(0xA2))
                            comp(Tbls::M92.slpTime, 0, (Tbls::M92.offset_y / (Settings.sens / 0.1)) * mult * ycont * 0.5, Settings.smoothing);
                        else
                            comp(Tbls::M92.slpTime, 0, (Tbls::M92.offset_y / (Settings.sens / 0.1)) * mult * ycont, Settings.smoothing);
                    }
                    break;
                case 9:
                    if (i < Tbls::P2.mag) {
                        if (GetAsyncKeyState(0xA2))
                            comp(Tbls::P2.slpTime, 0, (Tbls::P2.offset_y / (Settings.sens / 0.1)) * mult * ycont * 0.5, Settings.smoothing);
                        else
                            comp(Tbls::P2.slpTime, 0, (Tbls::P2.offset_y / (Settings.sens / 0.1)) * mult * ycont, Settings.smoothing);
                    }
                    break;
                case 10:
                    if (i < Tbls::M249.mag) {
                        if (GetAsyncKeyState(0xA2))
                            comp(Tbls::M249.slpTime - 0 /*tune*/, 0, (Tbls::M249.offset_y / (Settings.sens / 0.1)) * mult * ycont * 0.5, Settings.smoothing);
                        else
                            comp(Tbls::M249.slpTime - 0 /*tune*/, 0, (Tbls::M249.offset_y / (Settings.sens / 0.1)) * mult * ycont, Settings.smoothing);
                    }
                    break;
                }
                i++;
            }
        }
        Sleep(2);
    }
    exit(0);
}

void input() {
    for (;;) {
        while (GetAsyncKeyState(0x2D))
        {
            Settings.recoilenabled = !Settings.recoilenabled;
            Sleep(500);
        }
        while (GetAsyncKeyState(0x26))
        {
            Settings.adsonly = !Settings.adsonly;
            Sleep(500);
        }
        while (GetAsyncKeyState(0x25))
        {
            if (Settings.selectedWeapon == IM_ARRAYSIZE(weapons))
                Settings.selectedWeapon = 0;
            else
                Settings.selectedWeapon++;
            Sleep(250);
        }
        while (GetAsyncKeyState(0x28))
        {
            if (Settings.selectedSight == IM_ARRAYSIZE(sights))
                Settings.selectedSight = 0;
            else
                Settings.selectedSight++;
            Sleep(250);
        }
        while (GetAsyncKeyState(0x27))
        {
            if (Settings.selectedAttachment == IM_ARRAYSIZE(attachments))
                Settings.selectedAttachment = 0;
            else
                Settings.selectedAttachment++;
            Sleep(250);
        }
        Sleep(2);
    }
    exit(0);
}

void guii::boot() {
    CreateThread(0, 0, (LPTHREAD_START_ROUTINE)input, 0, 0, 0);
    CreateThread(0, 0, (LPTHREAD_START_ROUTINE)rec, 0, 0, 0);
}

int guii::menu()
{
    // Create application window
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T("ROGUE"), NULL };
    ::RegisterClassEx(&wc);

    HWND hwnd = ::CreateWindow(wc.lpszClassName, _T("ROGUE"), WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, 10, 10, 357, 601, NULL, NULL, wc.hInstance, NULL);
    SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

    // Initialize Direct3D
    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        ::UnregisterClass(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    // Show the window
    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer bindings
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Read 'docs/FONTS.txt' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != NULL);

    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;

    ImVec4 clear_color = ImVec4(0, 0, 0, 0);

    // Main loop
    MSG msg;
    ZeroMemory(&msg, sizeof(msg));
    while (msg.message != WM_QUIT)
    {
        // Poll and handle messages (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        if (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            continue;
        }

        // Start the Dear ImGui frame
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("                ROGUE | EXO.BLACK", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove);

        ImGuiStyle* style = &ImGui::GetStyle();
        ImGui::GetStyle().WindowRounding = 0.0f;
        //style->FrameBorderSize = 1.0;
        //style->WindowBorderSize = 3;
        ImGui::SetWindowSize(ImVec2(500, 601));
        // 500, 601
        ImGui::SetWindowPos(ImVec2(0, 0));
        io.IniFilename = NULL;

        ImGui::Text("Settings:");
        ImGui::Checkbox("Enable Recoil (Insert)", &Settings.recoilenabled);
        ImGui::Checkbox("ADS Only (Up Arrow)", &Settings.adsonly);

        ImGui::Text("Weapon: (Left Arrow)");
        ImGui::ListBox("", &Settings.selectedWeapon, weapons, IM_ARRAYSIZE(weapons), 5);
        ImGui::Text("Sight: (Down Arrow)");
        ImGui::ListBox(" ", &Settings.selectedSight, sights, IM_ARRAYSIZE(sights), IM_ARRAYSIZE(sights));
        ImGui::Text("Attachment: (Right Arrow)");
        ImGui::ListBox("  ", &Settings.selectedAttachment, attachments, IM_ARRAYSIZE(attachments), IM_ARRAYSIZE(attachments));

        ImGui::Text("Smoothing:");
        ImGui::SliderInt("    ", &Settings.smoothing, 1, 10);

        ImGui::Text("Control Amount (X):");
        ImGui::SliderInt("     ", &Settings.ctrl_amtx, 0, 100);

        ImGui::Text("Control Amount (Y):");
        ImGui::SliderInt("      ", &Settings.ctrl_amty, 0, 100);

        ImGui::Text("Sensitivity:");
        ImGui::InputFloat("       ", &Settings.sens, 0.1, 0.5);

        ImGui::End();

        // Rendering
        ImGui::Render();

        g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
        g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, (float*)&clear_color);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        g_pSwapChain->Present(1, 0); // Present with vsync
        //g_pSwapChain->Present(0, 0); // Present without vsync
    }

    // Cleanup
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClass(wc.lpszClassName, wc.hInstance);

    return 0;
}

// Helper functions

bool CreateDeviceD3D(HWND hWnd)
{
    // Setup swap chain
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
    //createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    if (D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext) != S_OK)
        return false;

    CreateRenderTarget();
    return true;
}

void CleanupDeviceD3D()
{
    CleanupRenderTarget();
    if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = NULL; }
    if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = NULL; }
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
}

void CreateRenderTarget()
{
    ID3D11Texture2D* pBackBuffer;
    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_mainRenderTargetView);
    pBackBuffer->Release();
}

void CleanupRenderTarget()
{
    if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = NULL; }
}



// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
        {
            CleanupRenderTarget();
            g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
            CreateRenderTarget();
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProc(hWnd, msg, wParam, lParam);
}
