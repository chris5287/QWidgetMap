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

#pragma once

// Qt includes.
#include <QtGui/QPixmap>

// Local includes.
#include "../../qwidgetmap_global.h"
#include "GeometryPointShape.h"

/// QWidgetMap namespace.
namespace qwm
{

    /// Drawable namespace.
    namespace draw
    {

        /// Geometry namespace.
        namespace geometry
        {

            /**
             * Implementation of the drawable point geometry item that displays an image.
             */
            class QWIDGETMAP_EXPORT GeometryPointImage : public GeometryPointShape
            {

            public:

                /**
                 * This constructor creates a point which will display an image pixmap.
                 * @param point_coord The point to draw the image at (world coordinates).
                 * @param image The image to draw.
                 * @param parent QObject parent ownership.
                 */
                GeometryPointImage(const util::PointWorldCoord& point_coord, const std::shared_ptr<QPixmap>& image, QObject* parent = nullptr);

                /**
                 * This constructor creates a point which will display an image pixmap.
                 * @param point_coord The point to draw the image at (world coordinates).
                 * @param image The image to draw.
                 * @param parent QObject parent ownership.
                 */
                GeometryPointImage(const util::PointWorldCoord& point_coord, const QPixmap& image, QObject* parent = nullptr)
                    : GeometryPointImage(point_coord, std::make_shared<QPixmap>(image), parent) { }

                /**
                 * This constructor creates a point which will display an image pixmap from the file.
                 * @param point_coord The point to draw the image at (world coordinates).
                 * @param filename The image file to draw.
                 * @param parent QObject parent ownership.
                 */
                GeometryPointImage(const util::PointWorldCoord& point_coord, const std::string& filename, QObject* parent = nullptr)
                    : GeometryPointImage(point_coord, std::make_shared<QPixmap>(filename.c_str()), parent) { }

                /// Disable copy constructor.
                GeometryPointImage(const GeometryPointImage&) = delete;

                /// Disable copy assignment.
                GeometryPointImage& operator=(const GeometryPointImage&) = delete;

                /// Destructor.
                ~GeometryPointImage() = default;

            public:

                /**
                 * Fetches the image pixmap.
                 * @return the image pixmap.
                 */
                const QPixmap& image() const;

                /**
                 * Set the image pixmap to draw.
                 * @param new_image The image pixmap to draw.
                 * @param update_shape Whether updateShape() should be called at the end of this function.
                 */
                void setImage(const std::shared_ptr<QPixmap>& new_image, const bool& update_shape = true);

                /**
                 * Set the image pixmap to draw.
                 * @param new_image The image pixmap to draw.
                 * @param update_shape Whether updateShape() should be called at the end of this function.
                 */
                void setImage(const QPixmap& new_image, const bool& update_shape = true);

            public:

                /**
                 * Draws the item to the provided painter.
                 * @param painter The painter to draw on.
                 * @param drawing_rect_world_coord The drawing rect in world coordinates.
                 * @param viewport The current viewport to use.
                 */
                void draw(QPainter& painter, const util::RectWorldCoord& drawing_rect_world_coord, const Viewport& viewport) const final;

            private:

                /// The image pixmap to draw.
                std::shared_ptr<QPixmap> m_image;

            };

        }

    }

}
