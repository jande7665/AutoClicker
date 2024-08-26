#include <wx/wx.h>
#include "MainWindow.h"

class MyApp : public wxApp
{
public:
    virtual bool OnInit();
};

wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit()
{
    MainWindow* mainWin = new MainWindow(wxT("Auto Clicker"));
    mainWin->Show(true);
    return true;
}
