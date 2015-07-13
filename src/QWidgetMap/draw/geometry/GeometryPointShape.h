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

#pragma once

// Local includes.
#include "../../qwidgetmap_global.h"
#include "GeometryPoint.h"

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
             * Interface to be implemented to make a drawable geometry item that displays a shape.
             */
            class QWIDGETMAP_EXPORT GeometryPointShape : public GeometryPoint
            {
                Q_OBJECT

            public:

                /**
                 * This constructor creates a shape at a specific point to be displayed.
                 * @param point_coord The point to be displayed (world coordinates).
                 * @param size_px The size of the shape (pixels).
                 * @param parent QObject parent ownership.
                 */
                GeometryPointShape(const util::PointWorldCoord& point_coord, const QSizeF& size_px, QObject* parent = nullptr);

                /// Disable copy constructor.
                GeometryPointShape(const GeometryPointShape&) = delete;

                /// Disable copy assignment.
                GeometryPointShape& operator=(const GeometryPointShape&) = delete;

                /// Destructor.
                virtual ~GeometryPointShape() = default;

            public:

                /**
                 * Sets the pen to draw the shape with (outline).
                 * @param pen The QPen to use for drawing.
                 */
                void setPen(const std::shared_ptr<QPen>& pen) final;

                /**
                 * Sets the pen to draw the shape with (outline).
                 * @param pen The QPen to use for drawing.
                 */
                void setPen(const QPen& pen) final;

                /**
                 * Sets the brush to draw the shape with (fill).
                 * @param brush The QBrush to use for drawing.
                 */
                void setBrush(const std::shared_ptr<QBrush>& brush) final;

                /**
                 * Sets the brush to draw the shape with (fill).
                 * @param brush The QBrush to use for drawing.
                 */
                void setBrush(const QBrush& brush) final;

                /**
                 * Fetches the size of the shape (pixels).
                 * @return the size of the shape (pixels).
                 */
                const QSizeF& sizePx() const;

                /**
                 * Sets the size of the shape (pixels).
                 * @param size_px The size of the shape to set (pixels).
                 * @param update_shape Whether updateShape() should be called at the end of this function.
                 */
                void setSizePx(const QSizeF& size_px, const bool& update_shape = true);

                /**
                 * Fetches the alignment type to use when drawing the shape.
                 * @return the alignment type to use when drawing the shape.
                 */
                const AlignmentType& alignmentType() const;

                /**
                 * Sets the alignment type to use when drawing the shape.
                 * @param alignment_type The alignment type to set.
                 * @param update_shape Whether updateShape() should be called at the end of this function.
                 */
                void setAlignmentType(const AlignmentType& alignment_type = AlignmentType::Middle, const bool& update_shape = true);

                /**
                 * Fetches the rotation of the shape (degrees).
                 * @return the rotation of the shape (degrees).
                 */
                const qreal& rotation() const;

                /**
                 * Sets the rotation of the shape (degrees).
                 * @param rotation The rotation to set (degrees).
                 * @param update_shape Whether updateShape() should be called at the end of this function.
                 * @note Rotation will be applied to the center of the shape.
                 */
                void setRotation(const qreal& rotation, const bool& update_shape = true);

            public:

                /**
                 * Fetches the bounding box (world coordinates).
                 * @param viewport The current viewport to use.
                 * @return the bounding box.
                 */
                virtual util::RectWorldCoord boundingBox(const Viewport& viewport) const override;

            protected:

                /**
                 * Updates the shape.
                 */
                virtual void updateShape();

            private:

                /// The shape size (pixels).
                QSizeF m_size_px;

                /// The alignment type to use when drawing the shape.
                AlignmentType m_alignment_type { AlignmentType::Middle };

                /// The rotation of the shape (applied to the center of the shape).
                qreal m_rotation { 0.0 };

            };

        }

    }

}
