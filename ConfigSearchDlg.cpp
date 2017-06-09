
// ConfigSearchDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "ConfigSearch.h"
#include "ConfigSearchDlg.h"
#include "afxdialogex.h"

#include <atlstr.h>
#import "msxml3.dll"



using namespace std;
using namespace boost::filesystem;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



static bool is_utf8(const char * string)
{
	if (!string)
		return true;

	const unsigned char * bytes = (const unsigned char *)string;
	int num;

	while (*bytes != 0x00)
	{
		if ((*bytes & 0x80) == 0x00)
		{
			// U+0000 to U+007F 
			num = 1;
		}
		else if ((*bytes & 0xE0) == 0xC0)
		{
			// U+0080 to U+07FF 
			num = 2;
		}
		else if ((*bytes & 0xF0) == 0xE0)
		{
			// U+0800 to U+FFFF 
			num = 3;
		}
		else if ((*bytes & 0xF8) == 0xF0)
		{
			// U+10000 to U+10FFFF 
			num = 4;
		}
		else
			return false;

		bytes += 1;
		for (int i = 1; i < num; ++i)
		{
			if ((*bytes & 0xC0) != 0x80)
				return false;
			bytes += 1;
		}
	}

	return true;
}



//_CRT_NON_CONFORMING_SWPRINTFS

// printf 포맷을 이용하는 OutputDebugString 함수
void Trace(LPCTSTR szFormat, ...)
{
    enum { BUFF_SIZE = 2048 };
    TCHAR szTempBuf[BUFF_SIZE] ;
    va_list vlMarker ;
    va_start(vlMarker,szFormat) ;
    _vstprintf(szTempBuf,szFormat,vlMarker) ;
    va_end(vlMarker) ;
    OutputDebugString(szTempBuf) ;
}


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CConfigSearchDlg 대화 상자



CConfigSearchDlg::CConfigSearchDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CConfigSearchDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CConfigSearchDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_FIND, m_FileList);
	DDX_Control(pDX, IDC_LIST_FILE, m_ListFile);
}

BEGIN_MESSAGE_MAP(CConfigSearchDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_SEARCH, &CConfigSearchDlg::OnBnClickedBtnSearch)
	ON_BN_CLICKED(IDOK, &CConfigSearchDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CConfigSearchDlg::OnBnClickedCancel)
	ON_NOTIFY(NM_CLICK, IDC_LIST_FIND, &CConfigSearchDlg::OnNMClickListFind)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_FIND, &CConfigSearchDlg::OnNMDblclkListFind)
	ON_BN_CLICKED(IDC_BTN_ALLOPEN, &CConfigSearchDlg::OnBnClickedBtnAllopen)
	ON_BN_CLICKED(IDC_BTN_SAVE, &CConfigSearchDlg::OnBnClickedBtnSave)
	ON_BN_CLICKED(IDC_BTN_CONFIGURE, &CConfigSearchDlg::OnBnClickedBtnConfigure)
	ON_BN_CLICKED(IDC_BTN_SEARCH_STOP, &CConfigSearchDlg::OnBnClickedBtnSearchStop)
	ON_BN_CLICKED(IDC_BTN_ALLOPEN_SEL, &CConfigSearchDlg::OnBnClickedBtnAllopenSel)
END_MESSAGE_MAP()


// CConfigSearchDlg 메시지 처리기

BOOL CConfigSearchDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
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

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	m_FileList.InsertColumn(0, _T("파일"), LVCFMT_LEFT, 550);
	m_FileList.InsertColumn(1, _T("찾은개수"), LVCFMT_RIGHT, 70);

	// 설정파일 로드
	//OnBnClickedBtnConfigure();

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CConfigSearchDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		if (nID == SC_CLOSE) {
			OnOK();
		}
		else {
			CDialogEx::OnSysCommand(nID, lParam);
		}
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CConfigSearchDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CConfigSearchDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CConfigSearchDlg::OnBnClickedBtnSearch()
{
	GetDlgItemText(IDC_EDIT_SEARCH, sSearch_);
	if (sSearch_.IsEmpty()) {
		AfxMessageBox("찾을 문자열을 넣으세요 ");
		return;
	}

	m_FileList.DeleteAllItems();
	m_ListFile.ResetContent();

	LoadConfig();

	GetDlgItem(IDC_BTN_SEARCH)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_CONFIGURE)->EnableWindow(FALSE);
	GetDlgItem(IDC_CHECK_CASE_SENS)->EnableWindow(FALSE);
	GetDlgItem(IDC_CHECK_REGEXP)->EnableWindow(FALSE);

	thread tr([&](CConfigSearchDlg *pthis) {
		pthis->SetRunFlag();

		std::for_each(conf_.Dir.begin(), conf_.Dir.end(), [&](ConfigType::DirType::value_type &dir) {
			pthis->FileListing((LPCSTR)sSearch_, dir.first, dir.second);
		});

		pthis->GetDlgItem(IDC_BTN_SEARCH)->EnableWindow(TRUE);
		pthis->GetDlgItem(IDC_BTN_CONFIGURE)->EnableWindow(TRUE);
		pthis->GetDlgItem(IDC_CHECK_CASE_SENS)->EnableWindow(TRUE);
		pthis->GetDlgItem(IDC_CHECK_REGEXP)->EnableWindow(TRUE);
		SetDlgItemText(IDC_EDIT_STATUS, _T(""));

		AfxMessageBox("검색완료");

	}, this);
	tr.detach();
}

