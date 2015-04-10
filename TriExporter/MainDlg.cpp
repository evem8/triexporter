#include "stdafx.h"
#include "MainDlg.h"
#include "grannyfile.h"
#include <IL/config.h>
#include <IL/il.h>
#include <IL/ilu.h>

LRESULT CMainDlg::OnTreeDblClick(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& bHandled)
{
	CTreeItem ti = m_Tree.GetSelectedItem();
	if(ti)
	{
		int out = ti.GetData()-1;
		if(out>=0)
		{
			CString f = sc.index[out].filename.c_str();
			if(!f.Right(4).Compare(".tri"))
			{
				delete file;
				file = new TriFile();
				Load(out);
			}
			else if(!f.Right(4).Compare(".dds"))
			{
				if(loaded)
				{
					Add(f.Right(f.GetLength()-f.ReverseFind('/')-1), out);
					m_p3d.TextureChange(sc, texdata);
				}
			}
			else if(!f.Right(4).Compare(".gr2"))
			{
				delete file;
				file = new GrannyTriFile();
				Load(out);
			}
			else
				MessageBox("WTF?", "Error", MB_ICONERROR | MB_OK);
		}
	}
	else
		if(!loaded)
			EnableAll(FALSE);
	return TRUE;
}


void CMainDlg::Load(string &out)
{

	CString SurType;
	if(!file->LoadFile(out))
	{
		MessageBox("Model not loaded", "Error", MB_ICONERROR | MB_OK);
		return;
	}
	SurType.Format("%i", file->surfaces[0].surfaceType);
	EnableAll();
	CString title = "TriExporter_sharedcache";
	title += " - ";
	title += out.c_str();
	SetWindowText(title);
	m_p3d.Open(*file);
	textures.clear();
	texdata.clear();
	textures.resize(file->header.numSurfaces);
	texdata.resize(file->header.numSurfaces);
	for(dword i = 0; i < file->header.numSurfaces; i++)
		texdata[i] = -1;
	CString VSize;
	CString TriVersion;
	CString VCount;
	CString BoxMax;
	CString BoxMin;
	CString SurCount;
	VSize.Format("%i", file->header.sizeVertex);
	TriVersion.Format("%i.%i", file->header.versionHi, file->header.versionLo);
	VCount.Format("%i", file->header.numVertices);
	BoxMax.Format("{%1.2f; %1.2f; %1.2f}", file->header.maxBox[0], file->header.maxBox[1], file->header.maxBox[2]);
	BoxMin.Format("{%1.2f; %1.2f; %1.2f}", file->header.minBox[0], file->header.minBox[1], file->header.minBox[2]);
	SurCount.Format("%i", file->header.numSurfaces);			
	m_VSize.SetWindowText(VSize);
	m_TriVersion.SetWindowText(TriVersion);
	m_VCount.SetWindowText(VCount);
	m_BoxMax.SetWindowText(BoxMax);
	m_BoxMin.SetWindowText(BoxMin);
	m_SurType.SetWindowText(SurType);
	m_SurCount.SetWindowText(SurCount);
	loaded = true;
}

