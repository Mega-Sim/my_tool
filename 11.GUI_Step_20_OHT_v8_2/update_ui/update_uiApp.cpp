/***************************************************************
 * Name:      update_uiApp.cpp
 * Purpose:   Code for Application Class
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

#include "update_uiApp.h"
#include "update_uiMain.h"

IMPLEMENT_APP(update_uiApp);

bool update_uiApp::OnInit()
{
    update_uiFrame* frame = new update_uiFrame(0L);
    
    frame->Show();
    
    return true;
}
