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

// STL includes.
#include <vector>

// Local includes.
#include "../../qwidgetmap_global.h"
#include "GeometryFixed.h"

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
             * Implementation of the drawable polygon geometry item.
             */
            class QWIDGETMAP_EXPORT GeometryPolygon : public GeometryFixed
            {
                Q_OBJECT

            public:

                /**
                 * This constructor takes a list of points, which form a polygon, to be displayed.
                 * @param points The list of points (world coordinates).
                 * @param parent QObject parent ownership.
                 */
                explicit GeometryPolygon(const std::vector<util::PointWorldCoord>& points, QObject* parent = nullptr);

                /// Disable copy constructor.
                GeometryPolygon(const GeometryPolygon&) = delete;

                /// Disable copy assignment.
                GeometryPolygon& operator=(const GeometryPolygon&) = delete;

                /// Destructor.
                virtual ~GeometryPolygon() = default;

            public:

                /**
                 * Fetches the list of points that form the polygon (world coordinates).
                 * @return the list of points that form the polygon (world coordinates).
                 */
                std::vector<util::PointWorldCoord> points() const;

                /**
                 * Fetches the QPolygonF representation of the polygon.
                 * @return the QPolygonF representation of the polygon.
                 */
                const QPolygonF& toQPolygonF() const;

            public:

                /**
                 * Fetches the fixed bounding box (world coordinates).
                 * @return the fixed bounding box.
                 */
                const util::RectWorldCoord& boundingBoxFixed() const final;

            public:

                /**
                 * Fetches the bounding box (world coordinates).
                 * @param viewport The current viewport to use.
                 * @return the bounding box.
                 */
                util::RectWorldCoord boundingBox(const Viewport& viewport) const final;

                /**
                 * Checks if the geometry touches (intersects) with another geometry.
                 * @param geometry The geometry to check against.
                 * @param viewport The current viewport to use.
                 * @return whether the geometries touch (intersects).
                 */
                bool touches(const Geometry& geometry, const Viewport& viewport) const final;

                /**
                 * Draws the item to the provided painter.
                 * @param painter The painter to draw on.
                 * @param drawing_rect_world_coord The drawing rect in world coordinates.
                 * @param viewport The current viewport to use.
                 */
                void draw(QPainter& painter, const util::RectWorldCoord& drawing_rect_world_coord, const Viewport& viewport) const final;

            private:

                /// The points that the polygon is made up of.
                std::vector<util::PointWorldCoord> m_points;

                /// The QPolygonF structure of the polygon points (calcualted on construction).
                QPolygonF m_qpolygonf;

                /// The fixed bounding box (calcualted on construction).
                util::RectWorldCoord m_bounding_box_fixed { util::PointWorldCoord { 0.0, 0.0 }, util::PointWorldCoord { 0.0, 0.0 } };

            };

        }

    }

}
