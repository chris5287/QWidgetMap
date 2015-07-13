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

#include "GeometryPointText.h"

// Local includes.
#include "../../projection/Projection.h"

using namespace qwm;
using namespace qwm::draw::geometry;

GeometryPointText::GeometryPointText(const util::PointWorldCoord& point_coord, const std::string& text, QObject* parent)
    : GeometryPoint(point_coord, parent),
      m_text(text)
{

}

void GeometryPointText::draw(QPainter& painter, const util::RectWorldCoord& /*drawing_rect_world_coord*/, const Viewport& viewport) const
{
    // Set the pen/font to use.
    painter.setPen(pen());
    painter.setFont(font());

    // Calculate the requried bounding box.
    const QRectF bounding_rect(painter.boundingRect(QRect(), m_text.c_str()));

    // Draw annotation text (using rect so that new line characters are drawn).
    painter.drawText(QRectF(qwm::projection::toPointWorldPx(viewport, coord()), bounding_rect.size()), Qt::TextWordWrap, m_text.c_str());
}
