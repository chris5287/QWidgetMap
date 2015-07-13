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
#include "../../qwidgetmap_global.h"
#include "GeometryPoint.h"

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
             * Implementation of the drawable point geometry item that displays text.
             */
            class QWIDGETMAP_EXPORT GeometryPointText : public GeometryPoint
            {

            public:

                /**
                 * This constructor creates a point which will display the text specified.
                 * @param point_coord The point to draw the text at (world coordinates).
                 * @param text The text to draw.
                 * @param parent QObject parent ownership.
                 */
                GeometryPointText(const util::PointWorldCoord& point_coord, const std::string& text, QObject* parent = nullptr);

                /// Disable copy constructor.
                GeometryPointText(const GeometryPointText&) = delete;

                /// Disable copy assignment.
                GeometryPointText& operator=(const GeometryPointText&) = delete;

                /// Destructor.
                ~GeometryPointText() = default;

            public:

                /**
                 * Draws the item to the provided painter.
                 * @param painter The painter to draw on.
                 * @param drawing_rect_world_coord The drawing rect in world coordinates.
                 * @param viewport The current viewport to use.
                 */
                void draw(QPainter& painter, const util::RectWorldCoord& drawing_rect_world_coord, const Viewport& viewport) const final;

            private:

                /// Text to be drawn.
                const std::string m_text;

            };

        }

    }

}
