/*
 * Copyright (C) by Claudio Cambra <claudio.cambra@nextcloud.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 */

#pragma once

#include <QObject>

#include "account.h"

class IntegrationTestHelper : public QObject
{
    Q_OBJECT

public:
    IntegrationTestHelper() = default;

signals:
    void accountReady(const OCC::AccountPtr &account);

public slots:
    void startAccountConfig();

private slots:
    void slotFindServer();
    void slotFoundServer(const QUrl &url, const QJsonObject &info);
    void slotNoServerFound(QNetworkReply *reply);
    void slotNoServerFoundTimeout(const QUrl &url);
    void slotConnectToNCUrl(const QString &url);

private:
    OCC::AccountPtr _account;
}
