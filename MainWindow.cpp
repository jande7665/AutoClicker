#include "MainWindow.h"
#include <ApplicationServices/ApplicationServices.h>
#include <wx/sound.h>
#include <wx/stdpaths.h>
#include <wx/filename.h>
#include <iostream>

// Event table for MainWindow
wxBEGIN_EVENT_TABLE(MainWindow, wxFrame)
    EVT_BUTTON(ID_Start, MainWindow::OnStart)
    EVT_BUTTON(ID_Stop, MainWindow::OnStop)
    EVT_SPINCTRL(ID_Interval, MainWindow::OnIntervalChange)
    EVT_CHECKBOX(ID_Mode, MainWindow::OnToggleMode)
wxEND_EVENT_TABLE()

// Constructor
MainWindow::MainWindow(const wxString& title)
    : wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxSize(300, 200)),
      m_clicking(false),
      m_interval(1000), // Default to 1 second (1000 milliseconds)
      m_clicksPerSecond(true) // Default to clicks per second mode
{
    // Create panel and sizer
    wxPanel* panel = new wxPanel(this, wxID_ANY);
    wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);

    // Interval label
    wxStaticText* label = new wxStaticText(panel, wxID_ANY, wxT("Interval:"));
    vbox->Add(label, 0, wxLEFT | wxTOP, 10);

    // Spin control for interval
    m_spinCtrl = new wxSpinCtrl(panel, ID_Interval, wxEmptyString, wxDefaultPosition, wxSize(100, -1));
    m_spinCtrl->SetRange(1, 60);  // Set range from 1 to 60
    m_spinCtrl->SetValue(1);
    m_spinCtrl->SetToolTip(wxT("Set the interval"));
    vbox->Add(m_spinCtrl, 0, wxLEFT | wxTOP, 10);

    // Checkbox to toggle mode
    m_modeCheckBox = new wxCheckBox(panel, ID_Mode, wxT("Clicks per Second"));
    m_modeCheckBox->SetValue(true);
    m_modeCheckBox->SetToolTip(wxT("Toggle between clicks per second and seconds per click"));
    vbox->Add(m_modeCheckBox, 0, wxLEFT | wxTOP, 10);

    // Start button
    m_startButton = new wxButton(panel, ID_Start, wxT("Start"));
    m_startButton->SetToolTip(wxT("Start the auto clicker"));
    vbox->Add(m_startButton, 0, wxLEFT | wxTOP, 10);

    // Stop button
    m_stopButton = new wxButton(panel, ID_Stop, wxT("Stop"));
    m_stopButton->SetToolTip(wxT("Stop the auto clicker"));
    m_stopButton->Enable(false); // Initially disabled
    vbox->Add(m_stopButton, 0, wxLEFT | wxTOP, 10);

    // Set sizer for panel
    panel->SetSizer(vbox);

    // Create status bar
    this->CreateStatusBar();
    this->SetStatusText(wxT("Ready"));
}

// Destructor
MainWindow::~MainWindow()
{
    m_clicking = false;
    if (m_clickThread.joinable())
    {
        m_clickThread.join();
    }
}

// Start button event handler
void MainWindow::OnStart(wxCommandEvent& event)
{
    m_clicking = true;
    m_interval = m_spinCtrl->GetValue();
    this->SetStatusText(wxT("Auto clicker started"));
    m_startButton->Enable(false);
    m_stopButton->Enable(true);

    m_clickThread = std::thread(&MainWindow::PerformClick, this);
}

// Stop button event handler
void MainWindow::OnStop(wxCommandEvent& event)
{
    m_clicking = false;
    if (m_clickThread.joinable())
    {
        m_clickThread.join();
    }
    this->SetStatusText(wxT("Auto clicker stopped"));
    m_startButton->Enable(true);
    m_stopButton->Enable(false);
}

// Spin control event handler
void MainWindow::OnIntervalChange(wxSpinEvent& event)
{
    m_interval = m_spinCtrl->GetValue();
}

// Checkbox event handler
void MainWindow::OnToggleMode(wxCommandEvent& event)
{
    m_clicksPerSecond = m_modeCheckBox->GetValue();
    if (m_clicksPerSecond)
    {
        m_modeCheckBox->SetLabel(wxT("Clicks per Second"));
    }
    else
    {
        m_modeCheckBox->SetLabel(wxT("Seconds per Click"));
    }
}

// Function to perform auto-clicking
void MainWindow::PerformClick()
{
    // Get the executable path and construct the full path to the sound file
    wxString execPath = wxStandardPaths::Get().GetExecutablePath();
    wxFileName execFileName(execPath);
    wxString soundFilePath = execFileName.GetPath() + wxFILE_SEP_PATH + wxT("click.wav");

    // Debug: Print the sound file path
    std::cout << "Sound file path: " << soundFilePath.ToStdString() << std::endl;

    wxSound clickSound(soundFilePath); // Load the sound file using the full path

    if (!clickSound.IsOk())
    {
        std::cerr << "Failed to load sound from " << soundFilePath.ToStdString() << std::endl;
        return;
    }

    while (m_clicking)
    {
        // Get the current mouse position
        CGPoint mousePos;
        CGEventRef event = CGEventCreate(nullptr);
        mousePos = CGEventGetLocation(event);
        CFRelease(event);

        // Simulate a mouse click at the current mouse position
        CGEventRef click_down = CGEventCreateMouseEvent(nullptr, kCGEventLeftMouseDown, mousePos, kCGMouseButtonLeft);
        CGEventRef click_up = CGEventCreateMouseEvent(nullptr, kCGEventLeftMouseUp, mousePos, kCGMouseButtonLeft);
        CGEventPost(kCGHIDEventTap, click_down);
        CGEventPost(kCGHIDEventTap, click_up);
        CFRelease(click_down);
        CFRelease(click_up);

        // Play the click sound
        clickSound.Play(wxSOUND_ASYNC);

        // Determine the interval duration
        int interval = m_interval.load();
        int sleep_duration;
        if (m_clicksPerSecond)
        {
            // 1-10 clicks per second
            sleep_duration = 1000 / interval;
        }
        else
        {
            // 1 click every 1-60 seconds
            sleep_duration = interval * 1000;
        }

        // Wait for the interval duration
        std::this_thread::sleep_for(std::chrono::milliseconds(sleep_duration));
    }
}