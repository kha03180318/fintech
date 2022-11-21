
// ProcessManagerDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "ProcessManager.h"
#include "ProcessManagerDlg.h"
#include "MD5.h"
#include "framework.h"
#include "afxdialogex.h"
#include<stdio.h>
#include<string.h>
#include<locale.h>
#include<format>
#include<string>
#include<stdlib.h>
#include<tchar.h>
#include <strsafe.h>
#include<windows.h>
#include<tlhelp32.h>
#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <fcntl.h>
#include <filesystem>
#include <psapi.h>
#include <atlstr.h>
#include <openssl/md5.h>



using namespace std;

#pragma warning(disable : 4996)
#define STRING_LEN 1000

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CProcessManagerDlg 대화 상자



CProcessManagerDlg::CProcessManagerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_PROCESSMANAGER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CProcessManagerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ProcessList);
}

BEGIN_MESSAGE_MAP(CProcessManagerDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CProcessManagerDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CProcessManagerDlg::OnBnClickedButton2)
	ON_NOTIFY(HDN_ITEMCLICK, 0, &CProcessManagerDlg::OnLvnColumnclickList1)
	ON_BN_CLICKED(IDC_BUTTON3, &CProcessManagerDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CProcessManagerDlg::OnBnClickedButton4)
END_MESSAGE_MAP()


// CProcessManagerDlg 메시지 처리기
BOOL CProcessManagerDlg::OnInitDialog()
{


	CDialogEx::OnInitDialog();

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.

	m_ProcessList.InsertColumn(0, _T("Process Name"), LVCFMT_LEFT, 180);
	m_ProcessList.InsertColumn(1, _T("PID"), LVCFMT_LEFT, 90);
	m_ProcessList.InsertColumn(2, _T("PPID"), LVCFMT_LEFT, 90);
	m_ProcessList.InsertColumn(3, _T("Thread"), LVCFMT_LEFT, 90);
	m_ProcessList.InsertColumn(4, _T("Path"), LVCFMT_LEFT, 450);
	m_ProcessList.ModifyStyle(LVS_TYPEMASK, LVS_REPORT);
	m_ProcessList.SetExtendedStyle(LVS_EX_DOUBLEBUFFER | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	//현재 프로세스의 상태 스냅

	if (hProcessSnap == INVALID_HANDLE_VALUE) {
		_tprintf(_T("CreateToolhelp32Snapshot error \n"));
		exit(EXIT_FAILURE);
	}

	PROCESSENTRY32 pe32; //프로세스 정보 저장 구조체
	pe32.dwSize = sizeof(PROCESSENTRY32);

	Process32First(hProcessSnap, &pe32);

	DWORD value = MAX_PATH; // 경로 문자열의 최대 크기
	LPWSTR buffer = new WCHAR[MAX_PATH]; // 경로 문자열을 담을 버퍼

	int num = m_ProcessList.GetItemCount();

	UpdateData(TRUE);

	CString processName;
	CString processPID;
	CString processPPID;
	CString processThread;
	CString processPath;

	do {

		// 메모리 비우기
		ZeroMemory(buffer, value * sizeof(WCHAR));

		// 프로세스의 PID 가져오기
		DWORD pid = pe32.th32ProcessID;

		// 프로세스에 접근하고 핸들 얻어오기
		HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);

		// 프로세스의 전체 경로(Full-path) 구하기
		BOOL result = GetModuleFileNameExW(hProcess, NULL, buffer, value);

		// 결과가 0이 아니라면 성공!
		if (result != 0) {
			wprintf(L"%d\t%s\n", pid, buffer);
		}
		else {
			// 오류가 발생할 경우 오류 코드 출력
			int errCode = GetLastError();
			if (errCode == 6) {
				printf("%d\tError: ACCESS_DENIED\n", pid);
			}
			else {
				printf("%d\tError Code: %d\n", pid, errCode);
			}
		}

		processName.Format(_T("%25s"), pe32.szExeFile);
		processPID.Format(_T("%8d"), pe32.th32ProcessID);
		processPPID.Format(_T("%8d"), pe32.th32ParentProcessID);
		processThread.Format(_T("%8d"), pe32.cntThreads);
		processPath.Format(_T("%s"), buffer);

		/*double val = 0.0;
		if (cpuUsage.getCpuUsage(&val))
			processUsage.Format(_T("%f"), val);*/

		m_ProcessList.InsertItem(num, processName);
		m_ProcessList.SetItemText(num, 1, processPID);
		m_ProcessList.SetItemText(num, 2, processPPID);
		m_ProcessList.SetItemText(num, 3, processThread);
		m_ProcessList.SetItemText(num, 4, processPath);


		//processName.Empty();
		//processPID.Empty();
		//processPPID.Empty();
		//processThread.Empty();
		//processUsage.Empty();

		UpdateData(FALSE);

	} while (Process32Next(hProcessSnap, &pe32));

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CProcessManagerDlg::OnPaint()
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
HCURSOR CProcessManagerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CProcessManagerDlg::OnBnClickedButton1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	// 특정프로세스 종료
	HANDLE hProcess = NULL;
	BOOL isKill = FALSE;
	int count = m_ProcessList.GetItemCount(); // 리스트 컨트롤의 현재 항목 수

	for (int i = count; i >= 0; i--)
	{
		int num = 0;

		HANDLE hProcessSnap =
			CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

		if (hProcessSnap == INVALID_HANDLE_VALUE) {
			_tprintf(_T("CreateToolhelp32Snapshot error! \n"));
			return;
		}

		PROCESSENTRY32 pe32;
		pe32.dwSize = sizeof(PROCESSENTRY32);

		if (!Process32First(hProcessSnap, &pe32)) {
			_tprintf(_T("Process32First error ! \n"));
			CloseHandle(hProcessSnap);
			return;
		}

		if (m_ProcessList.GetItemState(i, LVIS_SELECTED) != 0) // 선택된 항목 삭제
		{

			CString value;
			CString processPID;

			do {
				processPID.Format(_T("%8d"), pe32.th32ProcessID);
				value = m_ProcessList.GetItemText(i, 1);

				if (_tcscmp(processPID, value) == 0) {
					hProcess = OpenProcess(
						PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);

					if (hProcess != NULL) {
						TerminateProcess(hProcess, -1);
						//핸들 정상 -> 프로세스 종료

						isKill = TRUE;
						CloseHandle(hProcess);
						break;
					}
					num++;
				}
			} while (Process32Next(hProcessSnap, &pe32));

			CloseHandle(hProcessSnap);
			m_ProcessList.DeleteItem(i);
		}
	}
}


