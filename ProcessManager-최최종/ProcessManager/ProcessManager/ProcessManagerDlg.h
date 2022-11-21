
// ProcessManagerDlg.h: 헤더 파일
//

#pragma once


// CProcessManagerDlg 대화 상자
class CProcessManagerDlg : public CDialogEx
{
// 생성입니다.
public:
	CProcessManagerDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PROCESSMANAGER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_ProcessList;
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnLvnColumnclickList1(NMHDR* pNMHDR, LRESULT* pResult);
	static int CALLBACK CompareItem(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();

	BOOL m_bAscending;

	struct SORTPARAM {
		int iSortColumn;
		bool bSortDirect;
		CListCtrl* pList;
		int flag = -1;
	};
};
