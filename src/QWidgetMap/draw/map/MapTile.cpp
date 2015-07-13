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

#include "MapTile.h"

// STL includes.
#include <cmath>

// Local includes.
#include "../../util/ImageManager.h"

using namespace qwm;
using namespace qwm::draw::map;

MapTile::MapTile(const MapTileConfig& tile_config, QObject* parent)
    : Map(tile_config.m_config, parent),
      m_tile_config(tile_config)
{

}

void MapTile::draw(QPainter& painter, const util::RectWorldCoord& drawing_rect_world_coord, const Viewport& viewport) const
{
    // Check we have a base url to fetch map!
    if(baseUrl().isEmpty())
    {
        // We cannot fetch map tiles as no base url set!
        qDebug() << "Map adapater base url is empty!";
    }
    else
    {
        // Convert world rect from coords to pixels.
        const auto top_left_world_px(projection::toPointWorldPx(viewport, drawing_rect_world_coord.topLeftCoord()));
        const auto bottom_right_world_px(projection::toPointWorldPx(viewport, drawing_rect_world_coord.bottomRightCoord()));

        // Calculate required tiles.
        const int furthest_tile_left(std::floor(top_left_world_px.x() / viewport.tileSizePx().width()));
        const int furthest_tile_top(std::floor(top_left_world_px.y() / viewport.tileSizePx().height()));
        const int furthest_tile_right(furthest_tile_left + std::ceil((bottom_right_world_px.x() - (furthest_tile_left  * viewport.tileSizePx().width())) / viewport.tileSizePx().width()));
        const int furthest_tile_bottom(furthest_tile_top + std::ceil((bottom_right_world_px.y() - (furthest_tile_top  * viewport.tileSizePx().height())) / viewport.tileSizePx().height()));

        // Loop through the tiles to draw (left to right).
        for(int x = furthest_tile_left; x <= furthest_tile_right; ++x)
        {
            // Loop through the tiles to draw (top to bottom).
            for(int y = furthest_tile_top; y <= furthest_tile_bottom; ++y)
            {
                // Check that we are fetching valid tile numbers.
                if(x >= 0 && y >= 0)
                {
                    // Calculate the top left point.
                    const util::PointWorldPx top_left_px(x * viewport.tileSizePx().width(), y * viewport.tileSizePx().height());

                    // Draw the tile.
                    painter.drawPixmap(top_left_px, util::ImageManager::get().image(tileQuery(x, y, viewport.zoom()), viewport.tileSizePx()));
                }
            }
        }

        // Prefetch the next set of rows/column tiles (ready for when the user starts panning).
        const int prefetch_tile_left(furthest_tile_left - 1);
        const int prefetch_tile_top(furthest_tile_top - 1);
        const int prefetch_tile_right(furthest_tile_right + 1);
        const int prefetch_tile_bottom(furthest_tile_bottom + 1);

        // Fetch the top/bottom rows.
        for(int x = prefetch_tile_left; x <= prefetch_tile_right; ++x)
        {
            // Check that we are fetching valid tile numbers.
            if(x >= 0 && prefetch_tile_top >= 0 && prefetch_tile_bottom >= 0)
            {
                // Prefetch the tiles.
                util::ImageManager::get().prefetchImage(tileQuery(x, prefetch_tile_top, viewport.zoom()), viewport.tileSizePx());
                util::ImageManager::get().prefetchImage(tileQuery(x, prefetch_tile_bottom, viewport.zoom()), viewport.tileSizePx());
            }
        }

        // Fetch the left/right columns.
        for(int y = prefetch_tile_top; y <= prefetch_tile_bottom; ++y)
        {
            // Check that we are fetching valid tile numbers.
            if(y >= 0 && prefetch_tile_left >= 0 && prefetch_tile_right >= 0)
            {
                // Prefetch the tiles.
                util::ImageManager::get().prefetchImage(tileQuery(prefetch_tile_left, y, viewport.zoom()), viewport.tileSizePx());
                util::ImageManager::get().prefetchImage(tileQuery(prefetch_tile_right, y, viewport.zoom()), viewport.tileSizePx());
            }
        }
    }
}

QUrl MapTile::tileQuery(const int& x, const int& y, const int& zoom) const
{
    // Return a modified url with the %x, %y and %zoom values replaced.
    /// @note QUrl converts % into %25, so we replace %25x, %25y and %25zoom instead.
    return QUrl(baseUrl().toString()
                .replace("%25x", QString::number(x))
                .replace("%25y", QString::number(y))
                .replace("%25zoom", QString::number(toLocalZoom(zoom))));
}

int MapTile::toLocalZoom(const int& zoom) const
{
    // Default return zoom is viewport - offset.
    int return_local_zoom(zoom + m_tile_config.m_local_zoom_offset);

    // Does the map local zoom need to be inverted?
    if(m_tile_config.m_local_zoom_inverted)
    {
        // Zoom is 17 - viewport - offset (as minimum is larger number!).
        return_local_zoom = 17 - zoom - m_tile_config.m_local_zoom_offset;
    }

    // Return the local zoom.
    return return_local_zoom;
}
