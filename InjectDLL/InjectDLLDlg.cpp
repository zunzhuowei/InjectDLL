
// InjectDLLDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "InjectDLL.h"
#include "InjectDLLDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CInjectDLLDlg 对话框



CInjectDLLDlg::CInjectDLLDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_INJECTDLL_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CInjectDLLDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, dllPathEdit);
	DDX_Control(pDX, IDC_EDIT2, appNameEdit);
}

BEGIN_MESSAGE_MAP(CInjectDLLDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CInjectDLLDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CInjectDLLDlg 消息处理程序

BOOL CInjectDLLDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	LPCWSTR gameName = L"Plants vs. Zombies GOTY ";
	LPCWSTR dllPath = L"E:\\CPlusProject\\PlantVsBomiesDLL\\Release\\PlantVsBomiesDLL.dll";
	appNameEdit.SetWindowText(gameName);
	dllPathEdit.SetWindowText(dllPath);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CInjectDLLDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CInjectDLLDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CInjectDLLDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

/// <summary>
///  dll注入
/// </summary>
/// <param name="hanlde">进程句柄</param>
/// <param name="dllPath">dll完整路径</param>
/// <returns></returns>
//BOOL dllInject(HANDLE& hanlde, const char* dllPath) {
static BOOL dllInject(HANDLE& hanlde, CString dllPath) {
		//分配内存空间
	LPVOID addr = VirtualAllocEx(hanlde, NULL, 256, MEM_COMMIT, PAGE_READWRITE);
	if (addr == NULL)
	{
		return false;
	}
	//把地址写进内存中
	BOOL ret = WriteProcessMemory(hanlde, addr, (CStringA)dllPath, dllPath.GetLength() + 1, NULL);
	if (ret == NULL)
	{
		return false;
	}
	//创建远程线程
	HANDLE proccessHanlde = CreateRemoteThread
	(hanlde, NULL, NULL, (LPTHREAD_START_ROUTINE)LoadLibraryA, addr, NULL, NULL);
	if (proccessHanlde == NULL)
	{
		//释放内存空间
		VirtualFreeEx(hanlde, addr, 0, MEM_RELEASE);
		return false;
	}

	//等待线程，无限等待
	WaitForSingleObject(proccessHanlde, INFINITE);
	//关闭线程句柄
	CloseHandle(proccessHanlde);
	//释放内存空间
	VirtualFreeEx(hanlde, addr, 0, MEM_RELEASE);
	return TRUE;
}

/*
* 获取游戏进程句柄
*/
void getProccessHandle(LPCWSTR gameName, HANDLE& pHandle) {
	HWND hwnd = ::FindWindow(NULL, gameName);
	if (hwnd == NULL)
	{
		return;
	}
	DWORD pid;
	//获取进程id
	GetWindowThreadProcessId(hwnd, &pid);
	if (pid == NULL) return;
	//进程句柄
	pHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	if (pHandle == NULL)
	{
		return;
	}
}

/// <summary>
/// 点击注入按钮事件
/// 特别注意：需要生成对应平台的MFC应用才可以注入。
/// 如：需要将dll注入到32位的应用时，需要构建x86的应用；
/// 如：需要将dll注入到64位的应用时，需要构建x64的应用；
/// </summary>
void CInjectDLLDlg::OnBnClickedButton1()
{
	CString gameName;
	appNameEdit.GetWindowText(gameName);
	CString dllPath;
	dllPathEdit.GetWindowText(dllPath);
	//LPCWSTR gameName = L"Plants vs. Zombies GOTY ";
	HANDLE pHandle;
	getProccessHandle(gameName, pHandle);
	//char* p = (char*)dllPath.GetBuffer();
	//auto p = (CStringA)dllPath;
	//char* dllPath2 = "E:\\CPlusProject\\PlantVsBomiesDLL\\Release\\PlantVsBomiesDLL.dll";
	//dllInject(*handleP, p);
	dllInject(pHandle, dllPath);
}