int CConfigSearchDlg::FindStrInFile(const string &search, const string & sFile, stringstream &ss)
{
	std::ifstream fin(sFile.c_str());
	if (!fin) {
		throw exception("file open error ");
	}

	bool bCaseSens = (IsDlgButtonChecked(IDC_CHECK_CASE_SENS) ? true : false);
	bool bRegExp = (IsDlgButtonChecked(IDC_CHECK_REGEXP) ? true : false);
	
	string search_copy = search;
	if (!bCaseSens) {
		boost::to_lower(search_copy);
	}

	string sLine;
	int n = 1;
	int count = 0;
	while (!fin.eof()) {
		getline(fin, sLine);
		string s = sLine;

		if (is_utf8(s.c_str())) {
			CA2W s1(s.c_str(), CP_UTF8);
			CW2A s2(s1);
			sLine = s = s2;
		}

		if (!bCaseSens) { 
			boost::to_lower(s); 
		}

		if (bRegExp) {
			boost::regex re(search_copy);
			if (boost::regex_search(s, re)) {
				ss << (boost::format("[%d] ") % n) << boost::trim_copy(sLine) << endl;
				++count;
			}
		}
		else {
			if (s.find(search_copy) != string::npos) {
				boost::trim(sLine);
				ss << (boost::format("[%d] ") % n) << boost::trim_copy(sLine) << endl;
				++count;
			}
		}
		
		++n;
	}

	return count;
}


void CConfigSearchDlg::FileListing(const string &search, const string &sPath, bool sub) 
{
	try {
		path p(sPath);
		if (is_directory(p)) {

			// ignore 폴더 제외 
			if (conf_.Ignore.end() != std::find(conf_.Ignore.begin(), conf_.Ignore.end(), p.filename().string())) {
				Trace(_T("CConfigSearchDlg::FileListing IGNORE - %s"), sPath.c_str());
				return;
			}

			SetDlgItemText(IDC_EDIT_STATUS, sPath.c_str());
			std::for_each(directory_iterator(p), directory_iterator(), [&](directory_entry &entry) {
				if (!this->GetRunFlag()) {
					return;
				}

				if (is_directory(entry.path()) && sub) {
					FileListing(search, entry.path().string(), sub);
				}
				else {

					string e = entry.path().extension().string();
					e = boost::algorithm::to_lower_copy(e);

					if (conf_.Ext.end() != std::find(conf_.Ext.begin(), conf_.Ext.end(), e)) {

						stringstream ss;	
						ss.str("");
						int count = FindStrInFile(search, entry.path().string(), ss);
						if (count > 0) {
							stringstream toString;
							toString << count;
							
							int nLast = m_FileList.GetItemCount();
							m_FileList.InsertItem(nLast, entry.path().string().c_str()); 
							m_FileList.SetItemText(nLast, 1, toString.str().c_str());
						}
					}
				}
				
			});
		}
	}
	catch (...) {}
}


void CConfigSearchDlg::OnBnClickedOk()
{
	
	//CDialogEx::OnOK();
}


void CConfigSearchDlg::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//CDialogEx::OnCancel();
}


void CConfigSearchDlg::OnNMClickListFind(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	int nItem = pNMItemActivate->iItem;

	if (nItem >= 0) {
		m_ListFile.ResetContent();

		stringstream ss;
		FindStrInFile((LPCSTR)sSearch_, (LPCSTR)m_FileList.GetItemText(nItem, 0), ss);

		string s;
		while (!ss.eof()) {
			getline(ss, s);
			m_ListFile.InsertString(-1, s.c_str());
		}

	}

	*pResult = 0;
}


void CConfigSearchDlg::OnNMDblclkListFind(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	
	int nItem = pNMItemActivate->iItem;

	if (nItem >= 0) {
		ShellExecute(NULL, "open", conf_.Open.c_str(), (LPCSTR)m_FileList.GetItemText(nItem, 0), NULL, SW_NORMAL);
	}
	*pResult = 0;
}


void CConfigSearchDlg::OnBnClickedBtnAllopen()
{
	int nLast = m_FileList.GetItemCount();
	if (nLast > 0) {
		ostringstream os;

		for (int i = 0; i < nLast; ++i) {
			os << "\"" << (LPCSTR)m_FileList.GetItemText(i, 0) << "\" ";
		}

		ShellExecute(NULL, "open", conf_.Open.c_str(), os.str().c_str(), NULL, SW_NORMAL);
	}
	else {
		AfxMessageBox(_T("검색된 파일이 없습니다."));
	}
}


