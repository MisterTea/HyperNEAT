#include "HCUBE_Defines.h"

#ifndef HCUBE_NOGUI

#include "HCUBE_MainFrame.h"

#include "HCUBE_ViewIndividualFrame.h"

#include "Experiments/HCUBE_Experiment.h"

#include "HCUBE_UserEvaluationFrame.h"

//#include <ApplicationServices/ApplicationServices.h>

namespace HCUBE
{
    MainFrame::MainFrame(
        wxWindow* parent,
        int id,
        wxString title,
        wxPoint pos,
        wxSize size,
        int style
    )
            :
            MainFrameTemplate( parent, id, title, pos, size, style )
    {
        /*
        fileMenu = new wxMenu();
        menuBar = new wxMenuBar();

        experimentPanel = new ExperimentPanel(
        this,
        -1,
        wxPoint(50,50),
        wxSize(100,20)
        );
        */

        experimentRun.setFrame(this);

        /*
        experimentPanel->Enable(false);

        wxMenuItem *menuItem;

        menuItem = fileMenu->Append( ID_LoadExperiment, "&Load Experiment" );
        menuItem = fileMenu->Append( ID_RunExperiment, "&Run Experiment" );
        menuItem->Enable(false);
        menuItem = fileMenu->Append( ID_PauseExperiment, "Pause Expe&riment" );
        menuItem->Enable(false);
        menuItem = fileMenu->AppendSeparator();
        menuItem = fileMenu->Append( ID_LoadPopulation, "Load &Population" );
        menuItem = fileMenu->AppendSeparator();
        menuItem = fileMenu->Append( ID_About, "&About..." );
        menuItem = fileMenu->AppendSeparator();
        menuItem = fileMenu->Append( ID_Quit, "E&xit" );

        menuBar->Append( fileMenu, "&File" );

        SetMenuBar( menuBar );

        CreateStatusBar();
        */

        SetStatusText( CSTR_TO_WXSTRING("Welcome to HyperNEAT!") );

		//ProcessSerialNumber PSN;
		//GetCurrentProcess(&PSN);
		//TransformProcessType(&PSN,kProcessTransformToForegroundApplication);
		
    }

    MainFrame::~MainFrame()
    {}

    void MainFrame::exitApplication(wxCommandEvent& WXUNUSED(event))
    {
        Close(TRUE);
    }

    void MainFrame::aboutApplication(wxCommandEvent& WXUNUSED(event))
    {
        wxMessageBox(
            CSTR_TO_WXSTRING("Hypercube NEAT.  By Jason Gauci, UCF PhD. Student.  jgauci@cs.ucf.edu"),
            CSTR_TO_WXSTRING("About Hypercube NEAT"),
            wxOK | wxICON_INFORMATION,
            this
        );
    }

