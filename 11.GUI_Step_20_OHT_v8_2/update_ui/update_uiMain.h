/***************************************************************
 * Name:      update_uiMain.h
 * Purpose:   Defines Application Frame
 * Author:     ()
 * Created:   2022-03-08
 * Copyright:  ()
 * License:
 **************************************************************/

#ifndef UPDATE_UIMAIN_H
#define UPDATE_UIMAIN_H

#include <wx/msgdlg.h>

// --------------------------------
//       include more added
// --------------------------------
#include <stdio.h>
#include <sys/types.h>
#include<sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <sstream>
#include <iostream>


using namespace std;
// --------------------------------

#include "update_uiApp.h"
#include "GUIFrame.h"
#include "iniparser.hpp"

class update_uiFrame: public GUIFrame
{
    public:
        update_uiFrame(wxFrame *frame);
        ~update_uiFrame();
    private:
        virtual void OnClose(wxCloseEvent& event);
        virtual void OnQuit(wxCommandEvent& event);
        virtual void OnAbout(wxCommandEvent& event);
        virtual void OnButtonClick( wxCommandEvent& event );
    private:
        void load_version_info();
        std::string find_alarm_message(std::string);
        bool chkFileExist_firm_update();
        bool chkFileExist_slaveinfo();
        bool chkFileExist_firm_version();
        void chkProcess_EcMaster();
        int chkFirm_version();

        void chkFileExist_firmware_factory();
        void chkFileExist_firmware_new();
        void chkFileExist_param();

        int resetController(int controller_num);
		int GetCommon(int iSlave_Num, int iAddr_Index, int iAddr_Sub, int iData_Length, int& iGetValue);
		int SetCommon(int iSlave_Num, int iAddr_Index, int iAddr_Sub, int iData_Length, int  iSetValue);
		int SetSafeOp(int iSlave_Num, int iAddr_Index, int iAddr_Sub, int iData_Length, int  iSetValue);

		std::string getErrCode_DrvFront(int& iRunResult, int& iGetValue);
		std::string getErrCode_DrvRear(int& iRunResult, int& iGetValue);
		std::string getErrCode_Hoist(int& iRunResult, int& iGetValue);
		std::string getErrCode_Slide(int& iRunResult, int& iGetValue);

        void getErrCode_All();
};

#endif // UPDATE_UIMAIN_H
