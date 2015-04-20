// aboutdlg.h : interface of the CAboutDlg class
//
/////////////////////////////////////////////////////////////////////////////
#pragma once

class CUnstuffDlg : public CDialogImpl<CUnstuffDlg>
{
public:
	enum { IDD = IDD_UNSTUFF };
	BEGIN_MSG_MAP(CUnstuffDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
	END_MSG_MAP()
	CTreeItem* ti;
	SharedCache* sc;
	CStatic m_Text;
	CProgressBarCtrl m_Progress;
	CString path;
	CString start;
// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
	{
		CenterWindow(GetParent());
		m_Text.Attach(GetDlgItem(IDC_UNSTUFFTEXT));
		m_Progress.Attach(GetDlgItem(IDC_UNSTUFFPROG));
		DWORD dwThreadId;
		HANDLE hThread;
		ti = (CTreeItem*)((DWORD*)lParam)[0];
		sc = (SharedCache*)((DWORD*)lParam)[1];
		path = *(CString*)((DWORD*)lParam)[2];
		hThread = CreateThread(NULL, 0, Show, (LPVOID)this, 0, &dwThreadId);
		return TRUE;
	}
	void TreeWalker(CTreeItem &ti, vector<int> *ret)
	{
		do
		{
			int out = ti.GetData()-1;
			if(out < 0)
				TreeWalker(ti.GetChild(), ret);
			else
				ret->push_back(out);
			ti = ti.GetNextSibling();
		}while(ti);
	}
	static DWORD WINAPI Show(LPVOID lpParam)
	{
		CUnstuffDlg* me = (CUnstuffDlg*)lpParam;
		vector<int> ret;
		me->TreeWalker(me->ti->GetChild(), &ret);
		dword size = ret.size();
		me->m_Progress.SetRange(0,size);
		for(dword i=0; i < size; i++)
		{
			CacheEntry *sce = &me->sc->index[ret[i]];
			me->m_Text.SetWindowText(sce->filename.c_str());
			me->m_Progress.SetPos(i);
			me->sc->SaveFile(*sce, me->path, true);
		}
		me->EndDialog(0);
		return 0;
	}
};
