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

#include "GeometryPoint.h"

// Local includes.
#include "GeometryPolygon.h"
#include "../../projection/Projection.h"

using namespace qwm;
using namespace qwm::draw::geometry;

GeometryPoint::GeometryPoint(const util::PointWorldCoord& point_coord, QObject* parent)
    : Geometry(GeometryType::GeometryPoint, parent),
      m_point_coord(point_coord)
{

}

const util::PointWorldCoord& GeometryPoint::coord() const
{
    // Return the point to be displayed.
    return m_point_coord;
}

util::RectWorldCoord GeometryPoint::boundingBox(const Viewport& viewport) const
{
    // Calculate the world point in pixels.
    const util::PointWorldPx point_px(projection::toPointWorldPx(viewport, m_point_coord));

    // To ensure a point exists, give it a 1x1 size.
    const QSizeF object_size_px(1.0, 1.0);

    // Calculate the top-left point.
    const util::PointWorldPx top_left_point_px(calculateTopLeftPoint(point_px, AlignmentType::Middle, object_size_px));

    // Calculate the bottom-right point.
    const util::PointWorldPx bottom_right_point_px(top_left_point_px.x() + object_size_px.width(), top_left_point_px.y() + object_size_px.height());

    // Return the converted coord points.
    return util::RectWorldCoord(projection::toPointWorldCoord(viewport, top_left_point_px), projection::toPointWorldCoord(viewport, bottom_right_point_px));
}

bool GeometryPoint::touches(const Geometry& geometry, const Viewport& viewport) const
{
    // Default return success.
    bool return_touches(false);

    // Check we are visible.
    if(isVisible(viewport))
    {
        // Switch to the correct geometry type.
        switch(geometry.geometryType())
        {
            case GeometryType::GeometryEllipse:
            {
                // Set touches response based on ellipse vs point check.
                return_touches = geometry.touches(*this, viewport);

                // Finished.
                break;
            }
            case GeometryType::GeometryLineString:
            {
                // Set touches response based on line string vs point check.
                return_touches = geometry.touches(*this, viewport);

                // Finished.
                break;
            }
            case GeometryType::GeometryPoint:
            {
                // They have touched if the bounding boxes intersect.
                return_touches = geometry.boundingBox(viewport).intersects(boundingBox(viewport));

                // Finished.
                break;
            }
            case GeometryType::GeometryPolygon:
            {
                // Set touches response based on polygon vs point check.
                return_touches = geometry.touches(*this, viewport);

                // Finished.
                break;
            }
        }
    }

    // Return our success.
    return return_touches;
}

void GeometryPoint::draw(QPainter& painter, const util::RectWorldCoord& /*drawing_rect_world_coord*/, const Viewport& viewport) const
{
    // Calculate the point in pixels.
    const util::PointWorldPx point_px(projection::toPointWorldPx(viewport, m_point_coord));

    // Set the pen to use.
    painter.setPen(pen());

    // Draw the point.
    painter.drawPoint(point_px);
}
