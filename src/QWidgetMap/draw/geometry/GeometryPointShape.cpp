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

#include "GeometryPointShape.h"

// Local includes.
#include "../../projection/Projection.h"

using namespace qwm;
using namespace qwm::draw::geometry;

GeometryPointShape::GeometryPointShape(const util::PointWorldCoord& point_coord, const QSizeF& size_px, QObject* parent)
    : GeometryPoint(point_coord, parent),
      m_size_px(size_px)
{

}

void GeometryPointShape::setPen(const std::shared_ptr<QPen>& pen)
{
    // Set the pen to draw with.
    Geometry::setPen(pen);

    // Update the shape.
    updateShape();
}

void GeometryPointShape::setPen(const QPen& pen)
{
    // Set the pen to draw with.
    Geometry::setPen(pen);

    // Update the shape.
    updateShape();
}

void GeometryPointShape::setBrush(const std::shared_ptr<QBrush>& brush)
{
    // Set the brush to draw with.
    Geometry::setBrush(brush);

    // Update the shape.
    updateShape();
}

void GeometryPointShape::setBrush(const QBrush& brush)
{
    // Set the brush to draw with.
    Geometry::setBrush(brush);

    // Update the shape.
    updateShape();
}

const QSizeF& GeometryPointShape::sizePx() const
{
    // Return the size of the shape (pixels).
    return m_size_px;
}

void GeometryPointShape::setSizePx(const QSizeF& size_px, const bool& update_shape)
{
    // Set the size of the shape (pixels).
    m_size_px = size_px;

    // Should we updated the shape?
    if(update_shape)
    {
        // Update the shape.
        updateShape();
    }
    else
    {
        // Just emit that we need to redraw to display this change.
        emit requestRedraw();
    }
}

const AlignmentType& GeometryPointShape::alignmentType() const
{
    // Return the alignment type.
    return m_alignment_type;
}

void GeometryPointShape::setAlignmentType(const AlignmentType& alignment_type, const bool& update_shape)
{
    // Set the alignment type.
    m_alignment_type = alignment_type;

    // Should we updated the shape?
    if(update_shape)
    {
        // Update the shape.
        updateShape();
    }
    else
    {
        // Just emit that we need to redraw to display this change.
        emit requestRedraw();
    }
}

const qreal& GeometryPointShape::rotation() const
{
    // Return the rotation.
    return m_rotation;
}

void GeometryPointShape::setRotation(const qreal& rotation, const bool& update_shape)
{
    // Set the rotation.
    m_rotation = rotation;

    // Should we updated the shape?
    if(update_shape)
    {
        // Update the shape.
        updateShape();
    }
    else
    {
        // Just emit that we need to redraw to display this change.
        emit requestRedraw();
    }
}

util::RectWorldCoord GeometryPointShape::boundingBox(const Viewport& viewport) const
{
    // Calculate the world point in pixels.
    const util::PointWorldPx point_px(projection::toPointWorldPx(viewport, coord()));

    // Calculate the top-left point.
    const util::PointWorldPx top_left_point_px(calculateTopLeftPoint(point_px, m_alignment_type, m_size_px));

    // Calculate the bottom-right point.
    const util::PointWorldPx bottom_right_point_px(top_left_point_px.x() + m_size_px.width(), top_left_point_px.y() + m_size_px.height());

    // Return the converted coord points.
    return util::RectWorldCoord(projection::toPointWorldCoord(viewport, top_left_point_px), projection::toPointWorldCoord(viewport, bottom_right_point_px));
}

void GeometryPointShape::updateShape()
{
    // Emit that we need to redraw to display this change.
    emit requestRedraw();
}
