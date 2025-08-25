///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Oct 26 2018)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif //__BORLANDC__

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif //WX_PRECOMP

#include "GUIFrame.h"

///////////////////////////////////////////////////////////////////////////

GUIFrame::GUIFrame( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxSize( 400,960 ), wxSize( 400,960 ) );

	mbar = new wxMenuBar( 0 );
	fileMenu = new wxMenu();
	wxMenuItem* menuFileQuit;
	menuFileQuit = new wxMenuItem( fileMenu, idMenuQuit, wxString( wxT("&Quit") ) + wxT('\t') + wxT("Alt+F4"), wxT("Quit the application"), wxITEM_NORMAL );
	fileMenu->Append( menuFileQuit );

	mbar->Append( fileMenu, wxT("&File") );

	helpMenu = new wxMenu();
	wxMenuItem* menuHelpAbout;
	menuHelpAbout = new wxMenuItem( helpMenu, idMenuAbout, wxString( wxT("&About") ) + wxT('\t') + wxT("F1"), wxT("Show info about this application"), wxITEM_NORMAL );
	helpMenu->Append( menuHelpAbout );

	mbar->Append( helpMenu, wxT("&Help") );

	this->SetMenuBar( mbar );

	wxBoxSizer* topSizer = new wxBoxSizer(wxVERTICAL);

    wxBoxSizer* horizontalSizer = new wxBoxSizer(wxHORIZONTAL);

    wxStaticText* label = new wxStaticText(this, wxID_ANY, wxT("Extract Current Parameter "), wxDefaultPosition, wxDefaultSize, 0);
    horizontalSizer->Add(label, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);

    m_button_param_extraction = new wxButton(this, wxID_BUTTON_PARAM_EXTRACTION, wxT("Extraction"), wxDefaultPosition, wxDefaultSize, 0);
    horizontalSizer->Add(m_button_param_extraction, 0, wxALIGN_CENTER_VERTICAL);

    // 결과 표시를 위한 텍스트 박스 추가
    m_text_result_param = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY);
    horizontalSizer->Add(m_text_result_param, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 5);

    topSizer->Add(horizontalSizer, 0, wxEXPAND | wxALL, 5);

	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	bSizer1->Add(topSizer, 0, wxEXPAND, 5);

	wxStaticBoxSizer* FileCheck;
	FileCheck = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("   Auto Check and Setting  ") ), wxVERTICAL );

	wxGridSizer* gSizer1;
	gSizer1 = new wxGridSizer( 3, 4, 0, 0 );

	m_panel2 = new wxPanel( FileCheck->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );

	m_bitmap2 = new wxStaticBitmap( m_panel2, wxID_ANY, wxBitmap( wxT("SEMES_80.png"), wxBITMAP_TYPE_ANY ), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4->Add( m_bitmap2, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5 );


	m_panel2->SetSizer( bSizer4 );
	m_panel2->Layout();
	bSizer4->Fit( m_panel2 );
	gSizer1->Add( m_panel2, 1, wxEXPAND | wxALL, 5 );

	m_staticText1 = new wxStaticText( FileCheck->GetStaticBox(), wxID_ANY, wxT("Firm_update"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	gSizer1->Add( m_staticText1, 0, wxALL, 5 );

	m_staticText2 = new wxStaticText( FileCheck->GetStaticBox(), wxID_ANY, wxT(" Version file"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	gSizer1->Add( m_staticText2, 0, wxALL, 5 );

	m_staticText3 = new wxStaticText( FileCheck->GetStaticBox(), wxID_ANY, wxT(" E-CAT Port"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->Wrap( -1 );
	gSizer1->Add( m_staticText3, 0, wxALL, 5 );

	m_staticText4 = new wxStaticText( FileCheck->GetStaticBox(), wxID_ANY, wxT("File Check"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText4->Wrap( -1 );
	gSizer1->Add( m_staticText4, 0, wxALL, 5 );

	m_text_file_check_firm_update = new wxTextCtrl( FileCheck->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
	m_text_file_check_firm_update->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOW ) );

	gSizer1->Add( m_text_file_check_firm_update, 0, wxALL, 5 );

	m_text_file_check_version_file = new wxTextCtrl( FileCheck->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
	m_text_file_check_version_file->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOW ) );

	gSizer1->Add( m_text_file_check_version_file, 0, wxALL, 5 );

	m_text_file_check_ecat_port = new wxTextCtrl( FileCheck->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
	m_text_file_check_ecat_port->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOW ) );

	gSizer1->Add( m_text_file_check_ecat_port, 0, wxALL, 5 );

	m_staticText5 = new wxStaticText( FileCheck->GetStaticBox(), wxID_ANY, wxT("Auto Setting"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText5->Wrap( -1 );
	gSizer1->Add( m_staticText5, 0, wxALL, 5 );

	m_text_auto_set_firm_update = new wxTextCtrl( FileCheck->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
	m_text_auto_set_firm_update->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOW ) );

	gSizer1->Add( m_text_auto_set_firm_update, 0, wxALL, 5 );

	m_text_auto_set_version_file = new wxTextCtrl( FileCheck->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
	m_text_auto_set_version_file->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOW ) );

	gSizer1->Add( m_text_auto_set_version_file, 0, wxALL, 5 );

	m_text_auto_set_ecat_port = new wxTextCtrl( FileCheck->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
	m_text_auto_set_ecat_port->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOW ) );

	gSizer1->Add( m_text_auto_set_ecat_port, 0, wxALL, 5 );


	FileCheck->Add( gSizer1, 1, wxEXPAND, 5 );


	bSizer1->Add( FileCheck, 1, wxEXPAND, 5 );

	wxStaticBoxSizer* FilewareUpdate;
	FilewareUpdate = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("   Firmware Update  ") ), wxVERTICAL );

	wxGridSizer* gSizer2;
	gSizer2 = new wxGridSizer( 3, 4, 0, 0 );

    // 변경된 부분: 기존 m_panel3 제거, 대신 첫 칸에 버튼 배치
    m_button_new_driving = new wxButton( FilewareUpdate->GetStaticBox(), wxID_BUTTON_NEW_DRIVING, wxT("F. Update"), wxDefaultPosition, wxDefaultSize, 0 );
    gSizer2->Add( m_button_new_driving, 0, wxALL, 5 );

	m_staticText6 = new wxStaticText( FilewareUpdate->GetStaticBox(), wxID_ANY, wxT("[1] Driving"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText6->Wrap( -1 );
	gSizer2->Add( m_staticText6, 0, wxALL, 5 );

	m_staticText7 = new wxStaticText( FilewareUpdate->GetStaticBox(), wxID_ANY, wxT("[2] Hoist"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText7->Wrap( -1 );
	gSizer2->Add( m_staticText7, 0, wxALL, 5 );

	m_staticText8 = new wxStaticText( FilewareUpdate->GetStaticBox(), wxID_ANY, wxT("[3] Slide"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText8->Wrap( -1 );
	gSizer2->Add( m_staticText8, 0, wxALL, 5 );

	m_staticText9 = new wxStaticText( FilewareUpdate->GetStaticBox(), wxID_ANY, wxT("Current Ver"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText9->Wrap( -1 );
	gSizer2->Add( m_staticText9, 0, wxALL, 5 );

	m_text_current_ver_driving = new wxTextCtrl( FilewareUpdate->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
	gSizer2->Add( m_text_current_ver_driving, 0, wxALL, 5 );

	m_text_current_ver_hoist = new wxTextCtrl( FilewareUpdate->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
	gSizer2->Add( m_text_current_ver_hoist, 0, wxALL, 5 );

	m_text_current_ver_slide = new wxTextCtrl( FilewareUpdate->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
	gSizer2->Add( m_text_current_ver_slide, 0, wxALL, 5 );

	m_staticText12 = new wxStaticText( FilewareUpdate->GetStaticBox(), wxID_ANY, wxT("New File"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText12->Wrap( -1 );
	gSizer2->Add( m_staticText12, 0, wxALL, 5 );

	m_text_new_file_driving = new wxTextCtrl( FilewareUpdate->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
	gSizer2->Add( m_text_new_file_driving, 0, wxALL, 5 );

	m_text_new_file_hoist = new wxTextCtrl( FilewareUpdate->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
	gSizer2->Add( m_text_new_file_hoist, 0, wxALL, 5 );

	m_text_new_file_slide = new wxTextCtrl( FilewareUpdate->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
	gSizer2->Add( m_text_new_file_slide, 0, wxALL, 5 );
/*
	m_staticText13 = new wxStaticText( FilewareUpdate->GetStaticBox(), wxID_ANY, wxT("New Apply"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText13->Wrap( -1 );
	gSizer2->Add( m_staticText13, 0, wxALL, 5 );

	m_button_new_driving = new wxButton( FilewareUpdate->GetStaticBox(), wxID_BUTTON_NEW_DRIVING, wxT("N. Driving"), wxDefaultPosition, wxDefaultSize, 0 );
	gSizer2->Add( m_button_new_driving, 0, wxALL, 5 );

	m_button_new_hoist = new wxButton( FilewareUpdate->GetStaticBox(), wxID_BUTTON_NEW_HOIST, wxT("N.  Hoist"), wxDefaultPosition, wxDefaultSize, 0 );
	gSizer2->Add( m_button_new_hoist, 0, wxALL, 5 );

	m_button_new_slide = new wxButton( FilewareUpdate->GetStaticBox(), wxID_BUTTON_NEW_SLIDE, wxT("N.  Slide"), wxDefaultPosition, wxDefaultSize, 0 );
	gSizer2->Add( m_button_new_slide, 0, wxALL, 5 );
*/

	FilewareUpdate->Add( gSizer2, 1, wxEXPAND, 5 );


	bSizer1->Add( FilewareUpdate, 1, wxEXPAND, 5 );

	wxStaticBoxSizer* ParameterUpdate;
	ParameterUpdate = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("   Parameter Update  ") ), wxVERTICAL );

	wxGridSizer* gSizer4;
	gSizer4 = new wxGridSizer( 4, 4, 0, 0 );

	m_panel5 = new wxPanel( ParameterUpdate->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	gSizer4->Add( m_panel5, 1, wxEXPAND | wxALL, 5 );

	m_staticText16 = new wxStaticText( ParameterUpdate->GetStaticBox(), wxID_ANY, wxT("[1] Driving"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText16->Wrap( -1 );
	gSizer4->Add( m_staticText16, 0, wxALL, 5 );

	m_staticText17 = new wxStaticText( ParameterUpdate->GetStaticBox(), wxID_ANY, wxT("[2] Hoist"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText17->Wrap( -1 );
	gSizer4->Add( m_staticText17, 0, wxALL, 5 );

	m_staticText18 = new wxStaticText( ParameterUpdate->GetStaticBox(), wxID_ANY, wxT("[3] Slide"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText18->Wrap( -1 );
	gSizer4->Add( m_staticText18, 0, wxALL, 5 );

	m_staticText19 = new wxStaticText( ParameterUpdate->GetStaticBox(), wxID_ANY, wxT("Param File"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText19->Wrap( -1 );
	gSizer4->Add( m_staticText19, 0, wxALL, 5 );

	m_text_param_driving = new wxTextCtrl( ParameterUpdate->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
	gSizer4->Add( m_text_param_driving, 0, wxALL, 5 );

	m_text_param_hoist = new wxTextCtrl( ParameterUpdate->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
	gSizer4->Add( m_text_param_hoist, 0, wxALL, 5 );

	m_text_param_slide = new wxTextCtrl( ParameterUpdate->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
	gSizer4->Add( m_text_param_slide, 0, wxALL, 5 );

	m_staticText20 = new wxStaticText( ParameterUpdate->GetStaticBox(), wxID_ANY, wxT("Param Apply"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText20->Wrap( -1 );
	gSizer4->Add( m_staticText20, 0, wxALL, 5 );

	m_button_param_driving = new wxButton( ParameterUpdate->GetStaticBox(), wxID_BUTTON_PARAM_DRIVING, wxT("P.  Driving"), wxDefaultPosition, wxDefaultSize, 0 );
	gSizer4->Add( m_button_param_driving, 0, wxALL, 5 );

	m_button_param_hoist = new wxButton( ParameterUpdate->GetStaticBox(), wxID_BUTTON_PARAM_HOIST, wxT("P.  Hoist"), wxDefaultPosition, wxDefaultSize, 0 );
	gSizer4->Add( m_button_param_hoist, 0, wxALL, 5 );

	m_button_param_slide = new wxButton( ParameterUpdate->GetStaticBox(), wxID_BUTTON_PARAM_SLIDE, wxT("P.  Slide"), wxDefaultPosition, wxDefaultSize, 0 );
	gSizer4->Add( m_button_param_slide, 0, wxALL, 5 );

	m_staticText24 = new wxStaticText( ParameterUpdate->GetStaticBox(), wxID_ANY, wxT("Apply Result"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText24->Wrap( -1 );
	gSizer4->Add( m_staticText24, 0, wxALL, 5 );

	m_text_result_driving = new wxTextCtrl( ParameterUpdate->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	gSizer4->Add( m_text_result_driving, 0, wxALL, 5 );

	m_text_result_hoist = new wxTextCtrl( ParameterUpdate->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	gSizer4->Add( m_text_result_hoist, 0, wxALL, 5 );

	m_text_result_slide = new wxTextCtrl( ParameterUpdate->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	gSizer4->Add( m_text_result_slide, 0, wxALL, 5 );


	ParameterUpdate->Add( gSizer4, 1, wxEXPAND, 5 );


	bSizer1->Add( ParameterUpdate, 1, wxEXPAND, 5 );

	wxStaticBoxSizer* ErrorHandling;
	ErrorHandling = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT(" Error Handling ") ), wxVERTICAL );

	wxGridSizer* gSizer41;
	gSizer41 = new wxGridSizer( 4, 4, 0, 0 );

	m_staticText191 = new wxStaticText( ErrorHandling->GetStaticBox(), wxID_ANY, wxT("Front Drv."), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText191->Wrap( -1 );
	gSizer41->Add( m_staticText191, 0, wxALL, 5 );

	m_staticText25 = new wxStaticText( ErrorHandling->GetStaticBox(), wxID_ANY, wxT("Rear Drv."), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText25->Wrap( -1 );
	gSizer41->Add( m_staticText25, 0, wxALL, 5 );

	m_staticText201 = new wxStaticText( ErrorHandling->GetStaticBox(), wxID_ANY, wxT("[2] Hoist"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText201->Wrap( -1 );
	gSizer41->Add( m_staticText201, 0, wxALL, 5 );

	m_staticText21 = new wxStaticText( ErrorHandling->GetStaticBox(), wxID_ANY, wxT("[3] Slide"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText21->Wrap( -1 );
	gSizer41->Add( m_staticText21, 0, wxALL, 5 );

	m_text_err_code_front = new wxTextCtrl( ErrorHandling->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	gSizer41->Add( m_text_err_code_front, 0, wxALL, 5 );

	m_text_err_code_rear = new wxTextCtrl( ErrorHandling->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	gSizer41->Add( m_text_err_code_rear, 0, wxALL, 5 );

	m_text_err_code_hoist = new wxTextCtrl( ErrorHandling->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	gSizer41->Add( m_text_err_code_hoist, 0, wxALL, 5 );

	m_text_err_code_slide = new wxTextCtrl( ErrorHandling->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	gSizer41->Add( m_text_err_code_slide, 0, wxALL, 5 );

	m_button_err_code_front = new wxButton( ErrorHandling->GetStaticBox(), wxID_BUTTON_GETCODE_FRONT, wxT("Get Code"), wxDefaultPosition, wxDefaultSize, 0 );
	gSizer41->Add( m_button_err_code_front, 0, wxALL, 5 );

	m_button_err_code_rear = new wxButton( ErrorHandling->GetStaticBox(), wxID_BUTTON_GETCODE_REAR, wxT("Get Code"), wxDefaultPosition, wxDefaultSize, 0 );
	gSizer41->Add( m_button_err_code_rear, 0, wxALL, 5 );

	m_button_err_code_hoist = new wxButton( ErrorHandling->GetStaticBox(), wxID_BUTTON_GETCODE_HOIST, wxT("Get Code"), wxDefaultPosition, wxDefaultSize, 0 );
	gSizer41->Add( m_button_err_code_hoist, 0, wxALL, 5 );

	m_button_err_code_slide = new wxButton( ErrorHandling->GetStaticBox(), wxID_BUTTON_GETCODE_SLIDE, wxT("Get Code"), wxDefaultPosition, wxDefaultSize, 0 );
	gSizer41->Add( m_button_err_code_slide, 0, wxALL, 5 );

	m_button_err_clear_front = new wxButton( ErrorHandling->GetStaticBox(), wxID_BUTTON_ERR_CLEAR_FRONT, wxT("F. Err Clear"), wxDefaultPosition, wxDefaultSize, 0 );
	gSizer41->Add( m_button_err_clear_front, 0, wxALL, 5 );

	m_button_err_clear_rear = new wxButton( ErrorHandling->GetStaticBox(), wxID_BUTTON_ERR_CLEAR_REAR, wxT("R.Err Clear"), wxDefaultPosition, wxDefaultSize, 0 );
	gSizer41->Add( m_button_err_clear_rear, 0, wxALL, 5 );

	m_button_err_clear_hoist = new wxButton( ErrorHandling->GetStaticBox(), wxID_BUTTON_ERR_CLEAR_HOIST, wxT("Hoist Clear"), wxDefaultPosition, wxDefaultSize, 0 );
	gSizer41->Add( m_button_err_clear_hoist, 0, wxALL, 5 );

	m_button_err_clear_slide = new wxButton( ErrorHandling->GetStaticBox(), wxID_BUTTON_ERR_CLEAR_SLIDE, wxT("Slide Clear"), wxDefaultPosition, wxDefaultSize, 0 );
	gSizer41->Add( m_button_err_clear_slide, 0, wxALL, 5 );


	ErrorHandling->Add( gSizer41, 1, wxEXPAND, 5 );


	bSizer1->Add( ErrorHandling, 1, wxEXPAND, 5 );

	wxStaticBoxSizer* AbsoluteEncoder;
	AbsoluteEncoder = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT(" Absolute Encoder ") ), wxVERTICAL );

	wxGridSizer* gSizer55;
	gSizer55 = new wxGridSizer( 1, 4, 0, 0 );

	m_staticText26 = new wxStaticText( AbsoluteEncoder->GetStaticBox(), wxID_ANY, wxT("Encode Reset"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText26->Wrap( -1 );
	gSizer55->Add( m_staticText26, 0, wxALL, 5 );

	m_button_encoder_reset_driving = new wxButton( AbsoluteEncoder->GetStaticBox(), wxID_BUTTON_ENC_RESET_DRIVING, wxT("Driving [1]"), wxDefaultPosition, wxDefaultSize, 0 );
	gSizer55->Add( m_button_encoder_reset_driving, 0, wxALL, 5 );

	m_button_encoder_reset_hoist = new wxButton( AbsoluteEncoder->GetStaticBox(), wxID_BUTTON_ENC_RESET_HOIST, wxT("Hoist [2]"), wxDefaultPosition, wxDefaultSize, 0 );
	gSizer55->Add( m_button_encoder_reset_hoist, 0, wxALL, 5 );

	m_button_encoder_reset_slide = new wxButton( AbsoluteEncoder->GetStaticBox(), wxID_BUTTON_ENC_RESET_SLIDE, wxT("Slide [2]"), wxDefaultPosition, wxDefaultSize, 0 );
	gSizer55->Add( m_button_encoder_reset_slide, 0, wxALL, 5 );


	AbsoluteEncoder->Add( gSizer55, 1, wxEXPAND, 5 );


	bSizer1->Add( AbsoluteEncoder, 1, wxEXPAND, 5 );


	this->SetSizer( bSizer1 );
	this->Layout();
	statusBar = this->CreateStatusBar( 2, wxSTB_SIZEGRIP, wxID_ANY );

	// Connect Events
	this->Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( GUIFrame::OnClose ) );
	fileMenu->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( GUIFrame::OnQuit ), this, menuFileQuit->GetId());
	helpMenu->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( GUIFrame::OnAbout ), this, menuHelpAbout->GetId());
	m_button_new_driving->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( GUIFrame::OnButtonClick ), NULL, this );
//	m_button_new_hoist->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( GUIFrame::OnButtonClick ), NULL, this );
//	m_button_new_slide->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( GUIFrame::OnButtonClick ), NULL, this );
	m_button_param_driving->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( GUIFrame::OnButtonClick ), NULL, this );
	m_button_param_hoist->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( GUIFrame::OnButtonClick ), NULL, this );
	m_button_param_slide->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( GUIFrame::OnButtonClick ), NULL, this );
	m_button_err_code_front->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( GUIFrame::OnButtonClick ), NULL, this );
	m_button_err_code_rear->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( GUIFrame::OnButtonClick ), NULL, this );
	m_button_err_code_hoist->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( GUIFrame::OnButtonClick ), NULL, this );
	m_button_err_code_slide->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( GUIFrame::OnButtonClick ), NULL, this );
	m_button_err_clear_front->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( GUIFrame::OnButtonClick ), NULL, this );
	m_button_err_clear_rear->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( GUIFrame::OnButtonClick ), NULL, this );
	m_button_err_clear_hoist->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( GUIFrame::OnButtonClick ), NULL, this );
	m_button_err_clear_slide->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( GUIFrame::OnButtonClick ), NULL, this );
	m_button_encoder_reset_driving->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( GUIFrame::OnButtonClick ), NULL, this );
	m_button_encoder_reset_hoist->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( GUIFrame::OnButtonClick ), NULL, this );
	m_button_encoder_reset_slide->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( GUIFrame::OnButtonClick ), NULL, this );
	m_button_param_extraction->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( GUIFrame::OnButtonClick ), NULL, this );
}

GUIFrame::~GUIFrame()
{
	// Disconnect Events
	this->Disconnect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( GUIFrame::OnClose ) );
	m_button_new_driving->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( GUIFrame::OnButtonClick ), NULL, this );
//	m_button_new_hoist->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( GUIFrame::OnButtonClick ), NULL, this );
//	m_button_new_slide->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( GUIFrame::OnButtonClick ), NULL, this );
	m_button_param_driving->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( GUIFrame::OnButtonClick ), NULL, this );
	m_button_param_hoist->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( GUIFrame::OnButtonClick ), NULL, this );
	m_button_param_slide->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( GUIFrame::OnButtonClick ), NULL, this );
	m_button_err_code_front->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( GUIFrame::OnButtonClick ), NULL, this );
	m_button_err_code_rear->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( GUIFrame::OnButtonClick ), NULL, this );
	m_button_err_code_hoist->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( GUIFrame::OnButtonClick ), NULL, this );
	m_button_err_code_slide->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( GUIFrame::OnButtonClick ), NULL, this );
	m_button_err_clear_front->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( GUIFrame::OnButtonClick ), NULL, this );
	m_button_err_clear_rear->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( GUIFrame::OnButtonClick ), NULL, this );
	m_button_err_clear_hoist->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( GUIFrame::OnButtonClick ), NULL, this );
	m_button_err_clear_slide->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( GUIFrame::OnButtonClick ), NULL, this );
	m_button_encoder_reset_driving->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( GUIFrame::OnButtonClick ), NULL, this );
	m_button_encoder_reset_hoist->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( GUIFrame::OnButtonClick ), NULL, this );
	m_button_encoder_reset_slide->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( GUIFrame::OnButtonClick ), NULL, this );
	m_button_param_extraction->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( GUIFrame::OnButtonClick ), NULL, this );

}