void CProcessManagerDlg::OnBnClickedButton2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	m_ProcessList.DeleteAllItems();

	HANDLE hProcessSnap =
		CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	//현재 프로세스의 상태 스냅

	if (hProcessSnap == INVALID_HANDLE_VALUE) {
		_tprintf(_T("CreateToolhelp32Snapshot error \n"));
		exit(EXIT_FAILURE);
	}

	PROCESSENTRY32 pe32; //프로세스 정보 저장 구조체
	pe32.dwSize = sizeof(PROCESSENTRY32);

	Process32First(hProcessSnap, &pe32);

	UpdateData(TRUE);
	int num = m_ProcessList.GetItemCount();
	char malwareList[] = "msvsmon.exe, emet_agent.exe, emet_service.exe, firesvc.exe, firetray.exe, hipsvc.exe, mfevtps.exe, mcafeefire.exe, scan32.exe, shstat.exe, tbmon.exe, vstskmgr.exe, engineserver.exe, mfevtps.exe, mfeann.exe, mcscript.exe, updaterui.exe, udaterui.exe, naprdmgr.exe, frameworkservice.exe, cleanup.exe, cmdagent.exe, frminst.exe, mcscript_inuse.exe, mctray.exe, mcshield.exe, AAWTray.exe, MSASCui.exe, _avp32.exe, _avpcc.exe, _avpm.exe, aAvgApi.exe, ackwin32.exe, adaware.exe, advxdwin.exe, agentsvr.exe, agentw.exe, alertsvc.exe, alevir.exe, alogserv.exe, amon9x.exe, anti - trojan.exe, antivirus.exe, ants.exe, apimonitor.exe, aplica32.exe, apvxdwin.exe, arr.exe, atcon.exe, atguard.exe, atro55en.exe, atupdater.exe, atwatch.exe, au.exe, aupdate.exe, auto - protect.nav80try.exe, autodown.exe, autotrace.exe, autoupdate.exe, avconsol.exe, ave32.exe, avgcc32.exe, avgctrl.exe, avgemc.exe, avgnt.exe, avgrsx.exe, avgserv.exe, avgserv9.exe, avguard.exe, avgw.exe, avkpop.exe, avkserv.exe, avkservice.exe, avkwctl9.exe, avltmain.exe, avnt.exe, avp.exe, avp.exe, avp32.exe, avpcc.exe, avpdos32.exe, avpm.exe, avptc32.exe, avpupd.exe, avsched32.exe, avsynmgr.exe, avwin.exe, avwin95.exe, avwinnt.exe, avwupd.exe, avwupd32.exe, avwupsrv.exe, avxmonitor9x.exe, avxmonitornt.exe, avxquar.exe, backweb.exe, bargains.exe, bd_professional.exe, beagle.exe, belt.exe, bidef.exe, bidserver.exe, bipcp.exe, bipcpevalsetup.exe, bisp.exe, blackd.exe, blackice.exe, blink.exe, blss.exe, bootconf.exe, bootwarn.exe, borg2.exe, bpc.exe, brasil.exe, bs120.exe, bundle.exe, bvt.exe, ccapp.exe, ccevtmgr.exe, ccpxysvc.exe, ccsvchst.exe, cdp.exe, cfd.exe, cfgwiz.exe, cfiadmin.exe, cfiaudit.exe, cfinet.exe, cfinet32.exe, claw95.exe, claw95cf.exe, clean.exe, cleaner.exe, cleaner3.exe, cleanpc.exe, click.exe, cmesys.exe, cmgrdian.exe, cmon016.exe, connectionmonitor.exe, cpd.exe, cpf9x206.exe, cpfnt206.exe, ctrl.exe, cv.exe, cwnb181.exe, cwntdwmo.exe, datemanager.exe, dcomx.exe, defalert.exe, defscangui.exe, defwatch.exe, deputy.exe, divx.exe, dllcache.exe, dllreg.exe, doors.exe, dpf.exe, dpfsetup.exe, dpps2.exe, drwatson.exe, drweb32.exe, drwebupw.exe, dssagent.exe, dvp95.exe, dvp95_0.exe, ecengine.exe, efpeadm.exe, emsw.exe, ent.exe, esafe.exe, escanhnt.exe, escanv95.exe, espwatch.exe, ethereal.exe, etrustcipe.exe, evpn.exe, exantivirus - cnet.exe, avxw.exe, expert.exe, explore.exe, f - agnt95.exe, f - prot.exe, f - prot95.exe, f - stopw.exe, fameh32.exe, fast.exe, fch32.exe, fih32.exe, findviru.exe, firewall.exe, fnrb32.exe, fp - win.exe, fp - win_trial.exe, fprot.exe, frw.exe, fsaa.exe, fsav.exe, fsav32.exe, fsav530stbyb.exe, fsav530wtbyb.exe, fsav95.exe, fsgk32.exe, fsm32.exe, fsma32.exe, fsmb32.exe, gator.exe, gbmenu.exe, gbpoll.exe, generics.exe, gmt.exe, guard.exe, guarddog.exe, hacktracersetup.exe, hbinst.exe, hbsrv.exe, hotactio.exe, hotpatch.exe, htlog.exe, htpatch.exe, hwpe.exe, hxdl.exe, hxiul.exe, iamapp.exe, iamserv.exe, iamstats.exe, ibmasn.exe, ibmavsp.exe, icload95.exe, icloadnt.exe, icmon.exe, icsupp95.exe, icsuppnt.exe, idle.exe, iedll.exe, iedriver.exe, iface.exe, ifw2000.exe, inetlnfo.exe, infus.exe, infwin.exe, init.exe, intdel.exe, intren.exe, iomon98.exe, istsvc.exe, jammer.exe, jdbgmrg.exe, jedi.exe, kavlite40eng.exe, kavpers40eng.exe, kavpf.exe, kazza.exe, keenvalue.exe, kerio - pf - 213 - en - win.exe, kerio - wrl - 421 - en - win.exe, kerio - wrp - 421 - en - win.exe, kernel32.exe, killprocesssetup161.exe, launcher.exe, ldnetmon.exe, ldpro.exe, ldpromenu.exe, ldscan.exe, lnetinfo.exe, loader.exe, localnet.exe, lockdown.exe, lockdown2000.exe, lookout.exe, lordpe.exe, lsetup.exe, luall.exe, luau.exe, lucomserver.exe, luinit.exe, luspt.exe, mapisvc32.exe, mcagent.exe, mcmnhdlr.exe, mcshield.exe, mctool.exe, mcupdate.exe, mcvsrte.exe, mcvsshld.exe, md.exe, mfin32.exe, mfw2en.exe, mgavrtcl.exe, mgavrte.exe, mghtml.exe, mgui.exe, minilog.exe, mmod.exe, monitor.exe, moolive.exe, mostat.exe, mpfagent.exe, mpfservice.exe, mpftray.exe, mrflux.exe, msapp.exe, msbb.exe, msblast.exe, mscache.exe, msccn32.exe, mscman.exe, msconfig.exe, msdm.exe, msdos.exe, msiexe, c16.exe, msinfo32.exe, mslaugh.exe, msmgt.exe, msmsgri32.exe, mssmmc32.exe, mssys.exe, msvxd.exe, mu0311ad.exe, mwatch.exe, n32scanw.exe, nav.exe, navap.navapsvc.exe, navapsvc.exe, navapw32.exe, navdx.exe, navlu32.exe, navnt.exe, navstub.exe, navw32.exe, navwnt.exe, nc2000.exe, ncinst4.exe, ndd32.exe, neomonitor.exe, neowatchlog.exe, netarmor.exe, netd32.exe, netinfo.exe, netmon.exe, netscanpro.exe, netstat.exe, netutils.exe, nisserv.exe, nisum.exe, nmain.exe, nod32.exe, normist.exe, notstart.exe, npf40_tw_98_nt_me_2k.exe, npfmessenger.exe, nprotect.exe, npscheck.exe, npssvc.exe, nsched32.exe, nssys32.exe, nstask32.exe, nsupdate.exe, nt.exe, ntrtscan.exe, ntvdm.exe, ntxconfig.exe, nui.exe, nupgrade.exe, nvarch16.exe, nvc95.exe, nvsvc32.exe, nwinst4.exe, nwservice.exe, nwtool16.exe, ollydbg.exe, onsrvr.exe, optimize.exe, ostronet.exe, otfix.exe, outpost.exe, outpostinstall.exe, outpostproinstall.exe, padmin.exe, panixk.exe, patch.exe, pavcl.exe, pavproxy.exe, pavsched.exe, pavw.exe, pccwin98.exe, pcfwallicon.exe, pcip10117_0.exe, pcscan.exe, pdsetup.exe, periscope.exe, persfw.exe, perswf.exe, pf2.exe, pfwadmin.exe, pgmonitr.exe, pingscan.exe, platin.exe, pop3trap.exe, poproxy.exe, popscan.exe, portdetective.exe, portmonitor.exe, powerscan.exe, ppinupdt.exe, pptbc.exe, ppvstop.exe, prizesurfer.exe, prmt.exe, prmvr.exe, procdump.exe, processmonitor.exe, programauditor.exe, proport.exe, protectx.exe, pspf.exe, purge.exe, qconsole.exe, qserver.exe, rapapp.exe, rav7.exe, rav7win.exe, rav8win32eng.exe, ray.exe, rb32.exe, rcsync.exe, realmon.exe, reged.exe, regedit.exe, regedt32.exe, rescue.exe, rescue32.exe, rrguard.exe, rshell.exe, rtvscan.exe, rtvscn95.exe, rulaunch.exe, run32dll.exe, rundll.exe, rundll16.exe, ruxdll32.exe, safeweb.exe, sahagent.exe, scan32.exe, shstat.exe, tbmon.exe, vstskmgr.exe, engineserver.exe, mfevtps.exe, mfeann.exe, mcscript.exe, updaterui.exe, udaterui.exe, naprdmgr.exe, frameworkservice.exe, cleanup.exe, cmdagent.exe, frminst.exe, mcscript_inuse.exe, mctray.exe, mcshield.exe, save.exe, savenow.exe, sbserv.exe, sc.exe, scam32.exe, scan32.exe, scan95.exe, scanpm.exe, scrscan.exe, serv95.exe, setup_flowprotector_us.exe, setupvameeval.exe, sfc.exe, sgssfw32.exe, sh.exe, shellspyinstall.exe, shn.exe, showbehind.exe, smc.exe, sms.exe, smss32.exe, soap.exe, sofi.exe, sperm.exe, spf.exe, sphinx.exe, spoler.exe, spoolcv.exe, spoolsv32.exe, spyxx.exe, srexe.exe, srng.exe, ss3edit.exe, ssg_4104.exe, ssgrate.exe, st2.exe, start.exe, stcloader.exe, supftrl.exe, support.exe, supporter5.exe, svchostc.exe, svchosts.exe, sweep95.exe, sweepnet.sweepsrv.sys.swnetsup.exe, symproxysvc.exe, symtray.exe, sysedit.exe, sysupd.exe, taskmg.exe, taskmo.exe, taumon.exe, tbscan.exe, tc.exe, tca.exe, tcm.exe, tds2 - nt.exe, teekids.exe, tfak.exe, tfak5.exe, tgbob.exe, titanin.exe, titaninxp.exe, tracert.exe, trickler.exe, trjscan.exe, trjsetup.exe, trojantrap3.exe, tsadbot.exe, tvmd.exe, tvtmd.exe, undoboot.exe, updat.exe, update.exe, upgrad.exe, utpost.exe, vbcmserv.exe, vbcons.exe, vbust.exe, vbwin9x.exe, vbwinntw.exe, vcsetup.exe, vet32.exe, vet95.exe, vettray.exe, vfsetup.exe, vir - help.exe, virusmdpersonalfirewall.exe, vnlan300.exe, vnpc3000.exe, vpc32.exe, vpc42.exe, vpfw30s.exe, vptray.exe, vscan40.exe, vsched.exe, vsecomr.exe, vshwin32.exe, vsisetup.exe, vsmain.exe, vsmon.exe, vsstat.exe, vswin9xe.exe, vswinntse.exe, vswinperse.exe, w32dsm89.exe, w9x.exe, watchdog.exe, webdav.exe, webscanx.exe, webtrap.exe, wfindv32.exe, whoswatchingme.exe, wimmun32.exe, win - bugsfix.exe, win32.exe, win32us.exe, winactive.exe, window.exe, windows.exe, wininetd.exe, wininitx.exe, winlogin.exe, winmain.exe, winnet.exe, winppr32.exe, winrecon.exe, winservn.exe, winssk32.exe, winstart.exe, winstart001.exe, wintsk32.exe, winupdate.exe, wkufind.exe, wnad.exe, wnt.exe, wradmin.exe, wrctrl.exe, wsbgate.exe, wupdater.exe, wupdt.exe, wyvernworksfirewall.exe, xpf202en.exe, zapro.exe, zapsetup3001.exe, zatutor.exe, zonalm2601.exe, zonealarm.exe";
	CString processName;
	CString processPID;
	CString processPPID;
	CString processThread;
	CString processPath;

	DWORD value2 = MAX_PATH; // 경로 문자열의 최대 크기
	LPWSTR buffer = new WCHAR[MAX_PATH]; // 경로문자열을 담을 버퍼

	// 불필요한(악성) 프로세스 검출하면 출력해주는 소스
	char value[] = "[Dectection Process]\n\nProcess Name : ";

	do {

		// 메모리 비우기
		ZeroMemory(buffer, value2 * sizeof(WCHAR));

		// 프로세스의 PID 가져오기
		DWORD pid = pe32.th32ProcessID;

		// 프로세스에 접근하고 핸들 얻어오기
		HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);

		// 프로세스의 전체 경로(Full-path) 구하기
		BOOL result = GetModuleFileNameExW(hProcess, NULL, buffer, value2);

		// 결과가 0이 아니라면 성공!
		if (result != 0) {
			wprintf(L"%d\t%s\n", pid, buffer);
		}
		else {
			// 오류가 발생할 경우 오류 코드 출력
			int errCode = GetLastError();
			if (errCode == 6) {
				printf("%d\tError: ACCESS_DENIED\n", pid);
			}
			else {
				printf("%d\tError Code: %d\n", pid, errCode);
			}
		}

		processName.Format(_T("%25s"), pe32.szExeFile);
		processPID.Format(_T("%8d"), pe32.th32ProcessID);
		processPPID.Format(_T("%8d"), pe32.th32ParentProcessID);
		processThread.Format(_T("%8d"), pe32.cntThreads);
		processPath.Format(_T("%s"), buffer);

		m_ProcessList.InsertItem(num, processName);
		m_ProcessList.SetItemText(num, 1, processPID);
		m_ProcessList.SetItemText(num, 2, processPPID);
		m_ProcessList.SetItemText(num, 3, processThread);
		m_ProcessList.SetItemText(num, 4, processPath);

		char* pStr;
		int strSize = WideCharToMultiByte(CP_ACP, 0, pe32.szExeFile, -1, NULL, 0, NULL, NULL);
		pStr = new char[strSize];
		WideCharToMultiByte(CP_ACP, 0, pe32.szExeFile, -1, pStr, strSize, 0, 0);


		if (strstr(malwareList, pStr) != NULL)
		{
			char* malwareName = strcat(value, pStr);
			MessageBoxA(NULL, (LPCSTR)malwareName, "[INFO]", MB_OK);
			m_ProcessList.DeleteItem(*malwareName);
		}

		UpdateData(FALSE);

	} while (Process32Next(hProcessSnap, &pe32));
}

