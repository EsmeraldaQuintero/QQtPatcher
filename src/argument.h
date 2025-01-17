#ifndef QQBPARGUMENT_H
#define QQBPARGUMENT_H

#include <QString>
#include <QVersionNumber>

namespace ArgumentsAndSettings {

bool parse();

// parameters
bool verbose();
QString backupDir();
QString logFile();
bool force();
QString qtDir();
QString newDir();
QString compilerDir();
QString oldCompilerDir();
bool dryRun();
QStringList unknownParameters();

// config files
QString crossMkspec();
QString hostMkspec();
QString qtVersion();
QVersionNumber qtQVersion();
QString buildDir();

// detected from QMake
QString oldDir();

// helpers
void setQtDir(const QString &dir);
void setNewDir(const QString &dir);
void setCompilerDir(const QString &dir);
void setOldCompilerDir(const QString &dir);
void setCrossMkspec(const QString &mkspec);
void setHostMkspec(const QString &mkspec);
void setQtVersion(const QString &version);
void setOldDir(const QString &dir);

}

#endif