void CMainDlg::Load(int out)
{

	CString SurType;
	if(!file->LoadFile(sc.index[out]))
	{
		MessageBox("Model not loaded", "Error", MB_ICONERROR | MB_OK);
		return;
	}
	SurType.Format("%i", file->surfaces[0].surfaceType);
	EnableAll();
	CString title = "TriExporter_sharedcache";
	title += " - ";
	title += sc.index[out].filename.c_str();
	SetWindowText(title);
	m_p3d.Open(*file);
	textures.clear();
	texdata.clear();
	textures.resize(file->header.numSurfaces);
	texdata.resize(file->header.numSurfaces);
	for(dword i = 0; i < file->header.numSurfaces; i++)
		texdata[i] = -1;
	CString VSize;
	CString TriVersion;
	CString VCount;
	CString BoxMax;
	CString BoxMin;
	CString SurCount;
	VSize.Format("%i", file->header.sizeVertex);
	TriVersion.Format("%i.%i", file->header.versionHi, file->header.versionLo);
	VCount.Format("%i", file->header.numVertices);
	BoxMax.Format("{%1.2f; %1.2f; %1.2f}", file->header.maxBox[0], file->header.maxBox[1], file->header.maxBox[2]);
	BoxMin.Format("{%1.2f; %1.2f; %1.2f}", file->header.minBox[0], file->header.minBox[1], file->header.minBox[2]);
	SurCount.Format("%i", file->header.numSurfaces);

	// Loop through Checkboxes
	for(dword i=0; i<10; i++)
	{
		// Get current Checkbox
		CButton currButton = m_ChkSurfaces[i];

		// If the current model uses this surface, enable the checkbox and set it
		if(i < file->header.numSurfaces)
		{
			currButton.EnableWindow(true);
			currButton.SetCheck(true);
			m_p3d.drawSurface[i] = 1;
		}
		// Clear and disable the rest
		else
		{
			currButton.EnableWindow(false);
			currButton.SetCheck(false);
			m_p3d.drawSurface[i] = 0;
		}
	}

	m_VSize.SetWindowText(VSize);
	m_TriVersion.SetWindowText(TriVersion);
	m_VCount.SetWindowText(VCount);
	m_BoxMax.SetWindowText(BoxMax);
	m_BoxMin.SetWindowText(BoxMin);
	m_SurType.SetWindowText(SurType);
	m_SurCount.SetWindowText(SurCount);
	loaded = true;
}

LRESULT CMainDlg::OnSetFolder(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CFolderDialog fd(m_hWnd, "Path to EVE's folder:");
	if(fd.DoModal(m_hWnd)!= IDCANCEL)
	{
		CRegKey rk;
		rk.Create(HKEY_CURRENT_USER, "Software\\TriExporter");
		rk.SetStringValue("EVE", fd.m_szFolderPath);
		rk.Close();
		if (sc.LoadDir(fd.m_szFolderPath))
			FillTree();
		else
			MessageBox("Unable to load SharedCache", "Error", MB_ICONERROR | MB_OK);
	}
	return TRUE;
}
LRESULT CMainDlg::OnScaleTrack(UINT uMsg, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
	if(((HWND)lParam) == ((HWND)m_Scale))
	{
		CString str;
		str.Format("%1.2f", (float)((m_Scale.GetPos()/20.0) + 0.05));
		m_eScale.SetWindowText(str);
	}
	if(((HWND)lParam) == ((HWND)m_Light))
	{
		m_p3d.alight = m_Light.GetPos()/(180/D3DX_PI);
	}
	if(((HWND)lParam) == ((HWND)m_UpDown))
	{
		int sel = m_Textures.GetCaretIndex();
		if(sel!=-1)
		{
			switch(m_UpDown.GetPos())
			{
				case 0:
					if(sel)
					{
						swap(textures[sel], textures[sel-1]);
						swap(texdata[sel], texdata[sel-1]);
						FillTextures();
						m_p3d.SwapTexture(sel, sel-1);
						m_Textures.SetCaretIndex(sel-1);
					}
					break;
				case 2:
					if(sel+1< m_Textures.GetCount())
					{
						swap(textures[sel], textures[sel+1]);
						swap(texdata[sel], texdata[sel+1]);
						FillTextures();
						m_p3d.SwapTexture(sel, sel+1);
						m_Textures.SetCaretIndex(sel+1);
					}
					break;
			}
		}
		m_UpDown.SetPos(1);
	}
	return TRUE;
}

LRESULT CMainDlg::OnSelect(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	//CFileDialog fd(TRUE, 0, 0, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, "All files (*.*)\0*.*\0\0");
	//if(fd.DoModal(m_hWnd)!= IDCANCEL)
	//{
	//	m_Texture.SetWindowText(fd.m_szFileTitle);
	//}
	this->m_p3d.Reset();
	return 0;
}

