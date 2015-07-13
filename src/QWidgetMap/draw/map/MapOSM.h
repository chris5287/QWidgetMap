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
#include "MapTile.h"

/// QWidgetMap namespace.
namespace qwm
{

    /// Drawable namespace.
    namespace draw
    {

        /// Map namespace.
        namespace map
        {

            /**
             * Implementation of the drawable OpenStreetMap map tile service.
             */
            class QWIDGETMAP_EXPORT MapOSM : public MapTile
            {
                Q_OBJECT

            public:

                /**
                 * This constructs a OpenStreetMap Map.
                 * @param parent QObject parent ownership.
                 */
                explicit MapOSM(QObject* parent = nullptr);

                /// Disable copy constructor.
                MapOSM(const MapOSM&) = delete;

                /// Disable copy assignment.
                MapOSM& operator=(const MapOSM&) = delete;

                /// Destructor.
                ~MapOSM() = default;

            };

        }

    }

}
