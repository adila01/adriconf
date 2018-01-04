#include <iostream>
#include <fstream>
#include <boost/locale.hpp>
#include <libxml++/libxml++.h>
#include <gtkmm.h>
#include <glibmm/i18n.h>

#include "Section.h"
#include "Parser.h"
#include "ConfigurationResolver.h"
#include "DRIQuery.h"
#include "Writer.h"

Glib::ustring configureCurrentLocale();

Glib::ustring readSystemWideXML();

Glib::ustring readUserDefinedConfiguration();

int main(int argc, char *argv[]) {
    /* Load things and prepare the context to be used */
    Glib::ustring locale(configureCurrentLocale());

    DRI::DRIQuery query;
    auto driverSupportedOptions = query.queryDriverConfigurationOptions(locale);

    Glib::ustring systemWideXML(readSystemWideXML());

    std::list<DRI::Device> systemWideDevices = DRI::Parser::parseDevices(systemWideXML);
    DRI::Device systemWideDevice = systemWideDevices.front();

    Glib::ustring userXML(readUserDefinedConfiguration());
    std::list<DRI::Device> userDevices = DRI::Parser::parseDevices(userXML);

    /* Filter invalid options */
    userDevices = DRI::ConfigurationResolver::filterDriverUnsupportedOptions(driverSupportedOptions, userDevices);

    /* Start the GUI work */
    auto app = Gtk::Application::create(argc, argv, "br.com.jeanhertel.driconf");
    auto builder = Gtk::Builder::create();
    try {
        builder->add_from_file("DriConf.glade");
    }
    catch (const Glib::FileError &ex) {
        std::cerr << "FileError: " << ex.what() << std::endl;
        return 1;
    }
    catch (const Glib::MarkupError &ex) {
        std::cerr << "MarkupError: " << ex.what() << std::endl;
        return 1;
    }
    catch (const Gtk::BuilderError &ex) {
        std::cerr << "BuilderError: " << ex.what() << std::endl;
        return 1;
    }

    Gtk::Window *pWindow;
    builder->get_widget("mainwindow", pWindow);

    Gtk::ImageMenuItem *pQuitAction;
    builder->get_widget("quitAction", pQuitAction);
    if (pQuitAction) {
        pQuitAction->signal_activate().connect([&pWindow]() {
                pWindow->hide();
        });
    }

    Gtk::ImageMenuItem *pSaveAction;
    builder->get_widget("saveAction", pSaveAction);
    if (pSaveAction) {
        pSaveAction->signal_activate().connect([&systemWideDevice, &userDevices, &driverSupportedOptions]() {
            std::cout << _("Generating final XML for saving...") << std::endl;
            auto resolvedOptions = DRI::ConfigurationResolver::resolveOptionsForSave(
                    systemWideDevice, driverSupportedOptions, userDevices
            );
            auto rawXML = DRI::Writer::generateRawXml(resolvedOptions);
            std::cout << Glib::ustring::compose(_("Writing generated XML: %1"), rawXML) << std::endl;
            std::string userHome(std::getenv("HOME"));
            std::ofstream outFile(userHome + "/.drirc");
            outFile << rawXML;
            outFile.close();
        });
    }

    app->run(*pWindow);

    delete pWindow;

    return 0;
}

Glib::ustring configureCurrentLocale() {
    boost::locale::generator gen;
    std::locale l = gen("");
    std::locale::global(l);
    std::cout.imbue(l);
    Glib::ustring langCode(std::use_facet<boost::locale::info>(l).language());

    std::cout << Glib::ustring::compose(_("Current language code is %1"), langCode) << std::endl;

    return std::move(langCode);
}

Glib::ustring readSystemWideXML() {
    std::ostringstream buffer;
    std::ifstream input("/etc/drirc");
    buffer << input.rdbuf();
    Glib::ustring container(buffer.str());

    return std::move(container);
}

Glib::ustring readUserDefinedConfiguration() {
    Glib::ustring container;

    std::string userHome(std::getenv("HOME"));
    std::ifstream input(userHome + "/.drirc");

    if (!input.good()) {
        return container;
    }

    std::ostringstream buffer;
    buffer << input.rdbuf();

    container = buffer.str();

    return container;
}