LRESULT CMainDlg::OnSurfaces(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// Update all Checkboxes
	for(int i=0; i<10; i++)
		m_p3d.drawSurface[i] = m_ChkSurfaces[i].GetCheck();

	return 0;
}

LRESULT CMainDlg::OnOpenTri(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CFileDialog fd(TRUE, 0, 0, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, "All files (*.tri)\0*.tri\0\0");
	if(fd.DoModal(m_hWnd)!= IDCANCEL)
	{
		string filen;
		filen = fd.m_szFileName;
		Load(filen);
	}
	return 0;
}

LRESULT CMainDlg::OnWireOnOff(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	m_p3d.SwapFillMode();
	return 0;
}
LRESULT CMainDlg::OnExport(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CFileDialog fd(FALSE, 0, 0, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, "X files (*.x)\0*.x\0Obj files (*.obj)\0*.obj\0\x33\x64s files (*.3ds)\0*.3ds\0My files (*.my)\0*.my\0VBO files (*.vbo)\0*.vbo\0Android3D files (*.a3d)\0*.a3d\0FBX files (*.fbx)\0*.fbx\0\0");
	if(fd.DoModal(this->m_hWnd)!= IDCANCEL)
	{
		void (TriFile::*Export[])(float, string, string)={NULL, &TriFile::ExportX, &TriFile::ExportObj, &TriFile::Export3ds, &TriFile::ExportMy, &TriFile::ExportVbo, &TriFile::ExportA3D, &TriFile::ExportFBX};
		CString ext[] = {"",".x", ".obj", ".3ds", ".my", ".vbo", ".a3d", ".fbx"};
		int extl[] = {0, 2, 4, 4, 3, 4, 4, 4};
		CString efile = fd.m_szFileTitle;
		CString efilepath = fd.m_szFileName;
		CString msg;
		msg.Format("Export to %s file completed!", ext[fd.m_ofn.nFilterIndex].Right(extl[fd.m_ofn.nFilterIndex]-1));
		int lf = efile.GetLength();
		efilepath.Delete(efilepath.GetLength() - lf, lf);
		if(!efile.Right(extl[fd.m_ofn.nFilterIndex]).Compare(ext[fd.m_ofn.nFilterIndex]))
		{
			efile.Delete(lf - extl[fd.m_ofn.nFilterIndex], extl[fd.m_ofn.nFilterIndex]);
		}
		file->textures = textures;
		ExportTextures(efilepath);
		(file->*Export[fd.m_ofn.nFilterIndex])((float)((m_Scale.GetPos()/20.0) + 0.05), (LPCSTR)efile, (LPCSTR)efilepath);
		MessageBox(msg, "Information", MB_ICONINFORMATION|MB_OK);
	}
	return 0;
}

void CMainDlg::ExportTextures(const CString &path)
{
	ILuint handle;
	ilInit();
	iluInit();
	ilOriginFunc(IL_ORIGIN_LOWER_LEFT);
	ilEnable(IL_ORIGIN_SET);
	for(int i = 0; i < m_Textures.GetCount(); i++)
	{
		if(texdata[i] != -1)
		{
			ilGenImages(1, & handle);
			ilBindImage(handle);
			CString file;
			m_Textures.GetText(i, file);
			vector<char> data;
			data.resize(sc.index[texdata[i]].fileSize);
			ifstream is;
			is.sync_with_stdio(false);
			is.open(sc.index[texdata[i]].cachename.c_str(), ios::binary|ios::in);
			is.read(reinterpret_cast<char*>(&data[0]), sc.index[texdata[i]].fileSize);
			is.close();
			ILboolean ret = ilLoadL(IL_DDS, reinterpret_cast<char*>(&data[0]), sc.index[texdata[i]].fileSize);
			file.Replace(".dds", ".png");
			iluFlipImage();
			ilSaveImage((path + file));
			/*ofstream out;
			out.sync_with_stdio(false);
			out.open(path + file, ios::binary);
			out.write(reinterpret_cast<char*>(&data[0]), sc.index[texdata[i]].fileSize);
			out.close();*/
			ilDeleteImages(1, & handle);
		}
	}
}

