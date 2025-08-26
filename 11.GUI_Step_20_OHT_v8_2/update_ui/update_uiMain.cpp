/***************************************************************
 * Name:      update_uiMain.cpp
 * Purpose:   Code for Application Frame
 * Author:     ()
 * Created:   2022-03-08
 * Copyright:  ()
 * License:
 **************************************************************/

#ifdef WX_PRECOMP
#include "wx_pch.h"
#endif

#ifdef __BORLANDC__
#pragma hdrstop
#endif //__BORLANDC__

#include "update_uiMain.h"

// --------------------------------
//       sub utility function
// --------------------------------
// #####################################################################
//      Network exist checking function
// #####################################################################
#define IS_NOT_FOUND    0
#define IS_FILE         1
#define IS_DIRECTORY    2

#define FIRM_REFRESH_TIME   6
#define PARAM_REFRESH_TIME	5

#define RESULT_SUCCESS      1
#define RESULT_FAIL			0

#define AXIS_DRIVING		1
#define AXIS_HOIST			2
#define AXIS_SLIDE			3

// ============ Data type
// 1byte
#define TYPE_USINT		0
#define TYPE_SINT		1

// 2byte
#define TYPE_UINT		2
#define TYPE_INT		3

// 4byte
#define TYPE_UDINT		4
#define TYPE_DINT		5


// ============ Special define

// (1) Get error code for axis ------
#define GET_ERR_CODE_DRIVING_FRONT_MAIN	0x683F
#define GET_ERR_CODE_DRIVING_FRONT_SUB	0x0000

#define GET_ERR_CODE_DRIVING_REAR_MAIN	0x603F
#define GET_ERR_CODE_DRIVING_REAR_SUB	0x0000

#define GET_ERR_CODE_HOIST_MAIN	 		0x603F
#define GET_ERR_CODE_HOIST_SUB	 		0x0000

#define GET_ERR_CODE_SLIDE_MAIN	 		0x603F
#define GET_ERR_CODE_SLIDE_SUB	 		0x0000

#define GET_ERR_CODE_DATA_TYPE			TYPE_UINT


// (2) Error clear ------
#define ERROR_CLEAR_DRIVING_FRONT_MAIN	0x6840
#define ERROR_CLEAR_DRIVING_FRONT_SUB	0x0000

#define ERROR_CLEAR_DRIVING_REAR_MAIN	0x6040
#define ERROR_CLEAR_DRIVING_REAR_SUB	0x0000

#define ERROR_CLEAR_HOIST_MAIN	 		0x6040
#define ERROR_CLEAR_HOIST_SUB	 		0x0000

#define ERROR_CLEAR_SLIDE_MAIN	 		0x6040
#define ERROR_CLEAR_SLIDE_SUB	 		0x0000

#define ERROR_CLEAR_DATA_TYPE			TYPE_UINT


// (3) Encoder reset ------
#define ENCODER_RESET_DRIVING_MAIN		0x2C0A
#define ENCODER_RESET_DRIVING_SUB		0x0000

#define ENCODER_RESET_HOIST_MAIN		0x300A
#define ENCODER_RESET_HOIST_SUB			0x0000

#define ENCODER_RESET_SLIDE_MAIN		0x300A
#define ENCODER_RESET_SLIDE_SUB			0x0000

#define ENCODER_RESET_DATA_TYPE			TYPE_USINT
#define ENCODER_RESET_COMMAND			1


int iGetValue_Global = 0;

int checkIfFileExists(const char* filename) {
    struct stat buffer;
    int exist = stat(filename, &buffer);
    if (exist != -1)
    {
        if (S_ISDIR(buffer.st_mode))
            return IS_DIRECTORY;
        else
            return IS_FILE;  // no path. but is file
    }
    else
        return IS_NOT_FOUND;
}

void network_check(bool& net_02, bool& net_03)
{
    const char* network_02_file = "/sys/bus/pci/drivers/igb/0000:02:00.0";
    const char* network_03_file = "/sys/bus/pci/drivers/igb/0000:03:00.0";

    if (checkIfFileExists(network_02_file) == IS_DIRECTORY)
        net_02 = true;
    else
        net_02 = false;


    if (checkIfFileExists(network_03_file) == IS_DIRECTORY)
        net_03 = true;
    else
        net_03 = false;
}

// #####################################################################
//      network bind, unbind function
// #####################################################################
#define  BUFF_SIZE      1024
int network_bind_command(std::string szNetIP)
{
    char    buff[BUFF_SIZE];
    char    MakeCmd[BUFF_SIZE];
    bool     is_connected = false;
    FILE* fp;

    sprintf(MakeCmd, "echo {} | sudo -S sh -c \"echo %s | tee /sys/bus/pci/drivers/igb/bind\"", szNetIP.c_str());
    fp = popen(MakeCmd, "r");

    if (NULL != fp)
    {
        while (fgets(buff, BUFF_SIZE, fp))
        {
            if (NULL != index(buff, '1'))     // if result value => "1" exist.. ping success. else ping error
            {
                is_connected = true;
                break;
            }
        }
        pclose(fp);
    }
    return  is_connected;
}


int network_manager_command(std::string szNetIP)
{
    char    buff[BUFF_SIZE];
    char    MakeCmd[BUFF_SIZE];
    bool     is_connected = false;
    FILE* fp;

    sprintf(MakeCmd, "sudo ip link set enp2s0 %s", szNetIP.c_str());
	printf("##### Network Manager CMD : %s \n", MakeCmd);

    fp = popen(MakeCmd, "r");

    if (NULL != fp)
    {
        while (fgets(buff, BUFF_SIZE, fp))
        {
            if (NULL != index(buff, '1'))     // if result value => "1" exist.. ping success. else ping error
            {
                is_connected = true;
                break;
            }
        }
        pclose(fp);
    }

    printf("Waiting for port -> enp2s0.....\n");
    sleep(2);

    return  is_connected;
}

int network_unbind_command(std::string szNetIP)
{
    char    buff[BUFF_SIZE];
    char    MakeCmd[BUFF_SIZE];
    bool     is_connected = false;
    FILE* fp;

    sprintf(MakeCmd, "echo {} | sudo -S sh -c \"echo %s | tee /sys/bus/pci/drivers/igb/unbind\"", szNetIP.c_str());
    fp = popen(MakeCmd, "r");

    if (NULL != fp)
    {
        while (fgets(buff, BUFF_SIZE, fp))
        {
            if (NULL != index(buff, '1'))     // if result value => "1" exist.. ping success. else ping error
            {
                is_connected = true;
                break;
            }
        }
        pclose(fp);
    }
    return  is_connected;
}

