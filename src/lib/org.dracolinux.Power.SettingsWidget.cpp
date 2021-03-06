/*
#
# Draco Desktop Environment <https://dracolinux.org>
# Copyright (c) 2019, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>
#
*/

#include "org.dracolinux.Power.SettingsWidget.h"
#include "org.dracolinux.Power.Settings.h"
#include "org.dracolinux.Powerd.Manager.Backlight.h"
#include "org.dracolinux.Power.Client.h"

#include "power_def.h"
#include "draco.h"

#include <QTimer>

#define MAX_WIDTH 150

PowerSettingsWidget::PowerSettingsWidget(QWidget *parent)
    : QWidget(parent)
    , dbus(nullptr)
    , lidActionBattery(nullptr)
    , lidActionAC(nullptr)
    , criticalActionBattery(nullptr)
    , criticalBattery(nullptr)
    , autoSleepBattery(nullptr)
    , autoSleepAC(nullptr)
    , desktopSS(nullptr)
    , desktopPM(nullptr)
    , showNotifications(nullptr)
    , showSystemTray(nullptr)
    , disableLidAction(nullptr)
    , autoSleepBatteryAction(nullptr)
    , autoSleepACAction(nullptr)
    , backlightSliderBattery(nullptr)
    , backlightSliderAC(nullptr)
    , backlightBatteryCheck(nullptr)
    , backlightACCheck(nullptr)
    , backlightBatteryLowerCheck(nullptr)
    , backlightACHigherCheck(nullptr)
    , warnOnLowBattery(nullptr)
    , warnOnVeryLowBattery(nullptr)
    , lidActionACLabel(nullptr)
    , lidActionBatteryLabel(nullptr)
    , batteryBacklightLabel(nullptr)
    , acBacklightLabel(nullptr)
    , backlightMouseWheel(nullptr)
    , suspendLockScreen(nullptr)
    , suspendBatteryWakeTimer(nullptr)
    , suspendBatteryWakeTimerLabel(nullptr)
    , suspendACWakeTimer(nullptr)
    , suspendACWakeTimerLabel(nullptr)
{
    // setup dbus
    QDBusConnection session = QDBusConnection::sessionBus();
    dbus = new QDBusInterface(Draco::powerSessionName(),
                              Draco::powerSessionPath(),
                              Draco::powerSessionName(),
                              session, this);
    if (!dbus->isValid()) {
        QMessageBox::warning(this,
                             tr("Power manager not running"),
                             tr("Power manager is not running, please make sure it's running before using settings."));
    } else {

        // trigger generation of conf if not exists
        PowerSettings::getConf();

        setupWidgets(); // setup widgets
        populate(); // populate boxes
        loadSettings(); // load settings

        // connect widgets
        connect(lidActionBattery, SIGNAL(currentIndexChanged(int)),
                this, SLOT(handleLidActionBattery(int)));
        connect(lidActionAC, SIGNAL(currentIndexChanged(int)),
                this, SLOT(handleLidActionAC(int)));
        connect(criticalActionBattery, SIGNAL(currentIndexChanged(int)),
                this, SLOT(handleCriticalAction(int)));
        connect(criticalBattery, SIGNAL(valueChanged(int)),
                this, SLOT(handleCriticalBattery(int)));
        connect(autoSleepBattery, SIGNAL(valueChanged(int)),
                this, SLOT(handleAutoSleepBattery(int)));
        connect(autoSleepAC, SIGNAL(valueChanged(int)),
                this, SLOT(handleAutoSleepAC(int)));
        connect(desktopSS, SIGNAL(toggled(bool)),
                this, SLOT(handleDesktopSS(bool)));
        connect(desktopPM, SIGNAL(toggled(bool)),
                this, SLOT(handleDesktopPM(bool)));
        connect(showNotifications, SIGNAL(toggled(bool)),
                this, SLOT(handleShowNotifications(bool)));
        connect(showSystemTray, SIGNAL(toggled(bool)),
                this, SLOT(handleShowSystemTray(bool)));
        connect(disableLidAction, SIGNAL(toggled(bool)),
                this, SLOT(handleDisableLidAction(bool)));
        connect(autoSleepBatteryAction, SIGNAL(currentIndexChanged(int)),
                this, SLOT(handleAutoSleepBatteryAction(int)));
        connect(autoSleepACAction, SIGNAL(currentIndexChanged(int)),
                this, SLOT(handleAutoSleepACAction(int)));
        connect(backlightBatteryCheck, SIGNAL(toggled(bool)),
                this, SLOT(handleBacklightBatteryCheck(bool)));
        connect(backlightACCheck, SIGNAL(toggled(bool)),
                this, SLOT(handleBacklightACCheck(bool)));
        connect(backlightSliderBattery, SIGNAL(valueChanged(int)),
                this, SLOT(handleBacklightBatterySlider(int)));
        connect(backlightSliderAC, SIGNAL(valueChanged(int)),
                this, SLOT(handleBacklightACSlider(int)));
        connect(backlightBatteryLowerCheck, SIGNAL(toggled(bool)),
                this, SLOT(handleBacklightBatteryCheckLower(bool)));
        connect(backlightACHigherCheck, SIGNAL(toggled(bool)),
                this, SLOT(handleBacklightACCheckHigher(bool)));
        connect(warnOnLowBattery, SIGNAL(toggled(bool)),
                this, SLOT(handleWarnOnLowBattery(bool)));
        connect(warnOnVeryLowBattery, SIGNAL(toggled(bool)),
                this, SLOT(handleWarnOnVeryLowBattery(bool)));
        connect(notifyOnBattery, SIGNAL(toggled(bool)),
                this, SLOT(handleNotifyBattery(bool)));
        connect(notifyOnAC, SIGNAL(toggled(bool)),
                this, SLOT(handleNotifyAC(bool)));
        connect(backlightMouseWheel, SIGNAL(toggled(bool)),
                this, SLOT(handleBacklightMouseWheel(bool)));
        connect(suspendLockScreen, SIGNAL(toggled(bool)),
                this, SLOT(handleSuspendLockScreen(bool)));
        connect(suspendBatteryWakeTimer, SIGNAL(valueChanged(int)),
                this, SLOT(handleSuspendWakeBatteryTimer(int)));
        connect(suspendACWakeTimer, SIGNAL(valueChanged(int)),
                this, SLOT(handleSuspendWakeACTimer(int)));
    }
}

PowerSettingsWidget::~PowerSettingsWidget()
{
    PowerSettings::setValue(CONF_DIALOG, saveGeometry());
}

