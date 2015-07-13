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

#include "ImageManager.h"

// Qt includes.
#include <QtCore/QCryptographicHash>
#include <QtGui/QPainter>

using namespace qwm::util;

namespace
{
    /// Singleton instance of Image Manager.
    std::unique_ptr<ImageManager> m_instance(nullptr);

    QString toString(const QSize& size_px)
    {
        return QString::number(size_px.width()) + "x" + QString::number(size_px.height());
    }
}

ImageManager& ImageManager::get()
{
    // Does the singleton instance exist?
    if(m_instance == nullptr)
    {
        // Create a default instance.
        m_instance.reset(new ImageManager);
    }

    // Return the reference to the instance object.
    return *(m_instance.get());
}

void ImageManager::destory()
{
    // Ensure the singleton instance is destroyed.
    m_instance.reset(nullptr);
}

ImageManager::ImageManager(QObject* parent)
    : QObject(parent),
      m_pixmap_failed_expiry(30),
      m_persistent_cache(false),
      m_persistent_cache_expiry(0)
{
    // Connect signal/slot for image downloads.
    QObject::connect(this, &ImageManager::downloadImage, &m_nm, &NetworkManager::downloadImage);
    QObject::connect(&m_nm, &NetworkManager::downloadedImage, this, &ImageManager::imageDownloaded);
    QObject::connect(&m_nm, &NetworkManager::downloadFailed, this, &ImageManager::imageFailed);
    QObject::connect(&m_nm, &NetworkManager::downloadingInProgress, this, &ImageManager::downloadingInProgress);
    QObject::connect(&m_nm, &NetworkManager::downloadingFinished, this, &ImageManager::downloadingFinished);
}

void ImageManager::setProxy(const QNetworkProxy& proxy)
{
    // Set the proxy on the network manager.
    m_nm.setProxy(proxy);
}

void ImageManager::setFailedExpiry(const std::chrono::seconds& failed_expiry)
{
    // Set the failed expiry.
    m_pixmap_failed_expiry = failed_expiry;
}

bool ImageManager::enablePersistentCache(const std::chrono::minutes& expiry, const QDir& path)
{
    // Ensure that the path exists (still returns true when path already exists.
    bool success(path.mkpath(path.absolutePath()));

    // If the path does exist, enable persistent cache.
    if(success)
    {
        // Set the persistent cache directory path.
        m_persistent_cache_directory = path;

        // Set the persistent cache expiry.
        m_persistent_cache_expiry = expiry;

        // Enable persistent caching.
        m_persistent_cache = true;
    }
    else
    {
        // Log error.
        qDebug() << "Unable to create directory for persistent cache '" << path.absolutePath() << "'";
    }

    // Return success.
    return success;
}

void ImageManager::abortLoading()
{
    // Abort any remaing network manager downloads.
    m_nm.abortDownloads();

    // Forget any previous failed images.
    m_failed_images.clear();
}

int ImageManager::loadQueueSize() const
{
    // Return the network manager downloading queue size.
    return m_nm.downloadQueueSize();
}

QPixmap ImageManager::image(const QUrl& url, const QSize& size_px)
{
    // Holding resource for image to be loaded into.
    QPixmap return_pixmap(pixmapLoading(size_px));

    // Is the image already been downloaded by the network manager?
    if(m_nm.isDownloading(url, size_px) == false)
    {
        // Is the image in our volatile "in-memory" cache?
        const auto itr_find(m_pixmap_cache.find(md5hex(url, size_px)));
        if(itr_find != m_pixmap_cache.end())
        {
            // Set the return image to the "in-memory" cached version.
            return_pixmap = itr_find->second;
        }
        // Is the persistent cache enabled, and does the image exist in the persistent cache.
        else if(m_persistent_cache && persistentCacheFind(url, size_px, return_pixmap))
        {
            // Add the image to the volatile cache.
            m_pixmap_cache[md5hex(url, size_px)] = return_pixmap;
        }
        // Has it recently failed to download?
        else if(m_failed_images.contains(url))
        {
            // Has the failed timeout not expired?
            if(m_failed_images.value(url).secsTo(QDateTime::currentDateTimeUtc()) < m_pixmap_failed_expiry.count())
            {
                // Return a failed pixmap instead.
                return_pixmap = pixmapFailed(size_px);
            }
            // The failed timeout has expired, we can not re-download the image.
            else
            {
                // Remove the failed image mark.
                m_failed_images.remove(url);

                // Emit that we need to download the image using the network manager.
                emit downloadImage(url, size_px);
            }
        }
        else
        {
            // Emit that we need to download the image using the network manager.
            emit downloadImage(url, size_px);
        }
    }

    // Default return the image.
    return return_pixmap;
}