// Warning!! Un-bind function!!
int network_Un_bind_command(std::string szNetIP)
{
    char    buff[BUFF_SIZE];
    char    MakeCmd[BUFF_SIZE];
    bool     is_connected = false;
    FILE* fp;

    sprintf(MakeCmd, "echo {} | sudo -S sh -c \"echo %s | tee /sys/bus/pci/drivers/igb/unbind\"", szNetIP.c_str());
    fp = popen(MakeCmd, "r");

    if (NULL != fp)
    {
        while (fgets(buff, BUFF_SIZE, fp))
        {
            if (NULL != index(buff, '1'))     // if result value => "1" exist.. ping success. else ping error
            {
                is_connected = true;
                break;
            }
        }
        pclose(fp);
    }
    return  is_connected;
}

// #####################################################################
//      shell command
// #####################################################################

// sub return value -> only 0 or 1 only (integer)
// for firmware update
// 0 => fault,  1 => success
int shell_0_or_1(std::string shellCmd)
{
    int iReturn = 0;
    char    MakeCmd[BUFF_SIZE];

    sprintf(MakeCmd, "%s", shellCmd.c_str());
    iReturn = system(MakeCmd);
    iReturn = WEXITSTATUS(iReturn);

    return iReturn;
}

std::string shell_str_rtn(std::string shellCmd)
{
    char    MakeCmd[BUFF_SIZE];
    ostringstream output;

    sprintf(MakeCmd, "%s", shellCmd.c_str());
    FILE* stream = popen(MakeCmd, "r");

    while (!feof(stream) && !ferror(stream))
    {
        char buf[128];
        int bytesRead = fread(buf, 1, 128, stream);
        output.write(buf, bytesRead);
    }

    string result = output.str();
    // cout << "shell result" << result << endl;

    return result;
}

// #####################################################################
//      ecmaster process find & kill
// #####################################################################
int ecmaster_process_kill(int iProcessNum)
{
    char    buff[BUFF_SIZE];
    char    MakeCmd[BUFF_SIZE];
    bool     is_connected = false;
    FILE* fp;

    sprintf(MakeCmd, "sudo kill -9 %d", iProcessNum);
    fp = popen(MakeCmd, "r");

    if (NULL != fp)
    {
        while (fgets(buff, BUFF_SIZE, fp))
        {
            if (NULL != index(buff, '1'))
            {
                is_connected = true;
                break;
            }
        }
        pclose(fp);
    }
    return  is_connected;
}

bool ecmaster_process_find(std::string strProcessName)
{
    bool iResult = false;  // false - not found, true - founded
    FILE* fp;
    char    buff[BUFF_SIZE];

    // Step 1. make process list

    char    MakeCmd[BUFF_SIZE];

    sprintf(MakeCmd, "ps -ef | grep '%s' > ps_list.txt", strProcessName.c_str());
    fp = popen(MakeCmd, "r");

    if (NULL != fp)
    {
        fgets(buff, BUFF_SIZE, fp);
        pclose(fp);
    }

    // Step 2. process file line num check
    int iProcessLineCnt = 0;
    char c;
    fp = fopen("ps_list.txt", "r");

    while ((c = fgetc(fp)) != EOF)
        if (c == '\n')
            iProcessLineCnt++;
    fclose(fp);

    // Step 3. process kill Go / No Go
    int iProcessNum = 0;

    if (iProcessLineCnt < 3)    // no process found
    {
        iResult = false;
    }
    else // process kill
    {
#if 0   // 2023-06-12 fix
        fp = fopen("ps_list.txt", "r");
        fscanf(fp, "%s %d", buff, &iProcessNum);
        fclose(fp);

        ecmaster_process_kill(iProcessNum);
#else
        wxMessageBox(wxT("EC-Master program is already running!! \n\n EC-Master close and re-run please!"), wxT("ERROR"), wxITEM_CHECK);
        exit(-1);
#endif

        iResult = true;
    }

    return iResult;
}

// ==============================================

// helper functions
enum wxbuildinfoformat {
    short_f, long_f };

struct SEPERATE
{
    std::string driving;
    std::string hoist;
    std::string slide;
};

struct FILE_LOC
{
     SEPERATE FACTORY_ROM;
     SEPERATE NEW_ROM;
     SEPERATE PARAM;
} FILE_LOCATION;

wxString wxbuildinfo(wxbuildinfoformat format)
{
    wxString wxbuild(wxVERSION_STRING);

    if (format == long_f )
    {
#if defined(__WXMSW__)
        wxbuild << _T("-Windows");
#elif defined(__WXMAC__)
        wxbuild << _T("-Mac");
#elif defined(__UNIX__)
        wxbuild << _T("-Linux");
#endif

#if wxUSE_UNICODE
        wxbuild << _T("-Unicode build");
#else
        wxbuild << _T("-ANSI build");
#endif // wxUSE_UNICODE
    }

    return wxbuild;
}

void update_uiFrame::load_version_info()
{
 // initialize (INI load)
  std::string fullpath = "file_version.ini";
  INI::File Loader;
  Loader.Load(fullpath);

#if 0
  FILE_LOCATION.FACTORY_ROM.driving = Loader.GetSection("FACTORY_FIRMWARE")->GetValue("driving",-1).AsString();
  FILE_LOCATION.FACTORY_ROM.hoist   = Loader.GetSection("FACTORY_FIRMWARE")->GetValue("hoist",-1).AsString();
  FILE_LOCATION.FACTORY_ROM.slide   = Loader.GetSection("FACTORY_FIRMWARE")->GetValue("slide",-1).AsString();
#endif

  FILE_LOCATION.NEW_ROM.driving     = Loader.GetSection("NEW_FIRMWARE")->GetValue("driving",-1).AsString();
  FILE_LOCATION.NEW_ROM.hoist       = Loader.GetSection("NEW_FIRMWARE")->GetValue("hoist",-1).AsString();
  FILE_LOCATION.NEW_ROM.slide       = Loader.GetSection("NEW_FIRMWARE")->GetValue("slide",-1).AsString();

  FILE_LOCATION.PARAM.driving       = Loader.GetSection("PARAMETER")->GetValue("driving",-1).AsString();
  FILE_LOCATION.PARAM.hoist         = Loader.GetSection("PARAMETER")->GetValue("hoist",-1).AsString();
  FILE_LOCATION.PARAM.slide         = Loader.GetSection("PARAMETER")->GetValue("slide",-1).AsString();
}

