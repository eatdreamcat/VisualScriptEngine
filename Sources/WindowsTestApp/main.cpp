#include "BitmapContextGdi.hpp"
#include "BitmapContextGdiplus.hpp"
#include "WindowsAppUtilities.hpp"
#include "NodeEditorControl.hpp"
#include "DrawingControl.hpp"
#include "Debug.hpp"

#include <CommCtrl.h>
#include <iostream>
#include <fstream>

#include "wx/wx.h"
#include "wx/splitter.h"

class ApplicationState
{
public:
	ApplicationState () :
		currentFileName ()
	{

	}

	void ClearCurrentFileName ()
	{
		currentFileName.clear ();
	}

	void SetCurrentFileName (const std::wstring& newCurrentFileName)
	{
		currentFileName = newCurrentFileName;
	}

	bool HasCurrentFileName () const
	{
		return !currentFileName.empty ();
	}

	const std::wstring& GetCurrentFileName () const
	{
		return currentFileName;
	}

private:
	std::wstring currentFileName;
};

class MainFrame : public wxFrame
{
public:
	enum CommandId
	{
		File_New		= 1,
		File_Open		= 2,
		File_Save		= 3,
		File_SaveAs		= 4,
		File_Exit		= 5
	};

	MainFrame (const std::shared_ptr<ResultImage>& resultImage, NE::EvaluationEnv& evaluationEnv) :
		wxFrame (NULL, wxID_ANY, L"Node Engine Test App", wxDefaultPosition, wxSize (1000, 600)),
		menuBar (new wxMenuBar ()),
		fileMenu (new wxMenu ()),
		splitter (new wxSplitterWindow (this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3D | wxSP_LIVE_UPDATE)),
		drawingControl (new DrawingControl (splitter, resultImage)),
		updateInterface (drawingControl),
		nodeEditorControl (new NodeEditorControl (splitter, updateInterface, evaluationEnv)),
		applicationState ()
	{
		fileMenu->Append (CommandId::File_New, "New");
		fileMenu->Append (CommandId::File_Open, "Open...");
		fileMenu->Append (CommandId::File_Save, "Save...");
		fileMenu->Append (CommandId::File_SaveAs, "Save As...");
		fileMenu->AppendSeparator ();
		fileMenu->Append (CommandId::File_Exit, L"Exit");
		menuBar->Append (fileMenu, L"&File");
		SetMenuBar (menuBar);

		splitter->SetSashGravity (0.5);
		splitter->SetMinimumPaneSize (20);
		splitter->SplitVertically (nodeEditorControl, drawingControl, 700);

		CreateStatusBar ();
		UpdateStatusBar ();
	}

	~MainFrame ()
	{
	
	}

	void OnNew (wxCommandEvent& event)
	{
		nodeEditorControl->New ();
		drawingControl->ClearImage ();
		applicationState.ClearCurrentFileName ();
		UpdateStatusBar ();
	}

	void OnOpen (wxCommandEvent& event)
	{
		// TODO: Duplicated code
		OPENFILENAME openFileName;
		ZeroMemory (&openFileName, sizeof(openFileName));
		wchar_t fileName[MAX_PATH] = L"";
		openFileName.lStructSize = sizeof (openFileName); 
		openFileName.hwndOwner = (HWND) GetHandle ();
		openFileName.lpstrFile = (LPWSTR) fileName;
		openFileName.nMaxFile = MAX_PATH;
		openFileName.lpstrFilter = (LPCWSTR) L"Node Engine Files (*.ne)\0*.ne\0";
		openFileName.lpstrDefExt = (LPCWSTR) L"txt";
		if (GetOpenFileName (&openFileName)) {
			drawingControl->ClearImage ();
			// TODO: handle when open fails
			if (nodeEditorControl->Open (fileName)) {
				applicationState.SetCurrentFileName (fileName);
			}
		}
		UpdateStatusBar ();
	}

