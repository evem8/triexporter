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
	StuffFile* sf;
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
		sf = (StuffFile*)((DWORD*)lParam)[1];
		path = *(CString*)((DWORD*)lParam)[2];
		start = *(CString*)((DWORD*)lParam)[3];
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
			StuffFileEntry *sfe = &me->sf->files[ret[i]];
			me->m_Text.SetWindowText(sfe->filename.c_str());
			me->m_Progress.SetPos(i);
			sfe->Unstuff(me->path, me->start, 1);
		}
		me->EndDialog(0);
		return 0;
	}
};