void CProcessManagerDlg::OnLvnColumnclickList1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	// 클릭한 컬럼의 인덱스
	int nColumn = pNMLV->iItem;

	// 현 리스트 컨트롤에 있는 데이터 총 자료 개수만큼 저장
	for (int i = 0; i < (m_ProcessList.GetItemCount()); i++) {
		m_ProcessList.SetItemData(i, i);
	}

	// 정렬 방식 저장
	m_bAscending = ~m_bAscending;

	// 정렬 관련 구조체 변수 생성 및 데이터 초기화
	SORTPARAM sortparams;
	sortparams.pList = &m_ProcessList;
	sortparams.iSortColumn = nColumn;
	sortparams.bSortDirect = m_bAscending;

	// | 0	  |  1   |  2    |  3      |   4   |
	// |Name  | PID  |  PPID |  Thread |  Path |

		// NAME은 알파벳 정렬
	if (nColumn == 0)
		sortparams.flag = 0;

	// PID 정렬
	if (nColumn == 1)
		sortparams.flag = 1;

	// PPID 정렬
	if (nColumn == 2)
		sortparams.flag = 2;

	// Thread 정렬
	if (nColumn == 3)
		sortparams.flag = 3;

	// Path 정렬
	if (nColumn == 3 || nColumn == 4)
		sortparams.flag = 4;

	// 정렬 함수 호출
	m_ProcessList.SortItems(&CompareItem, (LPARAM)&sortparams);
	*pResult = 0;
}

