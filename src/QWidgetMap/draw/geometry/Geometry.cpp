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

#include "Geometry.h"
#include "../../projection/Projection.h"

using namespace qwm;
using namespace qwm::draw::geometry;

Geometry::Geometry(const GeometryType& geometry_type, QObject* parent)
    : Drawable(DrawableType::Geometry, parent),
      m_geometry_type(geometry_type)
{

}

const GeometryType& Geometry::geometryType() const
{
    // Return the geometry type.
    return m_geometry_type;
}

const QPen& Geometry::pen() const
{
    // Do we have a pen?
    if(m_pen == nullptr)
    {
        // Create a default pen.
        m_pen = std::make_shared<QPen>();
    }

    // Get the pen to draw with.
    return *(m_pen.get());
}

void Geometry::setPen(const std::shared_ptr<QPen>& pen)
{
    // Set the pen to draw with.
    m_pen = pen;

    // Emit that we need to redraw to display this change.
    emit requestRedraw();
}

void Geometry::setPen(const QPen& pen)
{
    // Set the pen to draw with.
    m_pen = std::make_shared<QPen>(pen);

    // Emit that we need to redraw to display this change.
    emit requestRedraw();
}

const QBrush& Geometry::brush() const
{
    // Do we have a brush?
    if(m_brush == nullptr)
    {
        // Create a default brush.
        m_brush = std::make_shared<QBrush>();
    }

    // Get the brush to draw with.
    return *(m_brush.get());
}

void Geometry::setBrush(const std::shared_ptr<QBrush>& brush)
{
    // Set the brush to draw with.
    m_brush = brush;

    // Emit that we need to redraw to display this change.
    emit requestRedraw();
}

void Geometry::setBrush(const QBrush& brush)
{
    // Set the brush to draw with.
    m_brush = std::make_shared<QBrush>(brush);

    // Emit that we need to redraw to display this change.
    emit requestRedraw();
}

const QFont& Geometry::font() const
{
    // Do we have a font?
    if(m_font == nullptr)
    {
        // Create a default font.
        m_font = std::make_shared<QFont>();
    }

    // Get the font to draw with.
    return *(m_font.get());
}

void Geometry::setFont(const std::shared_ptr<QFont>& font)
{
    // Set the font to draw with.
    m_font = font;

    // Emit that we need to redraw to display this change.
    emit requestRedraw();
}

void Geometry::setFont(const QFont& font)
{
    // Set the font to draw with.
    m_font = std::make_shared<QFont>(font);

    // Emit that we need to redraw to display this change.
    emit requestRedraw();
}

void Geometry::setMetadataDisplayed(const std::string& key, const int& zoom_minimum, const AlignmentType& alignment_type, const double& alignment_offset_px)
{
    // Set the meta-data key to use.
    m_metadata_displayed_key = key;
    m_metadata_displayed_zoom_minimum = zoom_minimum;
    m_metadata_displayed_alignment_type = alignment_type;
    m_metadata_displayed_alignment_offset_px = alignment_offset_px;

    // Emit that we need to redraw to display this change.
    emit requestRedraw();
}

