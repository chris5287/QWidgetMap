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
#include <QtCore/QDir>
#include <QtCore/QObject>
#include <QtCore/QList>
#include <QtCore/QUrl>
#include <QtGui/QPixmap>
#include <QtNetwork/QNetworkProxy>

// STL includes.
#include <chrono>
#include <map>
#include <memory>

// Local includes.
#include "../qwidgetmap_global.h"
#include "NetworkManager.h"

/// QWidgetMap namespace.
namespace qwm
{

    /// Utilities namespace.
    namespace util
    {

        /**
         * Manages the image url requests.
         */
        class QWIDGETMAP_EXPORT ImageManager : public QObject
        {
            Q_OBJECT

        public:

            /**
             * Get the singleton instance of the Image Manager.
             * @return the singleton instance.
             */
            static ImageManager& get();

            /// Destroys the singleton instance of Image Manager.
            static void destory();

        private:

            /**
             * This constructs a Image Manager.
             * @param parent QObject parent ownership.
             */
            explicit ImageManager(QObject* parent = nullptr);

        public:

            /// Disable copy constructor.
            ImageManager(const ImageManager&) = delete;

            /// Disable copy assignment.
            ImageManager& operator=(const ImageManager&) = delete;

            /// Destructor.
            ~ImageManager() = default;

        public:

            /**
             * Set the network proxy to use.
             * @param proxy The network proxy to use.
             */
            void setProxy(const QNetworkProxy& proxy);

            /**
             * Set the failed expiry timeout.
             * @param failed_expiry The failed expiry timeout to set.
             */
            void setFailedExpiry(const std::chrono::seconds& failed_expiry = std::chrono::seconds(30));

            /**
             * Enables the persistent cache, specifying the directory and expiry timeout.
             * @param path The path where the images should be stored.
             * @param expiry The max age (in minutes) of an image before its removed and a new one is requested (0 to keep forever).
             * @return whether the persistent cache was enabled.
             */
            bool enablePersistentCache(const std::chrono::minutes& expiry, const QDir& path);

            /**
             * Aborts all current loading threads.
             * This is useful when changing the zoom-factor.
             */
            void abortLoading();

            /**
             * Number of images pending in the load queue.
             * @return the number of images pending in the load queue.
             */
            int loadQueueSize() const;

            /**
             * Fetch the requested image either from an in-memory cache or persistent file cache (if
             * enabled). If the image does not exist, then it is fetched using a network manager and a "loading"
             * placeholder pixmap is returned. Once the image has been downloaded, the image manager
             * will emit "imageUpdated" to inform that the image is now ready.
             * @param url The image url to fetch.
             * @param size_px The image size required in pixels.
             * @return the pixmap of the image.
             */
            QPixmap image(const QUrl& url, const QSize& size_px);

            /**
             * Fetches the requested image using the getImage function, which has been deemed "offscreen".
             * @param url The image url to fetch.
             * @param size_px The image size required in pixels.
             * @return the pixmap of the image.
             */
            QPixmap prefetchImage(const QUrl& url, const QSize& size_px);

        private slots:

            /**
             * Slot to handle an image that has been downloaded.
             * @param url The url that the image was downloaded from.
             * @param pixmap The image.
             */
            void imageDownloaded(const QUrl& url, const QPixmap& pixmap);

            /**
             * Slot to handle when an image fails to download.
             * @param url The url of the image.
             */
            void imageFailed(const QUrl& url);

        private:

            /**
             * Fetches the loading pixmap at the required size in pixels.
             * @param size_px The size required in pixels.
             * @return the loading pixmap at the required size in pixels.
             */
            QPixmap pixmapLoading(const QSize& size_px);

            /**
             * Fetches the failed pixmap at the required size in pixels.
             * @param size_px The size required in pixels.
             * @return the failed pixmap at the required size in pixels.
             */
            QPixmap pixmapFailed(const QSize& size_px);

            /**
             * Generate a md5 hex for the given url.
             * @param url The url to generate a md5 hex for.
             * @param size_px The image size required in pixels.
             * @return the md5 hex of the url.
             */
            QString md5hex(const QUrl& url, const QSize& size_px);

            /**
             * Generate the persistent file path for the given url.
             * @param url The url to generate a file path for.
             * @param size_px The image size required in pixels.
             * @return the file path for the url.
             */
            QString persistentCacheFilename(const QUrl& url, const QSize& size_px);

            /**
             * Finds and loads the requested image if is exists in the persistent cache.
             * @param url The image url to fetch.
             * @param size_px The image size required in pixels.
             * @param return_pixmap The pixmap of the image to be populated.
             * @return whether the image was actually found and loaded.
             */
            bool persistentCacheFind(const QUrl& url, const QSize& size_px, QPixmap& return_pixmap);

            /**
             * Inserts the image into the persistent cache.
             * @param url The image url to insert.
             * @param pixmap The pixmap of the image to insert.
             * @return whether the image was actually inserted.
             */
            bool persistentCacheInsert(const QUrl& url, const QPixmap& pixmap);

        signals:

            /**
             * Signal emitted to schedule an image resource to be downloaded.
             * @param url The image url to download.
             * @param size_px The image size required in pixels.
             */
            void downloadImage(const QUrl& url, const QSize& size_px);

            /**
             * Signal emitted when a new image has been queued for download.
             * @param count The current size of the download queue.
             */
            void downloadingInProgress(const int& count);

            /**
             * Signal emitted when a image download has finished and the download queue is empty.
             */
            void downloadingFinished();

            /**
             * Signal emitted when an image has been downloaded by the network manager.
             * @param url The url that the image was downloaded from.
             */
            void imageUpdated(const QUrl& url);

        private:

            /// Network manager.
            NetworkManager m_nm;

            /// Cache of pixmaps already loaded.
            std::map<QString, QPixmap> m_pixmap_cache;

            /// Pixmap of an empty image with "LOADING..." text.
            std::map<QString, QPixmap> m_pixmap_loading;

            /// Pixmap of a failed image.
            std::map<QString, QPixmap> m_pixmap_failed;

            /// The failed image expiry.
            std::chrono::seconds m_pixmap_failed_expiry;

            /// A list of image urls being prefetched.
            QList<QUrl> m_prefetch_urls;

            /// Whether persistent caching is enabled.
            bool m_persistent_cache;

            /// The persistent cache's storage directory.
            QDir m_persistent_cache_directory;

            /// The persistent cache's image expiry.
            std::chrono::minutes m_persistent_cache_expiry;

            /// List of current failed images.
            QMap<QUrl, QDateTime> m_failed_images;

        };

    }

}
