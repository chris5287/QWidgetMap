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

#include "ProjectionEquirectangular.h"

// STL includes.
#include <cmath>

// Local include.
#include "../Viewport.h"
#include "../util/ImageManager.h"

using namespace qwm;
using namespace qwm::projection;

EPSG ProjectionEquirectangular::epsg() const
{
    // Retun the EPSG enum.
    return EPSG::Equirectangular;
}

int ProjectionEquirectangular::epsgNumber() const
{
    // Return the recognised EPSG number - 4326.
    return 4326;
}

qreal ProjectionEquirectangular::worldWidthPx(const Viewport& viewport) const
{
    // Return the number of tiles for the x-axis * tile size width.
    return (std::pow(2, viewport.zoom()) * 2) * viewport.tileSizePx().width();
}

qreal ProjectionEquirectangular::worldHeightPx(const Viewport& viewport) const
{
    // Return the number of tiles for the y-axis * tile size height.
    return std::pow(2, viewport.zoom()) * viewport.tileSizePx().height();
}

util::PointWorldPx ProjectionEquirectangular::toPointWorldPx(const Viewport& viewport, const util::PointWorldCoord& point_coord) const
{
    // Convert from coordinate to pixel by - top/left delta, then ratio of coords per pixel.
    const qreal x_px((point_coord.longitude() + 180.0) * worldWidthPx(viewport) / 360.0);
    const qreal y_px(-(point_coord.latitude() - 90.0) * worldHeightPx(viewport) / 180.0);

    // Return the converted point (x/y pixel point - 0,0 is screen top left).
    return util::PointWorldPx(x_px, y_px);
}

util::PointWorldCoord ProjectionEquirectangular::toPointWorldCoord(const Viewport& viewport, const util::PointWorldPx& point_px) const
{
    // Convert pixel into coordinate by * against ratio of pixels per coord, then + top/left delta offset.
    const qreal longitude((point_px.x() * 360.0 / worldWidthPx(viewport)) - 180.0);
    const qreal latitude(-(point_px.y() * 180.0 / worldHeightPx(viewport)) + 90.0);

    // Return the converted coordinate (longitude/latitude coordinate - 0,0 is screen middle).
    return util::PointWorldCoord(longitude, latitude);
}
