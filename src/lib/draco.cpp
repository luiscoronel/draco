/*
#
# Draco Desktop Environment <https://dracolinux.org>
# Copyright (c) 2019, Ole-André Rodlie <ole.andre.rodlie@gmail.com>
# All rights reserved.
#
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU Lesser Public License as published by
* the Free Software Foundation; either version 2.1 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU Lesser Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>
#
*/

#include "draco.h"
#include "LuminaXDG.h"
#include "LUtils.h"
#include <QTextStream>
#include <QDateTime>

Draco::Draco() {}

Draco::~Draco() {}

const QString Draco::desktopSessionName()
{
    return DESKTOP_SERVICE_NAME;
}

const QString Draco::desktopSessionPath()
{
    return QString("/Desktop");
}

const QString Draco::desktopSessionFullPath()
{
    return QString("/%1").arg(DESKTOP_SERVICE_NAME).replace(".", "/");
}

const QString Draco::powerdSessionName()
{
    return POWERD_SERVICE_NAME;
}

const QString Draco::powerdSessionPath()
{
    return QString("/Powerd");
}

const QString Draco::powerdSessionFullPath()
{
    return QString("/%1").arg(POWERD_SERVICE_NAME).replace(".", "/");
}

const QString Draco::powerSessionName()
{
    return POWER_SERVICE_NAME;
}

const QString Draco::powerSessionPath()
{
    return QString("/Power");
}

const QString Draco::powerSessionFullPath()
{
    return QString("/%1").arg(POWER_SERVICE_NAME).replace(".", "/");
}

const QString Draco::xconfig()
{
    return QString("%1-settings-x11").arg(DESKTOP_APP);
}

const QString Draco::storageApp()
{
    return STORAGE_SERVICE_NAME;
}

const QString Draco::powerApp()
{
    return POWER_SERVICE_NAME;
}

const QString Draco::desktopApp()
{
    return DESKTOP_SERVICE_NAME;
}

const QString Draco::launcherApp()
{
    return LUtils::isValidBinaryWithPath(XDG_OPEN);
}

const QString Draco::terminalApp()
{
    QString term = LXDG::findDefaultAppForMime("application/terminal");
    if (term.endsWith(".desktop")) {
        XDGDesktop DF(term);
        if (DF.isValid()) { return DF.getDesktopExec(); }
    }
    if (LUtils::isValidBinary("qterminal")) { term = "qterminal"; }
    else if (LUtils::isValidBinary("lxterminal")) { term = "lxterminal"; }
    else if (LUtils::isValidBinary("konsole")) { term = "konsole"; }
    else if (LUtils::isValidBinary("gnome-terminal")) { term = "gnome-terminal"; }
    else if (LUtils::isValidBinary("xfce4-terminal")) { term = "xfce4-terminal"; }
    else if (LUtils::isValidBinary("rxvt")) { term = "rxvt"; }
    else if (LUtils::isValidBinary("xterm")) { term = "xterm"; }
    return term;
}

const QString Draco::configDir()
{
    QString path = QString("%1/.config/%2").arg(QDir::homePath()).arg(DESKTOP_APP);
    QDir dir(path);
    if (!dir.exists(path)) { dir.mkpath(path); }
    return path;
}

const QString Draco::sessionSettingsFile()
{
    QString file = QString("%1/%2.conf")
            .arg(configDir())
            .arg(DE_SESSION_SETTINGS);
    if (!QFile::exists(file)) {
        qDebug() << "no user session settings!";
        QFile mkfile(file);
        if (mkfile.open(QIODevice::WriteOnly)) { mkfile.close(); }
    }
    return file;
}

const QString Draco::desktopSettingsFile()
{
    QString file = QString("%1/%2.conf")
            .arg(configDir())
            .arg(DE_DESKTOP_SETTINGS);
    if (!QFile::exists(file)) {
        qDebug() << "no user desktop settings!";
        QFile mkfile(file);
        if (mkfile.open(QIODevice::WriteOnly)) { mkfile.close(); }
    }
    return file;
}

const QString Draco::envSettingsFile()
{
    QString file = QString("%1/%2.conf")
            .arg(configDir())
            .arg(DE_ENV_SETTINGS);
    if (!QFile::exists(file)) {
        qDebug() << "no user env settings!";
        QFile mkfile(file);
        if (mkfile.open(QIODevice::WriteOnly)) { mkfile.close(); }
    }
    return file;
}

