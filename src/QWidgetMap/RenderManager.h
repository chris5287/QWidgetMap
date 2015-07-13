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

// STL includes.
#include <memory>
#include <mutex>
#include <thread>

// Local includes.
#include "qwidgetmap_global.h"
#include "LayerManager.h"
#include "ViewportManager.h"
#include "util/Rect.h"

/// QWidgetMap namespace.
namespace qwm
{

    /**
     * Manages the rendering process.
     */
    class QWIDGETMAP_EXPORT RenderManager : public QObject
    {
        Q_OBJECT

    public:

        /**
         * Constructor of Render Manager.
         * @param viewport_manager The viewport manager to use.
         * @param layer_manager The layer manager to use.
         * @param parent QObject parent ownership.
         */
        RenderManager(const std::shared_ptr<ViewportManager>& viewport_manager, const std::shared_ptr<LayerManager>& layer_manager, QObject* parent = nullptr);

        /// Disable copy constructor.
        RenderManager(const RenderManager&) = delete;

        /// Disable copy assignment.
        RenderManager& operator=(const RenderManager&) = delete;

        /// Destructor.
        ~RenderManager();

    public slots:

        /**
         * Slot to add a redraw request to the queue.
         */
        void requestRedraw();

    private:

        /**
         * Redraws the backbuffer image, which when ready will emit updatePrimaryScreen() for it to be stored/drawn.
         */
        void processRequests();

        /**
         * Calculates the drawing size in pixels based on the viewport provided.
         * @param viewport The viewport to use.
         * @return the drawing size in pixels.
         */
        QSize drawingSizePx(const Viewport& viewport) const;

        /**
         * Calculates the drawing rect in world coordinates based on the viewport provided.
         * @param viewport The viewport to use.
         * @return the drawing rect in world coordinates.
         */
        util::RectWorldCoord drawingRectWorldCoord(const Viewport& viewport) const;

        /**
         * Calculates the drawing rect in world pixels based on the viewport provided.
         * @param viewport The viewport to use.
         * @return the drawing rect in world pixels.
         */
        util::RectWorldPx drawingRectWorldPx(const Viewport& viewport) const;

    signals:

        /**
         * Signal emitted when rendering is in progress.
         */
        void renderingStarted();

        /**
         * Signal emitted when rendering has finished.
         */
        void renderingFinished();

        /**
         * Signal emitted when the rendering process has an updated pixmap available.
         * @param pixmap The updated pixmap.
         * @param rect_world_coord The rect of the updated pixmap in world coordinates.
         * @param zoom The zoom level of the updated pixmap.
         */
        void imageChanged(QPixmap pixmap, util::RectWorldCoord rect_world_coord, int zoom);

    private:

        /// The viewport manager to use.
        std::shared_ptr<ViewportManager> m_viewport_manager;

        /// The layer manager to use.
        std::shared_ptr<LayerManager> m_layer_manager;

    private:

        /// Whether processing is allowed.
        volatile bool m_processing_allowed { false };

        /// The rendering thread.
        std::thread m_thread_renderer;

        /// Mutex to protect the rendering queue.
        std::mutex m_queue_mutex;

        /// The rendering queue.
        std::vector<bool> m_queue;

    };

}
