#include "argument.h"
#include "log.h"
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QCoreApplication>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>

struct AasStorage
{
    bool parsed;

    // parameters
    bool verbose;
    QString backupDir;
    QString logfile;
    bool force;
    QString qtDir;
    QString newDir;
    QString compilerDir;
    QString oldCompilerDir;
    bool dryRun;
    QStringList unknownParameters;

    // config files
    QString crossMkspec;
    QString hostMkspec;
    QString qtVersion;
    QString buildDir;

    // detected from QMake
    QString oldDir;

    AasStorage()
        : parsed(false)
        , verbose(false)
        , force(false)
        , dryRun(false)
    {
    }
};

namespace {
AasStorage s;
}

bool ArgumentsAndSettings::parse()
{
    if (s.parsed)
        return false;

    s.parsed = true;

    QCommandLineParser parser;
    parser.setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);
    parser.setApplicationDescription(QString(QStringLiteral("Tool for patching paths in installed Qt library.\n"
                                                            "This is a reworked version of original QtBinPatcher.\n"
                                                            "Built using a %1 Qt " QT_VERSION_STR ".\n"
                                                            "Frank Su, 2019-2020. http://mogara.org\n\n"
                                                            "Thanks for Yuri V. Krugloff at http://www.tver-soft.org."))
#ifdef QT_STATIC
                                         .arg(QStringLiteral("staticly built")));
#else
                                         .arg(QStringLiteral("**DYNAMIC/SHARED BUILT**")));
#endif

    parser.addVersionOption();
    parser.addHelpOption();
    parser.addOption(QCommandLineOption({QStringLiteral("V"), QStringLiteral("verbose")}, QStringLiteral("Print extended runtime information.")));
    parser.addOption(QCommandLineOption({QStringLiteral("b"), QStringLiteral("backup")},
                                        QStringLiteral("If specified, the backup files made during patching will be saved to the specified path."
                                                       " If not, the backup files made during patching will be deleted if succeeded.\n"
                                                       "Note: the backup files will be restored if an error occurs."),
                                        QStringLiteral("path")));
    parser.addOption(
        QCommandLineOption({QStringLiteral("l"), QStringLiteral("logfile")}, QStringLiteral("Duplicate messages into logfile with name \"name\"."), QStringLiteral("name")));
    parser.addOption(QCommandLineOption({QStringLiteral("f"), QStringLiteral("force")}, QStringLiteral("Force patching (without old path actuality checking).")));
    parser.addOption(QCommandLineOption({QStringLiteral("q"), QStringLiteral("qt-dir")},
                                        QStringLiteral("Directory, where Qt or qmake is now located (may be relative).\n"
                                                       "If not specified, current directory will be used. Patcher will search qmake first in directory \"path\", "
                                                       "and then in its subdir \"bin\". Patcher is NEVER looking qmake in other directories.\n"
                                                       "WARNING: If nonstandard directory for binary files is used (not \"bin\"), select directory where located qmake."),
                                        QStringLiteral("path")));
    parser.addOption(QCommandLineOption({QStringLiteral("n"), QStringLiteral("new-dir")},
                                        QStringLiteral("Directory where Qt will be located (may be relative).\n"
                                                       "If not specified, current location will be used."),
                                        QStringLiteral("path")));
    parser.addOption(QCommandLineOption({QStringLiteral("d"), QStringLiteral("dry-run")}, QStringLiteral("Output the procedure only, do not really process the jobs.")));
    parser.addOption(QCommandLineOption({QStringLiteral("c"), QStringLiteral("compiler-dir")},
                                        QStringLiteral("Directory, where compiler is now located (may be relative).\n"
                                                       "If not specified, any patch to compiler dependencies will be applied. Only for Qt Kit version static."),
                                        QStringLiteral("path")));
    parser.addOption(QCommandLineOption({QStringLiteral("o"), QStringLiteral("old-compiler-dir")},
                                        QStringLiteral("Directory, where compiler was located (absolute path).\n"
                                                       "If not specified, any patch to compiler dependencies will be applied. Only for Qt Kit version static."),
                                        QStringLiteral("path")));

    parser.process(*qApp);
    if (parser.isSet(QStringLiteral("V")))
        s.verbose = true;
    if (parser.isSet(QStringLiteral("b")))
        s.backupDir = parser.value(QStringLiteral("b"));
    if (parser.isSet(QStringLiteral("l")))
        s.logfile = parser.value(QStringLiteral("l"));
    if (parser.isSet(QStringLiteral("f")))
        s.force = true;
    if (parser.isSet(QStringLiteral("q")))
        s.qtDir = parser.value(QStringLiteral("q"));
    if (parser.isSet(QStringLiteral("n")))
        s.newDir = parser.value(QStringLiteral("n"));
    if (parser.isSet(QStringLiteral("d")))
        s.dryRun = true;
    if (parser.isSet(QStringLiteral("c")))
        s.compilerDir = parser.value(QStringLiteral("c"));
    if (parser.isSet(QStringLiteral("o")))
        s.oldCompilerDir = parser.value(QStringLiteral("o"));
    s.unknownParameters = parser.unknownOptionNames() + parser.positionalArguments();

    QFile configFile(QStringLiteral("qbp.json"));
    if (configFile.exists() && configFile.open(QFile::ReadOnly | QFile::Text)) {
        QJsonParseError err;
        QJsonDocument doc = QJsonDocument::fromJson(configFile.readAll(), &err);
        if (err.error == QJsonParseError::NoError) {
            if (doc.isObject()) {
                QJsonObject ob = doc.object();
                if (ob.contains(QStringLiteral("crossMkspec")))
                    s.crossMkspec = ob.value(QStringLiteral("crossMkspec")).toString();
                if (ob.contains(QStringLiteral("hostMkspec")))
                    s.hostMkspec = ob.value(QStringLiteral("hostMkspec")).toString();
                if (ob.contains(QStringLiteral("qtVersion")))
                    s.qtVersion = ob.value(QStringLiteral("qtVersion")).toString();
                if (ob.contains(QStringLiteral("buildDir")))
                    s.buildDir = ob.value(QStringLiteral("buildDir")).toString();
            }
        }
    }

    return true;
}