void PowerSettingsWidget::setupWidgets()
{
    // setup widgets
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setMargin(5);
    layout->setSpacing(0);

    // battery
    QGroupBox *batteryContainer = new QGroupBox(this);
    batteryContainer->setTitle(tr("On Battery"));
    batteryContainer->setSizePolicy(QSizePolicy::Expanding,
                                    QSizePolicy::Expanding);
    QVBoxLayout *batteryContainerLayout = new QVBoxLayout(batteryContainer);
    batteryContainerLayout->setMargin(0);
    batteryContainerLayout->setSpacing(0);

    QWidget *lidActionBatteryContainer = new QWidget(this);
    QHBoxLayout *lidActionBatteryContainerLayout = new QHBoxLayout(lidActionBatteryContainer);
    lidActionBattery = new QComboBox(this);
    lidActionBattery->setMaximumWidth(MAX_WIDTH);
    lidActionBattery->setMinimumWidth(MAX_WIDTH);
    lidActionBatteryLabel = new QLabel(this);

    QLabel *lidActionBatteryIcon = new QLabel(this);
    lidActionBatteryIcon->setMaximumSize(48, 48);
    lidActionBatteryIcon->setMinimumSize(48, 48);
    lidActionBatteryIcon->setPixmap(QIcon::fromTheme(DEFAULT_VIDEO_ICON)
                                    .pixmap(QSize(48, 48)));
    lidActionBatteryLabel->setText(QString("<h3 style=\"font-weight:normal;\">%1</h3>")
                                   .arg(tr("Lid action")));
    lidActionBatteryLabel->setToolTip(tr("What to do when the lid is closed and running on battery."));
    lidActionBatteryContainerLayout->addWidget(lidActionBatteryIcon);
    lidActionBatteryContainerLayout->addWidget(lidActionBatteryLabel);
    lidActionBatteryContainerLayout->addStretch();
    lidActionBatteryContainerLayout->addWidget(lidActionBattery);
    batteryContainerLayout->addWidget(lidActionBatteryContainer);

    QWidget *criticalBatteryContainer = new QWidget(this);
    QHBoxLayout *criticalBatteryContainerLayout = new QHBoxLayout(criticalBatteryContainer);
    criticalBattery = new QSpinBox(this);
    criticalBattery->setMaximumWidth(MAX_WIDTH);
    criticalBattery->setMinimumWidth(MAX_WIDTH);
    criticalBattery->setMinimum(0);
    criticalBattery->setMaximum(99);
    criticalBattery->setSuffix(tr(" %"));
    QLabel *criticalBatteryLabel = new QLabel(this);

    QWidget *criticalActionBatteryContainer = new QWidget(this);
    criticalActionBatteryContainer->setContentsMargins(0, 0, 0, 0);
    QVBoxLayout *criticalActionBatteryContainerLayout = new QVBoxLayout(criticalActionBatteryContainer);
    criticalActionBatteryContainerLayout->setMargin(0);
    criticalActionBatteryContainerLayout->setSpacing(0);
    criticalActionBattery = new QComboBox(this);
    criticalActionBattery->setMaximumWidth(MAX_WIDTH);
    criticalActionBattery->setMinimumWidth(MAX_WIDTH);
    criticalActionBatteryContainerLayout->addWidget(criticalBattery);
    criticalActionBatteryContainerLayout->addWidget(criticalActionBattery);

    QLabel *criticalBatteryIcon = new QLabel(this);
    criticalBatteryIcon->setMaximumSize(48, 48);
    criticalBatteryIcon->setMinimumSize(48, 48);
    criticalBatteryIcon->setPixmap(QIcon::fromTheme(DEFAULT_BATTERY_ICON_CRIT)
                                   .pixmap(QSize(48, 48)));
    criticalBatteryLabel->setText(QString("<h3 style=\"font-weight:normal;\">%1</h3>")
                                  .arg(tr("Critical battery")));
    criticalBatteryLabel->setToolTip(tr("What to do when your battery is critical."));
    criticalBatteryContainerLayout->addWidget(criticalBatteryIcon);
    criticalBatteryContainerLayout->addWidget(criticalBatteryLabel);
    criticalBatteryContainerLayout->addStretch();
    criticalBatteryContainerLayout->addWidget(criticalActionBatteryContainer);

    QWidget *sleepBatteryContainer = new QWidget(this);
    QHBoxLayout *sleepBatteryContainerLayout = new QHBoxLayout(sleepBatteryContainer);
    autoSleepBattery = new QSpinBox(this);
    autoSleepBattery->setMaximumWidth(MAX_WIDTH);
    autoSleepBattery->setMinimumWidth(MAX_WIDTH);
    autoSleepBattery->setMinimum(0);
    autoSleepBattery->setMaximum(1000);
    autoSleepBattery->setSuffix(QString(" %1").arg(tr("min")));
    QLabel *sleepBatteryLabel = new QLabel(this);

    QWidget *sleepActionBatteryContainer = new QWidget(this);
    sleepActionBatteryContainer->setContentsMargins(0, 0, 0, 0);
    QVBoxLayout *sleepActionBatteryContainerLayout = new QVBoxLayout(sleepActionBatteryContainer);
    sleepActionBatteryContainerLayout->setMargin(0);
    sleepActionBatteryContainerLayout->setSpacing(0);
    autoSleepBatteryAction = new QComboBox(this);
    autoSleepBatteryAction->setMaximumWidth(MAX_WIDTH);
    autoSleepBatteryAction->setMinimumWidth(MAX_WIDTH);
    sleepActionBatteryContainerLayout->addWidget(autoSleepBattery);
    sleepActionBatteryContainerLayout->addWidget(autoSleepBatteryAction);

    QLabel *sleepBatteryIcon = new QLabel(this);
    sleepBatteryIcon->setMaximumSize(48, 48);
    sleepBatteryIcon->setMinimumSize(48, 48);
    sleepBatteryIcon->setPixmap(QIcon::fromTheme(DEFAULT_SUSPEND_ICON)
                                .pixmap(QSize(48, 48)));
    sleepBatteryLabel->setText(QString("<h3 style=\"font-weight:normal;\">%1</h3>")
                               .arg(tr("Suspend after")));
    sleepBatteryLabel->setToolTip(tr("Enable automatically suspend when on battery."));
    sleepBatteryContainerLayout->addWidget(sleepBatteryIcon);
    sleepBatteryContainerLayout->addWidget(sleepBatteryLabel);
    sleepBatteryContainerLayout->addStretch();
    sleepBatteryContainerLayout->addWidget(sleepActionBatteryContainer);

    // wake on battery
    suspendBatteryWakeTimer = new QSpinBox(this);
    suspendBatteryWakeTimer->setMaximumWidth(MAX_WIDTH);
    suspendBatteryWakeTimer->setMinimumWidth(MAX_WIDTH);
    suspendBatteryWakeTimer->setMinimum(0);
    suspendBatteryWakeTimer->setMaximum(1000);
    suspendBatteryWakeTimer->setSuffix(QString(" %1").arg(tr("min")));
    suspendBatteryWakeTimerLabel = new QLabel(this);
    suspendBatteryWakeTimerLabel->setMaximumWidth(MAX_WIDTH);
    suspendBatteryWakeTimerLabel->setWordWrap(true);
    suspendBatteryWakeTimerLabel->setText(tr("Wake device during suspend to <b>hibernate</b> after"));
    sleepActionBatteryContainerLayout->addSpacing(5);
    sleepActionBatteryContainerLayout->addWidget(suspendBatteryWakeTimerLabel);
    sleepActionBatteryContainerLayout->addWidget(suspendBatteryWakeTimer);

    // backlight battery
    backlightSliderBattery = new QSlider(this);
    backlightSliderBattery->setOrientation(Qt::Horizontal);
    backlightSliderBattery->setMinimum(1);
    backlightSliderBattery->setMaximum(1);
    backlightSliderBattery->setValue(0);
    backlightSliderBattery->setMaximumWidth(MAX_WIDTH);

    backlightBatteryCheck = new QCheckBox(this);
    backlightBatteryCheck->setCheckable(true);
    backlightBatteryCheck->setChecked(false);
    backlightBatteryCheck->setToolTip(tr("Enable/Disable brightness override on battery."));
    backlightBatteryCheck->setText(QString(" ")); // qt ui bug workaround

    backlightBatteryLowerCheck = new QCheckBox(this);
    backlightBatteryLowerCheck->setCheckable(true);
    backlightBatteryLowerCheck->setChecked(false);
    backlightBatteryLowerCheck->setText(tr("Don't apply if lower"));
    backlightBatteryLowerCheck->setToolTip(tr("If your current brightness value is lower"
                                              " do not apply a brightness override (on battery)."));

    QWidget *batteryBacklightOptContainer = new QWidget(this);
    QVBoxLayout *batteryBacklightOptContainerLayout = new QVBoxLayout(batteryBacklightOptContainer);
    batteryBacklightOptContainer->setContentsMargins(0, 0, 0, 0);
    batteryBacklightOptContainer->setMaximumWidth(MAX_WIDTH);
    batteryBacklightOptContainerLayout->setMargin(0);
    batteryBacklightOptContainerLayout->setContentsMargins(0, 0, 0, 0);
    batteryBacklightOptContainerLayout->addWidget(backlightSliderBattery);
    batteryBacklightOptContainerLayout->addWidget(backlightBatteryLowerCheck);

    QWidget *batteryBacklightContainer = new QWidget(this);
    QHBoxLayout *batteryBacklightContainerLayout = new QHBoxLayout(batteryBacklightContainer);
    batteryBacklightLabel = new QLabel(this);
    QLabel *batteryBacklightIcon = new QLabel(this);

    batteryBacklightIcon->setMaximumSize(48, 48);
    batteryBacklightIcon->setMinimumSize(48, 48);
    batteryBacklightIcon->setPixmap(QIcon::fromTheme(DEFAULT_BACKLIGHT_ICON)
                                .pixmap(QSize(48, 48)));
    batteryBacklightLabel->setText(QString("<h3 style=\"font-weight:normal;\">%1</h3>")
                                   .arg(tr("Brightness")));
    batteryBacklightLabel->setToolTip(tr("Override brightness when switched to battery power."));
    batteryBacklightContainerLayout->addWidget(batteryBacklightIcon);
    batteryBacklightContainerLayout->addWidget(batteryBacklightLabel);
    batteryBacklightContainerLayout->addWidget(backlightBatteryCheck);
    batteryBacklightContainerLayout->addStretch();
    batteryBacklightContainerLayout->addWidget(batteryBacklightOptContainer);

    // add battery widgets to container
    batteryContainerLayout->addWidget(sleepBatteryContainer);
    batteryContainerLayout->addWidget(criticalBatteryContainer);
    batteryContainerLayout->addWidget(batteryBacklightContainer);
    batteryContainerLayout->addStretch();

    // AC
    QGroupBox *acContainer = new QGroupBox(this);
    acContainer->setTitle(tr("On AC"));
    QVBoxLayout *acContainerLayout = new QVBoxLayout(acContainer);
    acContainerLayout->setMargin(0);
    acContainerLayout->setSpacing(0);

    QWidget *lidActionACContainer = new QWidget(this);
    QHBoxLayout *lidActionACContainerLayout = new QHBoxLayout(lidActionACContainer);

    lidActionAC = new QComboBox(this);
    lidActionAC->setMaximumWidth(MAX_WIDTH);
    lidActionAC->setMinimumWidth(MAX_WIDTH);
    lidActionACLabel = new QLabel(this);

    QLabel *lidActionACIcon = new QLabel(this);
    lidActionACIcon->setMaximumSize(48, 48);
    lidActionACIcon->setMinimumSize(48, 48);
    lidActionACIcon->setPixmap(QIcon::fromTheme(DEFAULT_VIDEO_ICON)
                               .pixmap(QSize(48, 48)));
    lidActionACLabel->setText(QString("<h3 style=\"font-weight:normal;\">%1</h3>")
                              .arg(tr("Lid action")));
    lidActionACLabel->setToolTip(tr("What to do when the lid is closed and running on AC."));
    lidActionACContainerLayout->addWidget(lidActionACIcon);
    lidActionACContainerLayout->addWidget(lidActionACLabel);
    lidActionACContainerLayout->addStretch();
    lidActionACContainerLayout->addWidget(lidActionAC);
    acContainerLayout->addWidget(lidActionACContainer);

    QWidget *sleepACContainer = new QWidget(this);
    QHBoxLayout *sleepACContainerLayout = new QHBoxLayout(sleepACContainer);
    autoSleepAC = new QSpinBox(this);
    autoSleepAC->setMaximumWidth(MAX_WIDTH);
    autoSleepAC->setMinimumWidth(MAX_WIDTH);
    autoSleepAC->setMinimum(0);
    autoSleepAC->setMaximum(1000);
    autoSleepAC->setSuffix(QString(" %1").arg(tr("min")));
    QLabel *sleepACLabel = new QLabel(this);

    sleepACContainerLayout->addWidget(sleepACLabel);
    acContainerLayout->addWidget(sleepACContainer);

    QWidget *sleepActionACContainer = new QWidget(this);
    sleepActionACContainer->setContentsMargins(0, 0, 0, 0);
    QVBoxLayout *sleepActionACContainerLayout = new QVBoxLayout(sleepActionACContainer);
    sleepActionACContainerLayout->setMargin(0);
    sleepActionACContainerLayout->setSpacing(0);
    autoSleepACAction = new QComboBox(this);
    autoSleepACAction->setMaximumWidth(MAX_WIDTH);
    autoSleepACAction->setMinimumWidth(MAX_WIDTH);
    sleepActionACContainerLayout->addWidget(autoSleepAC);
    sleepActionACContainerLayout->addWidget(autoSleepACAction);

    QLabel *sleepACIcon = new QLabel(this);
    sleepACIcon->setMaximumSize(48, 48);
    sleepACIcon->setMinimumSize(48, 48);
    sleepACIcon->setPixmap(QIcon::fromTheme(DEFAULT_SUSPEND_ICON)
                           .pixmap(QSize(48, 48)));
    sleepACLabel->setText(QString("<h3 style=\"font-weight:normal;\">%1</h3>")
                          .arg(tr("Suspend after")));
    sleepACLabel->setToolTip(tr("Enable automatically suspend when on AC."));
    sleepACContainerLayout->addWidget(sleepACIcon);
    sleepACContainerLayout->addWidget(sleepACLabel);
    sleepACContainerLayout->addStretch();
    sleepACContainerLayout->addWidget(sleepActionACContainer);

    // wake on ac
    suspendACWakeTimer = new QSpinBox(this);
    suspendACWakeTimer->setMaximumWidth(MAX_WIDTH);
    suspendACWakeTimer->setMinimumWidth(MAX_WIDTH);
    suspendACWakeTimer->setMinimum(0);
    suspendACWakeTimer->setMaximum(1000);
    suspendACWakeTimer->setSuffix(QString(" %1").arg(tr("min")));
    suspendACWakeTimerLabel = new QLabel(this);
    suspendACWakeTimerLabel->setMaximumWidth(MAX_WIDTH);
    suspendACWakeTimerLabel->setWordWrap(true);
    suspendACWakeTimerLabel->setText(tr("Wake device during suspend to <b>hibernate</b> after"));
    sleepActionACContainerLayout->addSpacing(5);
    sleepActionACContainerLayout->addWidget(suspendACWakeTimerLabel);
    sleepActionACContainerLayout->addWidget(suspendACWakeTimer);

    // backlight ac
    backlightSliderAC = new QSlider(this);
    backlightSliderAC->setOrientation(Qt::Horizontal);
    backlightSliderAC->setMinimum(1);
    backlightSliderAC->setMaximum(1);
    backlightSliderAC->setValue(0);
    backlightSliderAC->setMaximumWidth(MAX_WIDTH);

    backlightACCheck = new QCheckBox(this);
    backlightACCheck->setCheckable(true);
    backlightACCheck->setChecked(false);
    backlightACCheck->setToolTip(tr("Enable/Disable brightness override on AC."));
    backlightACCheck->setText(QString(" ")); // qt ui bug workaround

    backlightACHigherCheck = new QCheckBox(this);
    backlightACHigherCheck->setCheckable(true);
    backlightACHigherCheck->setChecked(false);
    backlightACHigherCheck->setText(tr("Don't apply if higher"));
    backlightACHigherCheck->setToolTip(tr("If your current brightness value is higher"
                                          " do not apply a brightness override (on AC)."));

    QWidget *acBacklightOptContainer = new QWidget(this);
    QVBoxLayout *acBacklightOptContainerLayout = new QVBoxLayout(acBacklightOptContainer);
    acBacklightOptContainer->setContentsMargins(0, 0, 0, 0);
    acBacklightOptContainer->setMaximumWidth(MAX_WIDTH);
    acBacklightOptContainerLayout->setMargin(0);
    acBacklightOptContainerLayout->setContentsMargins(0, 0, 0, 0);
    acBacklightOptContainerLayout->addWidget(backlightSliderAC);
    acBacklightOptContainerLayout->addWidget(backlightACHigherCheck);

    QWidget *acBacklightContainer = new QWidget(this);
    QHBoxLayout *acBacklightContainerLayout = new QHBoxLayout(acBacklightContainer);
    acBacklightLabel = new QLabel(this);
    QLabel *acBacklightIcon = new QLabel(this);

    acBacklightIcon->setMaximumSize(48, 48);
    acBacklightIcon->setMinimumSize(48, 48);
    acBacklightIcon->setPixmap(QIcon::fromTheme(DEFAULT_BACKLIGHT_ICON)
                                .pixmap(QSize(48, 48)));
    acBacklightLabel->setText(QString("<h3 style=\"font-weight:normal;\">%1</h3>")
                              .arg(tr("Brightness")));
    acBacklightLabel->setToolTip(tr("Override brightness when switched to AC power."));
    acBacklightContainerLayout->addWidget(acBacklightIcon);
    acBacklightContainerLayout->addWidget(acBacklightLabel);
    acBacklightContainerLayout->addWidget(backlightACCheck);
    acBacklightContainerLayout->addStretch();
    acBacklightContainerLayout->addWidget(acBacklightOptContainer);

    // add widgets to ac
    acContainerLayout->addWidget(sleepACContainer);
    acContainerLayout->addWidget(acBacklightContainer);
    acContainerLayout->addStretch();

    // advanced
    QGroupBox *advContainer = new QGroupBox(this);
    advContainer->setTitle(tr("Services"));
    QVBoxLayout *advContainerLayout = new QVBoxLayout(advContainer);

    desktopSS = new QCheckBox(this);
    desktopSS->setIcon(QIcon::fromTheme(DEFAULT_VIDEO_ICON));
    desktopSS->setText("org.freedesktop.ScreenSaver");
    desktopSS->setToolTip(tr("Enable/Disable the screen saver D-Bus service."
                             " Needed for applications to inhibit the running screen saver."));

    desktopPM = new QCheckBox(this);
    desktopPM->setIcon(QIcon::fromTheme(DEFAULT_BATTERY_ICON));
    desktopPM->setText("org.freedesktop.PowerManagement");
    desktopPM->setToolTip(tr("Enable/Disable the power management D-Bus service."
                             " Needed for applications to inhibit auto suspend action."));

    // add widgets to advanced
    advContainerLayout->addWidget(desktopSS);
    advContainerLayout->addWidget(desktopPM);
    advContainerLayout->addStretch();

    // common
    QGroupBox *daemonContainer = new QGroupBox(this);
    daemonContainer->setTitle(tr("Common"));
    QVBoxLayout *daemonContainerLayout = new QVBoxLayout(daemonContainer);

    showSystemTray  = new QCheckBox(this);
    showSystemTray->setIcon(QIcon::fromTheme(DEFAULT_TRAY_ICON));
    showSystemTray->setText(tr("Show system tray"));
    showSystemTray->setToolTip(tr("Enable/Disable the system tray icon."
                                  " Note that notifications will not work when the systemtray is disabled."));

    showNotifications = new QCheckBox(this);
    showNotifications->setIcon(QIcon::fromTheme(DEFAULT_NOTIFY_ICON));
    showNotifications->setText(tr("Show notifications"));
    showNotifications->setToolTip(tr("Show notifications for power related events."));

    disableLidAction = new QCheckBox(this);
    disableLidAction->setIcon(QIcon::fromTheme(DEFAULT_VIDEO_ICON));
    disableLidAction->setText(tr("Disable lid action if external"
                                 "\nmonitor(s) is connected"));
    disableLidAction->setToolTip(tr("Disable lid action if an external monitor is connected"
                                    " to your laptop."));

    backlightMouseWheel = new QCheckBox(this);
    backlightMouseWheel->setIcon(QIcon::fromTheme(DEFAULT_TRAY_ICON));
    backlightMouseWheel->setText(tr("Adjust backlight in system tray"));
    backlightMouseWheel->setToolTip(tr("Adjust the display backlight with the mouse wheel on the system tray icon."));

    suspendLockScreen = new QCheckBox(this);
    suspendLockScreen->setIcon(QIcon::fromTheme(DEFAULT_LOCK_ICON));
    suspendLockScreen->setText(tr("Lock screen on suspend"));
    suspendLockScreen->setToolTip(tr("Lock the screen before suspending the computer"));

    daemonContainerLayout->addWidget(suspendLockScreen);
    daemonContainerLayout->addWidget(showSystemTray);
    daemonContainerLayout->addWidget(showNotifications);
    daemonContainerLayout->addWidget(disableLidAction);
    daemonContainerLayout->addWidget(backlightMouseWheel);
    //daemonContainerLayout->addWidget(bypassKernel);

    // screensaver
    /*QGroupBox *ssContainer = new QGroupBox(this);
    ssContainer->setTitle(tr("Screensaver"));
    QVBoxLayout *ssContainerLayout = new QVBoxLayout(ssContainer);*/



    /*resumeLockScreen = new QCheckBox(this);
    resumeLockScreen->setIcon(QIcon::fromTheme(DEFAULT_LOCK_ICON));
    resumeLockScreen->setText(tr("Lock screen on resume"));
    resumeLockScreen->setToolTip(tr("Lock the screen before resuming the computer."));*/

    //ssContainerLayout->addWidget(suspendLockScreen);
    //ssContainerLayout->addWidget(resumeLockScreen);

    // notify
    QGroupBox *notifyContainer = new QGroupBox(this);
    notifyContainer->setTitle(tr("Notifications"));
    QVBoxLayout *notifyContainerLayout = new QVBoxLayout(notifyContainer);

    warnOnLowBattery = new QCheckBox(this);
    warnOnLowBattery->setIcon(QIcon::fromTheme(DEFAULT_NOTIFY_ICON));
    warnOnLowBattery->setText(tr("Notify on low battery"));
    warnOnLowBattery->setToolTip(tr("Show a notification when on low battery (%1% over critical)")
                                 .arg(LOW_BATTERY));

    warnOnVeryLowBattery = new QCheckBox(this);
    warnOnVeryLowBattery->setIcon(QIcon::fromTheme(DEFAULT_NOTIFY_ICON));
    warnOnVeryLowBattery->setText(tr("Notify on very low battery"));
    warnOnVeryLowBattery->setToolTip(tr("Show a notification when on very low battery (1% over critical)"));

    notifyOnBattery = new QCheckBox(this);
    notifyOnBattery->setIcon(QIcon::fromTheme(DEFAULT_NOTIFY_ICON));
    notifyOnBattery->setText(tr("Notify on battery"));
    notifyOnBattery->setToolTip(tr("Notify when switched on battery power"));

    notifyOnAC = new QCheckBox(this);
    notifyOnAC->setIcon(QIcon::fromTheme(DEFAULT_NOTIFY_ICON));
    notifyOnAC->setText(tr("Notify on AC"));
    notifyOnAC->setToolTip(tr("Notify when switched on AC power"));

    notifyContainerLayout->addWidget(warnOnLowBattery);
    notifyContainerLayout->addWidget(warnOnVeryLowBattery);
    notifyContainerLayout->addWidget(notifyOnBattery);
    notifyContainerLayout->addWidget(notifyOnAC);

    QWidget *settingsWidget = new QWidget(this);
    QVBoxLayout *settingsLayout = new QVBoxLayout(settingsWidget);
    QScrollArea *settingsContainerArea = new QScrollArea(this);
    settingsContainerArea->setSizePolicy(QSizePolicy::Expanding,
                                         QSizePolicy::Expanding);
    settingsContainerArea->setStyleSheet("QScrollArea {border:0;}");
    settingsContainerArea->setWidgetResizable(true);
    settingsContainerArea->setWidget(settingsWidget);

    // add widgets to settings
    settingsLayout->addWidget(batteryContainer);
    settingsLayout->addWidget(acContainer);
    settingsLayout->addWidget(daemonContainer);
    //settingsLayout->addWidget(ssContainer);
    settingsLayout->addWidget(notifyContainer);
    settingsLayout->addWidget(advContainer);
    settingsLayout->addStretch();

    layout->addWidget(settingsContainerArea);
}

