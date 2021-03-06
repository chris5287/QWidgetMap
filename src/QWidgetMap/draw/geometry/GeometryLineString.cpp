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
            // Calculate the degree distance of 1 px within viewport (used to calculate fuzz factor around lines).
            const util::PointWorldPx point_center_px(projection::toPointWorldPx(viewport, util::PointWorldCoord(0.0, 0.0)));
            const util::PointWorldPx point_fuzz_px(point_center_px.x() + 1.0, point_center_px.y() + 1.0);
            const util::PointWorldCoord point_fuzz_coord(projection::toPointWorldCoord(viewport, point_fuzz_px));

            // Whilst we have not found any touches and we have a part-line to check.
            for(std::size_t p_i = 0; return_touches == false && p_i < m_points.size() - 1; ++p_i)
            {
                // Construct a line to perform intersect checks against.
                QGraphicsLineItem line(QLineF(m_points.at(p_i), m_points.at(p_i + 1)));

                // Set pen width to fuzz factor required.
                QPen line_pen(line.pen());
                line_pen.setWidthF(point_fuzz_coord.x() * pen().widthF());
                line.setPen(line_pen);

                // Switch to the correct geometry type.
                switch(geometry.geometryType())
                {
                    case GeometryType::GeometryEllipse:
                    {
                        // Create an ellipse item to represent our GeometryEllipse.
                        QGraphicsEllipseItem ellipse_item(geometry.boundingBox(viewport));

                        // Set pen width to 0.0 (QPen defaults to 1 pixel).
                        QPen ellipse_item_pen(ellipse_item.pen());
                        ellipse_item_pen.setWidthF(0.0);
                        ellipse_item.setPen(ellipse_item_pen);

                        // They have touched if the shapes intersect.
                        return_touches = line.shape().intersects(ellipse_item.shape());

                        // Finished.
                        break;
                    }
                    case GeometryType::GeometryLineString:
                    {
                        // Compare the line agains the sub-line parts of the other geometry.
                        const auto& geometry_points(static_cast<const GeometryLineString&>(geometry).points());
                        for(std::size_t gp_i = 0; return_touches == false && gp_i < geometry_points.size() - 1; ++gp_i)
                        {
                            // Create a line item to represent our GeomtryLineString.
                            QGraphicsLineItem line_item(QLineF(geometry_points.at(gp_i), geometry_points.at(gp_i + 1)));

                            // Set pen width to fuzz factor required.
                            QPen line_item_pen(line_item.pen());
                            line_item_pen.setWidthF(point_fuzz_coord.x() * geometry.pen().widthF());
                            line_item.setPen(line_item_pen);

                            // They have touched if the shapes intersect.
                            return_touches = line.shape().intersects(line_item.shape());
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
                        // Create a polygon item to represent our GeomtryPolygon.
                        QGraphicsPolygonItem polygon_item(static_cast<const GeometryPolygon&>(geometry).toQPolygonF());

                        // Set pen width to 0.0 (QPen defaults to 1 pixel).
                        QPen polygon_item_pen(polygon_item.pen());
                        polygon_item_pen.setWidthF(0.0);
                        polygon_item.setPen(polygon_item_pen);

                        // They have touched if the shapes intersect.
                        return_touches = line.shape().intersects(polygon_item.shape());

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
