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

#include "GeometryPointImage.h"

// Local includes.
#include "../../projection/Projection.h"

using namespace qwm;
using namespace qwm::draw::geometry;

namespace
{
    /// Default empty image pixmap.
    std::unique_ptr<QPixmap> m_image_null(nullptr);
}

GeometryPointImage::GeometryPointImage(const util::PointWorldCoord& point_coord, const std::shared_ptr<QPixmap>& image, QObject* parent)
    : GeometryPointShape(point_coord, image->size(), parent),
      m_image(image)
{

}

const QPixmap& GeometryPointImage::image() const
{
    // Is the image pixmap currently null?
    if(m_image == nullptr)
    {
        // Have we already constructed the null image pixmap?
        if(m_image_null == nullptr)
        {
            // Construct the null image pixmap.
            m_image_null.reset(new QPixmap);
        }

        // Return the null image pixmap.
        return *(m_image_null.get());
    }
    else
    {
        // Return the image pixmap.
        return *(m_image.get());
    }
}

void GeometryPointImage::setImage(const std::shared_ptr<QPixmap>& new_image, const bool& update_shape)
{
    // Set the image pixmap.
    m_image = new_image;

    // Update the size (pixels).
    // This will also emit a redraw request.
    setSizePx(image().size(), update_shape);
}

void GeometryPointImage::setImage(const QPixmap& new_image, const bool& update_shape)
{
    // Set the pixmap.
    m_image = std::make_shared<QPixmap>(new_image);

    // Update the size (pixels).
    // This will also emit a redraw request.
    setSizePx(m_image->size(), update_shape);
}

void GeometryPointImage::draw(QPainter& painter, const util::RectWorldCoord& /*drawing_rect_world_coord*/, const Viewport& viewport) const
{
    // Calculate the pixmap rect in world coordinates.
    const util::RectWorldCoord pixmap_rect_world_coord(boundingBox(viewport));

    // Calculate the pixmap rect to draw within.
    const util::RectWorldPx pixmap_rect_px(projection::toPointWorldPx(viewport, pixmap_rect_world_coord.topLeftCoord()), projection::toPointWorldPx(viewport, pixmap_rect_world_coord.bottomRightCoord()));

    // Translate to center point with required rotation.
    painter.translate(pixmap_rect_px.centerPx());
    painter.rotate(rotation());

    // Draw the pixmap.
    painter.drawPixmap(-pixmap_rect_px.width() / 2.0, -pixmap_rect_px.height() / 2.0, image());

    // Un-translate.
    painter.rotate(-rotation());
    painter.translate(-pixmap_rect_px.centerPx());
}
