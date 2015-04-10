// MainDlg.h : interface of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "resource.h"
#include "TriFile.h"
#include "3d.h"
#include "AboutDlg.h"

class CMainDlg : public CDialogImpl<CMainDlg>,
		// public CUpdateUI<CMainDlg>,
		public CMessageFilter, 
		public CIdleHandler
		//, public CDialogResize<CMainDlg>
{
private:
	CTrackBarCtrl m_Scale;
	CTrackBarCtrl m_Light;
	CEdit m_eScale;
	CEdit m_Texture;
	CStatic m_VSize;
	CStatic m_TriVersion;
	CStatic m_VCount;
	CStatic m_BoxMax;
	CStatic m_BoxMin;
	CStatic m_SurType;
	CStatic m_SurCount;
	CStatic m_Fps;
	CListBox m_Textures;
	CUpDownCtrl m_UpDown;
	C3d m_p3d;
	CMenu menu;
	CButton m_Add;
	CButton m_Remove;
	CButton m_Select;
	CButton m_ChkSurfaces[10];
	TriFile* file;
	SharedCache sc;
	CImageList il;
	CTreeViewCtrlEx m_Tree;
	map<string, HTREEITEM> lvis;
	vector<string> textures;
	vector<int> texdata;
	bool loaded;
public:
	BEGIN_MSG_MAP(CMainDlg)
		NOTIFY_HANDLER(IDC_TREE, NM_DBLCLK, OnTreeDblClick)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		//MESSAGE_HANDLER(WM_SIZE, OnResize)
		COMMAND_ID_HANDLER(ID_HELP_ABOUT, OnAppAbout)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
		COMMAND_ID_HANDLER(IDC_ADD, OnAdd)
		COMMAND_ID_HANDLER(ID_FILE_SETFOLDER, OnSetFolder)
		COMMAND_ID_HANDLER(ID_FILE_EXPORTMOD, OnExport)
		COMMAND_ID_HANDLER(ID_FILE_WIREON, OnWireOnOff)
		COMMAND_ID_HANDLER(ID_FILE_OPENTRI, OnOpenTri)
		COMMAND_ID_HANDLER(IDC_REMOVE, OnRemove)
		COMMAND_ID_HANDLER(IDC_SELECT, OnSelect)
		COMMAND_ID_HANDLER(IDC_SURFACE1, OnSurfaces)
		COMMAND_ID_HANDLER(IDC_SURFACE2, OnSurfaces)
		COMMAND_ID_HANDLER(IDC_SURFACE3, OnSurfaces)
		COMMAND_ID_HANDLER(IDC_SURFACE4, OnSurfaces)
		COMMAND_ID_HANDLER(IDC_SURFACE5, OnSurfaces)
		COMMAND_ID_HANDLER(IDC_SURFACE6, OnSurfaces)
		COMMAND_ID_HANDLER(IDC_SURFACE7, OnSurfaces)
		COMMAND_ID_HANDLER(IDC_SURFACE8, OnSurfaces)
		COMMAND_ID_HANDLER(IDC_SURFACE9, OnSurfaces)
		COMMAND_ID_HANDLER(IDC_SURFACE10, OnSurfaces)
		COMMAND_ID_HANDLER(ID_FILE_EXIT, OnCancel)
		MESSAGE_HANDLER(WM_HSCROLL, OnScaleTrack)
		MESSAGE_HANDLER(WM_VSCROLL, OnScaleTrack)
		NOTIFY_HANDLER(IDC_TREE, TVN_KEYDOWN, OnTvnKeydownTree)
		//CHAIN_MSG_MAP(CDialogResize<CMainDlg>)
	END_MSG_MAP()
	//BEGIN_UPDATE_UI_MAP(CMainDlg)
	//END_UPDATE_UI_MAP()
	//BEGIN_DLGRESIZE_MAP(CMainDlg)
	//	DLGRESIZE_CONTROL(IDC_PREVIEW, DLSZ_SIZE_X|DLSZ_SIZE_Y)
	//END_DLGRESIZE_MAP()
	// Handler prototypes (uncomment arguments if needed):
	//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)
	enum { IDD = IDD_MAINDLG };
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnIdle();
	LRESULT OnTreeDblClick(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);
	void Load(int out);
	void Load(string & out);
	LRESULT OnOpenTri(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnSetFolder(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnScaleTrack(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSelect(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnExport(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnWireOnOff(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	void Add(const CString &texture,int data = -1);
	LRESULT OnAdd(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnRemove(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnSurfaces(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	void EnableAll(BOOL bEnable=1);
	void FillTree();
	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	//LRESULT OnResize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnAppAbout(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	void CloseDialog(int nVal);
	LRESULT OnTvnKeydownTree(int idCtrl, LPNMHDR pNMHDR, BOOL& bHandled);
	void FillTextures();
	void ExportTextures(const CString &path);
	void UpdateFPS();
};
