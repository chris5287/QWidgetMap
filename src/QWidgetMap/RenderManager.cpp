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

#include "RenderManager.h"

// Qt includes.
#include <QtGui/QImage>
#include <QtGui/QPainter>
#include <QtGui/QPixmap>

// Local includes.
#include "util/ImageManager.h"

using namespace qwm;

RenderManager::RenderManager(const std::shared_ptr<ViewportManager>& viewport_manager, const std::shared_ptr<LayerManager>& layer_manager, QObject* parent)
    : QObject(parent),
      m_viewport_manager(viewport_manager),
      m_layer_manager(layer_manager)
{
    // Register meta types.
    qRegisterMetaType<util::RectWorldCoord>("util::RectWorldCoord");

    // Connect signal/slots to process changes that require a redraw request.
    QObject::connect(m_layer_manager.get(), &LayerManager::layerChanged, this, &RenderManager::requestRedraw);
    QObject::connect(&(util::ImageManager::get()), &util::ImageManager::imageUpdated, this, &RenderManager::requestRedraw);

    // Start the render thread.
    m_thread_renderer = std::thread(&RenderManager::processRequests, this);
}

RenderManager::~RenderManager()
{
    // Instruct the renderer to stop processing requests.
    m_processing_allowed = false;

    // Is the renderer thread joinable?
    if(m_thread_renderer.joinable())
    {
        m_thread_renderer.join();
    }
}

void RenderManager::requestRedraw()
{
    // Get access to the queue mutex.
    std::lock_guard<std::mutex> locker(m_queue_mutex);

    // Add the request to the queue.
    m_queue.push_back(true);
}

void RenderManager::processRequests()
{
    // Mark that processing is allowed.
    m_processing_allowed = true;

    // While processing is allowed...
    while(m_processing_allowed)
    {
        // Discover the current rendering queue status.
        bool m_queue_empty(true);
        {
            // Get access to the queue mutex.
            std::lock_guard<std::mutex> locker(m_queue_mutex);

            // Get the current queue status, is it empty?
            m_queue_empty = m_queue.empty();

            // Empty the queue, as we can collapse all previous requests into this processing loop.
            m_queue.clear();
        }

        // Is the queue empty?
        if(m_queue_empty)
        {
            // Emit that rendering has finished.
            emit renderingFinished();
        }
        else
        {
            // Emit that rendering is in progress.
            emit renderingStarted();

            // Fetch the current viewport manager.
            const Viewport current_viewport(*(m_viewport_manager.get()));

            // Calculate the drawing rect in world pixels and coordinates.
            const util::RectWorldPx drawing_rect_world_px(drawingRectWorldPx(current_viewport));
            const util::RectWorldCoord draw_rect_world_coord(drawingRectWorldCoord(current_viewport));

            // Generate a drawing viewport image.
            QImage image_drawing_viewport(drawingSizePx(current_viewport), QImage::Format_ARGB32);

            // Clear the image (allows for background widget colours to be seen).
            image_drawing_viewport.fill(Qt::transparent);

            // Create a painter for the image.
            QPainter painter(&image_drawing_viewport);

            // Translate to the viewport's drawing top/left point.
            painter.translate(-drawing_rect_world_px.topLeftPx());

            // Loop through each layer and draw it to the viewport drawing image.
            for(const auto& layer : m_layer_manager->layers())
            {
                // Check the layer is visible.
                if(layer->isVisible(current_viewport))
                {
                    // Draw the layer to the image.
                    layer->draw(painter, draw_rect_world_coord, current_viewport);
                }
            }

            // Undo the viewport's drawing top/left point translation.
            painter.translate(drawing_rect_world_px.topLeftPx());

            // Emit that we have a new image to display.
            emit imageChanged(QPixmap::fromImage(image_drawing_viewport), draw_rect_world_coord, current_viewport.zoom());
        }

        // Sleep for a little while.
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
    }
}

QSize RenderManager::drawingSizePx(const Viewport& viewport) const
{
    // Return 2 x viewport size to allow for 'panning buffer'.
    return viewport.sizePx().toSize() * 2;
}

util::RectWorldCoord RenderManager::drawingRectWorldCoord(const Viewport& viewport) const
{
    // Fetch the viewport size.
    const QSizeF viewport_size_px(viewport.sizePx());

    // Calculate the viewport offset based off the drawingSizePx().
    const QSizeF viewport_offset_size_px((drawingSizePx(viewport) - viewport_size_px) / 2.0);
    const util::PointPx viewport_offset_px(viewport_offset_size_px.width(), viewport_offset_size_px.height());

    // Return the drawing rect including panning buffer in world coordinates.
    return util::RectWorldCoord(
                projection::toPointWorldCoord(viewport, util::PointViewportPx(0, 0) - viewport_offset_px),
                projection::toPointWorldCoord(viewport, util::PointViewportPx(viewport_size_px.width(), viewport_size_px.height()) + viewport_offset_px));
}

util::RectWorldPx RenderManager::drawingRectWorldPx(const Viewport& viewport) const
{
    // Fetch the viewport size.
    const QSizeF viewport_size_px(viewport.sizePx());

    // Calculate the viewport offset based off the drawingSizePx().
    const QSizeF viewport_offset_size_px((drawingSizePx(viewport) - viewport_size_px) / 2.0);
    const util::PointPx viewport_offset_px(viewport_offset_size_px.width(), viewport_offset_size_px.height());

    // Return the drawing rect including panning buffer in world pixels.
    return util::RectWorldPx(viewport.toPointWorldPx(util::PointViewportPx(0, 0) - viewport_offset_px),
                             viewport.toPointWorldPx(util::PointViewportPx(viewport_size_px.width(), viewport_size_px.height()) + viewport_offset_px));
}