int CProcessManagerDlg::CompareItem(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	CListCtrl* pList = ((SORTPARAM*)lParamSort)->pList;
	int iSortColumn = ((SORTPARAM*)lParamSort)->iSortColumn;
	bool bSortDirect = ((SORTPARAM*)lParamSort)->bSortDirect;
	int flag = ((SORTPARAM*)lParamSort)->flag;

	LVFINDINFO info1, info2;
	info1.flags = LVFI_PARAM;
	info1.lParam = lParam1;

	info2.flags = LVFI_PARAM;
	info2.lParam = lParam2;

	int irow1 = pList->FindItem(&info1, -1);
	int irow2 = pList->FindItem(&info2, -1);

	CString strItem1 = pList->GetItemText(irow1, iSortColumn);
	CString strItem2 = pList->GetItemText(irow2, iSortColumn);

	// NAME 정렬
	if (flag == 0)
	{
		return	bSortDirect ? strcmp(LPSTR(LPCTSTR(strItem1)), LPSTR(LPCTSTR(strItem2))) : -strcmp(LPSTR(LPCTSTR(strItem1)), LPSTR(LPCTSTR(strItem2)));
	}

	// PID 정렬
	else if (flag == 1)
	{
		int iItem1 = _tstoi(strItem1);
		int iItem2 = _tstoi(strItem2);

		if (bSortDirect) {
			return iItem1 == iItem2 ? 0 : iItem1 > iItem2;
		}
		else {
			return iItem1 == iItem2 ? 0 : iItem1 < iItem2;
		}
	}
	// PPID 정렬
	if (flag == 2)
	{
		int iItem1 = _tstoi(strItem1);
		int iItem2 = _tstoi(strItem2);

		if (bSortDirect) {
			return iItem1 == iItem2 ? 0 : iItem1 > iItem2;
		}
		else {
			return iItem1 == iItem2 ? 0 : iItem1 < iItem2;
		}
	}

	// Thread 정렬
	if (flag == 3)
	{
		int iItem1 = _tstoi(strItem1);
		int iItem2 = _tstoi(strItem2);

		if (bSortDirect) {
			return iItem1 == iItem2 ? 0 : iItem1 > iItem2;
		}
		else {
			return iItem1 == iItem2 ? 0 : iItem1 < iItem2;
		}
	}

	// Path 정렬
	else if (flag == 4)
	{
		return	bSortDirect ? strcmp(LPSTR(LPCTSTR(strItem1)), LPSTR(LPCTSTR(strItem2))) : -strcmp(LPSTR(LPCTSTR(strItem1)), LPSTR(LPCTSTR(strItem2)));

	}

}
//초기화 버튼
void CProcessManagerDlg::OnBnClickedButton3()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	m_ProcessList.DeleteAllItems();

	HANDLE hProcessSnap =
		CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	//현재 프로세스의 상태 스냅

	if (hProcessSnap == INVALID_HANDLE_VALUE) {
		_tprintf(_T("CreateToolhelp32Snapshot error \n"));
		exit(EXIT_FAILURE);
	}

	PROCESSENTRY32 pe32; //프로세스 정보 저장 구조체
	pe32.dwSize = sizeof(PROCESSENTRY32);

	Process32First(hProcessSnap, &pe32);

	UpdateData(TRUE);
	int num = m_ProcessList.GetItemCount();

	CString processName;
	CString processPID;
	CString processPPID;
	CString processThread;
	CString processPath;

	DWORD value2 = MAX_PATH; // 경로 문자열의 최대 크기
	LPWSTR buffer = new WCHAR[MAX_PATH]; // 경로 문자열을 담을 버퍼

	do {

		// 메모리 비우기
		ZeroMemory(buffer, value2 * sizeof(WCHAR));

		// 프로세스의 PID 가져오기
		DWORD pid = pe32.th32ProcessID;

		// 프로세스에 접근하고 핸들 얻어오기
		HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);

		// 프로세스의 전체 경로(Full-path) 구하기
		BOOL result = GetModuleFileNameExW(hProcess, NULL, buffer, value2);

		// 결과가 0이 아니라면 성공!
		if (result != 0) {
			wprintf(L"%d\t%s\n", pid, buffer);
		}
		else {
			// 오류가 발생할 경우 오류 코드 출력
			int errCode = GetLastError();
			if (errCode == 6) {
				printf("%d\tError: ACCESS_DENIED\n", pid);
			}
			else {
				printf("%d\tError Code: %d\n", pid, errCode);
			}
		}

		processName.Format(_T("%25s"), pe32.szExeFile);
		processPID.Format(_T("%8d"), pe32.th32ProcessID);
		processPPID.Format(_T("%8d"), pe32.th32ParentProcessID);
		processThread.Format(_T("%8d"), pe32.cntThreads);
		processPath.Format(_T("%s"), buffer);

		m_ProcessList.InsertItem(num, processName);
		m_ProcessList.SetItemText(num, 1, processPID);
		m_ProcessList.SetItemText(num, 2, processPPID);
		m_ProcessList.SetItemText(num, 3, processThread);
		m_ProcessList.SetItemText(num, 4, processPath);

		UpdateData(FALSE);
		cin.ignore();
		//buffer//memset((void*)lpbuffer, NULL, FileSize);
	}while (Process32Next(hProcessSnap, &pe32));
}
void CProcessManagerDlg::OnBnClickedButton4()
{

	bool isDebug = true;

	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	m_ProcessList.DeleteAllItems();

	HANDLE hProcessSnap =
		CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	//현재 프로세스의 상태 스냅

	if (hProcessSnap == INVALID_HANDLE_VALUE) {
		_tprintf(_T("CreateToolhelp32Snapshot error \n"));
		exit(EXIT_FAILURE);
	}

	PROCESSENTRY32 pe32; //프로세스 정보 저장 구조체
	pe32.dwSize = sizeof(PROCESSENTRY32);

	Process32First(hProcessSnap, &pe32);

	UpdateData(TRUE);
	int num = m_ProcessList.GetItemCount();
	char* filepath[] = {0};
	filepath[0] = { "C:\\Users\\USER\\Desktop\\test.exe" }; //C:\\Program Files (x86)\\Google\\Chrome Remote Desktop\\96.0.4664.39\\remoting_host.exe|72536,// "C:\ProgramData\Microsoft\Windows\Start Menu\Programs\Accessories\notepad.exe|1175"
	char* filemd5hash[] = { "9dd96f5cee9fca1cf46abd1a90036e14" }; //, "aa0af438799fc5069b1bcb4c320680951" e7a7334d8c5a4e1c5d1ab93cee72321a1

	// 김현아 작업구간 변수설정 시작 //
	//CString strInputPath= "C:\Qt\qtcreator - 6.0.2\QtCreatorUninst.exe";
	// 김현아 작업구간 변수설정 끝 //

	CString processName;
	CString processPID;
	CString processPPID;
	CString processThread;
	CString processPath;

	DWORD path_s = MAX_PATH; // 경로 문자열의 최대 크기
	LPWSTR f_path = new WCHAR[MAX_PATH]; // 경로 문자열을 담을 버퍼
	// 무결성 검사
	char value[] = "[Integ Process]\n\nProcess Name : ";

	do {

		// 메모리 비우기
		ZeroMemory(f_path, path_s * sizeof(WCHAR));

		// 프로세스의 PID 가져오기
		DWORD pid = pe32.th32ProcessID;

		// 프로세스에 접근하고 핸들 얻어오기
		HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);

		// 프로세스의 전체 경로(Full-path) 구하기
		BOOL result = GetModuleFileNameExW(hProcess, NULL, f_path, path_s);

		// 결과가 0이 아니라면 성공!
		if (result != 0) {

			wprintf(L"%d\t%s\n", pe32.th32ProcessID, f_path);
			//OutputDebugString(f_path);
			//OutputDebugString(_T("\n"));

			/* 여기에 프로세스 경로 저장되어 있음 */
			string lpString;

			/* LPWSTR -> char* 변환 */
			char* pStr;
			//char lpbuffer[4096];
			int strSize = WideCharToMultiByte(CP_ACP, 0, pe32.szExeFile, -1, NULL, 0, NULL, NULL);
			int Rresult;
			pStr = new char[strSize];
			WideCharToMultiByte(CP_ACP, 0, pe32.szExeFile, -1, pStr, strSize, 0, 0);
			int size = wcstombs(NULL, f_path, 0); // 유니코드 스트링을 멀티 바이트 스트링으로 변환, 메모리 사이즈를 리턴해 줍니다.
			char* buffer = (char*)malloc(size + 1); // wcslen 대신에 얻어온 사이즈로 malloc //-1하면 띄어쓰기 없어지지만 에러 //malloc는 그 크기만큼 동적 할당을 해서 void*으로 포인터를 반환하고 할당 실패시 NULL을 반환합니다.
			//경로가 잘리는 경우 wcstombs() 가 -1을 리턴하기 때문에 +1을 해줌
			wcstombs(buffer, f_path, size + 1); // 형 변경

			OutputDebugStringA(buffer);

			/* 파일경로를 MD5로 암호화해서 string 변수에 담음 */
			//MyString = MD5String(buffer);

			/* DEBUG : 파일경로 MD5 결과 출력
			if (isDebug) {
				OutputDebugStringA(MyString.c_str());
				OutputDebugString(_T("\n"));
			}
			OutputDebugStringA(MyString.c_str());
			OutputDebugString(_T("\n"));
			*/

			DWORD readn;
			/* 위에서 구해두었던 파일경로로 핸들 생성 */
			HANDLE hFile = CreateFile(f_path, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
			unsigned int FileSize = GetFileSize(hFile, NULL);
			//LPVOID* lpbuffer = (LPVOID*)malloc(FileSize);
			//BYTE lpbuffer[FileSize];

			BYTE* lpbuffer;

			lpbuffer = (BYTE*)malloc(FileSize);
			memset((void*)lpbuffer, NULL, FileSize);
			//Rresult = { 0 };
			Rresult = ReadFile(hFile, lpbuffer, FileSize, &readn, 0x0);

			unsigned char MD5FileBuffer[32];
			MD5_CTX ctx;


			MD5_Init(&ctx);
			MD5_Update(&ctx, lpbuffer, FileSize);
			MD5_Final(MD5FileBuffer, &ctx);

			//MD5FileBuffer = MD5String((char *)lpbuffer);
			//OutputDebugStringA((LPCSTR)lpbuffer);
			//OutputDebugString(_T("\n"));
			/* GetFileSize 함수로 사이즈 구해서 반환 */

			OutputDebugStringA((LPCSTR)MD5FileBuffer);
			//MD5FileBuffer[33] = NULL;
			free(lpbuffer); //free 안하면 메모리 많이 소요. 예외 처리 오류
			CloseHandle(hFile);

			char test1[33];

			for (int i = 0; i < 16; i++) {
				sprintf(test1+(2*i), "%02x", MD5FileBuffer[i]); //2바이트씩 읽어 오기
			}
			
			test1[32] = NULL; //해시값 만큼만 저장// 길어질 시 쓰게기값'?' 값 들어감


			std::string sibal_test1(reinterpret_cast<const char*>(MD5FileBuffer), sizeof(MD5FileBuffer) / sizeof(MD5FileBuffer[0]));
			//sibal_test1 = static_cast<std::string>(reinterpret_cast<const char*>(MD5FileBuffer));

			
			//파일크기를 구해온 경우에만
			if (FileSize != -1) {

				//파일경로와 파일크기를 하나의 문자열로 합하기
				//char cSize[500] = {}; //동적 할당 해제시 파일 경로 여백에 쓰레기 값 들어감

				/*string encryptData(buffer); //buffer -> path ->encryptData
				encryptData += '|'; //path
				//int로 받은 파일크기를 char* 로 변환
				itoa(FileSize, cSize, 10);

				string sSize(cSize);
				encryptData += sSize;
				//encryptData += lpbuffer;
			*/



			/*	if (isDebug) {
					 //DEBUG : 파일경로|파일크기 결과 출력
					OutputDebugStringA(encryptData.c_str());
					OutputDebugString(_T("\n"));
				}

				char temp[500] = {};
				strcpy(temp, encryptData.c_str());

				MyString = MD5String(temp);

				if (isDebug) {
					// DEBUG : md5결과 출력
					OutputDebugStringA(MyString.c_str());
					OutputDebugString(_T("\n"));
				}*/


			int length = 0;

			std::string sibal_test2(filemd5hash[0]);


			for (int i = 0; i <= length; i++) {
				if (strstr(filepath[i], buffer) != NULL) { //filepath[i]에서 buffer를 찾았다면
					//string testMD5 = MD5String(filepath[i]);
					//프로세스의 MD5와 저장해둔 값으로 얻은 MD5가 같다면
					if ((strcmp(test1, filemd5hash[i]))) {//&ctx와 test hash가 같지 않으면 //두개가 동일한데 들어감 왜????????????????????????????????????
						char* integrity_Name = strcat(value, pStr); //value : detection process name

						MessageBoxA(NULL, (LPCSTR)integrity_Name, "[INFO]", MB_OK);

					}
				}

			}// wcstombs 함수 종료 문자열 설정, 동적 배열 할당 해제 delete[] 설정, 시도 했지만 예외처리 중단점 에러 안사라짐



		}
		else {
			// 오류가 발생할 경우 오류 코드 출력
			int errCode = GetLastError();
			if (errCode == 6) {

				printf("%d\tError: ACCESS_DENIED\n", pe32.th32ProcessID);
			}
			else {
				printf("%d\tError Code: %d\n", pe32.th32ProcessID, errCode);
			}
		}


	}
		processName.Format(_T("%25s"), pe32.szExeFile);
		processPID.Format(_T("%8d"), pe32.th32ProcessID);
		processPPID.Format(_T("%8d"), pe32.th32ParentProcessID);
		processThread.Format(_T("%8d"), pe32.cntThreads);
		processPath.Format(_T("%s"), f_path);


		m_ProcessList.InsertItem(num, processName);
		m_ProcessList.SetItemText(num, 1, processPID);
		m_ProcessList.SetItemText(num, 2, processPPID);
		m_ProcessList.SetItemText(num, 3, processThread);
		m_ProcessList.SetItemText(num, 4, processPath);
		UpdateData(FALSE);

	} while(Process32Next(hProcessSnap, &pe32));
}