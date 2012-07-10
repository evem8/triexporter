

#pragma once
#include "StuffFile.h"
#include "TriFile.h"
#include <atlctrls.h>
#include <atlstr.h>
class CStuffDlg : public CDialogImpl<CStuffDlg>
{
public:
	enum { IDD = IDD_STUFFDLG };
	BEGIN_MSG_MAP(CAboutDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
		COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
		COMMAND_ID_HANDLER(IDC_LOAD, OnLoad)
	END_MSG_MAP()

	int *mout;
// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	INT_PTR DoModal(int *out)
	{
		mout = out;
		return CDialogImpl<CStuffDlg>::DoModal();
	}

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{

		if(sf.loaded)
			FillTree();
		SetWindowText("Select tri file: ");
		CenterWindow(GetParent());
		return TRUE;
	}

	
	LRESULT OnLoad(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{

		return 0;
	}



	LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		if(wID == IDOK)
		{
			if(sf.loaded)
			{

			}else
				wID = IDCANCEL;
		}
		
		EndDialog(wID);
		return 0;
	}
};