QPixmap ImageManager::prefetchImage(const QUrl& url, const QSize& size_px)
{
    // Add the url to the prefetch list.
    m_prefetch_urls.append(url);

    // Return the image for the url.
    return image(url, size_px);
}

void ImageManager::imageDownloaded(const QUrl& url, const QPixmap& pixmap)
{
    // Add it to the pixmap cache.
    m_pixmap_cache[md5hex(url, pixmap.size())] = pixmap;

    // Do we have the persistent cache enabled?
    if(m_persistent_cache)
    {
        // Add the pixmap to the persistent cache.
        persistentCacheInsert(url, pixmap);
    }

    // Is this a prefetch request?
    if(m_prefetch_urls.contains(url))
    {
        // Remove the url from the prefetch list.
        m_prefetch_urls.removeAt(m_prefetch_urls.indexOf(url));
    }
    else
    {
        // Let the world know we have received an updated image.
        emit imageUpdated(url);
    }
}

void ImageManager::imageFailed(const QUrl& url)
{
    qDebug() << "ImageManager::imagedFailed '" << url << "'";

    // Mark the image as failed to download.
    m_failed_images[url] = QDateTime::currentDateTimeUtc();

    // Let the world know we have received an updated image.
    emit imageUpdated(url);
}

QPixmap ImageManager::pixmapLoading(const QSize& size_px)
{
    // Does a pixmap alraedy exist at the requested size?
    if(m_pixmap_loading.find(toString(size_px)) == m_pixmap_loading.end())
    {
        // Create a new pixmap.
        QPixmap pixmap(size_px);

        // Make is transparent.
        pixmap.fill(Qt::transparent);

        // Add a pattern.
        QPainter painter(&pixmap);
        QBrush brush(Qt::lightGray, Qt::Dense5Pattern);
        painter.fillRect(pixmap.rect(), brush);

        // Add "LOADING..." text.
        painter.setPen(Qt::black);
        painter.drawText(pixmap.rect(), Qt::AlignCenter, "LOADING...");

        // Save the pixmap.
        m_pixmap_loading[toString(size_px)] = pixmap;
    }

    // Return the pixmap.
    return m_pixmap_loading[toString(size_px)];
}

QPixmap ImageManager::pixmapFailed(const QSize& size_px)
{
    // Does a pixmap alraedy exist at the requested size?
    if(m_pixmap_failed.find(toString(size_px)) == m_pixmap_failed.end())
    {
        // Create a new pixmap.
        QPixmap pixmap(size_px);

        // Make is transparent.
        pixmap.fill(Qt::transparent);

        // Save the pixmap.
        m_pixmap_failed[toString(size_px)] = pixmap;
    }

    // Return the pixmap.
    return m_pixmap_failed[toString(size_px)];
}

QString ImageManager::md5hex(const QUrl& url, const QSize& size_px)
{
    // Return the md5 hex value of the given url at a specific tile size.
    return toString(size_px) + "_" + QString(QCryptographicHash::hash(url.toString().toUtf8(), QCryptographicHash::Md5).toHex());
}

QString ImageManager::persistentCacheFilename(const QUrl& url, const QSize& size_px)
{
    // Return the persistent file path for the given url.
    return m_persistent_cache_directory.absolutePath() + QDir::separator() + md5hex(url, size_px);
}

bool ImageManager::persistentCacheFind(const QUrl& url, const QSize& size_px, QPixmap& return_pixmap)
{
    // Track our success.
    bool success(false);

    // The file for the given url from the persistent cache.
    QFile file(persistentCacheFilename(url, size_px));

    // Does the file exist?
    if(file.exists())
    {
        // Get the file information.
        QFileInfo file_info(file);

        // Is the persistent cache expiry set, and if so is the file older than the expiry time
        // allowed?
        if(m_persistent_cache_expiry.count() > 0
           && file_info.lastModified().msecsTo(QDateTime::currentDateTime()) > std::chrono::duration_cast<std::chrono::milliseconds>(m_persistent_cache_expiry).count())
        {
            // The file is too old, remove it.
            m_persistent_cache_directory.remove(file.fileName());

            // Log removing the file.
            qDebug() << "Removing '" << file.fileName() << "' from persistent cache for url '" << url << "'";
        }
        else
        {
            // Try to load the file into the pixmap, store the success result.
            success = return_pixmap.load(persistentCacheFilename(url, size_px));
        }
    }

    // Return success.
    return success;
}

bool ImageManager::persistentCacheInsert(const QUrl& url, const QPixmap& pixmap)
{
    // Return the result of saving the pixmap to the persistent cache.
    return pixmap.save(persistentCacheFilename(url, pixmap.size()), "PNG");
}
