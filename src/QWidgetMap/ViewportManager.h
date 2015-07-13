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
#include <chrono>
#include <mutex>

// Local includes.
#include "qwidgetmap_global.h"
#include "Viewport.h"

/// QWidgetMap namespace.
namespace qwm
{

    /**
     * Manages the viewport state (including projection and zoom management).
     */
    class QWIDGETMAP_EXPORT ViewportManager : public QObject, public Viewport
    {
        Q_OBJECT

    public:

        /**
         * Constructor of Viewport Manager.
         * @param size_px The size of the viewport required.
         * @param projection_epsg The EPSG of projection required.
         * @param tile_size_px The tile size in pixels required.
         * @param parent QObject parent ownership.
         */
        ViewportManager(const QSizeF& size_px, const projection::EPSG& projection_epsg, const QSize& tile_size_px, QObject* parent = nullptr);

        /// Disable copy constructor.
        ViewportManager(const ViewportManager&) = delete;

        /// Disable copy assignment.
        ViewportManager& operator=(const ViewportManager&) = delete;

        /// Destructor.
        ~ViewportManager() = default;

    public:

        // Size management.
        /**
         * Set the viewport size in pixels.
         * @param size_px The viewport size in pixels.
         */
        void setSizePx(const QSizeF& size_px) final;

        // Projection management.
        /**
         * Sets the required projection.
         * @param projection_epsg The EPSG of projection required.
         */
        void setProjection(const projection::EPSG& projection_epsg) final;

        // Focus point management.
        /**
         * Set the focus point to the given world coordinate.
         * @param world_coord The world coordinate to set the focus point on.
         */
        void setFocusPointWorldCoord(const util::PointWorldCoord& world_coord) final;

        /**
         * Sets the focus point, based on the mean average of all the world coordinates.
         * If request, it will also automatically calculate best zoom level and zoom to it.
         * @param world_coords The world coorinates to based the focus point on.
         * @param auto_zoom Whether to automatically zoom to the best level.
         */
        void setFocusPointWorldMean(const std::vector<util::PointWorldCoord>& world_coords, const bool& auto_zoom = false);

        /**
         * Smoothly moves the focus point to the given world coordinate.
         * @param world_coord The world coordinate that the focus point should moved to.
         * @param steps The number of steps to use in the animation.
         * @param step_interval The number of millseconds to wait between each step.
         */
        void setFocusPointWorldAnimated(const util::PointWorldCoord& world_coord, const int& steps = 25, const std::chrono::milliseconds& step_interval = std::chrono::milliseconds(50));

        // Scrolling managment.
        /**
         * Restricts the scrolling boundaries of the viewport rect (set defaults to disable this).
         * @param rect_world_coord The rect world coordinates of the scrolling boundary.
         */
        void setScrollRestriction(const util::RectWorldCoord& rect_world_coord = { util::PointWorldCoord { 0.0, 0.0 }, util::PointWorldCoord { 0.0, 0.0 } });

        /**
         * Scrolls the focus point by the given value in pixels.
         * @param delta_px The required number of pixels to scroll by.
         */
        void scroll(const util::PointPx& delta_px);

        /**
         * Scroll the focus point left by required pixels.
         * @param delta_px The required number of pixels to scroll by.
         */
        void scrollLeft(const double& delta_px = 10.0);

        /**
         * Scroll the focus point right by required pixels.
         * @param delta_px The required number of pixels to scroll by.
         */
        void scrollRight(const double& delta_px = 10.0);

        /**
         * Scroll the focus point up by required pixels.
         * @param delta_px The required number of pixels to scroll by.
         */
        void scrollUp(const double& delta_px = 10.0);

        /**
         * Scroll the focus point down by required pixels.
         * @param delta_px The required number of pixels to scroll by.
         */
        void scrollDown(const double& delta_px = 10.0);

        // Zooming management.
        /**
         * Fetches the minimum zoom allowed.
         * @return the minimum zoom allowed.
         */
        int zoomMinimum() const;

        /**
         * Set the minimum zoom allowed.
         * @param zoom The minimum zoom allowed.
         */
        void setZoomMinimum(const int& zoom);

        /**
         * Fetches the maximum zoom allowed.
         * @return the maximum zoom allowed.
         */
        int zoomMaximum() const;

        /**
         * Set the maximum zoom allowed.
         * @param zoom The maximum zoom allowed.
         */
        void setZoomMaximum(const int& zoom);

        /**
         * Zoom in one level.
         */
        void zoomIn();

        /**
         * Zoom out one level.
         */
        void zoomOut();

        /**
         * Set the zoom level required.
         * @param zoom The zoom level requested.
         */
        void setZoom(int zoom) final;

    private:

        /**
         * Check whether all world coordinates are visible in the viewport.
         * @param world_coords The world coordinates to check.
         * @return whether all the world coordinates are visible in the viewport.
         */
        bool viewportContains(const std::vector<util::PointWorldCoord>& world_coords) const;

        /**
         * Check that the zoom settings are valid, and if not fixes them.
         */
        void validateZoom();

    private slots:

        /**
         * Slot called during the animation loop to process the next step.
         */
        void animatedTick();

    signals:

        /**
         * Signal emitted when the viewport changes (viewport size, projection type, focus point movement, zoom change).
         */
        void viewportChanged();

    private:

        /// The scroll restriction rect in world coordinates (set to util::RectWorldCoord( { util::PointWorldCoord { 0.0, 0.0 }, util::PointWorldCoord { 0.0, 0.0 } } ) to disable.
        util::RectWorldCoord m_scroll_restriction { util::PointWorldCoord { -170.0, 80.0 }, util::PointWorldCoord { 170.0, -80.0 } };

        /// The minimum zoom allowed.
        int m_zoom_minimum { 0 };

        /// The maximum zoom allowed.
        int m_zoom_maximum { 17 };

        /// Mutex to protect the animation loop.
        std::mutex m_animated_mutex;

        /// Animation focus point target in world coordinates.
        util::PointWorldCoord m_animated_focus_world_coord { 0.0, 0.0 };

        /// Number of animation steps remaining.
        int m_animated_steps { 0 };

        /// The number of millseconds to wait between each step.
        std::chrono::milliseconds m_animated_interval { std::chrono::milliseconds(50) };

    };

}