void Geometry::drawMetadataDisplayed(QPainter& painter, const Viewport& viewport)
{
    // Do we have a meta-data value and should we display it at this zoom?
    if(viewport.zoom() >= m_metadata_displayed_zoom_minimum && metadata(m_metadata_displayed_key).isNull() == false)
    {
        // Fetch the bounding box of the geometry.
        const util::RectWorldCoord geometry_rect_coord(boundingBox(viewport));
        const util::RectWorldPx geometry_rect_px(projection::toPointWorldPx(viewport, geometry_rect_coord.topLeftCoord()), projection::toPointWorldPx(viewport, geometry_rect_coord.bottomRightCoord()));

        // Set the pen/font to use.
        painter.setPen(pen());
        painter.setFont(font());

        // Calculate the text rect box.
        const QRectF text_rect_px(painter.boundingRect(QRectF{}, Qt::AlignLeft, metadata(m_metadata_displayed_key).toString()));

        // Draw the text next to the point with an offset.
        switch(m_metadata_displayed_alignment_type)
        {
            case AlignmentType::Middle:
            {
                painter.drawText(geometry_rect_px.center() + util::PointPx(-text_rect_px.width() / 2.0, text_rect_px.height() / 2.0), metadata(m_metadata_displayed_key).toString());
                break;
            }
            case AlignmentType::MiddleLeft:
            {
                painter.drawText(util::PointWorldPx(geometry_rect_px.left(), geometry_rect_px.center().y()) + util::PointPx(-m_metadata_displayed_alignment_offset_px, 0) + util::PointPx(-text_rect_px.width(), text_rect_px.height() / 2.0), metadata(m_metadata_displayed_key).toString());
                break;
            }
            case AlignmentType::MiddleRight:
            {
                painter.drawText(util::PointWorldPx(geometry_rect_px.right(), geometry_rect_px.center().y()) + util::PointPx(m_metadata_displayed_alignment_offset_px, 0) + util::PointPx(0, text_rect_px.height() / 2.0), metadata(m_metadata_displayed_key).toString());
                break;
            }
            case AlignmentType::TopLeft:
            {
                painter.drawText(geometry_rect_px.bottomLeft() + util::PointPx(-m_metadata_displayed_alignment_offset_px, -m_metadata_displayed_alignment_offset_px) + util::PointPx(-text_rect_px.width(), 0), metadata(m_metadata_displayed_key).toString());
                break;
            }
            case AlignmentType::TopRight:
            {
                painter.drawText(geometry_rect_px.bottomRight() + util::PointPx(m_metadata_displayed_alignment_offset_px, -m_metadata_displayed_alignment_offset_px), metadata(m_metadata_displayed_key).toString());
                break;
            }
            case AlignmentType::TopMiddle:
            {
                painter.drawText(util::PointWorldPx(geometry_rect_px.center().x(), geometry_rect_px.bottom()) + util::PointPx(0, -m_metadata_displayed_alignment_offset_px) + util::PointPx(-text_rect_px.width() / 2.0, 0), metadata(m_metadata_displayed_key).toString());
                break;
            }
            case AlignmentType::BottomLeft:
            {
                painter.drawText(geometry_rect_px.topLeft() + util::PointPx(-m_metadata_displayed_alignment_offset_px, m_metadata_displayed_alignment_offset_px) + util::PointPx(-text_rect_px.width(), text_rect_px.height()), metadata(m_metadata_displayed_key).toString());
                break;
            }
            case AlignmentType::BottomRight:
            {
                painter.drawText(geometry_rect_px.topRight() + util::PointPx(m_metadata_displayed_alignment_offset_px, m_metadata_displayed_alignment_offset_px) + util::PointPx(0, text_rect_px.height()), metadata(m_metadata_displayed_key).toString());
                break;
            }
            case AlignmentType::BottomMiddle:
            {
                painter.drawText(util::PointWorldPx(geometry_rect_px.center().x(), geometry_rect_px.top()) + util::PointPx(0, m_metadata_displayed_alignment_offset_px) + util::PointPx(-text_rect_px.width() / 2.0, text_rect_px.height()), metadata(m_metadata_displayed_key).toString());
                break;
            }
        }
    }
}

util::PointWorldPx Geometry::calculateTopLeftPoint(const util::PointWorldPx& point_px, const AlignmentType& alignment_type, const QSizeF& geometry_size_px) const
{
    // Default world point to return.
    util::PointWorldPx top_left_point_px(point_px);

    // Check the alignment type and apply the required deltas to move the point to top-left.
    switch(alignment_type)
    {
        case AlignmentType::Middle:
        {
            // Move x/y from the middle to top-left.
            top_left_point_px.setX(point_px.x() - (geometry_size_px.width() / 2.0));
            top_left_point_px.setY(point_px.y() - (geometry_size_px.height() / 2.0));
            break;
        }
        case AlignmentType::MiddleLeft:
        {
            // Move y from the middle-left to top-left.
            top_left_point_px.setY(point_px.y() - (geometry_size_px.height() / 2.0));
            break;
        }
        case AlignmentType::MiddleRight:
        {
            // Move x/y from the middle-right to top-left.
            top_left_point_px.setX(point_px.x() - geometry_size_px.width());
            top_left_point_px.setY(point_px.y() - (geometry_size_px.height() / 2.0));
            break;
        }
        case AlignmentType::TopLeft:
        {
            // Already at top-left.
            break;
        }
        case AlignmentType::TopRight:
        {
            // Move x from the top-right to top-left.
            top_left_point_px.setX(point_px.x() - geometry_size_px.width());
            break;
        }
        case AlignmentType::TopMiddle:
        {
            // Move x from the top-middle to top-left.
            top_left_point_px.setX(point_px.x() - (geometry_size_px.width() / 2.0));
            break;
        }
        case AlignmentType::BottomLeft:
        {
            // Move y from the bottom-left to top-left.
            top_left_point_px.setY(point_px.y() - geometry_size_px.height());
            break;
        }
        case AlignmentType::BottomRight:
        {
            // Move x/y from the bottom-right to top-left.
            top_left_point_px.setX(point_px.x() - geometry_size_px.width());
            top_left_point_px.setY(point_px.y() - geometry_size_px.height());
            break;
        }
        case AlignmentType::BottomMiddle:
        {
            // Move x/y from the bottom-middle to top-left.
            top_left_point_px.setX(point_px.x() - (geometry_size_px.width() / 2.0));
            top_left_point_px.setY(point_px.y() - geometry_size_px.height());
            break;
        }
    }

    // Return the top-left point.
    return top_left_point_px;
}
