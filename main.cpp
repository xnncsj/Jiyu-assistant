#include <wx/wx.h>
#include <wx/app.h>
#include <wx/frame.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/panel.h>
#include <wx/statbmp.h>
#include <wx/artprov.h>
#include <wx/colour.h>
#include <wx/font.h>
#include <wx/icon.h>
#include <wx/dcclient.h>
#include <wx/display.h>
#include <windows.h>
#include <tlhelp32.h>
#include <string>

// 添加DPI感知清单
#ifdef _MSC_VER
// 对于Visual Studio编译器
#pragma comment(linker, "\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif

// 定义颜色
const wxColour BG_COLOR(230, 236, 240);
const wxColour BUTTON_COLOR(41, 128, 185);
const wxColour BUTTON_HOVER_COLOR(52, 152, 219);
const wxColour TEXT_COLOR(44, 62, 80);
const wxColour TITLE_COLOR(41, 128, 185);

// 自定义按钮类
class CustomButton : public wxButton
{
public:
    CustomButton(wxWindow* parent, wxWindowID id, const wxString& label)
        : wxButton(parent, id, label, wxDefaultPosition, wxDefaultSize)  // 使用默认大小，稍后通过SetMinSize设置
    {
        SetBackgroundColour(BUTTON_COLOR);
        SetForegroundColour(*wxWHITE);
        
        // 获取DPI缩放因子
        wxClientDC dc(parent);
        double scaleX = static_cast<double>(dc.GetPPI().x) / 96.0;
        double scaleY = static_cast<double>(dc.GetPPI().y) / 96.0;
        double scale = (scaleX > scaleY) ? scaleX : scaleY;
        if (scale < 1.0) scale = 1.0;
        
        // 设置字体
        wxFont font = GetFont();
        font.SetPointSize(static_cast<int>(10 * scale));
        font.SetWeight(wxFONTWEIGHT_BOLD);
        SetFont(font);
        
        // 设置最小高度
        SetMinSize(wxSize(-1, static_cast<int>(40 * scale)));
        
        Bind(wxEVT_ENTER_WINDOW, &CustomButton::OnMouseEnter, this);
        Bind(wxEVT_LEAVE_WINDOW, &CustomButton::OnMouseLeave, this);
    }
    
private:
    void OnMouseEnter(wxMouseEvent& event)
    {
        // 高亮背景色，同时将文字改为深色
        SetBackgroundColour(wxColour(173, 216, 230)); // 浅蓝色背景
        SetForegroundColour(wxColour(0, 51, 102));    // 深蓝色文字
        Refresh();
    }
    
    void OnMouseLeave(wxMouseEvent& event)
    {
        // 恢复正常状态
        SetBackgroundColour(BUTTON_COLOR);
        SetForegroundColour(*wxWHITE);
        Refresh();
    }
};

class JiyuApp : public wxApp
{
public:
    virtual bool OnInit() override;
};

class MainFrame : public wxFrame
{
public:
    MainFrame(const wxString& title);

private:
    void OnCloseJiyu(wxCommandEvent& event);
    void OnUninstallNetworkDriver(wxCommandEvent& event);
    void OnCloseMasterHelper(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);

    bool KillProcessByName(const wchar_t* processName);
    bool ExecuteCommand(const wchar_t* command);
    
    // 用于状态更新的成员变量
    wxStaticText* statusText;
};

wxIMPLEMENT_APP(JiyuApp);

bool JiyuApp::OnInit()
{
    // 启用高DPI支持
    #ifdef __WXMSW__
    SetProcessDPIAware();
    #endif
    
    // 设置应用程序名称，用于配置文件等
    SetAppName(wxT("极域解除工具"));
    
    MainFrame* frame = new MainFrame(wxT("极域解除工具"));
    // 不再设置固定客户区大小，由Fit()函数自动计算
    frame->Center();
    frame->Show();
    return true;
}

