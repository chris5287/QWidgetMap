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

#include "Viewport.h"

// STL includes.
#include <utility>

using namespace qwm;

Viewport::Viewport(const QSizeF& size_px, const projection::EPSG& projection_epsg, const QSize& tile_size_px)
    : m_size_px(size_px),
      m_projection_epsg(projection_epsg),
      m_tile_size_px(tile_size_px)
{

}

QSizeF Viewport::sizePx() const
{
    // Returns the viewport's size in pixels.
    return m_size_px;
}

util::PointViewportPx Viewport::sizePointCenterPx() const
{
    // Return the viewport center in pixels.
    return util::PointViewportPx(m_size_px.width() / 2.0, m_size_px.height() / 2.0);
}

const projection::EPSG& Viewport::projection() const
{
    // Return the viewport's projection.
    return m_projection_epsg;
}

const QSize& Viewport::tileSizePx() const
{
    // Return the tile size in pixels.
    return m_tile_size_px;
}

int Viewport::zoom() const
{
    // Return the viewport's zoom.
    return m_zoom;
}

util::PointWorldCoord Viewport::focusPointWorldCoord() const
{
    // Return the viewport's focus point in world coordinates.
    return m_focus_world_coord;
}

util::PointWorldPx Viewport::focusPointWorldPx() const
{
    // Return the current focus point converted into world pixels.
    return projection::toPointWorldPx(*this, m_focus_world_coord);
}

util::RectWorldCoord Viewport::rectWorldCoord() const
{
    // Return the viewport rect converted into world coordinates.
    return util::RectWorldCoord(projection::toPointWorldCoord(*this, util::PointViewportPx { 0.0, 0.0 }), projection::toPointWorldCoord(*this, util::PointViewportPx { m_size_px.width(), m_size_px.height() }));
}

util::PointWorldPx Viewport::toPointWorldPx(const util::PointViewportPx& viewport_px) const
{
    // Return the viewport pixel converted into a world pixel (uses the current world focus px).
    return toPointWorldPx(viewport_px, focusPointWorldPx());
}

util::PointWorldPx Viewport::toPointWorldPx(const util::PointViewportPx& viewport_px, const util::PointWorldPx& world_focus_px) const
{
    /*
     * Notes:
     * viewport_px.x() and y() are the mouse pixel points in relation to the viewport (visible-part of the layer).
     * focus_world_px.x() and y() are the current focus pixel points in relation to the map control (whole layer).
     *
     * Explanation:
     *   0 1 2 3 4 5 6
     * 0 |-----------|  Outside box is Map Control (whole layer) and goes from 0,0 to 6,6
     * 1 | |---------|  Inside box is Viewport (visible-part of the layer) and goes from 0,0 to 4,4 (Center is 2,2)
     * 2 | |         |  Map Focus is 3,3 (ie: Viewport center on Map Control).
     * 3 | | X       |  X is at Viewport position 1,2
     * 4 | |         |
     * 5 | |         |  X at Map Control position = X is at Viewport position - Viewport Center + Map Focus
     * 6 |-|---------|  X at Map Control position = 1,2 - 2,2 + 3,3
     *                  X at Map Control position = 2,3
     */

    // Returns the viewport pixel converted into a world pixel (viewport px - viewport center px = delta from world focus px).
    return world_focus_px + (viewport_px - sizePointCenterPx());
}

util::PointViewportPx Viewport::toPointViewportPx(const util::PointWorldPx& world_px) const
{
    // Return the viewport pixel converted into a world pixel (uses the current world focus px).
    return toPointViewportPx(world_px, focusPointWorldPx());
}

util::PointViewportPx Viewport::toPointViewportPx(const util::PointWorldPx& world_px, const util::PointWorldPx& world_focus_px) const
{
    // Returns the world coordinate converted into a viewport pixel (world px - world focus px = delta from center viewport px).
    return sizePointCenterPx() + (world_px - world_focus_px);
}

void Viewport::setSizePx(const QSizeF& size_px)
{
    // Set the viewport's size (visible-part of the layer) in pixels.
    m_size_px = size_px;
}

void Viewport::setProjection(const projection::EPSG& projection_epsg)
{
    // Set the viewport's projection.
    m_projection_epsg = projection_epsg;
}

void Viewport::setZoom(int zoom)
{
    // Set the viewport's zoom.
    m_zoom = zoom;
}

void Viewport::setFocusPointWorldCoord(const util::PointWorldCoord& world_coord)
{
    // Set the viewport's focus point in world coordinates.
    m_focus_world_coord = world_coord;
}
