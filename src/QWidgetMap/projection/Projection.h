/**
 * @copyright 2015 Chris Stylianou
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
#include <QtCore/QSize>

// Local includes.
#include "../qwidgetmap_global.h"
#include "../util/Point.h"

/// QWidgetMap namespace.
namespace qwm
{

    // Forward-declare viewport details.
    class Viewport;

    /// Projection namespace.
    namespace projection
    {

        /// Projection types.
        enum class EPSG
        {
            /// Equirectangular (EPSG:4326 - lat/long).
            Equirectangular = 4326,

            /// SphericalMercator (EPSG:3857 - meters)
            SphericalMercator = 3857
        };

        /**
         * Interface to be implemented to support a projection system.
         */
        class QWIDGETMAP_EXPORT Projection
        {

        public:

            /// Virtual Destructor.
            virtual ~Projection() = default;

        public:

            /**
             * Fetch the EPSG enum for the projection.
             * @return the EPSG enum for the projection.
             */
            virtual EPSG epsg() const = 0;

            /**
             * Fetch the recognised EPSG number for the projection.
             * @return the recognised EPSG number for the projection.
             */
            virtual int epsgNumber() const = 0;

            /**
             * The width of the world in pixels for the specified viewport.
             * @param viewport The viewport to use.
             * @return width of the world in pixels for the specified viewport
             */
            virtual qreal worldWidthPx(const Viewport& viewport) const = 0;

            /**
             * The height of the world in pixels for the specified viewport
             * @param viewport The viewport to use.
             * @return height of the world in pixels for the specified viewport
             */
            virtual qreal worldHeightPx(const Viewport& viewport) const = 0;

            /**
             * Converts a world coorindate point (longitude/latitude) into the pixel point for a given zoom.
             * @param viewport The viewport to use.
             * @param point_coord The world coordinate point to convert (longitude/latitude).
             * @return the converted world pixel point.
             */
            virtual util::PointWorldPx toPointWorldPx(const Viewport& viewport, const util::PointWorldCoord& point_coord) const = 0;

            /**
             * Converts a world pixel point into the coorindate point (longitude/latitude) for a given zoom.
             * @param viewport The viewport to use.
             * @param point_px The world pixel point to convert.
             * @return the converted world coorindate point (longitude/latitude).
             */
            virtual util::PointWorldCoord toPointWorldCoord(const Viewport& viewport, const util::PointWorldPx& point_px) const = 0;

        };

        /// @note these functions only support flat projections (eg: Equirectangular/SphericalMercator).

        /**
         * Fetch the recognised EPSG number for the projection.
         * @return the recognised EPSG number for the projection.
         */
        QWIDGETMAP_EXPORT int epsgNumber(const Viewport& viewport);

        /**
         * The width of the world in pixels for the specified viewport.
         * @param viewport The viewport to use.
         * @return width of the world in pixels for the specified viewport
         */
        QWIDGETMAP_EXPORT qreal worldWidthPx(const Viewport& viewport);

        /**
         * The height of the world in pixels for the specified viewport
         * @param viewport The viewport to use.
         * @return height of the world in pixels for the specified viewport
         */
        QWIDGETMAP_EXPORT qreal worldHeightPx(const Viewport& viewport);

        /**
         * Converts a world coordinate into viewport pixel.
         * @param viewport The viewport to use.
         * @param world_coord The world coordinate to convert.
         * @return the viewport pixel.
         */
        QWIDGETMAP_EXPORT util::PointViewportPx toPointViewportPx(const Viewport& viewport, const util::PointWorldCoord& world_coord);

        /**
         * Converts a world coorindate into a world pixel.
         * @param viewport The viewport to use.
         * @param world_coord The world coordinate to convert.
         * @return the world pixel.
         */
        QWIDGETMAP_EXPORT util::PointWorldPx toPointWorldPx(const Viewport& viewport, const util::PointWorldCoord& world_coord);

        /**
         * Converts a viewport pixel into a world coordinate (uses the current world focus pixel).
         * @param viewport The viewport to use.
         * @param viewport_px The viewport pixel to convert.
         * @return the world coordinate.
         */
        QWIDGETMAP_EXPORT util::PointWorldCoord toPointWorldCoord(const Viewport& viewport, const util::PointViewportPx& viewport_px);

        /**
         * Converts a world pixel into a world coorindate.
         * @param viewport The viewport to use.
         * @param world_px The world pixel to convert.
         * @return the world coorindate.
         */
        QWIDGETMAP_EXPORT util::PointWorldCoord toPointWorldCoord(const Viewport& viewport, const util::PointWorldPx& world_px);

    }

}