// populate widgets with default values
void PowerSettingsWidget::populate()
{
    lidActionBattery->clear();
    lidActionBattery->addItem(QIcon::fromTheme(DEFAULT_NONE_ICON),
                              tr("None"), lidNone);
    lidActionBattery->addItem(QIcon::fromTheme(DEFAULT_LOCK_ICON),
                              tr("Lock Screen"), lidLock);
    lidActionBattery->addItem(QIcon::fromTheme(DEFAULT_SUSPEND_ICON),
                              tr("Sleep"), lidSleep);
    lidActionBattery->addItem(QIcon::fromTheme(DEFAULT_HIBERNATE_ICON),
                              tr("Hibernate"), lidHibernate);
    lidActionBattery->addItem(QIcon::fromTheme(DEFAULT_SHUTDOWN_ICON),
                              tr("Shutdown"), lidShutdown);
    lidActionBattery->addItem(QIcon::fromTheme(DEFAULT_SUSPEND_ICON),
                              tr("Hybrid Sleep"), lidHybridSleep);

    lidActionAC->clear();
    lidActionAC->addItem(QIcon::fromTheme(DEFAULT_NONE_ICON),
                         tr("None"), lidNone);
    lidActionAC->addItem(QIcon::fromTheme(DEFAULT_LOCK_ICON),
                         tr("Lock Screen"), lidLock);
    lidActionAC->addItem(QIcon::fromTheme(DEFAULT_SUSPEND_ICON),
                         tr("Sleep"), lidSleep);
    lidActionAC->addItem(QIcon::fromTheme(DEFAULT_HIBERNATE_ICON),
                         tr("Hibernate"), lidHibernate);
    lidActionAC->addItem(QIcon::fromTheme(DEFAULT_SHUTDOWN_ICON),
                         tr("Shutdown"), lidShutdown);
    lidActionAC->addItem(QIcon::fromTheme(DEFAULT_SUSPEND_ICON),
                         tr("Hybrid Sleep"), lidHybridSleep);

    criticalActionBattery->clear();
    criticalActionBattery->addItem(QIcon::fromTheme(DEFAULT_NONE_ICON),
                                   tr("None"), criticalNone);
    criticalActionBattery->addItem(QIcon::fromTheme(DEFAULT_HIBERNATE_ICON),
                                   tr("Hibernate"), criticalHibernate);
    criticalActionBattery->addItem(QIcon::fromTheme(DEFAULT_SHUTDOWN_ICON),
                                   tr("Shutdown"), criticalShutdown);

    autoSleepBatteryAction->clear();
    autoSleepBatteryAction->addItem(QIcon::fromTheme(DEFAULT_NONE_ICON),
                                    tr("None"), suspendNone);
    autoSleepBatteryAction->addItem(QIcon::fromTheme(DEFAULT_SUSPEND_ICON),
                                    tr("Sleep"), suspendSleep);
    autoSleepBatteryAction->addItem(QIcon::fromTheme(DEFAULT_HIBERNATE_ICON),
                                    tr("Hibernate"), suspendHibernate);
    autoSleepBatteryAction->addItem(QIcon::fromTheme(DEFAULT_SHUTDOWN_ICON),
                                    tr("Shutdown"), suspendShutdown);
    autoSleepBatteryAction->addItem(QIcon::fromTheme(DEFAULT_SUSPEND_ICON),
                                    tr("Hybrid Sleep"), suspendHybrid);

    autoSleepACAction->clear();
    autoSleepACAction->addItem(QIcon::fromTheme(DEFAULT_NONE_ICON),
                               tr("None"), suspendNone);
    autoSleepACAction->addItem(QIcon::fromTheme(DEFAULT_SUSPEND_ICON),
                               tr("Sleep"), suspendSleep);
    autoSleepACAction->addItem(QIcon::fromTheme(DEFAULT_HIBERNATE_ICON),
                               tr("Hibernate"), suspendHibernate);
    autoSleepACAction->addItem(QIcon::fromTheme(DEFAULT_SHUTDOWN_ICON),
                               tr("Shutdown"), suspendShutdown);
    autoSleepACAction->addItem(QIcon::fromTheme(DEFAULT_SUSPEND_ICON),
                               tr("Hybrid Sleep"), suspendHybrid);
}

