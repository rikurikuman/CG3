#include "RImGui.h"
#include "RWindow.h"
#include "RDirectX.h"

RImGui* RImGui::GetInstance()
{
    static RImGui instance;
    return &instance;
}

void RImGui::Init()
{
    GetInstance()->InitInternal();
}

void RImGui::NewFrame()
{
    ImGui_ImplDX12_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

void RImGui::Render()
{
    RImGui* instance = GetInstance();
    ImGui::Render();
    RDirectX::GetCommandList()->SetDescriptorHeaps(1, &instance->srvHeap);
    ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), RDirectX::GetCommandList());
}

void RImGui::Final()
{
    GetInstance()->FinalInternal();
}

void RImGui::InitInternal()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    //ImGuiIO& io = ImGui::GetIO(); (void)io; ‚È‚ñ‚©IO‚·‚é‚È‚ç•K—v‚É‚È‚é

    ImGui::StyleColorsDark();

    ImGui_ImplWin32_Init(RWindow::GetWindowHandle());

    this->device = RDirectX::GetDevice();
    this->srvHeap = RDirectX::GetSRVHeap();
    ImGui_ImplDX12_Init(
        RDirectX::GetDevice(),
        static_cast<int>(RDirectX::GetBackBufferSize()),
        DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
        RDirectX::GetSRVHeap(),
        RDirectX::GetSRVHeap()->GetCPUDescriptorHandleForHeapStart(),
        RDirectX::GetSRVHeap()->GetGPUDescriptorHandleForHeapStart()
    );
}

void RImGui::FinalInternal()
{
    ImGui_ImplDX12_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}