std::string update_uiFrame::find_alarm_message(std::string strFind)
{
  std::string fullpath = "alarm.ini";
  INI::File Loader;
  Loader.Load(fullpath);

  std::string find_msg = Loader.GetSection(strFind)->GetValue("message",-1).AsString();

  return find_msg;
}

bool update_uiFrame::chkFileExist_firm_update()
{
  // (1-1) firm_update
  bool bFindResult = false;

  const char* loc__firm_update = "./firm_update";
  int chk_result__firm_update = checkIfFileExists(loc__firm_update);
  if(chk_result__firm_update == IS_FILE)
  {
        m_text_file_check_firm_update->SetValue(wxT("OK"));
        bFindResult = true;
  }
  else
  {
    m_text_file_check_firm_update->SetValue(wxT("ERR"));
    wxMessageBox(wxT("firm_update file not found. \n Check please!!"), wxEmptyString, wxITEM_NORMAL);
  }

  return bFindResult;
}

bool update_uiFrame::chkFileExist_slaveinfo()
{
  // (1-1) slaveinfo
  const char* loc__firm_update = "./slaveinfo";
  int chk_result__firm_update = checkIfFileExists(loc__firm_update);
  if(chk_result__firm_update == IS_FILE)
  {
      return true;
  }
  else
  {
      wxMessageBox(wxT("slaveinfo file not found. \n Check please!!"), wxEmptyString, wxITEM_NORMAL);
      return false;
  }
}

bool update_uiFrame::chkFileExist_firm_version()
{
  // (1-2) firm_version.ini
  bool bFindResult = false;

  const char* loc__firm_version = "./file_version.ini";
  int chk_result__firm_version = checkIfFileExists(loc__firm_version);
  if(chk_result__firm_version == IS_FILE)
  {
      m_text_file_check_version_file->SetValue(wxT("OK"));
      bFindResult = true;
  }
  else
  {
    m_text_file_check_version_file->SetValue(wxT("ERR"));
    wxMessageBox(wxT("file_version.ini file not found. \n Check please!!"), wxEmptyString, wxITEM_NORMAL);
  }

  return bFindResult;
}

void update_uiFrame::chkProcess_EcMaster()
{
    const char* network_02_open = "0000:02:00.0";
    const char* network_03_open = "0000:03:00.0";

    bool net_02 = false;
    bool net_03 = false;

    bool bFound_Ecmaster = ecmaster_process_find("ecmaster");
    if(bFound_Ecmaster == true)
    {
        sleep(1);
        m_text_file_check_ecat_port->SetValue(wxT("Auto Closed"));
    }
    else
    {
        m_text_file_check_ecat_port->SetValue(wxT("Not Detect"));
    }

    network_check(net_02, net_03);
    printf("\n\t NET_02 = %d  NET_03 = %d \n", net_02, net_03);

    if (net_02 == false)
    {
        network_bind_command(network_02_open);
        printf("Network port (02) auto open \n");
    }
    else
    {
        printf("Network port (02) already exist \n");
    }

    if (net_03 == false)
    {
        network_bind_command(network_03_open);
        printf("Network port (03) auto open \n");
    }
    else
    {
        printf("Network port (03) already exist \n");
    }

    if((net_02 == false) || (net_03 == false))
    {
            printf("Waiting for network openning.....\n");
            sleep(3);
            m_text_auto_set_ecat_port->SetValue(wxT("OPEN OK"));
    }
    else
            m_text_auto_set_ecat_port->SetValue(wxT("Already OK"));


	// network manager up/down
	const char* net_manager_up = "up";
	const char* net_manager_down = "down";
	network_manager_command(net_manager_up);
}

// file_version.txt loading
int update_uiFrame::chkFirm_version()
{
    FILE *fp;
    std::string strChmodCmd;
    int iRunResult = RESULT_FAIL;
    char cBuffer[100];

    // 01. motion
    strChmodCmd = "sudo ./firm_update /v enp2s0 1";
    iRunResult = shell_0_or_1(strChmodCmd);

	if(iRunResult == RESULT_SUCCESS)
	{
	    fp = fopen("firm_version.txt", "r");
	    if(fp != NULL)
	    {
	        fscanf(fp, "%s", cBuffer);
	        fclose(fp);
	        m_text_current_ver_driving->SetValue(cBuffer);
	    }
	}
    else
        m_text_current_ver_driving->SetValue(wxT("ERR"));

    // 02. hoist
    strChmodCmd = "sudo ./firm_update /v enp2s0 2";
    iRunResult = shell_0_or_1(strChmodCmd);

	if(iRunResult == RESULT_SUCCESS)
	{
	    fp = fopen("firm_version.txt", "r");
	    if(fp != NULL)
	    {
	        fscanf(fp, "%s", cBuffer);
	        fclose(fp);
	        m_text_current_ver_hoist->SetValue(cBuffer);
	    }
	}
    else
        m_text_current_ver_hoist->SetValue(wxT("ERR"));

    // 03. slide
    strChmodCmd = "sudo ./firm_update /v enp2s0 3";
    iRunResult = shell_0_or_1(strChmodCmd);

	if(iRunResult == RESULT_SUCCESS)
	{
	    fp = fopen("firm_version.txt", "r");
	    if(fp != NULL)
	    {
	        fscanf(fp, "%s", cBuffer);
	        fclose(fp);
	        m_text_current_ver_slide->SetValue(cBuffer);
	    }
	}
    else
        m_text_current_ver_slide->SetValue(wxT("ERR"));


    return iRunResult;
}

