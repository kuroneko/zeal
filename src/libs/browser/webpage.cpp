/****************************************************************************
**
** Copyright (C) 2019 Chris Collins
** Contact: https://go.zealdocs.org/l/contact
**
** This file is part of Zeal.
**
** Zeal is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** Zeal is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Zeal. If not, see <https://www.gnu.org/licenses/>.
**
****************************************************************************/

#include <QWebEnginePage>
#include <QWebChannel>
#include <QWebEngineContextMenuData>
#include <QWebEngineSettings>
#include <QWheelEvent>
#include <QPushButton>
#include <QMessageBox>
#include <QMenu>
#include <QDesktopServices>
#include <QCheckBox>
#include <ui/mainwindow.h>
#include <ui/browsertab.h>
#include <core/settings.h>
#include <core/application.h>
#include "webcontrol.h"
#include "webview.h"
#include "webpage.h"

using namespace Zeal::Browser;

WebPage::WebPage(QWebEngineProfile *profile, QObject *parent) : QWebEnginePage(profile, parent)
{
}

WebPage::WebPage(QObject *parent) : QWebEnginePage(parent)
{
}

bool
WebPage::acceptNavigationRequest(const QUrl &url, QWebEnginePage::NavigationType type, bool isMainFrame)
{
    if (isExternalUrl(url) && isMainFrame && type == QWebEnginePage::NavigationType::NavigationTypeLinkClicked) {
        // if we're redirecting around the same host, (or even following links), just accept it.
        //
        // This prevents being prompted multiple times for trivial redirects but ensures you still get asked
        // if you're leaving the safety of a site you've already accepted.
        if (mLastUrl.host() == url.host()) {
            mLastUrl = url;
            return true;
        }
        switch (Core::Application::instance()->settings()->externalLinkPolicy) {
        case Core::Settings::ExternalLinkPolicy::Open:
            mLastUrl = url;
            return true;
        case Core::Settings::ExternalLinkPolicy::Ask: {
            QScopedPointer<QMessageBox> mb(new QMessageBox());
            mb->setIcon(QMessageBox::Question);
            mb->setText(tr("How do you want to open the external link?<br>URL: <b>%1</b>")
                .arg(url.toString()));


            QCheckBox *checkBox = new QCheckBox("Do &not ask again");
            mb->setCheckBox(checkBox);

            QPushButton *openInBrowserButton = mb->addButton(tr("Open in &Desktop Browser"),
                QMessageBox::ActionRole);
            QPushButton *openInZealButton = mb->addButton(tr("Open in &Zeal"),
                QMessageBox::ActionRole);
            mb->addButton(QMessageBox::Cancel);

            mb->setDefaultButton(openInBrowserButton);

            if (mb->exec() == QMessageBox::Cancel) {
                return false;
            }

            if (mb->clickedButton() == openInZealButton) {
                if (checkBox->isChecked()) {
                    Core::Application::instance()->settings()->externalLinkPolicy
                        = Core::Settings::ExternalLinkPolicy::Open;
                    Core::Application::instance()->settings()->save();
                }
                mLastUrl = url;
                return true;
            }

            if (mb->clickedButton() == openInBrowserButton) {
                if (checkBox->isChecked()) {
                    Core::Application::instance()->settings()->externalLinkPolicy
                        = Core::Settings::ExternalLinkPolicy::OpenInSystemBrowser;
                    Core::Application::instance()->settings()->save();
                }
                QDesktopServices::openUrl(url);
                return false;
            }
        }
        case Core::Settings::ExternalLinkPolicy::OpenInSystemBrowser:
            QDesktopServices::openUrl(url);
            return false;
        default:
            Q_UNREACHABLE();
        }
    } else {
        bool willAccept = QWebEnginePage::acceptNavigationRequest(url, type, isMainFrame);
        if (willAccept) {
            mLastUrl = url;
        }
        return willAccept;
    }
}

QWebEnginePage *
Zeal::Browser::WebPage::createWindow(QWebEnginePage::WebWindowType type)
{
    return QWebEnginePage::createWindow(type);
}

bool
WebPage::isExternalUrl(const QUrl &url)
{
    static const QStringList localSchemes = {
        QStringLiteral("file"),
        QStringLiteral("qrc"),
    };

    const QString scheme = url.scheme();
    return !scheme.isEmpty() && !localSchemes.contains(scheme);
}