bool ArgumentsAndSettings::verbose()
{
    return s.verbose;
}

QString ArgumentsAndSettings::backupDir()
{
    return s.backupDir;
}

QString ArgumentsAndSettings::logFile()
{
    return s.logfile;
}

bool ArgumentsAndSettings::force()
{
    return s.force;
}

QString ArgumentsAndSettings::qtDir()
{
    return s.qtDir;
}

QString ArgumentsAndSettings::newDir()
{
    return s.newDir;
}

QString ArgumentsAndSettings::compilerDir()
{
    return s.compilerDir;
}

QString ArgumentsAndSettings::oldCompilerDir()
{
    return s.oldCompilerDir;
}

bool ArgumentsAndSettings::dryRun()
{
    return s.dryRun;
}

QStringList ArgumentsAndSettings::unknownParameters()
{
    return s.unknownParameters;
}

QString ArgumentsAndSettings::crossMkspec()
{
    return s.crossMkspec;
}

QString ArgumentsAndSettings::hostMkspec()
{
    return s.hostMkspec;
}

QString ArgumentsAndSettings::qtVersion()
{
    return s.qtVersion;
}

QVersionNumber ArgumentsAndSettings::qtQVersion()
{
    return QVersionNumber::fromString(s.qtVersion);
}

QString ArgumentsAndSettings::buildDir()
{
    return s.buildDir;
}

QString ArgumentsAndSettings::oldDir()
{
    return s.oldDir;
}

void ArgumentsAndSettings::setQtDir(const QString &dir)
{
    QBPLOGV(QStringLiteral("qtDir is set to ") + dir);
    s.qtDir = dir;
}

void ArgumentsAndSettings::setNewDir(const QString &dir)
{
    QBPLOGV(QStringLiteral("newDir is set to ") + dir);
    s.newDir = dir;
}

void ArgumentsAndSettings::setCompilerDir(const QString &dir)
{
    QBPLOGV(QStringLiteral("compiler is set to ") + dir);
    s.compilerDir = dir;
}

void ArgumentsAndSettings::setOldCompilerDir(const QString &dir)
{
    QBPLOGV(QStringLiteral("old compiler is set to ") + dir);
    s.oldCompilerDir = dir;
}

void ArgumentsAndSettings::setCrossMkspec(const QString &mkspec)
{
    QBPLOGV(QStringLiteral("crossMkspec is set to ") + mkspec);
    s.crossMkspec = mkspec;
}

void ArgumentsAndSettings::setHostMkspec(const QString &mkspec)
{
    QBPLOGV(QStringLiteral("hostMkspec is set to ") + mkspec);
    s.hostMkspec = mkspec;
}

void ArgumentsAndSettings::setQtVersion(const QString &version)
{
    QBPLOGV(QStringLiteral("qtVersion is set to ") + version);
    s.qtVersion = version;
}

void ArgumentsAndSettings::setOldDir(const QString &dir)
{
    QBPLOGV(QStringLiteral("oldDir is set to ") + dir);
    s.oldDir = dir;
}