	void OnSave (wxCommandEvent& event)
	{
		// TODO: Duplicated code
		OPENFILENAME openFileName;
		ZeroMemory (&openFileName, sizeof(openFileName));
		wchar_t fileName[MAX_PATH] = L"";
		openFileName.lStructSize = sizeof (openFileName); 
		openFileName.hwndOwner = (HWND) GetHandle ();
		openFileName.lpstrFile = (LPWSTR) fileName;
		openFileName.nMaxFile = MAX_PATH;
		openFileName.lpstrFilter = (LPCWSTR) L"Node Engine Files (*.ne)\0*.ne\0";
		openFileName.lpstrDefExt = (LPCWSTR) L"txt";
		if (applicationState.HasCurrentFileName ()) {
			nodeEditorControl->Save (applicationState.GetCurrentFileName ());
		} else if (GetSaveFileName (&openFileName)) {
			nodeEditorControl->Save (fileName);
			applicationState.SetCurrentFileName (fileName);
		}
		UpdateStatusBar ();
	}

	void OnSaveAs (wxCommandEvent& event)
	{
		// TODO: Duplicated code
		OPENFILENAME openFileName;
		ZeroMemory (&openFileName, sizeof(openFileName));
		wchar_t fileName[MAX_PATH] = L"";
		openFileName.lStructSize = sizeof (openFileName); 
		openFileName.hwndOwner = (HWND) GetHandle ();
		openFileName.lpstrFile = (LPWSTR) fileName;
		openFileName.nMaxFile = MAX_PATH;
		openFileName.lpstrFilter = (LPCWSTR) L"Node Engine Files (*.ne)\0*.ne\0";
		openFileName.lpstrDefExt = (LPCWSTR) L"txt";
		if (GetSaveFileName (&openFileName)) {
			nodeEditorControl->Save (fileName);
			applicationState.SetCurrentFileName (fileName);
		}
		UpdateStatusBar ();
	}

	void OnExit (wxCommandEvent& event)
	{
		Close (true);
	}

	void UpdateStatusBar ()
	{
		std::wstring currentFileText = L"No File";
		if (applicationState.HasCurrentFileName ()) {
			currentFileText = applicationState.GetCurrentFileName ();
		}
		SetStatusText (currentFileText);
	}

private:
	class DrawingUpdateInterface : public UpdateInterface
	{
	public:
		DrawingUpdateInterface (DrawingControl* drawingControl) :
			drawingControl (drawingControl)
		{
		
		}

		virtual void RedrawImage () override
		{
			drawingControl->RedrawImage ();
		}

	private:
		DrawingControl* drawingControl;
	};

	wxMenuBar*				menuBar;
	wxMenu*					fileMenu;

	wxSplitterWindow*		splitter;
	DrawingControl*			drawingControl;
	DrawingUpdateInterface	updateInterface;
	NodeEditorControl*		nodeEditorControl;

	ApplicationState		applicationState;

	DECLARE_EVENT_TABLE ()
};

BEGIN_EVENT_TABLE (MainFrame, wxFrame)
EVT_MENU (MainFrame::CommandId::File_New, MainFrame::OnNew)
EVT_MENU (MainFrame::CommandId::File_Open, MainFrame::OnOpen)
EVT_MENU (MainFrame::CommandId::File_Save, MainFrame::OnSave)
EVT_MENU (MainFrame::CommandId::File_SaveAs, MainFrame::OnSaveAs)
EVT_MENU (MainFrame::CommandId::File_Exit, MainFrame::OnExit)
END_EVENT_TABLE ()

class NodeEngineTestApplication : public wxApp
{
public:
	NodeEngineTestApplication () :
		resultImage (new ResultImage ()),
		evaluationData (new ResultImageEvaluationData (resultImage)),
		evaluationEnv (evaluationData),
		mainFrame (nullptr)
	{
	
	}

	virtual bool OnInit ()
	{
		mainFrame = new MainFrame (resultImage, evaluationEnv);
		mainFrame->Show (true);
		SetTopWindow (mainFrame);
		return true;
	}

private:
	std::shared_ptr<ResultImage>				resultImage;
	std::shared_ptr<ResultImageEvaluationData>	evaluationData;
	NE::EvaluationEnv							evaluationEnv;

	MainFrame*									mainFrame;
};

IMPLEMENT_APP (NodeEngineTestApplication)
