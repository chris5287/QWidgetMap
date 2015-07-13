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
             * Interface to be implemented to make a drawable geometry item that has a fixed boundary box.
             */
            class QWIDGETMAP_EXPORT GeometryFixed : public Geometry
            {

            protected:

                /**
                 * This constructs a Geometry.
                 * @param geometry_type The geometry type.
                 * @param parent QObject parent ownership.
                 */
                explicit GeometryFixed(const GeometryType& geometry_type, QObject* parent = nullptr);

            public:

                /// Disable copy constructor.
                GeometryFixed(const GeometryFixed&) = delete;

                /// Disable copy assignment.
                GeometryFixed& operator=(const GeometryFixed&) = delete;

                /// Destructor.
                virtual ~GeometryFixed() = default;

            public:

                /**
                 * Fetches the fixed bounding box (world coordinates).
                 * @return the fixed bounding box.
                 */
                virtual const util::RectWorldCoord& boundingBoxFixed() const = 0;

            };

        }

    }

}