MainFrame::MainFrame(const wxString& title)
    : wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxDefaultSize, 
              wxDEFAULT_FRAME_STYLE & ~(wxRESIZE_BORDER | wxMAXIMIZE_BOX))
{
    // 获取当前显示器DPI缩放系数
    wxClientDC dc(this);
    double scaleX = static_cast<double>(dc.GetPPI().x) / 96.0;
    double scaleY = static_cast<double>(dc.GetPPI().y) / 96.0;
    double scale = (scaleX > scaleY) ? scaleX : scaleY;
    
    // 如果缩放系数小于1，则设为1（避免界面变小）
    if (scale < 1.0) scale = 1.0;
    
    // 调整窗口尺寸和字体大小 - 增加窗口高度
    int scaledWidth = static_cast<int>(360 * scale);
    int scaledHeight = static_cast<int>(480 * scale); // 增加高度以确保所有内容都可见
    
    // 设置图标 - 使用内置图标，不再尝试加载资源
    SetIcon(wxArtProvider::GetIcon(wxART_INFORMATION, wxART_FRAME_ICON));
    
    // 创建主面板
    wxPanel* panel = new wxPanel(this, wxID_ANY);
    panel->SetBackgroundColour(BG_COLOR);
    
    // 创建垂直布局
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    
    // 添加标题
    wxStaticText* titleText = new wxStaticText(panel, wxID_ANY, wxT("极域解除工具"));
    wxFont titleFont = titleText->GetFont();
    titleFont.SetPointSize(static_cast<int>(16 * scale));
    titleFont.SetWeight(wxFONTWEIGHT_BOLD);
    titleText->SetFont(titleFont);
    titleText->SetForegroundColour(TITLE_COLOR);
    
    // 添加说明文字
    wxStaticText* descText = new wxStaticText(panel, wxID_ANY, 
        wxT("本工具可以帮助解除极域电子教室的限制"));
    wxFont descFont = descText->GetFont();
    descFont.SetPointSize(static_cast<int>(descFont.GetPointSize() * scale));
    descText->SetFont(descFont);
    descText->SetForegroundColour(TEXT_COLOR);
    
    // 创建按钮
    CustomButton* closeJiyuBtn = new CustomButton(panel, wxID_ANY, wxT("关闭极域"));
    CustomButton* uninstallNetworkDriverBtn = new CustomButton(panel, wxID_ANY, wxT("卸载网络驱动"));
    CustomButton* closeMasterHelperBtn = new CustomButton(panel, wxID_ANY, wxT("关闭网络限制"));
    CustomButton* exitBtn = new CustomButton(panel, wxID_ANY, wxT("退出"));
    
    // 添加按钮提示
    closeJiyuBtn->SetToolTip(wxT("强制关闭StudentMain.exe进程"));
    uninstallNetworkDriverBtn->SetToolTip(wxT("执行sc stop tdnetfilter命令"));
    closeMasterHelperBtn->SetToolTip(wxT("强制关闭MasterHelper.exe进程"));
    
    // 添加状态文本
    statusText = new wxStaticText(panel, wxID_ANY, wxT("准备就绪"));
    statusText->SetForegroundColour(wxColour(0, 120, 0));
    wxFont statusFont = statusText->GetFont();
    statusFont.SetPointSize(static_cast<int>(9 * scale));
    statusText->SetFont(statusFont);
    
    // 添加版权信息
    wxStaticText* copyrightText = new wxStaticText(panel, wxID_ANY, 
        wxT("© 2025 极域助手 - 仅供学习交流使用"));
    copyrightText->SetForegroundColour(wxColour(128, 128, 128));
    wxFont smallFont = copyrightText->GetFont();
    smallFont.SetPointSize(static_cast<int>(8 * scale));
    copyrightText->SetFont(smallFont);
    
    // 添加组件到布局中，调整边距
    int margin = static_cast<int>(15 * scale);
    int smallMargin = static_cast<int>(10 * scale);
    int sideMargin = static_cast<int>(30 * scale);
    
    mainSizer->Add(titleText, 0, wxALIGN_CENTER | wxALL, margin);
    mainSizer->Add(descText, 0, wxALIGN_CENTER | wxLEFT | wxRIGHT | wxBOTTOM, margin);
    mainSizer->AddSpacer(smallMargin);
    mainSizer->Add(closeJiyuBtn, 0, wxEXPAND | wxLEFT | wxRIGHT, sideMargin);
    mainSizer->AddSpacer(smallMargin);
    mainSizer->Add(uninstallNetworkDriverBtn, 0, wxEXPAND | wxLEFT | wxRIGHT, sideMargin);
    mainSizer->AddSpacer(smallMargin);
    mainSizer->Add(closeMasterHelperBtn, 0, wxEXPAND | wxLEFT | wxRIGHT, sideMargin);
    mainSizer->AddSpacer(margin);
    mainSizer->Add(statusText, 0, wxALIGN_CENTER | wxALL, 5);
    mainSizer->AddSpacer(5);
    mainSizer->Add(exitBtn, 0, wxEXPAND | wxLEFT | wxRIGHT, sideMargin);
    mainSizer->AddSpacer(margin);
    mainSizer->Add(copyrightText, 0, wxALIGN_CENTER | wxALL, smallMargin);
    
    // 设置布局
    panel->SetSizer(mainSizer);
    
    // 创建框架布局
    wxBoxSizer* frameSizer = new wxBoxSizer(wxVERTICAL);
    frameSizer->Add(panel, 1, wxEXPAND);
    SetSizer(frameSizer);
    
    // 调整窗口大小
    SetClientSize(scaledWidth, scaledHeight);
    
    // 使用Fit()函数自动调整窗口大小以适应所有内容
    mainSizer->Fit(panel);
    frameSizer->Fit(this);
    
    // 绑定事件
    closeJiyuBtn->Bind(wxEVT_BUTTON, &MainFrame::OnCloseJiyu, this);
    uninstallNetworkDriverBtn->Bind(wxEVT_BUTTON, &MainFrame::OnUninstallNetworkDriver, this);
    closeMasterHelperBtn->Bind(wxEVT_BUTTON, &MainFrame::OnCloseMasterHelper, this);
    exitBtn->Bind(wxEVT_BUTTON, &MainFrame::OnExit, this);
}