// load settings and set defaults
void PowerSettingsWidget::loadSettings()
{
    if (PowerSettings::isValid(CONF_DIALOG_GEOMETRY)) {
        restoreGeometry(PowerSettings::getValue(CONF_DIALOG_GEOMETRY).toByteArray());
    }

    int defaultAutoSleepBattery = AUTO_SLEEP_BATTERY;
    if (PowerSettings::isValid(CONF_SUSPEND_BATTERY_TIMEOUT)) {
        defaultAutoSleepBattery = PowerSettings::getValue(CONF_SUSPEND_BATTERY_TIMEOUT).toInt();
    }
    setDefaultAction(autoSleepBattery, defaultAutoSleepBattery);

    int defaultAutoSleepBatteryAction = DEFAULT_SUSPEND_BATTERY_ACTION;
    if (PowerSettings::isValid(CONF_SUSPEND_BATTERY_ACTION)) {
        defaultAutoSleepBatteryAction = PowerSettings::getValue(CONF_SUSPEND_BATTERY_ACTION).toInt();
    }
    setDefaultAction(autoSleepBatteryAction, defaultAutoSleepBatteryAction);

    int defaultAutoSleepAC = 0;
    if (PowerSettings::isValid(CONF_SUSPEND_AC_TIMEOUT)) {
        defaultAutoSleepAC = PowerSettings::getValue(CONF_SUSPEND_AC_TIMEOUT).toInt();
    }
    setDefaultAction(autoSleepAC, defaultAutoSleepAC);

    int defaultAutoSleepACAction = DEFAULT_SUSPEND_AC_ACTION;
    if (PowerSettings::isValid(CONF_SUSPEND_AC_ACTION)) {
        defaultAutoSleepACAction = PowerSettings::getValue(CONF_SUSPEND_AC_ACTION).toInt();
    }
    setDefaultAction(autoSleepACAction, defaultAutoSleepACAction);

    int defaultCriticalBattery = CRITICAL_BATTERY;
    if (PowerSettings::isValid(CONF_CRITICAL_BATTERY_TIMEOUT)) {
        defaultCriticalBattery = PowerSettings::getValue(CONF_CRITICAL_BATTERY_TIMEOUT).toInt();
    }
    setDefaultAction(criticalBattery, defaultCriticalBattery);

    int defaultLidActionBattery = LID_BATTERY_DEFAULT;
    if (PowerSettings::isValid(CONF_LID_BATTERY_ACTION)) {
        defaultLidActionBattery = PowerSettings::getValue(CONF_LID_BATTERY_ACTION).toInt();
    }
    setDefaultAction(lidActionBattery, defaultLidActionBattery);

    int defaultLidActionAC = LID_AC_DEFAULT;
    if (PowerSettings::isValid(CONF_LID_AC_ACTION)) {
        defaultLidActionAC = PowerSettings::getValue(CONF_LID_AC_ACTION).toInt();
    }
    setDefaultAction(lidActionAC, defaultLidActionAC);

    int defaultCriticalAction = CRITICAL_DEFAULT;
    if (PowerSettings::isValid(CONF_CRITICAL_BATTERY_ACTION)) {
        defaultCriticalAction = PowerSettings::getValue(CONF_CRITICAL_BATTERY_ACTION).toInt();
    }
    setDefaultAction(criticalActionBattery, defaultCriticalAction);

    bool defaultDesktopSS = true;
    if (PowerSettings::isValid(CONF_FREEDESKTOP_SS)) {
        defaultDesktopSS = PowerSettings::getValue(CONF_FREEDESKTOP_SS).toBool();
    }
    desktopSS->setChecked(defaultDesktopSS);

    bool defaultDesktopPM = true;
    if (PowerSettings::isValid(CONF_FREEDESKTOP_PM)) {
        defaultDesktopPM = PowerSettings::getValue(CONF_FREEDESKTOP_PM).toBool();
    }
    desktopPM->setChecked(defaultDesktopPM);

    bool defaultShowNotifications = true;
    if (PowerSettings::isValid(CONF_TRAY_NOTIFY)) {
        defaultShowNotifications = PowerSettings::getValue(CONF_TRAY_NOTIFY).toBool();
    }
    showNotifications->setChecked(defaultShowNotifications);

    bool defaultShowTray = true;
    if (PowerSettings::isValid(CONF_TRAY_SHOW)) {
        defaultShowTray = PowerSettings::getValue(CONF_TRAY_SHOW).toBool();
    }
    showSystemTray->setChecked(defaultShowTray);

    bool defaultDisableLidAction = true;
    if (PowerSettings::isValid(CONF_LID_DISABLE_IF_EXTERNAL)) {
        defaultDisableLidAction = PowerSettings::getValue(CONF_LID_DISABLE_IF_EXTERNAL).toBool();
    }
    disableLidAction->setChecked(defaultDisableLidAction);

    bool defaultWarnOnLowBattery = true;
    if (PowerSettings::isValid(CONF_WARN_ON_LOW_BATTERY)) {
        defaultWarnOnLowBattery = PowerSettings::getValue(CONF_WARN_ON_LOW_BATTERY).toBool();
    }
    warnOnLowBattery->setChecked(defaultWarnOnLowBattery);

    bool defaultWarnOnVeryLowBattery = true;
    if (PowerSettings::isValid(CONF_WARN_ON_VERYLOW_BATTERY)) {
        defaultWarnOnVeryLowBattery = PowerSettings::getValue(CONF_WARN_ON_VERYLOW_BATTERY).toBool();
    }
    warnOnVeryLowBattery->setChecked(defaultWarnOnVeryLowBattery);

    bool defaultNotifyOnBattery = true;
    if (PowerSettings::isValid(CONF_NOTIFY_ON_BATTERY)) {
        defaultNotifyOnBattery = PowerSettings::getValue(CONF_NOTIFY_ON_BATTERY).toBool();
    }
    notifyOnBattery->setChecked(defaultNotifyOnBattery);

    bool defaultNotifyOnAC = true;
    if (PowerSettings::isValid(CONF_NOTIFY_ON_AC)) {
        defaultNotifyOnAC = PowerSettings::getValue(CONF_NOTIFY_ON_AC).toBool();
    }
    notifyOnAC->setChecked(defaultNotifyOnAC);

    bool defaultSuspendLockScreen = true;
    if (PowerSettings::isValid(CONF_SUSPEND_LOCK_SCREEN)) {
        defaultSuspendLockScreen = PowerSettings::getValue(CONF_SUSPEND_LOCK_SCREEN).toBool();
    }
    suspendLockScreen->setChecked(defaultSuspendLockScreen);

    int defaultSuspendBatteryWakeTimer = 0;
    if (PowerSettings::isValid(CONF_SUSPEND_WAKEUP_HIBERNATE_BATTERY)) {
        defaultSuspendBatteryWakeTimer = PowerSettings::getValue(CONF_SUSPEND_WAKEUP_HIBERNATE_BATTERY).toInt();
    }
    setDefaultAction(suspendBatteryWakeTimer, defaultSuspendBatteryWakeTimer);

    int defaultSuspendACWakeTimer = 0;
    if (PowerSettings::isValid(CONF_SUSPEND_WAKEUP_HIBERNATE_AC)) {
        defaultSuspendACWakeTimer = PowerSettings::getValue(CONF_SUSPEND_WAKEUP_HIBERNATE_AC).toInt();
    }
    setDefaultAction(suspendACWakeTimer, defaultSuspendACWakeTimer);

    // check
    checkPerms();

    // backlight
    backlightDevice = PowerBacklight::getDevice();

    backlightSliderAC->setEnabled(true);
    backlightSliderBattery->setEnabled(true);

    int backlightMin = 1;
    int backlightMax = PowerBacklight::getMaxBrightness(backlightDevice);

    backlightSliderBattery->setMinimum(backlightMin);
    backlightSliderBattery->setMaximum(backlightMax);
    backlightSliderBattery->setValue(backlightSliderBattery->maximum());
    backlightSliderAC->setMinimum(backlightMin);
    backlightSliderAC->setMaximum(backlightMax);
    backlightSliderAC->setValue(backlightSliderAC->maximum());

    backlightBatteryCheck->setChecked(PowerSettings::getValue(CONF_BACKLIGHT_BATTERY_ENABLE)
                                      .toBool());
    backlightACCheck->setChecked(PowerSettings::getValue(CONF_BACKLIGHT_AC_ENABLE)
                                 .toBool());
    if (PowerSettings::isValid(CONF_BACKLIGHT_BATTERY)) {
        backlightSliderBattery->setValue(PowerSettings::getValue(CONF_BACKLIGHT_BATTERY)
                                         .toInt());
    }
    if (PowerSettings::isValid(CONF_BACKLIGHT_AC)) {
        backlightSliderAC->setValue(PowerSettings::getValue(CONF_BACKLIGHT_AC)
                                    .toInt());
    }
    if (PowerSettings::isValid(CONF_BACKLIGHT_BATTERY_DISABLE_IF_LOWER)) {
        backlightBatteryLowerCheck->setChecked(
                    PowerSettings::getValue(CONF_BACKLIGHT_BATTERY_DISABLE_IF_LOWER)
                    .toBool());
    }
    if (PowerSettings::isValid(CONF_BACKLIGHT_AC_DISABLE_IF_HIGHER)) {
        backlightACHigherCheck->setChecked(
                    PowerSettings::getValue(CONF_BACKLIGHT_AC_DISABLE_IF_HIGHER)
                    .toBool());
    }
    bool defaultBacklightMouseWheel = true;
    if (PowerSettings::isValid(CONF_BACKLIGHT_MOUSE_WHEEL)) {
        defaultBacklightMouseWheel = PowerSettings::getValue(CONF_BACKLIGHT_MOUSE_WHEEL).toBool();
    }
    backlightMouseWheel->setChecked(defaultBacklightMouseWheel);

    enableBacklight(backlightDevice.isEmpty()?false:true);
    enableLid(PowerClient::lidIsPresent(dbus));
    enableBattery(PowerClient::hasBattery(dbus));
}

