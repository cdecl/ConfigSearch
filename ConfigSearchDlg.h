
// ConfigSearchDlg.h : ��� ����
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/regex.hpp>

#include <vector>
using namespace std;

struct ConfigType 
{
	typedef std::vector<std::pair<string, bool> > DirType;
	typedef std::vector<std::string> ExtentionType;
	typedef std::vector<std::string> IgnoreDirType;

	void Reset() 
	{ 
		Dir.clear();
		Ext.clear();
	}

	DirType Dir;
	ExtentionType Ext;
	IgnoreDirType Ignore;
	std::string Open;
};

typedef std::vector<std::pair<string, bool> > DirConfigType;

// CConfigSearchDlg ��ȭ ����
class CConfigSearchDlg : public CDialogEx
{
// �����Դϴ�.
public:
	CConfigSearchDlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_CONFIGSEARCH_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �����Դϴ�.


// �����Դϴ�.
protected:
	HICON m_hIcon;

	// ������ �޽��� �� �Լ�
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	

public:
	int FindStrInFile(const string &search, const string & sFile, stringstream &ss);
	void FileListing(const string &search, const string &sPath, bool sub = false);
	std::string GetConfigFileName();
	void LoadConfig();

public:
	void SetRunFlag(bool b = true) { run_ = b; }
	bool GetRunFlag() { return run_; }
	
private:
	CListCtrl m_FileList;
	std::thread trRun;
	CString sSearch_;
	CListBox m_ListFile;
	ConfigType conf_;
	volatile bool run_;

public:
	afx_msg void OnBnClickedBtnSearch();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnNMClickListFind(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkListFind(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedBtnAllopen();
	afx_msg void OnBnClickedBtnSave();
	afx_msg void OnBnClickedBtnConfigure();
	afx_msg void OnBnClickedBtnSearchStop();
	afx_msg void OnBnClickedBtnAllopenSel();
};

