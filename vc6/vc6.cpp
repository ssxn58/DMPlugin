// vc6.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "vc6.h"
#include "vc6Dlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CVc6App

BEGIN_MESSAGE_MAP(CVc6App, CWinApp)
	//{{AFX_MSG_MAP(CVc6App)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVc6App construction

CVc6App::CVc6App()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CVc6App object

CVc6App theApp;

static BOOL RegDll(LPCTSTR file) 
{
	HMODULE hLib = LoadLibrary(file); 
	if (hLib < (HMODULE)HINSTANCE_ERROR) 	
	{ 
		return FALSE;
	} 
	

	HRESULT hResult = ::OleInitialize(NULL);
	if(hResult != S_OK)
	{
		FreeLibrary(hLib);
		return FALSE;
	}



	FARPROC lpDllEntryPoint = GetProcAddress(hLib,"DllRegisterServer"); 
	if (lpDllEntryPoint == NULL)
	{
		FreeLibrary(hLib);
		::OleUninitialize();
		return FALSE;
	}
	
	
	if(FAILED((*lpDllEntryPoint)()))
	{ 
		FreeLibrary(hLib);
		::OleUninitialize();
		return FALSE;
		
	}; 
	
	::OleUninitialize();
	FreeLibrary(hLib);
	
	return TRUE;
}


// 判断目录是否存在
static BOOL FolderExists(CString s)
{
    DWORD attr; 
    attr = GetFileAttributes(s); 
    return (attr != (DWORD)(-1) ) &&
        ( attr & FILE_ATTRIBUTE_DIRECTORY); 
}

// 创建目录
static BOOL CreateDir(CString P)
{
    int len=P.GetLength();
    if ( len <2 ) return false; 
    
    if('\\'==P[len-1])
    {
        P=P.Left(len-1);
        len=P.GetLength();
    }
    if ( len <=0 ) return false;
    
    if (len <=3) 
    {
        if (FolderExists(P))return true;
        else return false; 
    }
    
    if (FolderExists(P))return true;
 
    
    
    CString Parent;
    Parent=P.Left(P.ReverseFind('\\') );
    
    if(Parent.GetLength()<=0)return false; 
    
    BOOL Ret=CreateDir(Parent); 
    
    if(Ret) 
    {
        SECURITY_ATTRIBUTES sa;
        sa.nLength=sizeof(SECURITY_ATTRIBUTES);
        sa.lpSecurityDescriptor=NULL;
        sa.bInheritHandle=0;
        Ret=(CreateDirectory(P,&sa)==TRUE);
        return Ret;
    }
    else
        return FALSE;
}


static BOOL ExtraceRes(DWORD res_id,char * path)
{
	HINSTANCE instance = AfxGetInstanceHandle();
	
    //定位我们的自定义资源，这里因为我们是从本模块定位资源，所以将句柄简单地置为NULL即可
	HRSRC hRsrc = FindResource(instance, MAKEINTRESOURCE(res_id), TEXT("Res"));
	if (NULL == hRsrc)
		return FALSE;

	//获取资源的大小
	DWORD dwSize = SizeofResource(instance, hRsrc); 
	if (0 == dwSize)
		return FALSE;

	//加载资源
	HGLOBAL hGlobal = LoadResource(instance, hRsrc); 
	if (NULL == hGlobal)
		return FALSE;
    
	//锁定资源
	LPVOID pBuffer = LockResource(hGlobal); 
	if (NULL == pBuffer)
		return FALSE;

    FILE * f = fopen(path,"wb");
    if (f == NULL)
        return FALSE;
    
	fwrite(pBuffer,1,dwSize,f);

    fclose(f);

    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CVc6App initialization

BOOL CVc6App::InitInstance()
{
	AfxEnableControlContainer();
	
	// 步骤1 设置COM组件线程模型为sta
	CoInitialize(NULL);
	
	// 步骤2 这里可以做释放资源的动作. 比如把所有资源都释放到c:\test_game.
	// 这里我已经把dm.dll添加到了附件中
	// 我们把dm.dll来释放到c:\test_game,其他资源释放类似.
	// 这里要注意的是添加到资源的小节名必须是Res. 否则无法释放.
	CreateDir("c:\\test_game");
	ExtraceRes(IDR_RES_DM,"c:\\test_game\\dm.dll");

	
	// 步骤3 注册dm组件到系统中.
	char * dll_path = "c:\\test_game\\dm.dll";
	if (RegDll(dll_path) == FALSE)
	{
		MessageBox(NULL,"注册插件失败. 检测文件是否存在","错误",MB_OK);
		return FALSE;
	}

	//  步骤4 检查版本号 附件中的插件版本号是3.1208C. 如果要换插件，必须替换Res\dm.dll
	//  并且dm.hpp和dm.cpp必须要删除重新导入. 同时dm.hpp的文件头部加入#pragma once
	char * need_ver = "3.1220";
	Idmsoft dm;
	if (dm.CreateDispatch("dm.dmsoft") == FALSE)
	{
		MessageBox(NULL,"创建对象失败,检查是否正常注册","错误",MB_OK);
		return FALSE;
	}

	if (strcmp(need_ver,dm.Ver()) != 0)
	{
		CString tmp;
		tmp.Format("版本错误，当前版本号是:%s",dm.Ver());
		MessageBox(NULL,tmp,"错误",MB_OK);
		return FALSE;
	}
#if 0
	// 步骤5 收费用户注册. 如果是使用免费功能,可以略过这一步
	long ret = dm.Reg("abcde","");
	if (ret != 1)
	{
		CString tmp;
		tmp.Format("注册失败,返回值是:%d",ret);
		MessageBox(NULL,tmp,"错误",MB_OK);
		return FALSE;
	}
#endif

#if 0
	// 步骤6 如果有使用DmGuard功能, 可以在这里开启.如果没使用到保护盾,可以略过这一步
	if (dm.DmGuard(1,"xxx") == 0)
	{
		MessageBox(NULL,"开启保护盾失败","错误",MB_OK);
		return FALSE;
	}
#endif
	// OK 至此，插件已经可以完整的调用了. 后面,有如何创建线程来使用插件的具体代码.
	
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	CVc6Dlg dlg;
	dlg.dm_ver = dm.Ver();
	
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}
	

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