void PowerSettingsWidget::saveSettings()
{
    PowerSettings::setValue(CONF_LID_BATTERY_ACTION,
                              lidActionBattery->currentIndex());
    PowerSettings::setValue(CONF_LID_AC_ACTION,
                              lidActionAC->currentIndex());
    PowerSettings::setValue(CONF_CRITICAL_BATTERY_ACTION,
                              criticalActionBattery->currentIndex());
    PowerSettings::setValue(CONF_CRITICAL_BATTERY_TIMEOUT,
                              criticalBattery->value());
    PowerSettings::setValue(CONF_SUSPEND_BATTERY_TIMEOUT,
                              autoSleepBattery->value());
    PowerSettings::setValue(CONF_SUSPEND_AC_TIMEOUT,
                              autoSleepAC->value());
    PowerSettings::setValue(CONF_FREEDESKTOP_SS,
                              desktopSS->isChecked());
    PowerSettings::setValue(CONF_FREEDESKTOP_PM,
                              desktopPM->isChecked());
    PowerSettings::setValue(CONF_TRAY_NOTIFY,
                              showNotifications->isChecked());
    PowerSettings::setValue(CONF_TRAY_SHOW,
                              showSystemTray->isChecked());
    PowerSettings::setValue(CONF_LID_DISABLE_IF_EXTERNAL,
                              disableLidAction->isChecked());
    PowerSettings::setValue(CONF_SUSPEND_BATTERY_ACTION,
                              autoSleepBatteryAction->currentIndex());
    PowerSettings::setValue(CONF_SUSPEND_AC_ACTION,
                              autoSleepACAction->currentIndex());
    PowerSettings::setValue(CONF_BACKLIGHT_BATTERY_ENABLE,
                              backlightBatteryCheck->isChecked());
    PowerSettings::setValue(CONF_BACKLIGHT_AC_ENABLE,
                              backlightACCheck->isChecked());
    PowerSettings::setValue(CONF_BACKLIGHT_BATTERY,
                              backlightSliderBattery->value());
    PowerSettings::setValue(CONF_BACKLIGHT_AC,
                              backlightSliderAC->value());
    PowerSettings::setValue(CONF_BACKLIGHT_BATTERY_DISABLE_IF_LOWER,
                              backlightBatteryLowerCheck->isChecked());
    PowerSettings::setValue(CONF_BACKLIGHT_AC_DISABLE_IF_HIGHER,
                              backlightACHigherCheck->isChecked());
    PowerSettings::setValue(CONF_DIALOG,
                              saveGeometry());
    PowerSettings::setValue(CONF_WARN_ON_LOW_BATTERY,
                              warnOnLowBattery->isChecked());
    PowerSettings::setValue(CONF_WARN_ON_VERYLOW_BATTERY,
                              warnOnVeryLowBattery->isChecked());
    PowerSettings::setValue(CONF_NOTIFY_ON_BATTERY,
                              notifyOnBattery->isChecked());
    PowerSettings::setValue(CONF_NOTIFY_ON_AC,
                              notifyOnAC->isChecked());
    PowerSettings::setValue(CONF_BACKLIGHT_MOUSE_WHEEL,
                              backlightMouseWheel->isChecked());
    PowerSettings::setValue(CONF_SUSPEND_LOCK_SCREEN,
                              suspendLockScreen->isChecked());
    PowerSettings::setValue(CONF_SUSPEND_WAKEUP_HIBERNATE_BATTERY,
                            suspendBatteryWakeTimer->value());
    PowerSettings::setValue(CONF_SUSPEND_WAKEUP_HIBERNATE_AC,
                            suspendACWakeTimer->value());
}