const QString Draco::powerSettingsFile()
{
    QString file = QString("%1/power.conf")
            .arg(configDir());
    if (!QFile::exists(file)) {
        qDebug() << "no user power settings!";
        QFile mkfile(file);
        if (mkfile.open(QIODevice::WriteOnly)) { mkfile.close(); }
    }
    return file;
}

const QString Draco::storageSettingsFile()
{
    QString file = QString("%1/storage.conf")
            .arg(configDir());
    if (!QFile::exists(file)) {
        qDebug() << "no user storage settings!";
        QFile mkfile(file);
        if (mkfile.open(QIODevice::WriteOnly)) { mkfile.close(); }
    }
    return file;
}

const QString Draco::xconfigSettingsFile()
{
    QString file = QString("%1/monitors.conf")
            .arg(configDir());
    if (!QFile::exists(file)) {
        qDebug() << "no user monitors settings!";
        QFile mkfile(file);
        if (mkfile.open(QIODevice::WriteOnly)) { mkfile.close(); }
    }
    return file;
}

const QString Draco::keyboardSettingsFile()
{
    QString file = QString("%1/keyboard.conf")
            .arg(configDir());
    if (!QFile::exists(file)) {
        qDebug() << "no user keyboard settings!";
        QFile mkfile(file);
        if (mkfile.open(QIODevice::WriteOnly)) { mkfile.close(); }
    }
    return file;
}

const QString Draco::themeSettingsFile()
{
    themeEngineCheckConf();
    return QString("%1/.config/qt5ct/qt5ct.conf").arg(QDir::homePath());
}

QStringList Draco::iconLocations(const QString &appPath)
{
    QStringList result;
    result << QString("%1/.local/share/icons").arg(QDir::homePath());
    result << QString("%1/../share/icons").arg(appPath);
    result << "/usr/share/icons" << "/usr/local/share/icons";
    result << "/usr/pkg/share/icons";
    return result;
}

QStringList Draco::pixmapLocations(const QString &appPath)
{
    QStringList result;
    result << QString("%1/.local/share/pixmaps").arg(QDir::homePath());
    result << QString("%1/../share/pixmaps").arg(appPath);
    result << "/usr/share/pixmaps" << "/usr/local/share/pixmaps";
    result << "/usr/pkg/share/pixmaps";
    return result;
}

QStringList Draco::applicationLocations(const QString &appPath)
{
    QStringList result;
    result << QString("%1/.local/share/applications").arg(QDir::homePath());
    result << QString("%1/../share/applications").arg(appPath);
    result << "/usr/share/applications" << "/usr/local/share/applications";
    result << "/usr/pkg/share/applications";
    return result;
}

const QString Draco::xdgConfigHome()
{
    QString path = QString(getenv("XDG_CONFIG_HOME"));
    if (path.isEmpty()) { path = QString("%1/.config").arg(QDir::homePath()); }
    return path;
}

const QString Draco::xdgDataHome()
{
    QString path = QString(getenv("XDG_DATA_HOME"));
    if (path.isEmpty()) { path = QString("%1/.local/share").arg(QDir::homePath()); }
    return path;
}

bool Draco::xdgOpenCheck()
{
    // "replace" xdg-open with our own (since xdg-open does not support us yet)
    QString shadowPath =  QString("%1/bin").arg(configDir());
    QString shadowXDG = QString("%1/xdg-open").arg(shadowPath);
    if (!QFile::exists(shadowPath)) {
        QDir shadowDir(shadowPath);
        if (!shadowDir.mkpath(shadowPath)) {
            qWarning() << "Failed to create directory" << shadowPath;
            return false;
        }
    }
    QFileInfo info(shadowXDG);
    if (!info.isReadable()) {
        if (QFile::exists(shadowXDG) || info.isSymLink()) {
            if (!QFile::remove(shadowXDG)) {
                qWarning() << "Failed to remove existing symlink" << shadowXDG;
                return false;
            }
        }
        if (!QFile::link(launcherApp(), shadowXDG)) {
            qWarning() << "Failed to create symlink" << launcherApp() << shadowXDG;
            return false;
        }
    }
    return true;
}