// common_read.txt
int update_uiFrame::GetCommon(int iSlave_Num, int iAddr_Index, int iAddr_Sub, int iData_Length, int& iGetValue)
{
    FILE *fp;
    std::string strChmodCmd;
    int iRunResult = RESULT_FAIL;

    // 01. motion
    strChmodCmd = "sudo ./firm_update /g enp2s0 " + std::to_string(iSlave_Num) + " "
                    + std::to_string(iAddr_Index) + " "
                    + std::to_string(iAddr_Sub)   + " "
                    + std::to_string(iData_Length);

    iRunResult = shell_0_or_1(strChmodCmd);

    fp = fopen("common_read.txt", "r");
    if(fp != NULL)
    {
        fscanf(fp, "%d", &iGetValue);
        fclose(fp);
    }
    else
    {
        iRunResult = RESULT_FAIL;
    }

    // iRunResult = RESULT_SUCCESS;
    return iRunResult;
}

int update_uiFrame::SetCommon(int iSlave_Num, int iAddr_Index, int iAddr_Sub, int iData_Length, int  iSetValue)
{
    FILE *fp;
    std::string strChmodCmd;
    int iRunResult = RESULT_FAIL;

    // 01. motion
    strChmodCmd = "sudo ./firm_update /s enp2s0 " + std::to_string(iSlave_Num) + " "
                    + std::to_string(iAddr_Index) + " "
                    + std::to_string(iAddr_Sub)   + " "
                    + std::to_string(iData_Length)+ " "
                    + std::to_string(iSetValue);

    iRunResult = shell_0_or_1(strChmodCmd);

    return iRunResult;
}

int update_uiFrame::SetSafeOp(int iSlave_Num, int iAddr_Index, int iAddr_Sub, int iData_Length, int  iSetValue)
{
    FILE *fp;
    std::string strChmodCmd;
    int iRunResult = RESULT_FAIL;

    // 01. motion
    strChmodCmd = "sudo ./firm_update /e enp2s0 " + std::to_string(iSlave_Num) + " "
                    + std::to_string(iAddr_Index) + " "
                    + std::to_string(iAddr_Sub)   + " "
                    + std::to_string(iData_Length)+ " "
                    + std::to_string(iSetValue);

    iRunResult = shell_0_or_1(strChmodCmd);

    return iRunResult;
}

#if 0
void update_uiFrame::chkFileExist_firmware_factory()
{
    int chk_file_exist = 0;

    // driving(1)
    chk_file_exist = checkIfFileExists(FILE_LOCATION.FACTORY_ROM.driving.c_str());
    if(chk_file_exist == IS_FILE)
        m_text_factory_file_driving->SetValue(wxT("OK"));
    else
        m_text_factory_file_driving->SetValue(wxT("ERR"));

    // hoist(2)
    chk_file_exist = checkIfFileExists(FILE_LOCATION.FACTORY_ROM.hoist.c_str());
    if(chk_file_exist == IS_FILE)
        m_text_factory_file_hoist->SetValue(wxT("OK"));
    else
        m_text_factory_file_hoist->SetValue(wxT("ERR"));

    // slide(3)
    chk_file_exist = checkIfFileExists(FILE_LOCATION.FACTORY_ROM.slide.c_str());
    if(chk_file_exist == IS_FILE)
        m_text_factory_file_slide->SetValue(wxT("OK"));
    else
        m_text_factory_file_slide->SetValue(wxT("ERR"));
}
#endif
void update_uiFrame::chkFileExist_firmware_new()
{
    int chk_file_exist = 0;

    // driving(1)
    chk_file_exist = checkIfFileExists(FILE_LOCATION.NEW_ROM.driving.c_str());
    if(chk_file_exist == IS_FILE)
        m_text_new_file_driving->SetValue(wxT("OK"));
    else
        m_text_new_file_driving->SetValue(wxT("ERR"));

    // hoist(2)
    chk_file_exist = checkIfFileExists(FILE_LOCATION.NEW_ROM.hoist.c_str());
    if(chk_file_exist == IS_FILE)
        m_text_new_file_hoist->SetValue(wxT("OK"));
    else
        m_text_new_file_hoist->SetValue(wxT("ERR"));

    // slide(3)
    chk_file_exist = checkIfFileExists(FILE_LOCATION.NEW_ROM.slide.c_str());
    if(chk_file_exist == IS_FILE)
        m_text_new_file_slide->SetValue(wxT("OK"));
    else
        m_text_new_file_slide->SetValue(wxT("ERR"));
}

void update_uiFrame::chkFileExist_param()
{
    int chk_file_exist = 0;

    // driving(1)
    chk_file_exist = checkIfFileExists(FILE_LOCATION.PARAM.driving.c_str());
    if(chk_file_exist == IS_FILE)
        m_text_param_driving->SetValue(wxT("OK"));
    else
        m_text_param_driving->SetValue(wxT("ERR"));

    // hoist(2)
    chk_file_exist = checkIfFileExists(FILE_LOCATION.PARAM.hoist.c_str());
    if(chk_file_exist == IS_FILE)
        m_text_param_hoist->SetValue(wxT("OK"));
    else
        m_text_param_hoist->SetValue(wxT("ERR"));

    // slide(3)
    chk_file_exist = checkIfFileExists(FILE_LOCATION.PARAM.slide.c_str());
    if(chk_file_exist == IS_FILE)
        m_text_param_slide->SetValue(wxT("OK"));
    else
        m_text_param_slide->SetValue(wxT("ERR"));
}