// set default action in combobox
void PowerSettingsWidget::setDefaultAction(QComboBox *box, int action)
{
    for (int i=0;i<box->count();i++) {
        if (box->itemData(i).toInt() == action) {
            box->setCurrentIndex(i);
            return;
        }
    }
}

// set default value in spinbox
void PowerSettingsWidget::setDefaultAction(QSpinBox *box, int action)
{
    box->setValue(action);
}

// set default value in combobox
void PowerSettingsWidget::setDefaultAction(QComboBox *box, QString value)
{
    for (int i=0;i<box->count();i++) {
        if (box->itemText(i) == value) {
            box->setCurrentIndex(i);
            return;
        }
    }
}

// save current value and update power manager
void PowerSettingsWidget::handleLidActionBattery(int index)
{
    checkPerms();
    PowerSettings::setValue(CONF_LID_BATTERY_ACTION, index);
}

void PowerSettingsWidget::handleLidActionAC(int index)
{
    checkPerms();
    PowerSettings::setValue(CONF_LID_AC_ACTION, index);
}

void PowerSettingsWidget::handleCriticalAction(int index)
{
    checkPerms();
    PowerSettings::setValue(CONF_CRITICAL_BATTERY_ACTION, index);
}

void PowerSettingsWidget::handleCriticalBattery(int value)
{
    PowerSettings::setValue(CONF_CRITICAL_BATTERY_TIMEOUT, value);
}