// mimeapps.list is a mess, some apps use XDG_DATA_HOME/applications and some use XDG_CONFIG_HOME and some use both
// FORCE XDG_CONFIG_HOME/mimeapps.list as default mimeapps.list
void Draco::xdgMimeCheck()
{
    qDebug() << "check for mimeapps";
    QString configDir = xdgConfigHome();
    QString localDir = xdgDataHome();
    QString mimeApps = QString("%1/mimeapps.list").arg(configDir);
    QString legacyMimeApps = QString("%1/applications/mimeapps.list").arg(localDir);

    bool hasMimeApps = QFile::exists(mimeApps);
    bool hasLegacyMimeApps = QFile::exists(legacyMimeApps);

    if (!hasMimeApps) { // create a empty XDG_CONFIG_HOME/mimeapps.list
        qDebug() << "NEED TO CREATE" << mimeApps;
        if (!QFile::exists(configDir)) {
            qDebug() << "NEED TO CREATE" << configDir;
            QDir mkConfigDir(configDir);
            if (!mkConfigDir.mkpath(configDir)) {

                qWarning() << "FAILED TO CREATE" << configDir;
            }
        }
        QFile mkMimeApps(mimeApps);
        if (mkMimeApps.open(QIODevice::WriteOnly)) { mkMimeApps.close(); }
        else {
            qWarning() << "FAILED TO CREATE" << mimeApps;
        }
    }
    if (!hasLegacyMimeApps) { // symlink XDG_CONFIG_HOME/mimeapps.list to XDG_DATA_HOME/mimeapps.list
        qDebug() << "NEED TO CREATE" << legacyMimeApps;
        if (!QFile::exists(localDir)) {
            qDebug() << "NEED TO CREATE" << localDir;
            QDir mkLocalDir(localDir);
            if (!mkLocalDir.mkpath(localDir)) {
                qWarning() << "FAILED TO CREATE" << localDir;
            }
        }
        QFile mkMimeApps(mimeApps);
        if (!mkMimeApps.link(legacyMimeApps)) {
            qWarning() << "FAILED TO SYMLINK" << mimeApps << "TO" << legacyMimeApps;
        }
    }

    // check mimeapps.list
    QFileInfo mimeAppsInfo(mimeApps);
    if (mimeAppsInfo.isSymLink()) { // mimeapps.list should not be a shortcut!
        qDebug() << "MIMEAPPS.LIST IS A SYMLINK!" << mimeAppsInfo.symLinkTarget();
        QFile mimeFile(mimeApps);
        QByteArray mimeData;
        if (mimeFile.open(QIODevice::ReadOnly|QIODevice::Text)) {
            qDebug() << "READ MIMEAPPS.LIST BEFORE REMOVAL";
            mimeData = mimeFile.readAll();
            mimeFile.close();
        }
        if (mimeFile.remove()) {
            qDebug() << "REMOVED MIMEAPPS.LIST SYMLINK";
            if (mimeFile.open(QIODevice::WriteOnly)) {
                if (mimeData.size()>0) { mimeFile.write(mimeData); }
                mimeFile.close();
                qDebug() << "CREATED MIMEAPPS.LIST";
            }
        } else {
            qWarning() << "FAILED TO REMOVE MIMEAPPS.LIST SYMLINK";
        }
    }

    // check legacy mimeapps.list
    QFileInfo legacyMimeAppsInfo(legacyMimeApps);
    if (legacyMimeAppsInfo.isSymLink()) { // verify symlink
        if (legacyMimeAppsInfo.symLinkTarget() != mimeApps) {
            qDebug() << "LEGACY MIMEAPPS.LIST HAS WRONG SYMLINK PATH!" << legacyMimeAppsInfo.symLinkTarget();
            QFile mkMimeApps(mimeApps);
            if (!mkMimeApps.link(legacyMimeApps)) {
                qWarning() << "FAILED TO SYMLINK" << mimeApps << "TO" << legacyMimeApps;
            }
        }
    } else { // if file take backup and restore symlink
        qDebug() << "LEGACY MIMEAPPS.LIST IS NOT A SYMLINK!";
        QFile legacyMimeAppsFile(legacyMimeApps);
        if (legacyMimeAppsFile.copy(QString("%1.orig-%2").arg(legacyMimeApps).arg(QDateTime::currentDateTime().toMSecsSinceEpoch()))) {
            if (legacyMimeAppsFile.remove(legacyMimeApps)) {
                if (!legacyMimeAppsFile.link(mimeApps, legacyMimeApps)) {
                    qWarning() << "FAILED TO SYMLINK" << mimeApps << "TO" << legacyMimeApps;
                }
            }
        }
    }
}