update_uiFrame::update_uiFrame(wxFrame *frame)
    : GUIFrame(frame)
{
#if wxUSE_STATUSBAR
    statusBar->SetStatusText(_("Motion Firmware Setup Utility"), 0);
    statusBar->SetStatusText(wxbuildinfo(short_f), 1);
#endif

// [ 01 ] (auto check & auto setting) ------------------------------------------
  // (1) File Update check  ================
  bool bExist_firm_update  = chkFileExist_firm_update();
  if(bExist_firm_update == true)
  {
      std::string strChmodCmd;
      strChmodCmd = "sudo chmod 755 ./firm_update";  // chmod exit code : 0 =no err(success)
      int iRunResult = shell_0_or_1(strChmodCmd);

      if (iRunResult == 0)
        m_text_auto_set_firm_update->SetValue(wxT("Permit OK"));
      else
        m_text_auto_set_firm_update->SetValue(wxT("Permit ERR"));

      printf("[firm_update] chmod 755 result = %d \n", iRunResult);
  }
  else
  {
      m_text_auto_set_firm_update->SetValue(wxT("File ERR"));
  }

  // more add
  bool bExist_slaveinfo  = chkFileExist_slaveinfo();
  if(bExist_slaveinfo == true)
  {
      std::string strChmodCmd;
      strChmodCmd = "sudo chmod 755 ./slaveinfo";  // chmod exit code : 0 =no err(success)
      int iRunResult = shell_0_or_1(strChmodCmd);
      printf("[slave info] chmod 755 result = %d \n", iRunResult);
  }

  // (2) File exist check, load ================
  bool bExist_file_version = chkFileExist_firm_version();
  if(bExist_file_version == true)
  {
      load_version_info();
      m_text_auto_set_version_file->SetValue(wxT("LOAD OK"));
  }
  else
  {
      m_text_auto_set_version_file->SetValue(wxT("LOAD ERR"));
  }

  // (3) Lan Port check & setting
  chkProcess_EcMaster();

// [ 02 ] (Version, File Name display) ------------------------------------------------
   if(bExist_firm_update == true)
      chkFirm_version();

    // file exist check
    // chkFileExist_firmware_factory();
    chkFileExist_firmware_new();
    chkFileExist_param();

    getErrCode_All();

    printf("Set Firmware refresh time = %d \n", 	FIRM_REFRESH_TIME);
	printf("Set Parameter refresh time = %d \n\n", 	PARAM_REFRESH_TIME);
}

update_uiFrame::~update_uiFrame()
{
    const char* network_02_close = "0000:02:00.0";
    const char* network_03_close = "0000:03:00.0";

    network_unbind_command(network_02_close);
    printf("Network port (02) auto close \n");

    network_unbind_command(network_03_close);
    printf("Network port (03) auto close \n");

    printf("Network port close waiting... \n");
	sleep(3);

	// network manager up/down
	const char* net_manager_up = "up";
	const char* net_manager_down = "down";
	network_manager_command(net_manager_down);	
}

void update_uiFrame::OnClose(wxCloseEvent &event)
{
    Destroy();
}

void update_uiFrame::OnQuit(wxCommandEvent &event)
{
    Destroy();
}

void update_uiFrame::OnAbout(wxCommandEvent &event)
{
    wxString msg = wxbuildinfo(long_f);
    //wxMessageBox(msg, _("Welcome to..."));
}

int update_uiFrame::resetController(int controller_num)
{
    std::string makeCommand = "sudo ./firm_update /r enp2s0 " + std::to_string(controller_num);

    int iRunResult = shell_0_or_1(makeCommand);

    /*
    if(iRunResult == 1)
        wxMessageBox(wxT("Controller Reset Ok"),  wxEmptyString, wxITEM_NORMAL);
    else
        wxMessageBox(wxT("Controller Reset Fail!!"), wxT("ERROR"), wxITEM_CHECK);
        */
    return iRunResult;
}

std::string update_uiFrame::getErrCode_DrvFront(int& iRunResult, int& iGetValue)
{
        wxString wxStrTmp;
        char cTempBuffer[50];

        iRunResult = GetCommon(AXIS_DRIVING,
                            GET_ERR_CODE_DRIVING_FRONT_MAIN,
                            GET_ERR_CODE_DRIVING_FRONT_SUB,
                            GET_ERR_CODE_DATA_TYPE,
                            iGetValue);

        if(iRunResult == RESULT_SUCCESS)
        {
                sprintf(cTempBuffer, "0x%04X\0",iGetValue , iGetValue);
                wxStrTmp = wxString::FromUTF8(cTempBuffer);
                m_text_err_code_front->SetValue(wxStrTmp);
        }
        else
        {       m_text_err_code_front->SetValue(wxT("READ ERR"));}

        std::string strReturn(wxStrTmp.mb_str());
        return strReturn;
}
std::string update_uiFrame::getErrCode_DrvRear(int& iRunResult, int& iGetValue)
{
        wxString wxStrTmp;
        char cTempBuffer[50];

        iRunResult = GetCommon(AXIS_DRIVING,
                            GET_ERR_CODE_DRIVING_REAR_MAIN,
                            GET_ERR_CODE_DRIVING_REAR_SUB,
                            GET_ERR_CODE_DATA_TYPE,
                            iGetValue);

		if(iRunResult == RESULT_SUCCESS)
		{
				sprintf(cTempBuffer, "0x%04X\0",iGetValue , iGetValue);
				wxStrTmp = wxString::FromUTF8(cTempBuffer);
				m_text_err_code_rear->SetValue(wxStrTmp);
		}
		else
		{		m_text_err_code_rear->SetValue(wxT("READ ERR")); }

        std::string strReturn(wxStrTmp.mb_str());
        return strReturn;
}
std::string update_uiFrame::getErrCode_Hoist(int& iRunResult, int& iGetValue)
{
        wxString wxStrTmp;
        char cTempBuffer[50];

        iRunResult = GetCommon(AXIS_HOIST,
                            GET_ERR_CODE_HOIST_MAIN,
                            GET_ERR_CODE_HOIST_SUB,
                            GET_ERR_CODE_DATA_TYPE,
                            iGetValue);

		if(iRunResult == RESULT_SUCCESS)
		{
				sprintf(cTempBuffer, "0x%04X\0",iGetValue , iGetValue);
				wxStrTmp = wxString::FromUTF8(cTempBuffer);
				m_text_err_code_hoist->SetValue(wxStrTmp);
		}
		else
		{		m_text_err_code_hoist->SetValue(wxT("READ ERR"));  }

        std::string strReturn(wxStrTmp.mb_str());
        return strReturn;
}
std::string update_uiFrame::getErrCode_Slide(int& iRunResult, int& iGetValue)
{
        wxString wxStrTmp;
        char cTempBuffer[50];

        iRunResult = GetCommon(AXIS_SLIDE,
                            GET_ERR_CODE_SLIDE_MAIN,
                            GET_ERR_CODE_SLIDE_SUB,
                            GET_ERR_CODE_DATA_TYPE,
                            iGetValue);

		if(iRunResult == RESULT_SUCCESS)
		{
				sprintf(cTempBuffer, "0x%04X\0",iGetValue , iGetValue);
				wxStrTmp = wxString::FromUTF8(cTempBuffer);
				m_text_err_code_slide->SetValue(wxStrTmp);
		}
		else
		{		m_text_err_code_slide->SetValue(wxT("READ ERR")); }

        std::string strReturn(wxStrTmp.mb_str());
        return strReturn;
}

