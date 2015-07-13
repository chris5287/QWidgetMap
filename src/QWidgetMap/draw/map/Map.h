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

// Qt includes.
#include <QtCore/QUrl>

// STL includes.
#include <set>

// Local includes.
#include "../../qwidgetmap_global.h"
#include "../../projection/Projection.h"
#include "../Drawable.h"

/// QWidgetMap namespace.
namespace qwm
{

    /// Drawable namespace.
    namespace draw
    {

        /// Map namespace.
        namespace map
        {

            /// Captures the configuration details for a map service.
            struct QWIDGETMAP_EXPORT MapConfig
            {
                /// The base url path of the map server.
                const QUrl m_base_url;

                /// The supported EPSG projections.
                const std::set<projection::EPSG> m_projections_supported;
            };

            /**
             * Interface to be implemented to make a drawable map item.
             */
            class QWIDGETMAP_EXPORT Map : public Drawable
            {
                Q_OBJECT

            protected:

                /**
                 * This constructs a Map.
                 * @param config The config of the map server.
                 * @param parent QObject parent ownership.
                 */
                Map(const MapConfig& config, QObject* parent = nullptr);

            public:

                /// Disable copy constructor.
                Map(const Map&) = delete;

                /// Disable copy assignment.
                Map& operator=(const Map&) = delete;

                /// Destructor.
                virtual ~Map() = default;

            public:

                /**
                 * Get the base url.
                 * @return the base url.
                 */
                const QUrl& baseUrl() const;

            public:

                /**
                 * Fetches whether the drawable item is visible at a specific zoom level.
                 * @param viewport The viewport to check.
                 * @return whether the drawable item is visible at a specific zoom level.
                 */
                bool isVisible(const Viewport& viewport) const final;

            private:

                /// The map server configuration to use.
                const MapConfig m_config;

            };

        }

    }

}
