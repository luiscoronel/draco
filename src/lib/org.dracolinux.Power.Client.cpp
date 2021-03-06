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

#include "org.dracolinux.Power.Client.h"
#include <QDebug>

double PowerClient::getBatteryLeft(QDBusInterface *iface)
{
    qDebug() << "check for battery left";
    if (!iface->isValid()) { return false; }
    QDBusMessage reply = iface->call("BatteryLeft");
    double ok = reply.arguments().last().toDouble();
    qDebug() << "we have battery left" << ok;
    return  ok;
}

bool PowerClient::hasBattery(QDBusInterface *iface)
{
    qDebug() << "check if we have any battery";
    if (!iface->isValid()) { return false; }
    QDBusMessage reply = iface->call("HasBattery");
    bool ok = reply.arguments().last().toBool();
    qDebug() << "we have any battery?" << ok;
    return  ok;
}

bool PowerClient::onBattery(QDBusInterface *iface)
{
    qDebug() << "check if we are on battery";
    if (!iface->isValid()) { return false; }
    QDBusMessage reply = iface->call("OnBattery");
    bool ok = reply.arguments().last().toBool();
    qDebug() << "we are on battery?" << ok;
    return  ok;
}

qlonglong PowerClient::timeToEmpty(QDBusInterface *iface)
{
    qDebug() << "check for time to empty";
    if (!iface->isValid()) { return false; }
    QDBusMessage reply = iface->call("TimeToEmpty");
    qlonglong ok = reply.arguments().last().toLongLong();
    qDebug() << "we have time to empty?" << ok;
    return  ok;
}

bool PowerClient::canHibernate(QDBusInterface *iface)
{
    qDebug() << "check if we can hibernate";
    if (!iface->isValid()) { return false; }
    QDBusMessage reply = iface->call("CanHibernate");
    bool ok = reply.arguments().last().toBool();
    qDebug() << "we can hibernate?" << ok;
    return  ok;
}

bool PowerClient::canSuspend(QDBusInterface *iface)
{
    qDebug() << "check if we can suspend";
    if (!iface->isValid()) { return false; }
    QDBusMessage reply = iface->call("CanSuspend");
    bool ok = reply.arguments().last().toBool();
    qDebug() << "we can suspend?" << ok;
    return  ok;
}

bool PowerClient::canRestart(QDBusInterface *iface)
{
    qDebug() << "check if we can restart";
    if (!iface->isValid()) { return false; }
    QDBusMessage reply = iface->call("CanRestart");
    bool ok = reply.arguments().last().toBool();
    qDebug() << "we can restart?" << ok;
    return  ok;
}

bool PowerClient::canPowerOff(QDBusInterface *iface)
{
    qDebug() << "check if we can poweroff";
    if (!iface->isValid()) { return false; }
    QDBusMessage reply = iface->call("CanPowerOff");
    bool ok = reply.arguments().last().toBool();
    qDebug() << "we can poweroff?" << ok;
    return  ok;
}

bool PowerClient::lidIsPresent(QDBusInterface *iface)
{
    qDebug() << "check if we have a lid";
    if (!iface->isValid()) { return false; }
    QDBusMessage reply = iface->call("LidIsPresent");
    bool ok = reply.arguments().last().toBool();
    qDebug() << "we have a lid?" << ok;
    return  ok;
}

void PowerClient::lockScreen(QDBusInterface *iface)
{
    qDebug() << "lock screen";
    if (!iface->isValid()) { return; }
    QDBusMessage reply = iface->call("LockScreen");
    bool ok = reply.errorMessage().isEmpty();
    qDebug() << "reply" << ok;
}

void PowerClient::hibernate(QDBusInterface *iface)
{
    qDebug() << "hibernate";
    if (!iface->isValid()) { return; }
    QDBusMessage reply = iface->call("Hibernate");
    bool ok = reply.errorMessage().isEmpty();
    qDebug() << "reply" << ok;
}

void PowerClient::suspend(QDBusInterface *iface)
{
    qDebug() << "suspend";
    if (!iface->isValid()) { return; }
    QDBusMessage reply = iface->call("Suspend");
    bool ok = reply.errorMessage().isEmpty();
    qDebug() << "reply" << ok;
}

bool PowerClient::restart(QDBusInterface *iface)
{
    qDebug() << "restart";
    if (!iface->isValid()) { return false; }
    QDBusMessage reply = iface->call("Restart");
    bool ok = reply.errorMessage().isEmpty();
    qDebug() << "reply" << ok;
    return ok;
}

bool PowerClient::poweroff(QDBusInterface *iface)
{
    qDebug() << "poweroff";
    if (!iface->isValid()) { return false; }
    QDBusMessage reply = iface->call("PowerOff");
    bool ok = reply.errorMessage().isEmpty();
    qDebug() << "reply" << ok;
    return ok;
}