void CMainDlg::Add(const CString &texture,int data)
{
	dword tc = (dword)m_Textures.GetCount();
	if(tc < file->header.numSurfaces)
	{
		textures[tc] = (LPCSTR)texture;
		texdata[tc] = data;
		FillTextures();
	}
	else
	{
		MessageBox("You have to remove one of textures to add another", "Error", MB_ICONERROR | MB_OK);
	}
}

void CMainDlg::FillTextures()
{
	dword tc = (dword)m_Textures.GetCount();
	for(int i = tc; i >= 0 ; --i)
		m_Textures.DeleteString(i);
	for(dword i = 0; i < textures.size() ; i++)
		if(textures[i].compare(""))
			m_Textures.AddString(textures[i].c_str());
}

LRESULT CMainDlg::OnAdd(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CString texture;
	m_Texture.GetWindowText(texture);
	Add(texture);
	m_p3d.TextureChange(sc, texdata);
	return 0;
}

LRESULT CMainDlg::OnRemove(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	vector<string> tmp;
	vector<int> tmp1;
	tmp = textures;
	dword tc = (dword)m_Textures.GetCount();
	int sel = m_Textures.GetCaretIndex();
	if(sel!=-1)
	{
		for(dword i = 0, c = 0; i < tc; i++, c++)
		{
			if(i == sel)
				c++;
			if(c < tc)
			{
				textures[i] = textures[c];
				texdata[i] = texdata[c];
			}
			else
			{
				textures[i] = "";
				texdata[i] = -1;
			}
		}
		FillTextures();
		m_p3d.TextureChange(sc, texdata);
	}
	return 0;
}

void CMainDlg::EnableAll(BOOL bEnable)
{
	for(int i = m_Textures.GetCount(); i >= 0 ; i--)
		m_Textures.DeleteString(i);
	m_Light.EnableWindow(bEnable);
	m_Scale.EnableWindow(bEnable);
	m_eScale.EnableWindow(bEnable);
	m_Scale.SetPos(19);
	m_Texture.EnableWindow(bEnable);
	m_Textures.EnableWindow(bEnable);
	m_Add.EnableWindow(bEnable);
	m_Select.EnableWindow(bEnable);
	m_Remove.EnableWindow(bEnable);
	m_eScale.SetWindowText("1.0");
	m_Texture.SetWindowText("");
	m_VSize.SetWindowText("0");
	m_UpDown.SetPos(1);
	m_UpDown.EnableWindow(bEnable);
	m_TriVersion.SetWindowText("0.0");
	m_VCount.SetWindowText("0");
	m_BoxMax.SetWindowText("{0.0; 0.0; 0.0}");
	m_BoxMin.SetWindowText("{0.0; 0.0; 0.0}");
	m_SurType.SetWindowText("0");
	m_SurCount.SetWindowText("0");
	if(bEnable)
		menu.EnableMenuItem(ID_FILE_EXPORTMOD, MF_ENABLED);
	else
	{
		menu.EnableMenuItem(ID_FILE_EXPORTMOD, MF_GRAYED);
		SetWindowText("TriExporter_sharedcache");
	}
}

