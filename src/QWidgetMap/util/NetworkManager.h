/**
 * @copyright 2015 Chris Stylianou
 * @copyright 2007 - 2008 Kai Winter
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

// Qt includes.
#include <QtCore/QObject>
#include <QtCore/QMutex>
#include <QtCore/QUrl>
#include <QtGui/QPixmap>
#include <QtNetwork/QAuthenticator>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkProxy>

// Local includes.
#include "../qwidgetmap_global.h"

/// QWidgetMap namespace.
namespace qwm
{

    /// Utilities namespace.
    namespace util
    {

        /**
         * Manages the network download requests.
         */
        class QWIDGETMAP_EXPORT NetworkManager : public QObject
        {
            Q_OBJECT

        public:

            /**
             * This constructs a Network Manager.
             * @param parent QObject parent ownership.
             */
            explicit NetworkManager(QObject* parent = nullptr);

            /// Disable copy constructor.
            NetworkManager(const NetworkManager&) = delete;

            /// Disable copy assignment.
            NetworkManager& operator=(const NetworkManager&) = delete;

            /// Destructor.
            ~NetworkManager();

        public:

            /**
             * Set the network proxy to use.
             * @param proxy The network proxy to use.
             */
            void setProxy(const QNetworkProxy& proxy);

            /**
             * Aborts all current downloading threads.
             * This is useful when changing the zoom-factor to cancel previous requests.
             */
            void abortDownloads();

            /**
             * Get the number of current downloads.
             * @return size of the downloading queues.
             */
            int downloadQueueSize() const;

            /**
             * Checks if the given url resource is currently being downloaded.
             * @param url The url of the resource.
             * @param size_px The image size required in pixels.
             * @return whether the url resource is already downloading.
             */
            bool isDownloading(const QUrl& url, const QSize& size_px) const;

        public slots:

            /**
             * Downloads an image resource for the given url.
             * @param url The image url to download.
             * @param size_px The image size required in pixels.
             */
            void downloadImage(const QUrl& url, const QSize& size_px);

        private slots:

            /**
             * Slot to ask user for proxy authentication details.
             * @param proxy The proxy details.
             * @param authenticator The authenticator to fill out (username/password).
             */
            void proxyAuthenticationRequired(const QNetworkProxy& proxy, QAuthenticator* authenticator);

            /**
             * Slot to handle a download that has finished.
             * @param reply The reply that contains the downloaded data.
             */
            void downloadFinished(QNetworkReply* reply);

        signals:

            /**
             * Signal emitted when a resource has been queued for download.
             * @param count The current size of the download queue.
             */
            void downloadingInProgress(const int& count);

            /**
             * Signal emitted when a resource download has finished and the current download queue is empty.
             */
            void downloadingFinished();

            /**
             * Signal emitted when an image has been downloaded.
             * @param url The url that the image was downloaded from.
             * @param pixmap The downloaded image.
             */
            void downloadedImage(const QUrl& url, const QPixmap& pixmap);

            /**
             * Signal emitted when a resource download has failed.
             * @param url The url of the image.
             */
            void downloadFailed(const QUrl& url);

        private:

            /// Network access manager.
            QNetworkAccessManager m_nam;

            /// Mutex protecting downloading queue.
            mutable QMutex m_mutex_downloading_queue;

            /// Downloading queue.
            QMap<QNetworkReply*, std::pair<QUrl, QSize>> m_downloading_queue;

        };

    }

}
