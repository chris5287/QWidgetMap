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

#include "Map.h"

using namespace qwm;
using namespace qwm::draw::map;

Map::Map(const MapConfig& config, QObject* parent)
    : Drawable(DrawableType::Map, parent),
      m_config(config)
{

}

const QUrl& Map::baseUrl() const
{
    // Return the url.
    return m_config.m_base_url;
}

bool Map::isVisible(const Viewport& viewport) const
{
    // Default visibility.
    bool visibility(Drawable::isVisible(viewport));

    // Are we still visible?
    if(visibility)
    {
        // Returns whether the projection is supported.
        visibility = (m_config.m_projections_supported.find(viewport.projection()) != m_config.m_projections_supported.end());
    }

    // Return the visibility.
    return visibility;
}
