///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////
#ifndef __HCUBE_GUITemplate__
#define __HCUBE_GUITemplate__
namespace HCUBE{ class EvaluationPanel; }
namespace HCUBE{ class NetworkPanel; }
namespace HCUBE{ class NeuralNetworkPanel; }
namespace HCUBE{ class NeuralNetworkWeightGrid; }
#include <wx/statusbr.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/menu.h>
#include <wx/spinctrl.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/button.h>
#include <wx/listbox.h>
#include <wx/panel.h>
#include <wx/notebook.h>
#include <wx/textctrl.h>
#include <wx/frame.h>
#include <wx/tglbtn.h>
#include <wx/grid.h>
///////////////////////////////////////////////////////////////////////////
namespace HCUBE
{
	///////////////////////////////////////////////////////////////////////////////
	/// Class MainFrameTemplate
	///////////////////////////////////////////////////////////////////////////////
	class MainFrameTemplate : public wxFrame 
	{
		private:
		
		protected:
			enum
			{
				wxID_MAIN_MENU_BAR = 5000,
				wxID_LOADEXPERIMENT_MENUITEM,
				wxID_RUNEXPERIMENT_MENUITEM,
				wxID_RESTARTEXPERIMENT_MENUITEM,
				wxID_PAUSEEXPERIMENT_MENUITEM,
				wxID_LOADPOPULATION_MENUITEM,
				wxID_GENERATION_SPINNER,
				wxID_INDIVIDUAL_SPINNER,
				wxID_VIEWINDIVIDUAL_BUTTON,
				wxID_ANALYZEINDIVIDUAL_BUTTON,
				wxID_PARAMETER_LISTBOX,
				wxID_CONSOLEOUTPUT_TEXTBOX,
			};
			
			wxStatusBar* m_statusBar1;
			wxMenuBar* menubar;
			wxMenu* fileMenu;
			wxMenu* helpMenu;
			wxSpinCtrl* generationSpinner;
			wxSpinCtrl* individualSpinner;
			wxButton* viewIndividualButton;
			wxButton* analyzeIndividualButton;
			wxNotebook* m_notebook3;
			wxPanel* parametersPage;
			wxListBox* parameterListBox;
			wxPanel* outputLogPage;
			wxTextCtrl* consoleOutputTextBox;
			
			// Virtual event handlers, overide them in your derived class
			virtual void loadExperiment( wxCommandEvent& event ){ event.Skip(); }
			virtual void runExperiment( wxCommandEvent& event ){ event.Skip(); }
			virtual void restartExperiment( wxCommandEvent& event ){ event.Skip(); }
			virtual void pauseExperiment( wxCommandEvent& event ){ event.Skip(); }
			virtual void loadPopulation( wxCommandEvent& event ){ event.Skip(); }
			virtual void exitApplication( wxCommandEvent& event ){ event.Skip(); }
			virtual void aboutApplication( wxCommandEvent& event ){ event.Skip(); }
			virtual void viewIndividual( wxCommandEvent& event ){ event.Skip(); }
			virtual void analyzeIndividual( wxCommandEvent& event ){ event.Skip(); }
			
		
		public:
			MainFrameTemplate( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Jason Gauci's HyperNEAT"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 640,480 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
			~MainFrameTemplate();
		
	};
	
	///////////////////////////////////////////////////////////////////////////////
	/// Class UserEvaluationFrameTemplate
	///////////////////////////////////////////////////////////////////////////////
	class UserEvaluationFrameTemplate : public wxFrame 
	{
		private:
		
		protected:
			HCUBE::EvaluationPanel* evaluationPanel0;
			HCUBE::EvaluationPanel* evaluationPanel1;
			HCUBE::EvaluationPanel* evaluationPanel2;
			HCUBE::EvaluationPanel* evaluationPanel3;
			HCUBE::EvaluationPanel* evaluationPanel4;
			HCUBE::EvaluationPanel* evaluationPanel5;
			HCUBE::EvaluationPanel* evaluationPanel6;
			HCUBE::EvaluationPanel* evaluationPanel7;
			wxNotebook* m_notebook1;
			HCUBE::EvaluationPanel* highResPanel;
			HCUBE::NeuralNetworkPanel* networkPanel;
			wxPanel* votingPanel;
			wxToggleButton* voteButton0;
			wxToggleButton* voteButton1;
			wxToggleButton* voteButton2;
			wxToggleButton* voteButton3;
			wxToggleButton* voteButton4;
			wxToggleButton* voteButton5;
			wxToggleButton* voteButton6;
			wxToggleButton* voteButton7;
			wxButton* advanceGenerationButton;
			wxButton* resetVotesButton;
			
			// Virtual event handlers, overide them in your derived class
			virtual void advanceGeneration( wxCommandEvent& event ){ event.Skip(); }
			virtual void resetVotes( wxCommandEvent& event ){ event.Skip(); }
			
		
		public:
			UserEvaluationFrameTemplate( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("User Evaluations"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 800,600 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
			~UserEvaluationFrameTemplate();
		
	};
	
	///////////////////////////////////////////////////////////////////////////////
	/// Class NeuralNetworkPanelTemplate
	///////////////////////////////////////////////////////////////////////////////
	class NeuralNetworkPanelTemplate : public wxPanel 
	{
		private:
		
		protected:
			HCUBE::NetworkPanel* networkVisualizationPanel;
			HCUBE::NeuralNetworkWeightGrid* networkAdjacency;
		
		public:
			NeuralNetworkPanelTemplate( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 512,512 ), long style = wxTAB_TRAVERSAL );
			~NeuralNetworkPanelTemplate();
		
	};
	
	///////////////////////////////////////////////////////////////////////////////
	/// Class ViewIndividualFrameTemplate
	///////////////////////////////////////////////////////////////////////////////
	class ViewIndividualFrameTemplate : public wxFrame 
	{
		private:
		
		protected:
			wxNotebook* m_notebook2;
			HCUBE::EvaluationPanel* evaluationPanel;
			HCUBE::NeuralNetworkPanel* networkPanel;
		
		public:
			ViewIndividualFrameTemplate( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 640,480 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
			~ViewIndividualFrameTemplate();
		
	};
	
	///////////////////////////////////////////////////////////////////////////////
	/// Class TicTacToeGameExperimentPanelTemplate
	///////////////////////////////////////////////////////////////////////////////
	class TicTacToeGameExperimentPanelTemplate : public wxPanel 
	{
		private:
		
		protected:
		
		public:
			TicTacToeGameExperimentPanelTemplate( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxTAB_TRAVERSAL );
			~TicTacToeGameExperimentPanelTemplate();
		
	};
	
} // namespace HCUBE
#endif //__HCUBE_GUITemplate__
