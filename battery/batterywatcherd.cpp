/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2012 Razor team
 * Authors:
 *   Christian Surlykke <christian@surlykke.dk>
 *
 * This program or library is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 *
 * END_COMMON_COPYRIGHT_HEADER */
#include <QtCore/QDebug>
#include <QtCore/QTimer>
#include <QtCore/QCoreApplication>
#include <razorqt/razorautostartentry.h>

#include "batterywatcherd.h"
#include "../config/constants.h"

BatteryWatcherd::BatteryWatcherd(QObject *parent) :
    QObject(parent),
    mRazorNotification(tr("Power low"), this),
    mActionTime(),
    mSettings("razor-autosuspend")
{
    bool performFirstRunCheck = mSettings.value(FIRSTRUNCHECK_KEY, false).toBool();
    if (performFirstRunCheck)
    {
        mSettings.setValue(FIRSTRUNCHECK_KEY, false);
    }

    mBattery = new Battery(this);
    if (!mBattery->haveBattery())
    {
        if (performFirstRunCheck)
        {
            qWarning() << "No battery detected - disabling Razor Autosuspend";
            RazorAutostartEntry autostartEntry("razor-autosuspend.desktop");
            autostartEntry.setEnabled(false);
            autostartEntry.commit();

            // We can't quit if the event loop did not start yet
            QTimer::singleShot(0, qApp, SLOT(quit()));
            return;
        }

        RazorNotification::notify(tr("No battery!"),
                                  tr("Razor autosuspend could not find data about any battery - actions on power low will not work"),
                                  "razor-autosuspend");
    }

    mRazorNotification.setIcon("razor-autosuspend");
    mRazorNotification.setUrgencyHint(RazorNotification::UrgencyCritical);
    mRazorNotification.setTimeout(2000);

    new TrayIcon(mBattery, this);
    connect(mBattery, SIGNAL(batteryChanged()), this, SLOT(batteryChanged()));
}

BatteryWatcherd::~BatteryWatcherd()
{
}

void BatteryWatcherd::batteryChanged()
{
    qDebug() <<  "BatteryChanged"
             <<  "discharging:"  << mBattery->discharging() 
             << "chargeLevel:" << mBattery->chargeLevel() 
             << "powerlow:"    << mBattery->powerLow() 
             << "actionTime:"  << mActionTime;

    if (mBattery->powerLow() && mActionTime.isNull() && powerLowAction() > 0)
    {
        int warningTimeMsecs = mSettings.value(POWERLOWWARNING_KEY, 30).toInt()*1000;
        mActionTime = QTime::currentTime().addMSecs(warningTimeMsecs);
        startTimer(100);
        // From here everything is handled by timerEvent below
    }
}

void BatteryWatcherd::timerEvent(QTimerEvent *event)
{
    if (mActionTime.isNull() || powerLowAction() == 0 || ! mBattery->powerLow())
    {
            killTimer(event->timerId());
            mActionTime = QTime();
    }
    else if (QTime::currentTime().msecsTo(mActionTime) > 0)
    {
        QString notificationMsg;
        switch (powerLowAction())
        {
        case SLEEP:
            notificationMsg = tr("Sleeping in %1 seconds");
            break;
        case HIBERNATE:
            notificationMsg = tr("Hibernating in %1 seconds");
            break;
        case POWEROFF:
            notificationMsg = tr("Shutting down in %1 seconds");
            break;
        }

        mRazorNotification.setBody(notificationMsg.arg(QTime::currentTime().msecsTo(mActionTime)/1000));
        mRazorNotification.update();
    }
    else
    {
        doAction(powerLowAction());
        mActionTime = QTime();
        killTimer(event->timerId());
    }
}

void BatteryWatcherd::doAction(int action)
{
    switch (action)
    {
    case SLEEP:
        mRazorPower.suspend();
        break;
    case HIBERNATE:
        mRazorPower.hibernate();
        break;
    case POWEROFF:
        mRazorPower.shutdown();
        break;
    }
}

int BatteryWatcherd::powerLowAction()
{
    return mSettings.value(POWERLOWACTION_KEY).toInt();
}

