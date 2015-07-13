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

#include "GeometryLineString.h"

// Qt includes.
#include <QtWidgets/QGraphicsLineItem>
#include <QtWidgets/QGraphicsPolygonItem>

// Local includes.
#include "GeometryEllipse.h"
#include "GeometryPolygon.h"
#include "../../projection/Projection.h"

using namespace qwm;
using namespace qwm::draw::geometry;

GeometryLineString::GeometryLineString(const std::vector<util::PointWorldCoord>& points, QObject* parent)
    : GeometryFixed(GeometryType::GeometryLineString, parent),
      m_points(points)
{
    // Loop through each point to add to the polygon.
    QPolygonF polygon_line;
    for(const auto& point : m_points)
    {
        // Add the point to be drawn.
        polygon_line.append(point);
    }

    // Set the fixed bounding box.
    m_bounding_box_fixed = util::RectWorldCoord::fromQRectF(polygon_line.boundingRect());
}

std::vector<util::PointWorldCoord> GeometryLineString::points() const
{
    // Return the points.
    return m_points;
}

const util::RectWorldCoord& GeometryLineString::boundingBoxFixed() const
{
    // Return the fixed bounding box.
    return m_bounding_box_fixed;
}

util::RectWorldCoord GeometryLineString::boundingBox(const Viewport& /*viewport*/) const
{
    // Return the fixed bounding box.
    return boundingBoxFixed();
}

bool GeometryLineString::touches(const Geometry& geometry, const Viewport& viewport) const
{
    // Default return success.
    bool return_touches(false);

    // Check we are visible.
    if(isVisible(viewport))
    {
        // Check we have at least 2 points!
        if(m_points.size() > 1)
        {
            // Calculate fuzz factor around line.
            const util::PointWorldPx point_center_px(projection::toPointWorldPx(viewport, util::PointWorldCoord(0.0, 0.0)));
            const util::PointWorldPx point_fuzz_px(point_center_px.x() + geometry.pen().widthF(), point_center_px.y() + geometry.pen().widthF());
            const util::PointWorldCoord point_fizz_coord(projection::toPointWorldCoord(viewport, point_fuzz_px));

            // Whilst we have not found any touches and we have a part-line to check.
            for(std::size_t p_i = 0; return_touches == false && p_i < m_points.size() - 1; ++p_i)
            {
                // Construct a line to perform intersect checks against, with the appropriate line width.
                QGraphicsLineItem line(QLineF(m_points.at(p_i), m_points.at(p_i + 1)));
                QPen line_pen(line.pen());
                line_pen.setWidthF(point_fizz_coord.x());
                line.setPen(line_pen);

                // Switch to the correct geometry type.
                switch(geometry.geometryType())
                {
                    case GeometryType::GeometryEllipse:
                    {
                        // They have touched if the shapes intersect.
                        return_touches = line.shape().intersects(QGraphicsEllipseItem(geometry.boundingBox(viewport)).shape());

                        // Finished.
                        break;
                    }
                    case GeometryType::GeometryLineString:
                    {
                        // Compare the line agains the sub-line parts of the other geometry.
                        const auto& geometry_points(static_cast<const GeometryLineString&>(geometry).points());
                        for(std::size_t gp_i = 0; return_touches == false && gp_i < geometry_points.size() - 1; ++gp_i)
                        {
                            // They have touched if the shapes intersect.
                            return_touches = line.shape().intersects(QGraphicsLineItem(QLineF(geometry_points.at(gp_i), geometry_points.at(gp_i + 1))).shape());
                        }

                        // Finished.
                        break;
                    }
                    case GeometryType::GeometryPoint:
                    {
                        // They have touched if the shapes intersect.
                        return_touches = line.shape().intersects(geometry.boundingBox(viewport));

                        // Finished.
                        break;
                    }
                    case GeometryType::GeometryPolygon:
                    {
                        // They have touched if the shapes intersect.
                        return_touches = line.shape().intersects(QGraphicsPolygonItem(static_cast<const GeometryPolygon&>(geometry).toQPolygonF()).shape());

                        // Finished.
                        break;
                    }
                }
            }
        }
    }

    // Return our success.
    return return_touches;
}

void GeometryLineString::draw(QPainter& painter, const util::RectWorldCoord& /*drawing_rect_world_coord*/, const Viewport& viewport) const
{
    // Create a polygon of the points.
    QPolygonF polygon_line_px;

    // Loop through each point to add to the polygon.
    for(const auto& point : m_points)
    {
        // Add the point to be drawn.
        polygon_line_px.append(projection::toPointWorldPx(viewport, point));
    }

    // Set the pen to use.
    painter.setPen(pen());

    // Draw the polygon line.
    painter.drawPolyline(polygon_line_px);
}
