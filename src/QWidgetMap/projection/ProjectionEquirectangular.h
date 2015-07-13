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

// Local includes.
#include "../qwidgetmap_global.h"
#include "Projection.h"

/// QWidgetMap namespace.
namespace qwm
{

    /// Projection namespace.
    namespace projection
    {

        /**
         * Implementation of the Equirectangular projection.
         */
        class QWIDGETMAP_EXPORT ProjectionEquirectangular : public Projection
        {

        public:

            /**
             * Projection Equirectangular (EPSG:4326 - lat/long) constructor.
             */
            ProjectionEquirectangular() = default;

            /// Disable copy constructor.
            ProjectionEquirectangular(const ProjectionEquirectangular&) = delete;

            /// Disable copy assignment.
            ProjectionEquirectangular& operator=(const ProjectionEquirectangular&) = delete;

            /// Destructor.
            ~ProjectionEquirectangular() = default;

        public:

            /**
             * Fetch the EPSG enum for the projection.
             * @return the EPSG enum for the projection.
             */
            EPSG epsg() const final;

            /**
             * Fetch the recognised EPSG number for the projection.
             * @return the recognised EPSG number for the projection.
             */
            int epsgNumber() const final;

            /**
             * The width of the world in pixels for the specified viewport.
             * @param viewport The viewport to use.
             * @return width of the world in pixels for the specified viewport
             */
            qreal worldWidthPx(const Viewport& viewport) const final;

            /**
             * The height of the world in pixels for the specified viewport
             * @param viewport The viewport to use.
             * @return height of the world in pixels for the specified viewport
             */
            qreal worldHeightPx(const Viewport& viewport) const final;

            /**
             * Converts a world coorindate point (longitude/latitude) into the pixel point for a given zoom.
             * @param viewport The viewport to use.
             * @param point_coord The world coordinate point to convert (longitude/latitude).
             * @return the converted world pixel point.
             */
            util::PointWorldPx toPointWorldPx(const Viewport& viewport, const util::PointWorldCoord& point_coord) const final;

            /**
             * Converts a world pixel point into the coorindate point (longitude/latitude) for a given zoom.
             * @param viewport The viewport to use.
             * @param point_px The world pixel point to convert.
             * @return the converted world coorindate point (longitude/latitude).
             */
            util::PointWorldCoord toPointWorldCoord(const Viewport& viewport, const util::PointWorldPx& point_px) const final;

        };

    }

}
