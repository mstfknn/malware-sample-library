
// builder_guiDlg.h : файл заголовка
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// диалоговое окно Cbuilder_guiDlg
class Cbuilder_guiDlg : public CDialogEx
{
// Создание
public:
	Cbuilder_guiDlg(CWnd* pParent = NULL);	// стандартный конструктор

// Данные диалогового окна
	enum { IDD = IDD_BUILDER_GUI_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// поддержка DDX/DDV


// Реализация
protected:
	HICON m_hIcon;

	// Созданные функции схемы сообщений
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	HBRUSH hbrHead; //фон для раздела

	bool LoadConfig( CString& fileName );
	bool SaveConfig( CString& fileName );
	void Normalize();
	bool Verify( bool onlyConfig, bool nameConfig );
	bool LoadSets();
	bool SaveSets();
	bool CreateBuild( const CString& fileNameConfig, const CString& src, const CString& dst, CString& res );

public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnDestroy();
	CString fileConfig;
	CString namePrefix;
	CString srcFileName;
	CString dstFileName;
	CString hostAdminka1;
	CString hostAdminka2;
	CString hostAdminka3;
	int periodAdminka;
	CSpinButtonCtrl spinPeriod;
	CString passwordAdminka;
	CString addrServer1;
	CString addrServer2;
	CString addrServer3;
	BOOL individualServerProcess;
	BOOL connectServer;
	int downtimeServer;
	afx_msg void OnBnClickedConfig();
	afx_msg void OnBnClickedSrc();
	afx_msg void OnBnClickedDst();
	afx_msg void OnBnClickedSaveconfig();
	afx_msg void OnBnClickedSaveasconfig();
	afx_msg void OnBnClickedBuild();
	CEdit resBuild;
	BOOL autorun;
	BOOL sployty;
	BOOL check_dublication;
	BOOL plugins_server;
	BOOL not_used_svchost;
	afx_msg void OnBnClickedPublickey();
	CString publicKey;
};
