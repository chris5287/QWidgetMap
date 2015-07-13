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
#include "GeometryPointImage.h"

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
             * Implementation of the drawable point geometry item that displays an arrow shape.
             */
            class QWIDGETMAP_EXPORT GeometryPointArrow : public GeometryPointImage
            {

            public:

                /**
                 * This constructor creates a point which will display an image pixmap with an arrow.
                 * @param point_coord The point (world coordinates).
                 * @param size_px The arrow size in pixels.
                 * @param parent QObject parent ownership.
                 */
                explicit GeometryPointArrow(const util::PointWorldCoord& point_coord, const QSizeF& size_px = QSizeF(10.0, 10.0), QObject* parent = nullptr);

                /// Disable copy constructor.
                GeometryPointArrow(const GeometryPointArrow&) = delete;

                /// Disable copy assignment.
                GeometryPointArrow& operator=(const GeometryPointArrow&) = delete;

                /// Destructor.
                ~GeometryPointArrow() = default;

            protected:

                /**
                 * Updates the shape (draws an arrow on to the image pixmap).
                 */
                void updateShape() final;

            };

        }

    }

}