const QString Draco::windowManager()
{
    QString wm = "openbox";
    return wm;
}

const QString Draco::windowManagerConf()
{
    QString conf = QString("%1/rc.xml").arg(configDir());
    if (!QFile::exists(conf)) {
        qDebug() << "RC XML MISSING!";
        QFile rc(conf);
        QFile def(":/theme/rc.xml");
        if (def.open(QIODevice::ReadOnly|QIODevice::Text)) {
            qDebug() << "OPEN FALLBACK RC XML";
            if (rc.open(QIODevice::WriteOnly|QIODevice::Text)) {
                rc.write(def.readAll());
                qDebug() << "WRITE NEW RC XML";
                rc.close();
            }
            def.close();
        }
    }
    return conf;
}

const QString Draco::windowManagerCmdStart()
{
    QString cmd = QString("%1 --config-file %2").arg(windowManager()).arg(windowManagerConf());
    return cmd;
}

const QString Draco::windowManagerCmdReConf()
{
    QString reconf = QString("%1 --reconfigure").arg(windowManagerCmdStart());
    return reconf;
}

const QString Draco::dracoStyleConf()
{
    QString style = QString("%1/draco.qss").arg(configDir());
    if (!QFile::exists(style)) { return ":/theme/draco.qss"; }
    return style;
}

const QString Draco::panelStyleSheet()
{
    QString result;
    QFile qss(dracoStyleConf());
    if (qss.open(QIODevice::ReadOnly|QIODevice::Text)) {
        result = qss.readAll();
        qss.close();
    }
    return result;
}

const QString Draco::desktopStyleSheet()
{
    QString result;
    QFile qss(dracoStyleConf());
    if (qss.open(QIODevice::ReadOnly|QIODevice::Text)) {
        result = qss.readAll();
        qss.close();
    }
    return result;
}

void Draco::themeEngineCheckConf(const QString &theme)
{
    QString conf = QString("%1/.config/qt5ct/qt5ct.conf").arg(QDir::homePath());
    if (!QFile::exists(conf)) {
        qDebug() << "QT5CT CONF MISSING!";
        QFile file(conf);
        QFile def(":/theme/qt5ct.conf");
        if (def.open(QIODevice::ReadOnly|QIODevice::Text)) {
            qDebug() << "OPEN FALLBACK QT5CT CONF";
            if (file.open(QIODevice::WriteOnly|QIODevice::Text)) {
                file.write(def.readAll());
                qDebug() << "WRITE NEW QT5CT CONF";
                file.close();
            }
            def.close();
        }
    }
    if (theme.isEmpty()) { return; }
}

