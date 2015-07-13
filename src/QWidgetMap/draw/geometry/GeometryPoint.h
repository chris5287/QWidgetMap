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
#include "../../qwidgetmap_global.h"
#include "Geometry.h"

/// QWidgetMap namespace.
namespace qwm
{

    /// Drawable namespace.
    namespace draw
    {

        /// Geometry namespace.
        namespace geometry
        {

            /**
             * Implementation of the drawable point geometry item.
             */
            class QWIDGETMAP_EXPORT GeometryPoint : public Geometry
            {
                Q_OBJECT

            public:

                /**
                 * This constructor creates a point to be displayed.
                 * @param point_coord The point to be displayed (world coordinates).
                 * @param parent QObject parent ownership.
                 */
                explicit GeometryPoint(const util::PointWorldCoord& point_coord, QObject* parent = nullptr);

                /// Disable copy constructor.
                GeometryPoint(const GeometryPoint&) = delete;

                /// Disable copy assignment.
                GeometryPoint& operator=(const GeometryPoint&) = delete;

                /// Destructor.
                virtual ~GeometryPoint() = default;

            public:

                /**
                 * Fetches the point to be displayed (world coordinates).
                 * @return the point to be displayed (world coordinates).
                 */
                const util::PointWorldCoord& coord() const;

            public:

                /**
                 * Fetches the bounding box (world coordinates).
                 * @param viewport The current viewport to use.
                 * @return the bounding box.
                 */
                virtual util::RectWorldCoord boundingBox(const Viewport& viewport) const override;

                /**
                 * Checks if the geometry touches (intersects) with another geometry.
                 * @param geometry The geometry to check against.
                 * @param viewport The current viewport to use.
                 * @return whether the geometries touch (intersects).
                 */
                virtual bool touches(const Geometry& geometry, const Viewport& viewport) const override;

                /**
                 * Draws the item to the provided painter.
                 * @param painter The painter to draw on.
                 * @param drawing_rect_world_coord The drawing rect in world coordinates.
                 * @param viewport The current viewport to use.
                 */
                void draw(QPainter& painter, const util::RectWorldCoord& drawing_rect_world_coord, const Viewport& viewport) const override;

            private:

                /// The point to be displayed (world coordinates).
                const util::PointWorldCoord m_point_coord;

            };

        }

    }

}