void CMainDlg::FillTree()
{
	m_Tree.DeleteAllItems();
	lvis.clear();
	string old = "";
	lvis[old] = TVI_ROOT;
	for(dword i = 0; i < sc.index.size(); i++)
	{
		CString f = sc.index[i].filename.c_str();
		//f.Replace('\\', '/');
		int u = 0;
		while((u = f.Find('/', u+1)) != -1)
		{
			CString n = f.Left(u);
			if(!lvis[(string)n])
			{
				CTreeItem a = m_Tree.InsertItem(n.Right(n.GetLength()- (old.length()==0?-1:old.length())-1),  1, 1, lvis[old] , TVI_SORT);
				a.SetData(0);
				lvis[(string)n] = a;
			}	
			old = n;
		}
	}
	for(dword i = 0; i < sc.index.size(); i++)
	{
		CString f = sc.index[i].filename.c_str();
		//f.Replace('\\', '/');
		int u = 0;
		while((u = f.Find('/', u+1)) != -1)
		{
			CString n = f.Left(u);
			if(!lvis[(string)n])
			{
				CTreeItem a = m_Tree.InsertItem(n.Right(n.GetLength()- (old.length()==0?-1:old.length())-1),  1, 1, lvis[old] , TVI_SORT);
				a.SetData(0);
				lvis[(string)n] = a;
			}	
			old = n;
		}
		CString fil;
		fil.Format("%s - %1.2fKB", f.Right(f.GetLength()-f.ReverseFind('/')-1), sc.index[i].fileSize/1024.0);
		CTreeItem fe;
		HTREEITEM order = TVI_LAST; //TVI_SORT
		if(!f.Right(4).Compare(".dds"))
			fe = m_Tree.InsertItem(fil, 2,2, lvis[old] , order);
		else if (!f.Right(4).Compare(".tri"))
			fe = m_Tree.InsertItem(fil, lvis[old] , order);
		else if (!f.Right(5).Compare(".blue"))
			fe = m_Tree.InsertItem(fil, 3,3, lvis[old] , order);
		else if (!f.Right(4).Compare(".ogg") || !f.Right(4).Compare(".wav") || !f.Right(4).Compare(".m3u") || !f.Right(4).Compare(".mp3"))
			fe = m_Tree.InsertItem(fil, 4,4, lvis[old] , order);
		else if (!f.Right(4).Compare(".jpg"))
			fe = m_Tree.InsertItem(fil, 5,5, lvis[old] , order);
		else if (!f.Right(4).Compare(".png"))
			fe = m_Tree.InsertItem(fil, 6,6, lvis[old] , order);
		else if (!f.Right(3).Compare(".py"))
			fe = m_Tree.InsertItem(fil, 7,7, lvis[old] , order);
		else if (!f.Right(4).Compare(".txt"))
			fe = m_Tree.InsertItem(fil, 8,8, lvis[old] , order);
		else if (!f.Right(4).Compare(".tga"))
			fe = m_Tree.InsertItem(fil, 9,9, lvis[old] , order);
		else if (!f.Right(3).Compare(".mb"))
			fe = m_Tree.InsertItem(fil, 9,9, lvis[old] , order);
		else if (!f.Right(4).Compare(".gr2"))
			fe = m_Tree.InsertItem(fil, 11,11, lvis[old] , order);
		else
			fe = m_Tree.InsertItem(fil, 10,10, lvis[old] , order);
		fe.SetData(i+1);
	}
}

