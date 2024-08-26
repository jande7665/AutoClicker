#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <wx/wx.h>
#include <wx/spinctrl.h>
#include <thread>
#include <atomic>

class MainWindow : public wxFrame
{
public:
    MainWindow(const wxString& title);
    ~MainWindow();

private:
    void OnStart(wxCommandEvent& event);
    void OnStop(wxCommandEvent& event);
    void PerformClick();
    void OnIntervalChange(wxSpinEvent& event);
    void OnToggleMode(wxCommandEvent& event);

    wxSpinCtrl* m_spinCtrl;
    wxButton* m_startButton;
    wxButton* m_stopButton;
    wxCheckBox* m_modeCheckBox;
    std::thread m_clickThread;
    std::atomic<bool> m_clicking;
    std::atomic<int> m_interval; // Interval in milliseconds
    std::atomic<bool> m_clicksPerSecond; // Mode flag

    wxStatusBar* m_statusBar;

    wxDECLARE_EVENT_TABLE();
};

enum
{
    ID_Start = 1,
    ID_Stop = 2,
    ID_Interval = 3,
    ID_Mode = 4
};

#endif // MAINWINDOW_H