void CConfigSearchDlg::OnBnClickedBtnAllopenSel()
{
	POSITION pos = m_FileList.GetFirstSelectedItemPosition();
	if (pos) {
		ostringstream os;

		while (pos) {
			int nItem = m_FileList.GetNextSelectedItem(pos);
			os << "\"" << (LPCSTR)m_FileList.GetItemText(nItem, 0) << "\" ";
		}

		ShellExecute(NULL, "open", conf_.Open.c_str(), os.str().c_str(), NULL, SW_NORMAL);
	}
	else {
		AfxMessageBox(_T("선택된 파일이 없습니다."));
	}
}



void CConfigSearchDlg::OnBnClickedBtnSave()
{
	auto time = boost::posix_time::second_clock::local_time();
	string sTime = boost::posix_time::to_iso_string(time);

	const char szFILE[] = "ConfigSearch.Result";
	ostringstream osFile;
	osFile << szFILE << "-" << sTime << ".txt";
	string sLogFile = osFile.str();
	
	int nLast = m_FileList.GetItemCount();
	if (nLast > 0) {

		std::ofstream fout(sLogFile.c_str());
		ostringstream os;

		for (int i = 0; i < nLast; ++i) {
			fout << (LPCSTR)m_FileList.GetItemText(i, 0) << endl;
		}

		ShellExecute(NULL, "open", conf_.Open.c_str(), sLogFile.c_str(), NULL, SW_NORMAL);
	}
}

std::string CConfigSearchDlg::GetConfigFileName() 
{
	TCHAR szBuff[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, szBuff);

	return str(boost::format("%1%\\ConfigSearch.config") % szBuff);
}

void CConfigSearchDlg::LoadConfig()
{
	using namespace MSXML2;
	using namespace boost;

	try {
		::CoInitialize(NULL);

		{
			MSXML2::IXMLDOMDocumentPtr doc;
			doc.CreateInstance(__uuidof(DOMDocument30));
			doc->put_async(VARIANT_FALSE);
			VARIANT_BOOL b = doc->load(GetConfigFileName().c_str());
			if (!b) throw -1;

			conf_.Reset();

			// 경로 
			auto path = doc->selectNodes("/configuration/paths/path");

			for (int i = 0; i < path->length; ++i) {
				string sub = path->Getitem(i)->attributes->getNamedItem("sub")->text;
				string dir = path->Getitem(i)->text;

				conf_.Dir.push_back(std::make_pair(dir, (sub == "0" ? false : true)));
			}

			// 확장자 
			string ext = (LPCSTR)doc->selectSingleNode("/configuration/extention")->text;
			boost::split(conf_.Ext, ext, is_any_of(";"), token_compress_on);

			std::for_each(conf_.Ext.begin(), conf_.Ext.end(), [](string &s) {
				s  = "." + boost::to_lower_copy(s);
			});

			// Ignore 디렉토리 
			string ignore_dir = (LPCSTR)doc->selectSingleNode("/configuration/ignore_dir")->text;
			boost::split(conf_.Ignore, ignore_dir, is_any_of(";"), token_compress_on);

			// 파일 오픈 툴
			conf_.Open = (LPCSTR)doc->selectSingleNode("/configuration/open")->text;
		}

		::CoUninitialize();
	}
	catch (...) {
		AfxMessageBox(_T("설정파일 로드 에러"));
	}

	try {		
		// 확인 로직 
		m_ListFile.ResetContent();

		m_ListFile.InsertString(-1, "[configuration/open]");
		m_ListFile.InsertString(-1, conf_.Open.c_str());
		m_ListFile.InsertString(-1, "");

		m_ListFile.InsertString(-1, "[configuration/extention]");
		std::for_each(conf_.Ext.begin(), conf_.Ext.end(), [&](string &s) {
			m_ListFile.InsertString(-1, str(boost::format("  %s") % s).c_str());
		});
		m_ListFile.InsertString(-1, "");

		m_ListFile.InsertString(-1, "[configuration/ignore_dir]");
		std::for_each(conf_.Ignore.begin(), conf_.Ignore.end(), [&](string &s) {
			m_ListFile.InsertString(-1, str(boost::format("  %s") % s).c_str());
		});
		m_ListFile.InsertString(-1, "");

		m_ListFile.InsertString(-1, "[configuration/paths/path]");
		std::for_each(conf_.Dir.begin(), conf_.Dir.end(), [&](ConfigType::DirType::value_type &dir) {
			string s = str(boost::format("  %s   %s") % dir.first % (dir.second ? "[-하위디렉토리포함]" : ""));
			m_ListFile.InsertString(-1, s.c_str());
		});
		m_ListFile.InsertString(-1, "");
	}
	catch (...) {
		AfxMessageBox(_T("설정파일 세팅 에러"));
	}

}

void CConfigSearchDlg::OnBnClickedBtnConfigure()
{
	LoadConfig();
}

void CConfigSearchDlg::OnBnClickedBtnSearchStop()
{
	SetRunFlag(false);
}