void MainFrame::OnCloseJiyu(wxCommandEvent& event)
{
    statusText->SetLabel(wxT("正在关闭极域..."));
    statusText->SetForegroundColour(wxColour(200, 100, 0));
    Refresh();
    Update();
    
    if (KillProcessByName(L"StudentMain.exe")) {
        statusText->SetLabel(wxT("极域已成功关闭"));
        statusText->SetForegroundColour(wxColour(0, 120, 0));
        wxMessageBox(wxT("极域已成功关闭！"), wxT("成功"), wxOK | wxICON_INFORMATION, this);
    } else {
        statusText->SetLabel(wxT("无法关闭极域进程"));
        statusText->SetForegroundColour(wxColour(200, 0, 0));
        wxMessageBox(wxT("无法关闭极域进程！"), wxT("错误"), wxOK | wxICON_ERROR, this);
    }
    Refresh();
}

void MainFrame::OnUninstallNetworkDriver(wxCommandEvent& event)
{
    statusText->SetLabel(wxT("正在卸载网络驱动..."));
    statusText->SetForegroundColour(wxColour(200, 100, 0));
    Refresh();
    Update();
    
    if (ExecuteCommand(L"/c sc stop tdnetfilter")) {
        statusText->SetLabel(wxT("网络驱动已成功卸载"));
        statusText->SetForegroundColour(wxColour(0, 120, 0));
        wxMessageBox(wxT("网络驱动已成功卸载！"), wxT("成功"), wxOK | wxICON_INFORMATION, this);
    } else {
        statusText->SetLabel(wxT("无法卸载网络驱动"));
        statusText->SetForegroundColour(wxColour(200, 0, 0));
        wxMessageBox(wxT("无法卸载网络驱动！"), wxT("错误"), wxOK | wxICON_ERROR, this);
    }
    Refresh();
}