void Draco::checkGtk2Conf(const QString &theme, QFont font)
{
    QString conf = QString("%1/.gtkrc-2.0").arg(QDir::homePath());
    if (!QFile::exists(conf)) {
        QFile file(conf);
        QFile def(":/theme/gtkrc-2.0");
        if (def.open(QIODevice::ReadOnly|QIODevice::Text)) {
            if (file.open(QIODevice::WriteOnly|QIODevice::Text)) {
                file.write(def.readAll());
                file.close();
            }
            def.close();
        }
    }
    if ((theme.isEmpty() || theme == "hicolor") && font.family().isEmpty()) { return; }
    QFile gtkconf(conf);
    if (!gtkconf.open(QIODevice::ReadOnly|QIODevice::Text)) { return; }
    QString data = gtkconf.readAll();
    gtkconf.close();
    QString foundTheme, foundFont;
    QString themeFont = QString("%1 %2").arg(font.family()).arg(font.pointSize());
    QStringList lines = data.split("\n", QString::SkipEmptyParts);
    for (int i=0;i<lines.size();++i) {
        QString line = lines.at(i);
        if (line.startsWith("gtk-icon-theme-name")) {
            foundTheme = line
                         .replace("gtk-icon-theme-name", "")
                         .replace("=", "")
                         .replace("\"", "")
                         .trimmed();
        }
        if (line.startsWith("gtk-font-name")) {
            foundFont = line
                        .replace("gtk-font-name", "")
                        .replace("=", "")
                        .replace("\"", "")
                        .trimmed();
        }
    }
    if ((foundTheme.toUpper() != theme.toUpper()) ||
        (foundFont.toUpper() != themeFont.toUpper())) {
        if (gtkconf.open(QIODevice::WriteOnly|QIODevice::Text|QIODevice::Truncate)) {
            bool foundIconLine = false,foundFontLine = false;
            for (int i=0;i<lines.size();++i) {
                QString line = lines.at(i);
                QTextStream stream(&gtkconf);
                if (line.startsWith("gtk-icon-theme-name")) {
                    foundIconLine = true;
                    stream << QString("gtk-icon-theme-name = \"%1\"").arg(theme) << endl;
                } else if (line.startsWith("gtk-font-name")) {
                    foundFontLine = true;
                    stream << QString("gtk-font-name = \"%1\"").arg(themeFont) << endl;
                } else { stream << line << endl; }
            }
            if (!foundIconLine || !foundFontLine) {
                QTextStream stream(&gtkconf);
                if (!foundIconLine) {
                    stream << QString("gtk-icon-theme-name = \"%1\"").arg(theme) << endl;
                }
                if (!foundFontLine) {
                    stream << QString("gtk-font-name = \"%1\"").arg(themeFont) << endl;
                }
            }
            gtkconf.close();
        }
    }
}

void Draco::checkGtk3Conf(const QString &theme, QFont font)
{
    QString gtkdir = QString("%1/.config/gtk-3.0").arg(QDir::homePath());
    QString conf = QString("%1/settings.ini").arg(gtkdir);
    if (!QFile::exists(conf)) {
        if (!QFile::exists(gtkdir)) {
            QDir dir(gtkdir);
            dir.mkpath(gtkdir);
        }
        QFile file(conf);
        QFile def(":/theme/settings.ini");
        if (def.open(QIODevice::ReadOnly|QIODevice::Text)) {
            if (file.open(QIODevice::WriteOnly|QIODevice::Text)) {
                file.write(def.readAll());
                file.close();
            }
            def.close();
        }
    }
    if ((theme.isEmpty() || theme == "hicolor") && font.family().isEmpty()) { return; }
    QSettings gtk3conf(conf, QSettings::IniFormat);
    gtk3conf.beginGroup("Settings");
    QString foundTheme = gtk3conf.value("gtk-icon-theme-name").toString();
    QString foundFont = gtk3conf.value("gtk-font-name").toString();
    QString themeFont = QString("%1 %2").arg(font.family()).arg(font.pointSize());
    if (theme.toUpper() != foundTheme.toUpper()) {
        gtk3conf.setValue("gtk-icon-theme-name", theme);
    }
    if (themeFont.toUpper() != foundFont.toUpper()) {
        gtk3conf.setValue("gtk-font-name", themeFont);
    }
    gtk3conf.endGroup();
}

void Draco::checkConfigs()
{
    xdgOpenCheck();
    xdgMimeCheck();
    themeEngineCheckConf();
    checkGtk2Conf();
    checkGtk3Conf();
}

