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

// Local includes.
#include "qwidgetmap_global.h"
#include "projection/Projection.h"
#include "util/Rect.h"

/// QWidgetMap namespace.
namespace qwm
{

    /**
     * Captures the current viewport state (size, projection, zoom and focus point).
     */
    class QWIDGETMAP_EXPORT Viewport
    {

    public:

        /**
         * Constructor of Viewport.
         * @param size_px The size of the viewport required.
         * @param projection_epsg The EPSG of projection required.
         * @param tile_size_px The tile size in pixels required.
         */
        Viewport(const QSizeF& size_px, const projection::EPSG& projection_epsg, const QSize& tile_size_px);

        /// Copy constructor.
        Viewport(const Viewport&) = default;

        /// Copy assignment.
        Viewport& operator=(const Viewport&) = default;

        /// Destructor.
        virtual ~Viewport() = default;

    public:

        /**
         * Fetches the viewport size in pixels.
         * @return the viewport size in pixels.
         */
        QSizeF sizePx() const;

        /**
         * Fetches the center point of the viewport size in pixels.
         * @return the center point of the viewport size in pixels.
         */
        util::PointViewportPx sizePointCenterPx() const;

        /**
         * Fetches the projection.
         * @return The EPSG of projection.
         */
        const projection::EPSG& projection() const;

        /**
         * Fetches the tile size in pixels.
         * @return the tile size in pixels.
         */
        const QSize& tileSizePx() const;

        /**
         * Fetches the current zoom level.
         * @return the current zoom level.
         */
        int zoom() const;

        /**
         * Fetches the focus point in world coordinates (location on map in relation to the center of the screen).
         * @return The focus point in world coordinates.
         */
        util::PointWorldCoord focusPointWorldCoord() const;

    public:

        /**
         * Fetches the focus point in world pixels (location on map in relation to the center of the screen).
         * @return the focus point in world pixels.
         */
        util::PointWorldPx focusPointWorldPx() const;

        /**
         * Fetches the visible viewport rect in world coordinatess.
         * @return the visible viewport rect in world coordinatess.
         */
        util::RectWorldCoord rectWorldCoord() const;

        /**
         * Converts a viewport pixel into a world pixel (uses the current world focus pixel).
         * @param viewport_px The viewport pixel to convert.
         * @return the world pixel.
         */
        util::PointWorldPx toPointWorldPx(const util::PointViewportPx& viewport_px) const;

        /**
         * Converts a viewport pixel into a world pixel.
         * @param viewport_px The viewport pixel to convert.
         * @param world_focus_px The world focus pixel to use.
         * @return the world pixel.
         */
        util::PointWorldPx toPointWorldPx(const util::PointViewportPx& viewport_px, const util::PointWorldPx& world_focus_px) const;

        /**
         * Converts a world pixel into a viewport pixel (uses the current world focus pixel).
         * @param world_px The world pixel to convert.
         * @return the viewport pixel.
         */
        util::PointViewportPx toPointViewportPx(const util::PointWorldPx& world_px) const;

        /**
         * Converts a world pixel into a viewport pixel.
         * @param world_px The world pixel to convert.
         * @param world_focus_px The world focus pixel to use.
         * @return the viewport pixel.
         */
        util::PointViewportPx toPointViewportPx(const util::PointWorldPx& world_px, const util::PointWorldPx& world_focus_px) const;

    protected:

        /**
         * Set the viewport size in pixels.
         * @param size_px The viewport size in pixels.
         */
        virtual void setSizePx(const QSizeF& size_px);

        /**
         * Sets the required projection.
         * @param projection_epsg The EPSG of projection required.
         */
        virtual void setProjection(const projection::EPSG& projection_epsg);

        /**
         * Set the zoom level required.
         * @param zoom The zoom level requested.
         */
        virtual void setZoom(int zoom);

        /**
         * Set the focus point to the given world coordinate.
         * @param world_coord The world coordinate to set the focus point on.
         */
        virtual void setFocusPointWorldCoord(const util::PointWorldCoord& world_coord);

    private:

        /// The viewport's size in pixels.
        QSizeF m_size_px;

        /// The viewport's projection.
        projection::EPSG m_projection_epsg;

        /// The viewport's tile size in pixels.
        const QSize m_tile_size_px;

        /// The viewport's zoom.
        int m_zoom { 0 };

        /// The viewport's focus point in world coordinates.
        util::PointWorldCoord m_focus_world_coord { 0.0, 0.0 };

    };

}