void PowerSettingsWidget::handleAutoSleepBattery(int value)
{
    PowerSettings::setValue(CONF_SUSPEND_BATTERY_TIMEOUT, value);
 }

void PowerSettingsWidget::handleAutoSleepAC(int value)
{
    PowerSettings::setValue(CONF_SUSPEND_AC_TIMEOUT, value);
}

void PowerSettingsWidget::handleDesktopSS(bool triggered)
{
    PowerSettings::setValue(CONF_FREEDESKTOP_SS, triggered);
    QMessageBox::information(this, tr("Restart required"),
                             tr("You must restart powerkit to apply this setting"));
    // TODO: add restart now?
}

void PowerSettingsWidget::handleDesktopPM(bool triggered)
{
    PowerSettings::setValue(CONF_FREEDESKTOP_PM, triggered);
    QMessageBox::information(this, tr("Restart required"),
                             tr("You must restart powerkit to apply this setting"));
    // TODO: add restart now?
}

void PowerSettingsWidget::handleShowNotifications(bool triggered)
{
    PowerSettings::setValue(CONF_TRAY_NOTIFY, triggered);
}

void PowerSettingsWidget::handleShowSystemTray(bool triggered)
{
    PowerSettings::setValue(CONF_TRAY_SHOW, triggered);
}

void PowerSettingsWidget::handleDisableLidAction(bool triggered)
{
    PowerSettings::setValue(CONF_LID_DISABLE_IF_EXTERNAL, triggered);
}

void PowerSettingsWidget::handleAutoSleepBatteryAction(int index)
{
    checkPerms();
    PowerSettings::setValue(CONF_SUSPEND_BATTERY_ACTION, index);
}

void PowerSettingsWidget::handleAutoSleepACAction(int index)
{
    checkPerms();
    PowerSettings::setValue(CONF_SUSPEND_AC_ACTION, index);
}