void update_uiFrame::getErrCode_All()
{
    int iRunResult, iGetValue;

    getErrCode_DrvFront(iRunResult, iGetValue);
    getErrCode_DrvRear (iRunResult, iGetValue);
    getErrCode_Hoist   (iRunResult, iGetValue);
    getErrCode_Slide   (iRunResult, iGetValue);
}

void update_uiFrame::OnButtonClick( wxCommandEvent& event )
{
    int iRunResult = 0; // 0 = fail. 1 = success
    auto id = event.GetId();
    std::string makeCommand = "sudo ./firm_update ";

    int iGetValue = -1;
    char cTempBuffer[50];

    // (1) Extraction of Current Parameters
    if (id == wxID_BUTTON_PARAM_EXTRACTION)
    {
        makeCommand = makeCommand + "/x enp2s0 1 ";
        int iRunResult = shell_0_or_1(makeCommand);

        if (iRunResult == RESULT_SUCCESS)
        {
            wxMessageBox(wxT("Parameter Extraction OK"), wxEmptyString, wxICON_INFORMATION);
            m_text_result_param->SetValue(wxT("OK"));
        }
        else
        {
            wxMessageBox(wxT("Parameter Extraction Failed"), wxT("ERROR"), wxICON_ERROR);
            m_text_result_param->SetValue(wxT("ERR"));
        }

        getErrCode_All();  // 에러 코드 점검
    }

    // (2) Firmware - New
    if (id == wxID_BUTTON_NEW_DRIVING)
    {
        struct AxisInfo { int axis; std::string file; } axes[] = {
            {AXIS_DRIVING, FILE_LOCATION.NEW_ROM.driving},
            {AXIS_HOIST,   FILE_LOCATION.NEW_ROM.hoist},
            {AXIS_SLIDE,   FILE_LOCATION.NEW_ROM.slide}
        };

        bool success = true;

        for (const auto& a : axes)
        {
            makeCommand = "sudo ./firm_update /f enp2s0 1 " + a.file;

            iRunResult = shell_0_or_1(makeCommand);
            if(iRunResult == RESULT_SUCCESS)
            {
                resetController(a.axis);
                sleep(FIRM_REFRESH_TIME);
            }
            else
            {
                success = false;
                break;
            }
        }

        chkFirm_version();
        sleep(1);
        getErrCode_All();

        if(success)
            wxMessageBox(wxT("Running Ok"),  wxEmptyString, wxITEM_NORMAL);
        else
            wxMessageBox(wxT("Update fail!!"), wxT("ERROR"), wxITEM_CHECK);

    }
/*
    if (id == wxID_BUTTON_NEW_DRIVING)
    {
        makeCommand = makeCommand + "/f enp2s0 1 " + FILE_LOCATION.NEW_ROM.driving;

        iRunResult = shell_0_or_1(makeCommand);
        if(iRunResult == RESULT_SUCCESS)
        {
            resetController(AXIS_DRIVING);

            sleep(FIRM_REFRESH_TIME);
            chkFirm_version();

            sleep(1);
            getErrCode_All();

            wxMessageBox(wxT("Running Ok"),  wxEmptyString, wxITEM_NORMAL);
        }
        else
        {
            sleep(1);
            getErrCode_All();

            wxMessageBox(wxT("Update fail!!"), wxT("ERROR"), wxITEM_CHECK);
        }


    }
    else if(id == wxID_BUTTON_NEW_HOIST)
    {
        makeCommand = makeCommand + "/f enp2s0 2 " + FILE_LOCATION.NEW_ROM.hoist;

        iRunResult = shell_0_or_1(makeCommand);
        if(iRunResult == RESULT_SUCCESS)
        {
            resetController(AXIS_HOIST);

            sleep(FIRM_REFRESH_TIME);
            chkFirm_version();

            sleep(1);
            getErrCode_All();

            wxMessageBox(wxT("Running Ok"),  wxEmptyString, wxITEM_NORMAL);
        }
        else
        {
            sleep(1);
            getErrCode_All();

            wxMessageBox(wxT("Update fail!!"), wxT("ERROR"), wxITEM_CHECK);
        }
    }
    else if(id == wxID_BUTTON_NEW_SLIDE)
    {
        makeCommand = makeCommand + "/f enp2s0 3 " + FILE_LOCATION.NEW_ROM.slide;

        iRunResult = shell_0_or_1(makeCommand);
        if(iRunResult == RESULT_SUCCESS)
        {
            resetController(AXIS_SLIDE);

            sleep(FIRM_REFRESH_TIME);
            chkFirm_version();

            sleep(1);
            getErrCode_All();

            wxMessageBox(wxT("Running Ok"),  wxEmptyString, wxITEM_NORMAL);
        }
        else
        {
            sleep(1);
            getErrCode_All();

            wxMessageBox(wxT("Update fail!!"), wxT("ERROR"), wxITEM_CHECK);
        }
    }
*/
    // (3) Parameter update
    if(id == wxID_BUTTON_PARAM_DRIVING)
    {
         makeCommand = makeCommand + "/p enp2s0 1 " + FILE_LOCATION.PARAM.driving;
         iRunResult = shell_0_or_1(makeCommand);

		 if(iRunResult == RESULT_SUCCESS)
		 {
			resetController(AXIS_DRIVING);

			sleep(PARAM_REFRESH_TIME);
         	wxMessageBox(wxT("Parameter Save Ok"),  wxEmptyString, wxITEM_NORMAL);
			m_text_result_driving->SetValue(wxT("OK"));
		 }
        else
        {
            wxMessageBox(wxT("Parameter Update fail!!"), wxT("ERROR"), wxITEM_CHECK);
			m_text_result_driving->SetValue(wxT("ERR"));
        }

        getErrCode_All();
    }
    else if(id == wxID_BUTTON_PARAM_HOIST)
    {
        makeCommand = makeCommand + "/p enp2s0 1 " + FILE_LOCATION.PARAM.hoist;
        iRunResult = shell_0_or_1(makeCommand);

		if(iRunResult == RESULT_SUCCESS)
		{
			resetController(AXIS_HOIST);

			sleep(PARAM_REFRESH_TIME);
	     	wxMessageBox(wxT("Parameter Save Ok"),  wxEmptyString, wxITEM_NORMAL);
			m_text_result_hoist->SetValue(wxT("OK"));
		}
        else
        {
            wxMessageBox(wxT("Parameter Update fail!!"), wxT("ERROR"), wxITEM_CHECK);
			m_text_result_hoist->SetValue(wxT("ERR"));
        }

        getErrCode_All();
    }
    else if(id == wxID_BUTTON_PARAM_SLIDE)
    {
        makeCommand = makeCommand + "/p enp2s0 1 " + FILE_LOCATION.PARAM.slide;
        iRunResult = shell_0_or_1(makeCommand);

		if(iRunResult == RESULT_SUCCESS)
		{
		    sleep(2);
			resetController(AXIS_SLIDE);

			sleep(PARAM_REFRESH_TIME);
	     	wxMessageBox(wxT("Parameter Save Ok"),  wxEmptyString, wxITEM_NORMAL);
			m_text_result_slide->SetValue(wxT("OK"));
		}
        else
        {
            wxMessageBox(wxT("Parameter Update fail!!"), wxT("ERROR"), wxITEM_CHECK);
			m_text_result_slide->SetValue(wxT("ERR"));
        }

        getErrCode_All();
    }

    // (4-1) Error Handling => Get Code
    if(id == wxID_BUTTON_GETCODE_FRONT)
    {
        /* std::string strErrCode = "0xff05";
        iRunResult = RESULT_SUCCESS;
        iGetValue = 1;  */
        std::string strErrCode = getErrCode_DrvFront(iRunResult, iGetValue);
        wxString wxMessageTitle = wxT("[Front] ") + wxString::FromUTF8(strErrCode.c_str());

        if(iRunResult == RESULT_SUCCESS)
        {
            if(iGetValue != 0)
            {
                std::string result = find_alarm_message(strErrCode);
                wxMessageBox(wxString::FromUTF8(result.c_str()), wxMessageTitle, wxITEM_CHECK);
            }
        }
    }
    else if(id == wxID_BUTTON_GETCODE_REAR)
	{
        std::string strErrCode = getErrCode_DrvRear(iRunResult, iGetValue);
        wxString wxMessageTitle = wxT("[Rear] ") + wxString::FromUTF8(strErrCode.c_str());

        if(iRunResult == RESULT_SUCCESS)
        {
            if(iGetValue != 0)
            {
                std::string result = find_alarm_message(strErrCode);
                wxMessageBox(wxString::FromUTF8(result.c_str()), wxMessageTitle, wxITEM_CHECK);
            }
        }
	}
    else if(id == wxID_BUTTON_GETCODE_HOIST)
	{
        std::string strErrCode = getErrCode_Hoist(iRunResult, iGetValue);
        wxString wxMessageTitle = wxT("[Hoist] ") + wxString::FromUTF8(strErrCode.c_str());

        if(iRunResult == RESULT_SUCCESS)
        {
            if(iGetValue != 0)
            {
                std::string result = find_alarm_message(strErrCode);
                wxMessageBox(wxString::FromUTF8(result.c_str()), wxMessageTitle, wxITEM_CHECK);
            }
        }
	}
    else if(id == wxID_BUTTON_GETCODE_SLIDE)
	{
        std::string strErrCode = getErrCode_Slide(iRunResult, iGetValue);
        wxString wxMessageTitle = wxT("[Slide] ") + wxString::FromUTF8(strErrCode.c_str());

        if(iRunResult == RESULT_SUCCESS)
        {
            if(iGetValue != 0)
            {
                std::string result = find_alarm_message(strErrCode);
                wxMessageBox(wxString::FromUTF8(result.c_str()), wxMessageTitle, wxITEM_CHECK);
            }
        }
	}


    // (4-2) Error Handling => Error Clear
    if(id == wxID_BUTTON_ERR_CLEAR_FRONT)
    {
        // Step 01. Current data read
        iRunResult = GetCommon(AXIS_DRIVING,
							   	ERROR_CLEAR_DRIVING_FRONT_MAIN,
								ERROR_CLEAR_DRIVING_FRONT_SUB,
								ERROR_CLEAR_DATA_TYPE,
								iGetValue);

		// Step 02. Compute & write#1
		uint16_t uMask = 127; // bin 0111 1111
		iGetValue = iGetValue & uMask;   // (AND OP) bit 7 -> set 0
		sleep(1);
		iRunResult = SetSafeOp(AXIS_DRIVING,
								ERROR_CLEAR_DRIVING_FRONT_MAIN,
								ERROR_CLEAR_DRIVING_FRONT_SUB,
								ERROR_CLEAR_DATA_TYPE,
								iGetValue);

		// Step 03. Compute & write#2
		uMask = 128;	// bin 1000 0000
		iGetValue = iGetValue | uMask;   // (OR OP) bit 7 -> set 1
		sleep(1);
		iRunResult = SetSafeOp(AXIS_DRIVING,
								ERROR_CLEAR_DRIVING_FRONT_MAIN,
								ERROR_CLEAR_DRIVING_FRONT_SUB,
								ERROR_CLEAR_DATA_TYPE,
								iGetValue);

        sleep(1);
        getErrCode_All();
    }
    else if(id == wxID_BUTTON_ERR_CLEAR_REAR)
    {
        // Step 01. Current data read
        iRunResult = GetCommon(AXIS_DRIVING,
							   	ERROR_CLEAR_DRIVING_REAR_MAIN,
								ERROR_CLEAR_DRIVING_REAR_SUB,
								ERROR_CLEAR_DATA_TYPE,
								iGetValue);

		// Step 02. Compute & write#1
		uint16_t uMask = 127; // bin 0111 1111
		iGetValue = iGetValue & uMask;   // (AND OP) bit 7 -> set 0
		sleep(1);
		iRunResult = SetSafeOp(AXIS_DRIVING,
								ERROR_CLEAR_DRIVING_REAR_MAIN,
								ERROR_CLEAR_DRIVING_REAR_SUB,
								ERROR_CLEAR_DATA_TYPE,
								iGetValue);

		// Step 03. Compute & write#2
		uMask = 128;	// bin 1000 0000
		iGetValue = iGetValue | uMask;   // (OR OP) bit 7 -> set 1
		sleep(1);
		iRunResult = SetSafeOp(AXIS_DRIVING,
								ERROR_CLEAR_DRIVING_REAR_MAIN,
								ERROR_CLEAR_DRIVING_REAR_SUB,
								ERROR_CLEAR_DATA_TYPE,
								iGetValue);

        //sleep(1);
        //getErrCode_All();

    }
    else if(id == wxID_BUTTON_ERR_CLEAR_HOIST)
    {
        // Step 01. Current data read
        iRunResult = GetCommon(AXIS_HOIST,
							   	ERROR_CLEAR_HOIST_MAIN,
								ERROR_CLEAR_HOIST_SUB,
								ERROR_CLEAR_DATA_TYPE,
								iGetValue);

		// Step 02. Compute & write#1
		uint16_t uMask = 127; // bin 0111 1111
		iGetValue = iGetValue & uMask;   // (AND OP) bit 7 -> set 0
		sleep(1);
		iRunResult = SetSafeOp(AXIS_HOIST,
								ERROR_CLEAR_HOIST_MAIN,
								ERROR_CLEAR_HOIST_SUB,
								ERROR_CLEAR_DATA_TYPE,
								iGetValue);

		// Step 03. Compute & write#2
		sleep(1);
		uMask = 128;	// bin 1000 0000
		iGetValue = iGetValue | uMask;   // (OR OP) bit 7 -> set 1

		iGetValue_Global = iGetValue;

		iRunResult = SetSafeOp(AXIS_HOIST,
								ERROR_CLEAR_HOIST_MAIN,
								ERROR_CLEAR_HOIST_SUB,
								ERROR_CLEAR_DATA_TYPE,
								iGetValue);

        //sleep(1);
        //getErrCode_All();
//#endif
    }
    else if(id == wxID_BUTTON_ERR_CLEAR_SLIDE)
    {
    /*  std::string test = "0xFF55";
        std::string result = find_alarm_message(test);
        wxString wxStrTmp = wxString::FromUTF8(result.c_str());
        wxMessageBox(wxStrTmp, wxEmptyString, wxITEM_CHECK);    */

#if 0
		iRunResult = SetCommon(AXIS_HOIST,
								ERROR_CLEAR_HOIST_MAIN,
								ERROR_CLEAR_HOIST_SUB,
								ERROR_CLEAR_DATA_TYPE,
								iGetValue_Global);
#endif

 #if 1
        // Step 01. Current data read
        iRunResult = GetCommon(AXIS_SLIDE,
							   	ERROR_CLEAR_SLIDE_MAIN,
								ERROR_CLEAR_SLIDE_SUB,
								ERROR_CLEAR_DATA_TYPE,
								iGetValue);

		// Step 02. Compute & write#1
		uint16_t uMask = 127; // bin 0111 1111
		iGetValue = iGetValue & uMask;   // (AND OP) bit 7 -> set 0
		sleep(1);
		iRunResult = SetSafeOp(AXIS_SLIDE,
								ERROR_CLEAR_SLIDE_MAIN,
								ERROR_CLEAR_SLIDE_SUB,
								ERROR_CLEAR_DATA_TYPE,
								iGetValue);

		// Step 03. Compute & write#2
		uMask = 128;	// bin 1000 0000
		iGetValue = iGetValue | uMask;   // (OR OP) bit 7 -> set 1
		sleep(1);
		iRunResult = SetSafeOp(AXIS_SLIDE,
								ERROR_CLEAR_SLIDE_MAIN,
								ERROR_CLEAR_SLIDE_SUB,
								ERROR_CLEAR_DATA_TYPE,
								iGetValue);

        //sleep(1);
        //getErrCode_All();
#endif
    }

    // (5) Absolute Encoder Reset
    if(id == wxID_BUTTON_ENC_RESET_DRIVING)
    {
		iRunResult = SetCommon(AXIS_DRIVING,
								ENCODER_RESET_DRIVING_MAIN,
								ENCODER_RESET_DRIVING_SUB,
								ENCODER_RESET_DATA_TYPE,
								ENCODER_RESET_COMMAND);
		if(iRunResult == RESULT_SUCCESS)
			wxMessageBox(wxT("Reset Ok"),  wxEmptyString, wxITEM_NORMAL);
		else
			wxMessageBox(wxT("Reset fail!!"), wxT("ERROR"), wxITEM_CHECK);

    }
    else if(id == wxID_BUTTON_ENC_RESET_HOIST)
    {
		iRunResult = SetCommon(AXIS_HOIST,
								ENCODER_RESET_HOIST_MAIN,
								ENCODER_RESET_HOIST_SUB,
								ENCODER_RESET_DATA_TYPE,
								ENCODER_RESET_COMMAND);
		if(iRunResult == RESULT_SUCCESS)
			wxMessageBox(wxT("Reset Ok"),  wxEmptyString, wxITEM_NORMAL);
		else
			wxMessageBox(wxT("Reset fail!!"), wxT("ERROR"), wxITEM_CHECK);

        getErrCode_All();

    }
    else if(id == wxID_BUTTON_ENC_RESET_SLIDE)
    {
		iRunResult = SetCommon(AXIS_SLIDE,
								ENCODER_RESET_SLIDE_MAIN,
								ENCODER_RESET_SLIDE_SUB,
								ENCODER_RESET_DATA_TYPE,
								ENCODER_RESET_COMMAND);
		if(iRunResult == RESULT_SUCCESS)
			wxMessageBox(wxT("Reset Ok"),  wxEmptyString, wxITEM_NORMAL);
		else
			wxMessageBox(wxT("Reset fail!!"), wxT("ERROR"), wxITEM_CHECK);

        getErrCode_All();
    }


 #if 0
    printf("Hello AAA\n");

    int return_var = wxNO;
    wxMessageBox(wxT("Test Popup"), wxEmptyString, wxITEM_NORMAL);

    wxMessageDialog *dlg = new wxMessageDialog(NULL, _("Save?"), _("Cancel?"), wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION);
    return_var = dlg->ShowModal();
    switch(return_var)
    {
        case wxYES:
            wxMessageBox(wxT("Saving..."),  wxEmptyString, wxITEM_NORMAL);
        break;

        case wxNO:
            wxMessageBox(wxT("Cancel..."),  wxEmptyString, wxITEM_NORMAL);
        break;
    }
#endif
}

