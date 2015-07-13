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

#include "GeometryPolygon.h"

// Local includes.
#include "../../projection/Projection.h"

using namespace qwm;
using namespace qwm::draw::geometry;

GeometryPolygon::GeometryPolygon(const std::vector<util::PointWorldCoord>& points, QObject* parent)
    : GeometryFixed(GeometryType::GeometryPolygon, parent),
      m_points(points)
{
    // Pre-calculate the QPolygonF and bounding box.

    // Loop through each point to add to the polygon.
    m_qpolygonf.clear();
    for(const auto& point : m_points)
    {
        // Add the point to be drawn.
        m_qpolygonf.append(point);
    }

    // Calculate the bounding box.
    m_bounding_box_fixed = util::RectWorldCoord::fromQRectF(m_qpolygonf.boundingRect());
}

std::vector<util::PointWorldCoord> GeometryPolygon::points() const
{
    // Return the points.
    return m_points;
}

const QPolygonF& GeometryPolygon::toQPolygonF() const
{
    // Return the QPolygonF.
    return m_qpolygonf;
}

const util::RectWorldCoord& GeometryPolygon::boundingBoxFixed() const
{
    // Return the fixed bounding box.
    return m_bounding_box_fixed;
}

util::RectWorldCoord GeometryPolygon::boundingBox(const Viewport& /*viewport*/) const
{
    // Return the fixed bounding box.
    return boundingBoxFixed();
}

bool GeometryPolygon::touches(const Geometry& geometry, const Viewport& viewport) const
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
                // They have touched if the polygons intersect (point/widget polygon is its bounding box).
                return_touches = (toQPolygonF().intersected(geometry.boundingBox(viewport)).empty() == false);

                // Finished.
                break;
            }
            case GeometryType::GeometryPolygon:
            {
                // They have touched if the polygons intersect.
                return_touches = (toQPolygonF().intersected(static_cast<const GeometryPolygon&>(geometry).toQPolygonF()).empty() == false);

                // Finished.
                break;
            }
        }
    }

    // Return our success.
    return return_touches;
}

void GeometryPolygon::draw(QPainter& painter, const util::RectWorldCoord& /*drawing_rect_world_coord*/, const Viewport& viewport) const
{
    // Create a polygon of the points.
    QPolygonF polygon;

    // Loop through each point to add to the polygon.
    for(const auto& point : m_points)
    {
        // Add the point to be drawn.
        polygon.append(projection::toPointWorldPx(viewport, point));
    }

    // Set the pen to use.
    painter.setPen(pen());

    // Set the brush to use.
    painter.setBrush(brush());

    // Draw the polygon line.
    painter.drawPolygon(polygon);
}