void PowerSettingsWidget::checkPerms()
{
    bool weCanHibernate = PowerClient::canHibernate(dbus);
    suspendACWakeTimer->setEnabled(weCanHibernate);
    suspendACWakeTimerLabel->setEnabled(weCanHibernate);
    suspendBatteryWakeTimer->setEnabled(weCanHibernate);
    suspendBatteryWakeTimerLabel->setEnabled(weCanHibernate);

    if (!weCanHibernate) {
        bool warnCantHibernate = false;
        if (criticalActionBattery->currentIndex() == criticalHibernate) {
            warnCantHibernate = true;
            criticalActionBattery->setCurrentIndex(criticalShutdown);
            handleCriticalAction(criticalShutdown);
        }
        if (lidActionAC->currentIndex() == lidHibernate ||
            lidActionAC->currentIndex() == lidHybridSleep) {
            warnCantHibernate = true;
            lidActionAC->setCurrentIndex(lidSleep);
            handleLidActionAC(lidSleep);
        }
        if (lidActionBattery->currentIndex() == lidHibernate ||
            lidActionBattery->currentIndex() == lidHybridSleep) {
            warnCantHibernate = true;
            lidActionBattery->setCurrentIndex(lidSleep);
            handleLidActionBattery(lidSleep);
        }
        if (autoSleepACAction->currentIndex() == suspendHibernate ||
            autoSleepACAction->currentIndex() == suspendHybrid) {
            warnCantHibernate = true;
            autoSleepACAction->setCurrentIndex(suspendSleep);
            handleAutoSleepACAction(suspendSleep);
        }
        if (autoSleepBatteryAction->currentIndex() == suspendHibernate ||
            autoSleepBatteryAction->currentIndex() == suspendHybrid) {
            warnCantHibernate = true;
            autoSleepBatteryAction->setCurrentIndex(suspendSleep);
            handleAutoSleepBatteryAction(suspendSleep);
        }
        if (warnCantHibernate) { hibernateWarn(); }
    }
    if (!PowerClient::canSuspend(dbus)) {
        bool warnCantSleep = false;
        if (lidActionAC->currentIndex() == lidSleep) {
            warnCantSleep = true;
            lidActionAC->setCurrentIndex(lidLock);
            handleLidActionAC(lidLock);
        }
        if (lidActionBattery->currentIndex() == lidSleep) {
            warnCantSleep = true;
            lidActionBattery->setCurrentIndex(lidLock);
            handleLidActionBattery(lidLock);
        }
        if (autoSleepACAction->currentIndex() == suspendSleep) {
            warnCantSleep = true;
            autoSleepACAction->setCurrentIndex(suspendNone);
            handleAutoSleepACAction(suspendNone);
        }
        if (autoSleepBatteryAction->currentIndex() == suspendSleep) {
            warnCantSleep = true;
            autoSleepBatteryAction->setCurrentIndex(suspendNone);
            handleAutoSleepBatteryAction(suspendNone);
        }
        if (warnCantSleep) { sleepWarn(); }
    }
}

void PowerSettingsWidget::handleBacklightBatteryCheck(bool triggered)
{
    PowerSettings::setValue(CONF_BACKLIGHT_BATTERY_ENABLE, triggered);
}

void PowerSettingsWidget::handleBacklightACCheck(bool triggered)
{
    PowerSettings::setValue(CONF_BACKLIGHT_AC_ENABLE, triggered);
}

void PowerSettingsWidget::handleBacklightBatterySlider(int value)
{
    PowerSettings::setValue(CONF_BACKLIGHT_BATTERY, value);
}

void PowerSettingsWidget::handleBacklightACSlider(int value)
{
    PowerSettings::setValue(CONF_BACKLIGHT_AC, value);
}

void PowerSettingsWidget::hibernateWarn()
{
    QMessageBox::warning(this, tr("Hibernate not supported"),
                         tr("Hibernate not supported, consult your OS documentation."));
}

void PowerSettingsWidget::sleepWarn()
{
    QMessageBox::warning(this, tr("Sleep not supported"),
                         tr("Sleep not supported, consult your OS documentation."));
}

void PowerSettingsWidget::handleBacklightBatteryCheckLower(bool triggered)
{
    PowerSettings::setValue(CONF_BACKLIGHT_BATTERY_DISABLE_IF_LOWER, triggered);
}

void PowerSettingsWidget::handleBacklightACCheckHigher(bool triggered)
{
    PowerSettings::setValue(CONF_BACKLIGHT_AC_DISABLE_IF_HIGHER, triggered);
}

void PowerSettingsWidget::enableBacklight(bool enabled)
{
    backlightSliderBattery->setEnabled(enabled);
    backlightSliderAC->setEnabled(enabled);
    backlightBatteryCheck->setEnabled(enabled);
    backlightACCheck->setEnabled(enabled);
    backlightBatteryLowerCheck->setEnabled(enabled);
    backlightACHigherCheck->setEnabled(enabled);
    batteryBacklightLabel->setEnabled(enabled);
    acBacklightLabel->setEnabled(enabled);
    backlightMouseWheel->setEnabled(enabled);
}

void PowerSettingsWidget::handleWarnOnLowBattery(bool triggered)
{
    PowerSettings::setValue(CONF_WARN_ON_LOW_BATTERY, triggered);
}

void PowerSettingsWidget::handleWarnOnVeryLowBattery(bool triggered)
{
    PowerSettings::setValue(CONF_WARN_ON_VERYLOW_BATTERY, triggered);
}

void PowerSettingsWidget::handleNotifyBattery(bool triggered)
{
    PowerSettings::setValue(CONF_NOTIFY_ON_BATTERY, triggered);
}

void PowerSettingsWidget::handleNotifyAC(bool triggered)
{
    PowerSettings::setValue(CONF_NOTIFY_ON_AC, triggered);
}

void PowerSettingsWidget::enableLid(bool enabled)
{
    lidActionAC->setEnabled(enabled);
    lidActionBattery->setEnabled(enabled);
    lidActionACLabel->setEnabled(enabled);
    lidActionBatteryLabel->setEnabled(enabled);
    disableLidAction->setEnabled(enabled);
}

void PowerSettingsWidget::handleBacklightMouseWheel(bool triggered)
{
    PowerSettings::setValue(CONF_BACKLIGHT_MOUSE_WHEEL, triggered);
}

void PowerSettingsWidget::handleSuspendLockScreen(bool triggered)
{
    PowerSettings::setValue(CONF_SUSPEND_LOCK_SCREEN, triggered);
}

void PowerSettingsWidget::enableBattery(bool enabled)
{
    lidActionBattery->setEnabled(enabled);
    lidActionBatteryLabel->setEnabled(enabled);

    criticalBattery->setEnabled(enabled);
    criticalActionBattery->setEnabled(enabled);

    autoSleepBattery->setEnabled(enabled);
    autoSleepBatteryAction->setEnabled(enabled);

    backlightSliderBattery->setEnabled(enabled);
    backlightBatteryCheck->setEnabled(enabled);
    backlightBatteryLowerCheck->setEnabled(enabled);
    batteryBacklightLabel->setEnabled(enabled);

    suspendBatteryWakeTimer->setEnabled(enabled);
    suspendBatteryWakeTimerLabel->setEnabled(enabled);
}

void PowerSettingsWidget::handleSuspendWakeBatteryTimer(int value)
{
    PowerSettings::setValue(CONF_SUSPEND_WAKEUP_HIBERNATE_BATTERY, value);
}

void PowerSettingsWidget::handleSuspendWakeACTimer(int value)
{
    PowerSettings::setValue(CONF_SUSPEND_WAKEUP_HIBERNATE_AC, value);
}
