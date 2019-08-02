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

#ifndef ZEAL_WEBPAGE_H
#define ZEAL_WEBPAGE_H

#include <QWebEnginePage>

namespace Zeal {
namespace Browser {

class WebPage : public QWebEnginePage
{
    Q_OBJECT
    Q_DISABLE_COPY(WebPage)

public:
    WebPage(QWebEngineProfile *profile, QObject *parent = nullptr);
    WebPage(QObject *parent = nullptr);

protected:
    bool acceptNavigationRequest(const QUrl &url, QWebEnginePage::NavigationType type, bool isMainFrame) override;
    QWebEnginePage *createWindow(QWebEnginePage::WebWindowType type) override;

    static bool isExternalUrl(const QUrl &url);

    QUrl    mLastUrl;
};

} // namespace Browser
} // namespace Zeal

#endif //ZEAL_WEBPAGE_H