const QString Draco::getProperMime(const QString &mime)
{
    QString result;
    if (mime.startsWith("image")) {
        result = "image-x-generic";
    } else if(mime.startsWith("audio")) {
        result = "audio-x-generic";
    } else if(mime.startsWith("video")) {
        result = "video-x-generic";
    } else if (mime.contains("-tar") ||
               mime.contains("compressed") ||
               mime.contains("xz") ||
               mime.contains("bz2") ||
               mime.contains("gz") ||
               mime.contains("rar") ||
               mime.contains("zip") ||
               mime.contains("rpm") ||
               mime.contains("deb")) {
        result = "package-x-generic";
    } else if (mime.endsWith("cd-image") ||
               mime.endsWith("apple-diskimage") ||
               mime.endsWith("disk-image") ||
               mime.endsWith("saturn-rom") ||
               mime.endsWith("wii-rom") ||
               mime.endsWith("gamecube-rom") ||
               mime.endsWith("appimage")) {
        result = "media-optical";
    } else if (mime.contains("office")) {
        if (mime.contains("document")) {
            result = "x-office-document";
        }
        else if (mime.contains("drawing")) {
            result = "x-office-drawing";
        }
        else if (mime.contains("presentation")) {
            result = "x-office-presentation";
        }
        else if (mime.contains("spreadsheet")) {
            result = "x-office-spreadsheet";
        }
    } else if (mime.startsWith("text")) {
        if (mime.contains("python") ||
                mime.contains("perl") ||
                mime.contains("php") ||
                mime.contains("ruby") ||
                mime.contains("script") ||
                mime.contains("shell"))
        {
            result = "text-x-script";
        } else if (mime.contains("html")) {
            result = "text-html";
        } else {
            result = "text-x-generic";
        }
    } else if (mime.endsWith("-executable")) {
        result = "application-x-executable";
    } else {
        result = "text-x-generic";
    }
    if (result.isEmpty()) { return mime; }
    return  result;
}

const QString Draco::filterIconName(const QString &name)
{
    QString result;
    if (name == "xscreensaver") { result = "preferences-desktop-screensaver"; }
    else if (name == "xine") { result = "applications-multimedia"; }
    else if (name == "cups") { result = "printer"; }
    else if (name == "/usr/share/icons/Humanity/devices/48/printer.svg") { result = "printer"; }
    else if (name == "QtIcon") { result = "qt-logo"; }
    else if (name.contains("distcc")) { result = "utilities-system-monitor"; }
    else if (name == "system-reboot") { result = "system-shutdown"; }
    else if (name == "system") { result = "computer"; }
    else if (name == "dialog-cancel") { result = "process-stop"; }
    else if (name == "dialog") { result = "user-available"; }
    else if (name == "preferences-desktop-icons") { result = "preferences-desktop-theme"; }
    else if (name == "transform") { result = "object-rotate-left"; }
    else if (name == "transform-scale") { result = "view-fullscreen"; }
    else if (name == "transform-move") { result = "view-restore"; }
    if (result.isEmpty()) { return name; }
    return result;
}

QVariant Draco::readSetting(QString conf, QString key, QVariant fallback)
{
    QSettings settings(conf, QSettings::IniFormat);
    return settings.value(key, fallback);
}

void Draco::writeSetting(QString conf, QString key, QVariant value)
{
    QSettings settings(conf, QSettings::IniFormat);
    settings.setValue(key, value);
}

bool Draco::isBlacklistedApplication(const QString &exec)
{
    QStringList blacklisted;
    blacklisted << "qtfm-tray" << "powerkit" << "xfce" << "xfdesktop";
    blacklisted << "thunar-settings" << "thunar-volman-settings" << "thunar-volman";
    blacklisted << "xfce4-power-manager" << "xfce4-power-manager-settings";
    blacklisted << "xfce4-appfinder" << "hp-uiscan" << LUtils::isValidBinaryWithPath("hp-uiscan");
    blacklisted << "obconf" << "obconf-qt";

    if (blacklisted.contains(exec)) { return true; }
    return false;
}

const QString Draco::getOSReleaseInfo(const QString &type)
{
    QString result;
    if (!QFile::exists("/etc/os-release") || type.isEmpty()) { return result; }
    QFile os_release("/etc/os-release");
    if (!os_release.open(QIODevice::ReadOnly|QIODevice::Text)) { return result; }
    QString tmp = os_release.readAll();
    os_release.close();
    QStringList lines = tmp.split("\n");
    for (int i=0;i<lines.size();++i) {
        QString line = lines.at(i);
        if (line.startsWith(QString("%1=").arg(type))) {
            result =  line.replace(QString("%1=").arg(type), "").replace("\"", "").trimmed();
            break;
        }
    }
    return result;
}

bool Draco::kernelCanResume()
{
    if (getOSReleaseInfo("ID") != "slackware") {
        // filter out distros that we know don't need this check
        // for now we only check for slackware.
        return true;
    }
    QFile cmdline("/proc/cmdline");
    if (cmdline.open(QIODevice::ReadOnly)) {
        QByteArray result = cmdline.readAll();
        cmdline.close();
        if (result.contains("resume=")) { return true; }
    }
    return false;
}
