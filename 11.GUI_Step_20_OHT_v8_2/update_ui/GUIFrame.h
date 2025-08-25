///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Oct 26 2018)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#pragma once

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/string.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/menu.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/statbmp.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/statbox.h>
#include <wx/button.h>
#include <wx/statusbr.h>
#include <wx/frame.h>

///////////////////////////////////////////////////////////////////////////

#define idMenuQuit 1000
#define idMenuAbout 1001
#define wxID_BUTTON_NEW_DRIVING 1002
#define wxID_BUTTON_NEW_HOIST 1003
#define wxID_BUTTON_NEW_SLIDE 1004
#define wxID_BUTTON_PARAM_DRIVING 1005
#define wxID_BUTTON_PARAM_HOIST 1006
#define wxID_BUTTON_PARAM_SLIDE 1007
#define wxID_BUTTON_GETCODE_FRONT 1008
#define wxID_BUTTON_GETCODE_REAR 1009
#define wxID_BUTTON_GETCODE_HOIST 1010
#define wxID_BUTTON_GETCODE_SLIDE 1011
#define wxID_BUTTON_ERR_CLEAR_FRONT 1012
#define wxID_BUTTON_ERR_CLEAR_REAR 1013
#define wxID_BUTTON_ERR_CLEAR_HOIST 1014
#define wxID_BUTTON_ERR_CLEAR_SLIDE 1015
#define wxID_BUTTON_ENC_RESET_DRIVING 1016
#define wxID_BUTTON_ENC_RESET_HOIST 1017
#define wxID_BUTTON_ENC_RESET_SLIDE 1018
#define wxID_BUTTON_PARAM_EXTRACTION 1019

///////////////////////////////////////////////////////////////////////////////
/// Class GUIFrame
///////////////////////////////////////////////////////////////////////////////
class GUIFrame : public wxFrame
{
	private:

	protected:
		wxMenuBar* mbar;
		wxMenu* fileMenu;
		wxMenu* helpMenu;
		wxPanel* m_panel2;
		wxStaticBitmap* m_bitmap2;
		wxStaticText* m_staticText1;
		wxStaticText* m_staticText2;
		wxStaticText* m_staticText3;
		wxStaticText* m_staticText4;
		wxTextCtrl* m_text_file_check_firm_update;
		wxTextCtrl* m_text_file_check_version_file;
		wxTextCtrl* m_text_file_check_ecat_port;
		wxStaticText* m_staticText5;
		wxTextCtrl* m_text_auto_set_firm_update;
		wxTextCtrl* m_text_auto_set_version_file;
		wxTextCtrl* m_text_auto_set_ecat_port;
		wxPanel* m_panel3;
		wxStaticText* m_staticText6;
		wxStaticText* m_staticText7;
		wxStaticText* m_staticText8;
		wxStaticText* m_staticText9;
		wxTextCtrl* m_text_current_ver_driving;
		wxTextCtrl* m_text_current_ver_hoist;
		wxTextCtrl* m_text_current_ver_slide;
		wxStaticText* m_staticText12;
		wxTextCtrl* m_text_new_file_driving;
		wxTextCtrl* m_text_new_file_hoist;
		wxTextCtrl* m_text_new_file_slide;
		wxStaticText* m_staticText13;
		wxButton* m_button_new_driving;
		wxButton* m_button_new_hoist;
		wxButton* m_button_new_slide;
		wxButton* m_button_param_extraction;
		wxPanel* m_panel5;
		wxStaticText* m_staticText16;
		wxStaticText* m_staticText17;
		wxStaticText* m_staticText18;
		wxStaticText* m_staticText19;
		wxTextCtrl* m_text_param_driving;
		wxTextCtrl* m_text_param_hoist;
		wxTextCtrl* m_text_param_slide;
		wxStaticText* m_staticText20;
		wxButton* m_button_param_driving;
		wxButton* m_button_param_hoist;
		wxButton* m_button_param_slide;
		wxStaticText* m_staticText24;
		wxTextCtrl* m_text_result_param;
		wxTextCtrl* m_text_result_driving;
		wxTextCtrl* m_text_result_hoist;
		wxTextCtrl* m_text_result_slide;
		wxStaticText* m_staticText191;
		wxStaticText* m_staticText25;
		wxStaticText* m_staticText201;
		wxStaticText* m_staticText21;
		wxTextCtrl* m_text_err_code_front;
		wxTextCtrl* m_text_err_code_rear;
		wxTextCtrl* m_text_err_code_hoist;
		wxTextCtrl* m_text_err_code_slide;
		wxButton* m_button_err_code_front;
		wxButton* m_button_err_code_rear;
		wxButton* m_button_err_code_hoist;
		wxButton* m_button_err_code_slide;
		wxButton* m_button_err_clear_front;
		wxButton* m_button_err_clear_rear;
		wxButton* m_button_err_clear_hoist;
		wxButton* m_button_err_clear_slide;
		wxStaticText* m_staticText26;
		wxButton* m_button_encoder_reset_driving;
		wxButton* m_button_encoder_reset_hoist;
		wxButton* m_button_encoder_reset_slide;
		wxStatusBar* statusBar;

		// Virtual event handlers, overide them in your derived class
		virtual void OnClose( wxCloseEvent& event ) { event.Skip(); }
		virtual void OnQuit( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnAbout( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnButtonClick( wxCommandEvent& event ) { event.Skip(); }


	public:

		GUIFrame( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Updater  (for OHT 8.2) - v2.0.1"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 400,650 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );

		~GUIFrame();

};

