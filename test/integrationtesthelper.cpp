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

#include "account.h"
#include "accountstate.h"
#include "accountmanager.h"
#include "testhelper.h"
#include "creds/httpcredentials.h"
#include "libsync/theme.h"

#include <QNetworkProxy>
#include <QJsonObject>

#include "integrationtesthelper.h"

constexpr auto serverUrl = "https://127.0.0.1/server";
constexpr auto testUsername = "test";
constexpr auto testPassword = "test";

class IntegrationTestHttpCredentials : public OCC::HttpCredentials
{
    Q_OBJECT

public:
    explicit IntegrationTestHttpCredentials()
        : OCC::HttpCredentials()
    {
        _user = QString(testUsername);
        _password = QString(testPassword);
    };

    void askFromUser() override {};
};

void IntegrationTestHelper::startAccountConfig()
{
    const auto accountManager = OCC::AccountManager::instance();
    _account = accountManager->createAccount();

    _account->setCredentials(new IntegrationTestHttpCredentials);
    _account->setUrl(OCC::Theme::instance()->overrideServerUrl());

    const auto serverUrlString = QString(serverUrl);
    _account->setUrl(serverUrlString);

    _account->networkAccessManager()->setProxy(QNetworkProxy(QNetworkProxy::NoProxy));
    _account->setSslConfiguration(QSslConfiguration());

    slotConnectToNCUrl(serverUrlString);
}

void IntegrationTestHelper::slotConnectToNCUrl(const QString &url)
{
    qDebug() << "Connect to url: " << url;

    const auto fetchUserNameJob = new OCC::JsonApiJob(_account->sharedFromThis(), QStringLiteral("/ocs/v1.php/cloud/user"));
    connect(fetchUserNameJob, &OCC::JsonApiJob::jsonReceived, this, [this, url](const QJsonDocument &json, int statusCode) {
        if (statusCode != 100) {
            qDebug() << "Could not fetch username.";
        }

        sender()->deleteLater();

        const auto objData = json.object().value("ocs").toObject().value("data").toObject();
        const auto userId = objData.value("id").toString("");
        const auto displayName = objData.value("display-name").toString("");
        _account->setDavUser(userId);
        _account->setDavDisplayName(displayName);

        emit accountReady(_account);
    });
    fetchUserNameJob->start();
}
