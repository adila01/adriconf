#ifndef ADRICONF_GUI_H
#define ADRICONF_GUI_H

#include <gtkmm.h>
#include <glibmm/i18n.h>
#include "Device.h"
#include "DriverConfiguration.h"
#include "ConfigurationLoader.h"

namespace DRI {
    class GUI {
    private:
        /* GUI-Related */
        Gtk::Window *pWindow;
        Gtk::AboutDialog aboutDialog;
        Gtk::MenuItem *pMenuAddApplication;
        Gtk::MenuItem *pMenuRemoveApplication;

        /* State-related */
        std::shared_ptr<DRI::Device> systemWideConfiguration;
        std::list<DRI::DriverConfiguration> driverConfiguration;
        std::list<std::shared_ptr<DRI::Device>> userDefinedConfiguration;
        Glib::ustring currentSelectedDriver;
        Glib::ustring currentSelectedApplication;
        std::shared_ptr<DRI::Application> currentApp;
        DRI::DriverConfiguration currentDriver;
        std::map<Glib::ustring, Gtk::ComboBoxText *> currentComboBoxes;
        std::map<Glib::ustring, Gtk::SpinButton *> currentSpinButtons;

        /* Helpers */
        Glib::RefPtr<Gtk::Builder> gladeBuilder;
        Glib::ustring locale;

        void setupLocale();

        void drawApplicationSelectionMenu();

        void drawApplicationOptions();

        void setupAboutDialog();

    public:
        GUI();

        virtual ~GUI();

        Gtk::Window *getWindowPointer();

        /* Signal Handlers */
        void onQuitPressed();

        void onSavePressed();

        void onApplicationSelected(Glib::ustring, Glib::ustring);

        void onCheckboxChanged(Glib::ustring);

        void onComboboxChanged(Glib::ustring);

        void onNumberEntryChanged(Glib::ustring);

        void onRemoveApplicationPressed();

        void onAddApplicationPressed();
    };
}

#endif
