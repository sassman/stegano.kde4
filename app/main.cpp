#include <KApplication>
#include <KAboutData>
#include <KCmdLineArgs>
#include <KLocale>
#include <KConfig>
#include "steganodialog.h"

using namespace Stegano;

int main (int argc, char *argv[]) {

    KAboutData aboutData(
        "stegano", 0,
        ki18n("Stegano for KDE4"), "0.1.2",
        ki18n("Steganographic tool"),
        KAboutData::License_GPL,
        ki18n("Copyright (c) 2012 Lubico Business")
    );
    KCmdLineArgs::init( argc, argv, &aboutData );
    KApplication app;
    
    // KConfig* cfg = app.sessionConfig();
    SteganoDialog* window = new SteganoDialog();
    window->show();

    return app.exec();
}
