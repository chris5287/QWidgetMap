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

#include "Projection.h"

// Local includes.
#include "ProjectionEquirectangular.h"
#include "ProjectionSphericalMercator.h"
#include "../Viewport.h"

#include <stdexcept>

using namespace qwm;

namespace
{
    /// Instance of the Equirectangular projection.
    const projection::ProjectionEquirectangular m_projection_equirectangular { };

    /// Instance of the Spherical Mercator projection.
    const projection::ProjectionSphericalMercator m_projection_spherical_mercator { };

    /**
     * Fetches the projection.
     * @param viewport The viewport to use.
     * @return The projection.
     */
    const projection::Projection& fetch(const Viewport& viewport)
    {
        // Return the viewport's projection.
        switch(viewport.projection())
        {
            case projection::EPSG::Equirectangular:
            {
                return m_projection_equirectangular;
            }
            case projection::EPSG::SphericalMercator:
            {
               return m_projection_spherical_mercator;
            }
        }

        // Throw that we do not support this projection.
        throw std::runtime_error("Projection not supported");
    }
}

int projection::epsgNumber(const Viewport& viewport)
{
    // Return the EPSG number.
    return fetch(viewport).epsgNumber();
}

qreal projection::worldWidthPx(const Viewport& viewport)
{
    // Return the world width in pixels.
    return fetch(viewport).worldWidthPx(viewport);
}

qreal projection::worldHeightPx(const Viewport& viewport)
{
    // Return the world height in pixels.
    return fetch(viewport).worldHeightPx(viewport);
}

util::PointViewportPx projection::toPointViewportPx(const Viewport& viewport, const util::PointWorldCoord& world_coord)
{
    // Returns the world cooridate converted into a viewport pixel.
    return viewport.toPointViewportPx(fetch(viewport).toPointWorldPx(viewport, world_coord));
}

util::PointWorldPx projection::toPointWorldPx(const Viewport& viewport, const util::PointWorldCoord& world_coord)
{
    // Returns the world coordinate converted into a world pixel.
    return fetch(viewport).toPointWorldPx(viewport, world_coord);
}

util::PointWorldCoord projection::toPointWorldCoord(const Viewport& viewport, const util::PointViewportPx& viewport_px)
{
    // Return the viewport pixel converted into a world coordinate (uses the current world focus px).
    return toPointWorldCoord(viewport, viewport.toPointWorldPx(viewport_px, viewport.focusPointWorldPx()));
}

util::PointWorldCoord projection::toPointWorldCoord(const Viewport& viewport, const util::PointWorldPx& world_px)
{
    // Returns the world pixel convereted into a world coordinate.
    return fetch(viewport).toPointWorldCoord(viewport, world_px);
}
