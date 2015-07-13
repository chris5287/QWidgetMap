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
#include "Map.h"

/// QWidgetMap namespace.
namespace qwm
{

    /// Drawable namespace.
    namespace draw
    {

        /// Map namespace.
        namespace map
        {

            /// Captures the configuration details for a map tile service.
            struct QWIDGETMAP_EXPORT MapTileConfig
            {
                /// Map config.
                const MapConfig m_config;

                /// The offset to apply to the zoom level.
                const int m_local_zoom_offset;

                /// Whether the zoom should be inverted.
                const bool m_local_zoom_inverted;
            };

            /**
             * Implementation of the drawable map tile service.
             */
            class QWIDGETMAP_EXPORT MapTile : public Map
            {
                Q_OBJECT

            public:

                /**
                 * This constructs a Map for Tiled Map (Slippymap) Services.
                 * @param tile_config The config of the map tile server.
                 * @param parent QObject parent ownership.
                 */
                MapTile(const MapTileConfig& tile_config, QObject* parent = nullptr);

                /**
                 * This constructs a Map for Tiled Map (Slippymap) Services.
                 * @param base_url The base url of the map server.
                 * @param projections_supported The supported EPSG projections.
                 * @param local_zoom_offset The offset to apply to the zoom level.
                 * @param local_zoom_inverted Whether the zoom should be inverted.
                 * @param parent QObject parent ownership.
                 */
                MapTile(const QUrl& base_url, const std::set<projection::EPSG>& projections_supported, const int& local_zoom_offset = 0, const bool& local_zoom_inverted = false, QObject* parent = nullptr)
                    : MapTile(MapTileConfig { MapConfig { base_url, projections_supported}, local_zoom_offset, local_zoom_inverted }, parent)
                { }

                /// Disable copy constructor.
                MapTile(const MapTile&) = delete;

                /// Disable copy assignment.
                MapTile& operator=(const MapTile&) = delete;

                /// Destructor.
                virtual ~MapTile() = default;

            public:

                /**
                 * Draws ESRI Shapefiles to a pixmap using the provided painter.
                 * @param painter The painter to draw on.
                 * @param drawing_rect_world_coord The drawing rect in world coordinates.
                 * @param viewport The current viewport to use.
                 */
                void draw(QPainter& painter, const util::RectWorldCoord& drawing_rect_world_coord, const Viewport& viewport) const final;

            private:

                /**
                 * Generates the url required to fetch the image tile for the specified x, y and zoom.
                 * @param x The x coordinate required.
                 * @param y The y coordinate required.
                 * @param zoom The zoom level to use.
                 * @return the generated url.
                 */
                QUrl tileQuery(const int& x, const int& y, const int& zoom) const;

                /**
                 * Translates the viewport zoom in the map local zoom.
                 * @param zoom The zoom level to translate.
                 * @return the map local zoom.
                 */
                int toLocalZoom(const int& zoom) const;

            private:

                /// The map tile server configuration to use.
                const MapTileConfig m_tile_config;

            };

        }

    }

}
