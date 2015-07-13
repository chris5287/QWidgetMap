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

#include "GeometryPointArrow.h"

using namespace qwm;
using namespace qwm::draw::geometry;

GeometryPointArrow::GeometryPointArrow(const util::PointWorldCoord& point_coord, const QSizeF& size_px, QObject* parent)
    : GeometryPointImage(point_coord, QPixmap(size_px.toSize()), parent)
{
    // Update the shape (to draw the initial image pixmap).
    updateShape();
}

void GeometryPointArrow::updateShape()
{
    // Create a pixmap of the required size.
    QPixmap image_pixmap(sizePx().toSize());

    // Reset the image pixmap.
    image_pixmap.fill(Qt::transparent);

    // Create a painter for the image pixmap.
    QPainter painter(&image_pixmap);

    // Ensure antialiasing is enabled.
    painter.setRenderHints(QPainter::Antialiasing, true);

    // Set the pen and brush.
    painter.setPen(pen());
    painter.setBrush(brush());

    // Add points to create arrow shape.
    QPolygonF arrow;
    arrow << util::PointPx((image_pixmap.width() / 2.0), 0.0);
    arrow << util::PointPx(image_pixmap.width(), image_pixmap.height());
    arrow << util::PointPx((image_pixmap.width() / 2.0), (image_pixmap.height() / 2.0));
    arrow << util::PointPx(0.0, image_pixmap.height());

    // Draw the arrow.
    painter.drawPolygon(arrow);

    // Set the image pixmap.
    setImage(image_pixmap, false);
}
