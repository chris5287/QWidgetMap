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
             * Implementation of the drawable ellipse geometry item.
             */
            class QWIDGETMAP_EXPORT GeometryEllipse : public GeometryFixed
            {
                Q_OBJECT

            public:

                /**
                 * This constructor creates an Ellipse to be displayed.
                 * @param point The center point of the ellipse (world coordinates).
                 * @param semi_major_m The semi-major distance in meters.
                 * @param semi_minor_m The semi-minor distance in meters.
                 * @param bearing_deg The semi-major bearing in degrees.
                 * @param parent QObject parent ownership.
                 */
                GeometryEllipse(const util::PointWorldCoord& point, const double& semi_major_m, const double& semi_minor_m, const double& bearing_deg, QObject* parent = nullptr);

                /// Disable copy constructor.
                GeometryEllipse(const GeometryEllipse&) = delete;

                /// Disable copy assignment.
                GeometryEllipse& operator=(const GeometryEllipse&) = delete;

                /// Destructor.
                virtual ~GeometryEllipse() = default;

            public:

                /**
                 * Fetches the center point of the ellipse (world coordinates).
                 * @return the center point of the ellipse (world coordinates).
                 */
                const util::PointWorldCoord& centerCoord() const;

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

            public:

                /**
                 * Helper function to draw a projected circle preview.
                 * @param painter The painter to draw on.
                 * @param viewport The current viewport to use.
                 * @param mouse_position_pressed_px The position of the mouse press.
                 */
                void drawPreview(QPainter& painter, const Viewport& viewport, const util::PointViewportPx& mouse_position_pressed_px) const;

            private:

                /// The location of the semi-major point (0 degrees).
                util::PointWorldCoord m_point_semi_major { 0.0, 0.0 };

                /// The location of the semi-minor point (+90 degrees).
                util::PointWorldCoord m_point_semi_minor { 0.0, 0.0 };

                /// The fixed bounding box (calcualted on construction).
                util::RectWorldCoord m_bounding_box_fixed { util::PointWorldCoord { 0.0, 0.0 }, util::PointWorldCoord { 0.0, 0.0 } };

                /// The location of the center point.
                util::PointWorldCoord m_point_center;

                /// The semi-major bearing in degrees.
                double m_bearing_deg;

            };

        }

    }

}