    void MainFrame::loadExperiment(wxCommandEvent& WXUNUSED(event))
    {
        try
        {
            if (experimentRun.isStarted())
            {
                throw CREATE_LOCATEDEXCEPTION_INFO("Loading a new experiment while one is running is not yet supported!\n");
            }
            else
            {
                wxFileDialog loadParametersDialog(
                    this,
                    _T("Choose a Parameter File"),
                    wxGetCwd(),
                    _T(""),
                    _T("*.dat"),
                    wxOPEN|wxFILE_MUST_EXIST
                );

                int retVal = loadParametersDialog.ShowModal();

                string fileName = WXSTRING_TO_STRING(loadParametersDialog.GetFilename());

                fileName = fileName.substr(0,fileName.length()-3) + string("xml");

                if (retVal==wxID_OK)
                {
                    wxFileDialog outputFileDialog(
                        this,
                        _T("Choose an Output File"),
                        loadParametersDialog.GetDirectory(),
                        STRING_TO_WXSTRING(fileName),
                        _T("*.xml;*.gz"),
                        wxSAVE
                    );

                    retVal = outputFileDialog.ShowModal();

                    if (retVal==wxID_OK)
                    {

                        NEAT::Globals::init(WXSTRING_TO_STRING(loadParametersDialog.GetPath()));

                        int experimentType = int(NEAT::Globals::getSingleton()->getParameterValue("ExperimentType")+0.001);

                        cout << "Loading Experiment: " << experimentType << endl;

                        experimentRun.setupExperiment(experimentType,WXSTRING_TO_STRING(outputFileDialog.GetPath()));

                        cout << "Experiment set up\n";

                        experimentRun.createPopulation();

                        cout << "Population Created\n";

                        fileMenu->FindItem(wxID_RUNEXPERIMENT_MENUITEM)->Enable(true);
                        setPopulationSize(int(NEAT::Globals::getSingleton()->getParameterValue("PopulationSize")));

                        if (experimentRun.getExperiment()->performUserEvaluations())
                        {
                            cout << "Creating User Evaluation Window...";
                            //Create a Frame for User Evaluations
                            userEvaluationFrame = new UserEvaluationFrame(&experimentRun,(wxWindow*)this);
                            cout << "...Done!\n";
                        }

                        StackMap<string,double,4096>::iterator parameterIterator = NEAT::Globals::getSingleton()->getMapBegin();
                        for (;
                                parameterIterator != NEAT::Globals::getSingleton()->getMapEnd();
                                parameterIterator++)
                        {
                            ostringstream ostr;
                            ostr << parameterIterator->first << ":     " << parameterIterator->second;
                            parameterListBox->AppendString(STRING_TO_WXSTRING(ostr.str()));
                        }

                        return;
                    }
                }
            }
        }
        catch (const std::exception &ex)
        {
			cout << "CAUGHT ERROR AT " << __FILE__ << " : " << __LINE__ << endl;
            CREATE_PAUSE(ex.what());
        }
        catch (...)
        {
			cout << "CAUGHT ERROR AT " << __FILE__ << " : " << __LINE__ << endl;
            CREATE_PAUSE("AN UNKNOWN EXCEPTION HAS OCCURED!");
        }
    }

    void MainFrame::loadPopulation(wxCommandEvent& WXUNUSED(event))
    {
        if (experimentRun.isStarted())
            {}
        else
        {
            wxFileDialog loadParametersDialog(
                this,
                _T("Choose a Population File"),
                wxGetCwd(),
                _T(""),
                _T("*.xml;*.gz"),
                wxOPEN|wxFILE_MUST_EXIST
            );

            int retVal = loadParametersDialog.ShowModal();

            if (retVal==wxID_OK)
            {
                cout << "Loading population file: " << WXSTRING_TO_STRING(loadParametersDialog.GetPath()) << endl;

                populationFileName = WXSTRING_TO_STRING(loadParametersDialog.GetFilename());

                experimentRun.setupExperimentInProgress(
                    WXSTRING_TO_STRING(loadParametersDialog.GetPath()),
                    WXSTRING_TO_STRING(loadParametersDialog.GetPath())+string(".new")
                );

                fileMenu->FindItem(wxID_RUNEXPERIMENT_MENUITEM)->Enable(true);
                int genCount = experimentRun.getPopulation()->getGenerationCount();
                updateNumGenerations(genCount);
                setPopulationSize(int(NEAT::Globals::getSingleton()->getParameterValue("PopulationSize")));
                return;
            }
        }
    }

    void MainFrame::runExperiment(wxCommandEvent& WXUNUSED(event))
    {
        if (!experimentRun.isStarted())
        {
			if(EXPERIMENT_BLOCKS_GUI)
			{
				cout << "Experiment is blocking GUI.  GUI will be "
						"unresponsive until experiment finishes\n";
				experimentRun.start();
			}
			else
			{
				cout << "Creating Boost Thread\n";
				boost::thread thread( boost::bind(&ExperimentRun::start,&experimentRun) );
			}
            fileMenu->FindItem(wxID_RUNEXPERIMENT_MENUITEM)->SetText(_T("Rerun &Experiment"));
            fileMenu->FindItem(wxID_PAUSEEXPERIMENT_MENUITEM)->Enable(true);
        }
    }