void MainFrame::OnCloseMasterHelper(wxCommandEvent& event)
{
    statusText->SetLabel(wxT("正在关闭网络限制..."));
    statusText->SetForegroundColour(wxColour(200, 100, 0));
    Refresh();
    Update();
    
    if (KillProcessByName(L"MasterHelper.exe")) {
        statusText->SetLabel(wxT("网络限制已成功关闭"));
        statusText->SetForegroundColour(wxColour(0, 120, 0));
        wxMessageBox(wxT("网络限制已成功关闭！"), wxT("成功"), wxOK | wxICON_INFORMATION, this);
    } else {
        statusText->SetLabel(wxT("无法关闭网络限制进程"));
        statusText->SetForegroundColour(wxColour(200, 0, 0));
        wxMessageBox(wxT("无法关闭网络限制进程！"), wxT("错误"), wxOK | wxICON_ERROR, this);
    }
    Refresh();
}

void MainFrame::OnExit(wxCommandEvent& event)
{
    // 先禁用所有控件，防止用户再次点击
    Enable(false);
    
    // 显示确认对话框
    int answer = wxMessageBox(wxT("确定要退出程序吗？"), 
                             wxT("退出确认"), 
                             wxYES_NO | wxICON_QUESTION,
                             this);
    
    if (answer == wxYES) {
        // 如果用户确认，正常关闭窗口
        Close(true);
    } else {
        // 如果用户取消，重新启用控件
        Enable(true);
    }
}

bool MainFrame::KillProcessByName(const wchar_t* processName)
{
    // 保存窗口状态
    bool wasEnabled = IsEnabled();
    if (wasEnabled) {
        Enable(false); // 禁用窗口，防止用户再次点击
    }
    
    bool result = false;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    
    if (hSnapshot != INVALID_HANDLE_VALUE) {
        PROCESSENTRY32W pe32;
        pe32.dwSize = sizeof(PROCESSENTRY32W);
        
        if (Process32FirstW(hSnapshot, &pe32)) {
            do {
                if (wcscmp(pe32.szExeFile, processName) == 0) {
                    HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pe32.th32ProcessID);
                    if (hProcess != NULL) {
                        if (TerminateProcess(hProcess, 0)) {
                            result = true;
                        }
                        CloseHandle(hProcess);
                    }
                }
            } while (Process32NextW(hSnapshot, &pe32));
        }
        CloseHandle(hSnapshot);
    }
    
    // 恢复窗口状态
    if (wasEnabled) {
        Enable(true);
        SetFocus(); // 确保窗口重新获得焦点
    }
    
    return result;
}

bool MainFrame::ExecuteCommand(const wchar_t* command)
{
    // 保存窗口状态
    bool wasEnabled = IsEnabled();
    if (wasEnabled) {
        Enable(false); // 禁用窗口，防止用户再次点击
    }
    
    SHELLEXECUTEINFOW sei = {0};
    sei.cbSize = sizeof(SHELLEXECUTEINFOW);
    sei.fMask = SEE_MASK_NOCLOSEPROCESS;
    sei.hwnd = GetHandle();
    sei.lpVerb = L"runas"; // 请求管理员权限
    sei.lpFile = L"cmd.exe";
    sei.lpParameters = command;
    sei.nShow = SW_HIDE;
    
    bool result = false;
    
    if (ShellExecuteExW(&sei)) {
        if (sei.hProcess) {
            // 等待进程结束
            WaitForSingleObject(sei.hProcess, INFINITE);
            
            // 获取进程退出码
            DWORD exitCode = 0;
            if (GetExitCodeProcess(sei.hProcess, &exitCode)) {
                result = (exitCode == 0);
            }
            
            // 关闭进程句柄
            CloseHandle(sei.hProcess);
        }
    }
    
    // 恢复窗口状态
    if (wasEnabled) {
        Enable(true);
        SetFocus(); // 确保窗口重新获得焦点
        Raise();    // 将窗口提到前台
    }
    
    return result;
} 