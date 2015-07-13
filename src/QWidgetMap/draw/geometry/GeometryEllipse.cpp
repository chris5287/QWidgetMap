/**
 * @copyright 2015 Chris Stylianou
 *
 * This program is free software: you can redistribute it ansemi_major_m/or modify
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

#include "GeometryEllipse.h"

// Qt includes.
#include <QtWidgets/QGraphicsEllipseItem>
#include <QtWidgets/QGraphicsPolygonItem>

// STL includes.
#include <cmath>

// Local includes.
#include "GeometryPolygon.h"
#include "../../projection/Projection.h"
#include "../../util/Algorithms.h"

using namespace qwm;
using namespace qwm::draw::geometry;

GeometryEllipse::GeometryEllipse(const util::PointWorldCoord& point, const double& semi_major_m, const double& semi_minor_m, const double& bearing_deg, QObject* parent)
    : GeometryFixed(GeometryType::GeometryEllipse, parent),
      m_point_center(point),
      m_bearing_deg(bearing_deg)
{
    // We need to calculate the destination coordiantes for the semi-major and semi-minor distances, as this is what is used to draw the actual ellipse.

    // Set the destinations.
    m_point_semi_major = util::algorithms::destinationPoint(point, semi_major_m, bearing_deg);
    m_point_semi_minor = util::algorithms::destinationPoint(point, semi_minor_m, bearing_deg + 90.0);

    // Also calculate bounding box using the semi-major distance.
    const auto point_north(util::algorithms::destinationPoint(point, semi_major_m, 0.0));
    const auto point_east(util::algorithms::destinationPoint(point, semi_major_m, 90.0));
    const auto point_south(util::algorithms::destinationPoint(point, semi_major_m, 180.0));
    const auto point_west(util::algorithms::destinationPoint(point, semi_major_m, 270.0));
    m_bounding_box_fixed = util::RectWorldCoord(util::PointWorldCoord(point_west.longitude(), point_north.latitude()), util::PointWorldCoord(point_east.longitude(), point_south.latitude()));
}

const util::PointWorldCoord& GeometryEllipse::centerCoord() const
{
    // Return the points.
    return m_point_center;
}

const util::RectWorldCoord& GeometryEllipse::boundingBoxFixed() const
{
    // Return the fixed bounding box.
    return m_bounding_box_fixed;
}

util::RectWorldCoord GeometryEllipse::boundingBox(const Viewport& /*viewport*/) const
{
    // Return the fixed bounding box.
    return boundingBoxFixed();
}

bool GeometryEllipse::touches(const Geometry& geometry, const Viewport& viewport) const
{
    // Default return success.
    bool return_touches(false);

    // Check we are visible.
    if(isVisible(viewport))
    {
        /// @todo create QGraphicsEllipseItem with rotation and semi-major/minor.

        // Construct an ellipse to perform intersect checks against.
        const QGraphicsEllipseItem ellipse(boundingBox(viewport));

        // Switch to the correct geometry type.
        switch(geometry.geometryType())
        {
            case GeometryType::GeometryEllipse:
            {
                // They have touched if the shapes intersect.
                return_touches = ellipse.shape().intersects(QGraphicsEllipseItem(geometry.boundingBox(viewport)).shape());

                // Finished.
                break;
            }
            case GeometryType::GeometryLineString:
            {
                // Set touches response based on line string vs ellipse check.
                return_touches = geometry.touches(*this, viewport);

                // Finished.
                break;
            }
            case GeometryType::GeometryPoint:
            {
                // They have touched if the shapes intersect.
                return_touches = ellipse.shape().intersects(geometry.boundingBox(viewport));

                // Finished.
                break;
            }
            case GeometryType::GeometryPolygon:
            {
                // They have touched if the shapes intersect.
                return_touches = ellipse.shape().intersects(QGraphicsPolygonItem(static_cast<const GeometryPolygon&>(geometry).toQPolygonF()).shape());

                // Finished.
                break;
            }
        }
    }

    // Return our success.
    return return_touches;
}

void GeometryEllipse::draw(QPainter& painter, const util::RectWorldCoord& /*drawing_rect_world_coord*/, const Viewport& viewport) const
{
    // Convert each destination point to pixels.
    const util::PointWorldPx point_px(projection::toPointWorldPx(viewport, m_point_center));
    const util::PointWorldPx point_semi_major_px(projection::toPointWorldPx(viewport, m_point_semi_major));
    const util::PointWorldPx point_semi_minor_px(projection::toPointWorldPx(viewport, m_point_semi_minor));

    // Calculate the ellipse rect to draw within.
    const util::RectWorldPx ellipse_rect_px(projection::toPointWorldPx(viewport, boundingBox(viewport).topLeftCoord()), projection::toPointWorldPx(viewport, boundingBox(viewport).bottomRightCoord()));

    // Translate to center point with required rotation.
    painter.translate(ellipse_rect_px.centerPx());
    painter.rotate(m_bearing_deg);

    // Set the pen/brush.
    painter.setPen(pen());
    painter.setBrush(brush());

    // Draw the ellipse.
    painter.drawEllipse(QPointF(0.0, 0.0),
                        std::sqrt(std::pow(std::abs(point_px.x() - point_semi_minor_px.x()), 2) + std::pow(std::abs(point_px.y() - point_semi_minor_px.y()), 2)),
                        std::sqrt(std::pow(std::abs(point_px.x() - point_semi_major_px.x()), 2) + std::pow(std::abs(point_px.y() - point_semi_major_px.y()), 2)));

    // Un-translate.
    painter.rotate(-m_bearing_deg);
    painter.translate(-ellipse_rect_px.centerPx());
}

void GeometryEllipse::drawPreview(QPainter& painter, const Viewport& viewport, const util::PointViewportPx& mouse_position_pressed_px) const
{
    // Convert each destination point to pixels.
    const util::PointWorldPx point_px(projection::toPointWorldPx(viewport, m_point_center));
    const util::PointWorldPx point_semi_major_px(projection::toPointWorldPx(viewport, m_point_semi_major));
    const util::PointWorldPx point_semi_minor_px(projection::toPointWorldPx(viewport, m_point_semi_minor));

    // Draw the ellipse.
    painter.drawEllipse(mouse_position_pressed_px,
                        std::sqrt(std::pow(std::abs(point_px.x() - point_semi_minor_px.x()), 2) + std::pow(std::abs(point_px.y() - point_semi_minor_px.y()), 2)),
                        std::sqrt(std::pow(std::abs(point_px.x() - point_semi_major_px.x()), 2) + std::pow(std::abs(point_px.y() - point_semi_major_px.y()), 2)));
}
