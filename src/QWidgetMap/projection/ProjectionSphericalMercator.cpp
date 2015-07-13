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

#include "ProjectionSphericalMercator.h"

// STL includes.
#include <cmath>

// Local include.
#include "../Viewport.h"
#include "../util/ImageManager.h"

using namespace qwm;
using namespace qwm::projection;

EPSG ProjectionSphericalMercator::epsg() const
{
    // Retun the EPSG enum.
    return EPSG::SphericalMercator;
}

int ProjectionSphericalMercator::epsgNumber() const
{
    // Return the recognised EPSG number - 3857.
    return 3857;
}

qreal ProjectionSphericalMercator::worldWidthPx(const Viewport& viewport) const
{
    // Return the number of tiles for the x-axis * tile size width.
    return std::pow(2, viewport.zoom()) * viewport.tileSizePx().width();
}

qreal ProjectionSphericalMercator::worldHeightPx(const Viewport& viewport) const
{
    // Return the number of tiles for the y-axis * tile size height.
    return std::pow(2, viewport.zoom()) * viewport.tileSizePx().height();
}

util::PointWorldPx ProjectionSphericalMercator::toPointWorldPx(const Viewport& viewport, const util::PointWorldCoord& point_coord) const
{
    /**
      Formula basis: http://wiki.openstreetmap.org/wiki/Slippy_map_tilenames#Lon..2Flat._to_tile_numbers
        n = 2 ^ display_zoom * tile_size (as lat/lon is actual point, we need to multiple by tile size).
        xtile = n * ((lon_deg + 180) / 360) : floor the answer?
        ytile = n * (1 - (log(tan(lat_rad) + sec(lat_rad)) / π)) / 2 : floor the answer?
     */
    const qreal x_px(worldWidthPx(viewport) * ((point_coord.longitude() + 180.0) / 360.0));
    const qreal y_px(worldHeightPx(viewport) * (1.0 - (std::log(std::tan(point_coord.latitude() * M_PI / 180.0) + (1.0 / std::cos(point_coord.latitude() * M_PI / 180.0))) / M_PI )) / 2.0);

    // Return the converted point (x/y pixel point - 0,0 is screen top left).
    return util::PointWorldPx(x_px, y_px);
}

util::PointWorldCoord ProjectionSphericalMercator::toPointWorldCoord(const Viewport& viewport, const util::PointWorldPx& point_px) const
{
    /**
      Formula basis: http://wiki.openstreetmap.org/wiki/Slippy_map_tilenames#Tile_numbers_to_lon..2Flat.
        n = 2 ^ display_zoom * tile_size (as x/y is pixel point and not tile number, we need to multiple by tile size).
        lon_deg = x_point / n * 360.0 - 180.0
        lat_rad = arctan(sinh(π * (1 - 2 * y_point / n)))
        lat_deg = lat_rad * 180.0 / π
     */
    const qreal longitude(point_px.x() / worldWidthPx(viewport) * 360.0 - 180.0);
    const qreal latitude(std::atan(std::sinh(M_PI * (1.0 - 2.0 * point_px.y() / worldHeightPx(viewport)))) * 180.0 / M_PI);

    // Return the converted coordinate (longitude/latitude coordinate - 0,0 is screen middle).
    return util::PointWorldCoord(longitude, latitude);
}