    void MainFrame::restartExperiment( wxCommandEvent& event )
    {}

    void MainFrame::pauseExperiment(wxCommandEvent& WXUNUSED(event))
    {
        if (experimentRun.isRunning())
        {
            experimentRun.setRunning(false);
            fileMenu->FindItem(wxID_PAUSEEXPERIMENT_MENUITEM)->SetText(_T("Continue Ex&periment"));
        }
        else
        {
            experimentRun.setRunning(true);
            fileMenu->FindItem(wxID_PAUSEEXPERIMENT_MENUITEM)->SetText(_T("&Pause Experiment"));
        }
    }

    /*void MainFrame::loadExperiment(const string &parameters,const string &outputFileName)
    {
    NEAT::Globals::init(parameters);

    int experimentType = int(NEAT::Globals::getSingleton()->getParameterValue("ExperimentType")+0.001);

    cout << "Loading Experiment: " << experimentType << endl;

    experimentRun.setupExperiment(experimentType);

    experimentRun.createPopulation(populationString);

    experimentPanel->setPopulationSize(NEAT::Globals::getSingleton()->getParameterValue("PopulationSize"));

    cout << "Finished loading and ready to run!\n";
    }*/

    void MainFrame::viewIndividual( wxCommandEvent& event )
    {
        int generation = generationSpinner->GetValue();
        int individual = individualSpinner->GetValue();

        string title;

        title +=
            populationFileName +
            string("_") +
            string("Generation: ") +
            toString(generation) +
            string(" Individual: ") +
            toString(individual);

        mutex *populationMutex = experimentRun.getPopulationMutex();
        {
            mutex::scoped_lock scoped_lock(*populationMutex);

            shared_ptr<Experiment> experiment(experimentRun.getExperiment()->clone());

            experiment->setExperimentName(title);

            //We do -1 because indicies are 0-based
            shared_ptr<NEAT::GeneticIndividual> indiv =
                shared_ptr<NEAT::GeneticIndividual>(
                    new NEAT::GeneticIndividual(
                        *(experimentRun.getIndividual(generation-1,individual-1).get())
                    )
                );

            ViewIndividualFrame *viewFrame =
                new ViewIndividualFrame(
                experiment,
                indiv,
                this,
                wxID_ANY,
                STRING_TO_WXSTRING(title),
                wxDefaultPosition,
                wxSize(720,480)
            );

            viewFrame->Show(TRUE);
        }
    }

    void MainFrame::analyzeIndividual( wxCommandEvent& event )
    {
        int generation = generationSpinner->GetValue();
        int individual = individualSpinner->GetValue();

        string title;

        title +=
            populationFileName +
            string("_") +
            string("Generation: ") +
            toString(generation) +
            string(" Individual: ") +
            toString(individual);

        mutex *populationMutex = experimentRun.getPopulationMutex();
        {
            mutex::scoped_lock scoped_lock(*populationMutex);

            Experiment *experiment = experimentRun.getExperiment()->clone();

            shared_ptr<NEAT::GeneticIndividual> indiv(
                new NEAT::GeneticIndividual(
                    *(experimentRun.getIndividual(generation-1,individual-1).get())
                )
            );

            experiment->processIndividualPostHoc(indiv);

            if (indiv->getUserData().length())
            {
                cout << "PRINTING USER DATA RESULTS:\n";
                cout << indiv->getUserData() << endl;
            }

            delete experiment;

        }
    }

    void MainFrame::setPopulationSize(int newPopulationSize)
    {
        individualSpinner->SetRange(1,newPopulationSize);
    }

    void MainFrame::updateNumGenerations(int numGenerations)
    {
        if (numGenerations>0)
        {
            cout << "Enabling control...\n";
            viewIndividualButton->Enable(true);
            analyzeIndividualButton->Enable(true);

            generationSpinner->SetRange(1,numGenerations);

            if (generationSpinner->GetValue()==0)
                generationSpinner->SetValue(1);

            cout << "Refreshing...\n";
            Refresh();
            cout << "Done Refreshing\n";
        }
    }
}

#endif