LRESULT CMainDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// center the dialog on the screen
	//DlgResize_Init();
	
	//InitResizing(TRUE, TRUE, WS_THICKFRAME | WS_CLIPCHILDREN);
	CenterWindow();
	// set icons
	HICON hIcon = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDI_TRIEXPORTER), 
		IMAGE_ICON, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON), LR_DEFAULTCOLOR);
	SetIcon(hIcon, TRUE);
	HICON hIconSmall = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDI_SMALL), 
		IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);
	SetIcon(hIconSmall, FALSE);
	file = new TriFile;
	// register object for message filtering and idle updates
	loaded = false;
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->AddMessageFilter(this);
	pLoop->AddIdleHandler(this);
	//UIAddChildWindowContainer(m_hWnd);
	menu.Attach(GetMenu());
	m_Tree = GetDlgItem(IDC_TREE);
	il.Create(16,16, ILC_COLOR32,0,0);
	HANDLE bmp = LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDB_ICONS),IMAGE_BITMAP,0,0,LR_LOADTRANSPARENT);
	il.Add((HBITMAP)bmp);
	DeleteObject(bmp);
	m_Tree.SetImageList(il, TVSIL_NORMAL);
	m_p3d.SubclassWindow(GetDlgItem(IDC_PREVIEW));
	m_Scale.Attach(GetDlgItem(IDC_SCALE));
	m_Light.Attach(GetDlgItem(IDC_LIGHT));
	m_Fps.Attach(GetDlgItem(IDC_FPS));
	m_eScale.Attach(GetDlgItem(IDC_ESCALE));
	m_Texture.Attach(GetDlgItem(IDC_TEXTURE));
	m_VSize.Attach(GetDlgItem(IDC_VSIZE));
	m_TriVersion.Attach(GetDlgItem(IDC_TRIVERSION));
	m_VCount.Attach(GetDlgItem(IDC_VCOUNT));
	m_BoxMax.Attach(GetDlgItem(IDC_BOXMAX));
	m_BoxMin.Attach(GetDlgItem(IDC_BOXMIN));
	m_SurType.Attach(GetDlgItem(IDC_SURTYPE));
	m_SurCount.Attach(GetDlgItem(IDC_SURCOUNT));
	m_Textures.Attach(GetDlgItem(IDC_TEXTURES));
	m_Add.Attach(GetDlgItem(IDC_ADD));
	m_Remove.Attach(GetDlgItem(IDC_REMOVE));
	m_Select.Attach(GetDlgItem(IDC_SELECT));
	// Attach all 10 Checkboxes
	for(int i=0; i<10; i++)
		m_ChkSurfaces[i].Attach(GetDlgItem(IDC_SURFACE1+i));
	m_Scale.SetRange(0, 19);
	m_Scale.SetPageSize(5);
	m_UpDown.Attach(GetDlgItem(IDC_UPDOWN));
	m_Light.SetRange(0, 360);
	m_Light.SetPageSize(1);
	m_Light.SetPos(100);
	m_p3d.alight = 100/(180/D3DX_PI);
	CRegKey rk;
	if(rk.Open(HKEY_CURRENT_USER, "Software\\TriExporter") == ERROR_SUCCESS)
	{
		ULONG g=256;
		TCHAR path[256];
		rk.QueryStringValue("EVE", path, &g);
		rk.Close();
		if (sc.LoadDir(path))
			FillTree();
	}
	m_p3d.Render();
	EnableAll(FALSE);
	return TRUE;
}

LRESULT CMainDlg::OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CAboutDlg dlg;
	dlg.DoModal();
	return 0;
}

LRESULT CMainDlg::OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& bHandled)
{
	CloseDialog(0);
	return 0;
}

void CMainDlg::CloseDialog(int nVal)
{
	m_p3d.Detach();
	il.Destroy();
	delete file;
	DestroyWindow();
	::PostQuitMessage(nVal);
}


BOOL CMainDlg::PreTranslateMessage(MSG* pMsg)
{
	return CWindow::IsDialogMessage(pMsg);
}

void CMainDlg::UpdateFPS()
{
	const int FRAMEINTERVAL = 1000;
	static DWORD nFrames = 0;
	static DWORD nLastTick = GetTickCount();
	DWORD nTick = GetTickCount();
	if(nTick-nLastTick>=FRAMEINTERVAL)
	{	
		float fFPS = 1000.0f*(float)nFrames/(float)(nTick-nLastTick);
		nLastTick = nTick;
		nFrames = 0;
		char szFPS[256];
		sprintf_s(szFPS,"%.2f", fFPS);
		//snprintf(szFPS, "%.2f", fFPS);
		m_Fps.SetWindowText(szFPS);
	}
	nFrames++;
}

BOOL CMainDlg::OnIdle()
{
	UpdateFPS();
	m_p3d.Render();
	return FALSE;
}

LRESULT CMainDlg::OnTvnKeydownTree(int idCtrl, LPNMHDR pNMHDR, BOOL& bHandled)
{
	LPNMTVKEYDOWN pTVKeyDown = reinterpret_cast<LPNMTVKEYDOWN>(pNMHDR);
	if(pTVKeyDown->wVKey == VK_SPACE)
		OnTreeDblClick(idCtrl, pNMHDR, bHandled);
	return 0;
}
