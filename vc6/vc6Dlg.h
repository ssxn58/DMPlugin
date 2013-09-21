// vc6Dlg.h : header file
//

#if !defined(AFX_VC6DLG_H__902899D5_897A_4A15_A6C1_2D61B067A38E__INCLUDED_)
#define AFX_VC6DLG_H__902899D5_897A_4A15_A6C1_2D61B067A38E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CVc6Dlg dialog

class CVc6Dlg : public CDialog
{
// Construction
public:
	CVc6Dlg(CWnd* pParent = NULL);	// standard constructor
	
	// 这里保存插件版本
	CString dm_ver;
	
	// 这里保存所有启动的线程的信息
	vector<ScriptInfo *>  all_script_info;

	
// Dialog Data
	//{{AFX_DATA(CVc6Dlg)
	enum { IDD = IDD_VC6_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVc6Dlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CVc6Dlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual void OnCancel();
	afx_msg void OnButton1();
	afx_msg void OnButton2();
	virtual void OnOK();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VC6DLG_H__902899D5_897A_4A15_A6C1_2D61B067A38E__INCLUDED_)
