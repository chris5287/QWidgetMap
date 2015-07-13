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
             * Implementation of the drawable Google map tile service.
             */
            class QWIDGETMAP_EXPORT MapGoogle : public MapTile
            {
                Q_OBJECT

            public:

                /// Google layer types.
                enum class GoogleLayerType
                {
                    /// Map layer.
                    MAPS,

                    /// Satelite layer.
                    SATELLITE,

                    /// Terrain layer.
                    TERRAIN,

                    /// Hypnrid layer.
                    HYBRID,

                    /// Ratser layer.
                    RASTER
                };

            public:

                /**
                 * This constructs a Google Map.
                 * @param layer_type The Google layer type to display.
                 * @param parent QObject parent ownership.
                 */
                explicit MapGoogle(const GoogleLayerType& layer_type = GoogleLayerType::MAPS, QObject* parent = nullptr);

                /// Disable copy constructor.
                MapGoogle(const MapGoogle&) = delete;

                /// Disable copy assignment.
                MapGoogle& operator=(const MapGoogle&) = delete;

                /// Destructor.
                ~MapGoogle() = default;

            private:

                /**
                 * Convert the enum to a 1-character representation.
                 * @param layer_type The layer type to convert.
                 * @return the 1-character representation.
                 */
                QString layerTypeToString(const MapGoogle::GoogleLayerType& layer_type);

            };

        }

    }

}
