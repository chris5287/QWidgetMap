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

// Qt includes.
#include <QtCore/QPointF>

// Local includes.
#include "../qwidgetmap_global.h"

/// QWidgetMap namespace.
namespace qwm
{

    /// Utilities namespace.
    namespace util
    {

        /**
         * Represents a real-world coordinate point in decimal degrees.
         */
        class QWIDGETMAP_EXPORT PointWorldCoord : public QPointF
        {

        public:

            /**
             * PointWorldCoord represents a real-world coordinate point in decimal degrees.
             * @param longitude The longitude value (x-axis) in decimal degrees.
             * @param latitude The latitude value (y-axis) in decimal degrees.
             */
            PointWorldCoord(const qreal& longitude, const qreal& latitude) : QPointF(longitude, latitude) { }

            /**
             * Returns the longitude value (x-axis) in decimal degrees.
             * @return the longitude value (x-axis) in decimal degrees.
             */
            inline qreal longitude() const { return QPointF::x(); }

            /**
             * Set the longitude value (x-axis) in decimal degrees.
             * @param longitude The longitude value (x-axis) to set in decimal degrees.
             */
            inline void setLongitude(const qreal& longitude) { QPointF::setX(longitude); }

            /**
             * Returns the latitude value (y-axis) in decimal degrees.
             * @return the latitude value (y-axis) in decimal degrees.
             */
            inline qreal latitude() const { return QPointF::y(); }

            /**
             * Set the latitude value (y-axis) in decimal degrees.
             * @param latitude The latitude value (y-axis) to set in decimal degrees.
             */
            inline void setLatitude(const qreal& latitude) { QPointF::setY(latitude); }

        };

        /**
         * Represents a point in pixels (no relationship to viewport or world).
         */
        class QWIDGETMAP_EXPORT PointPx : public QPointF
        {

        public:

            /**
             * PointPx represents a point in pixels (no relationship to viewport or world).
             * @param x The x-axis value to set.
             * @param y The y-axis value to set.
             */
            PointPx(const qreal& x, const qreal& y) : QPointF(x, y) { }

            /**
             * Divides the x-axis and y-axis point by the specified value.
             * @param value The value to use when dividing.
             * @return the divided point in pixels.
             */
            inline PointPx operator/(qreal value) const { return PointPx(QPointF::x() / value, QPointF::y() / value); }

        };

        /**
         * Represents a real-world point in pixels.
         */
        class QWIDGETMAP_EXPORT PointWorldPx : public PointPx
        {

        public:

            /**
             * PointWorldPx represents a real-world point in pixels.
             * @param x The x-axis in pixels.
             * @param y The y-axis in pixels.
             */
            PointWorldPx(const qreal& x, const qreal& y) : PointPx(x, y) { }

            /**
             * Adds the pixel point to the world pixel point.
             * @param p The pixel point to add.
             * @return the world pixel point moved by the pixel point value.
             */
            inline PointWorldPx operator+(const PointPx &p) const { return PointWorldPx(x() + p.x(), y() + p.y()); }

            /**
             * Removes the pixel point from the world pixel point.
             * @param p The pixel point to remove.
             * @return the world pixel point moved by the pixel point value.
             */
            inline PointWorldPx operator-(const PointPx &p) const { return PointWorldPx(x() - p.x(), y() - p.y()); }

            /**
             * Adds the real-world points together to the total number of pixel points.
             * @param p The real-world point to add.
             * @return the total number of pixel points.
             */
            inline PointPx operator+(const PointWorldPx &p) const { return PointPx(x() + p.x(), y() + p.y()); }

            /**
             * Calcaulte the difference between two real-world points.
             * @param p The real-world point to remove.
             * @return the difference in pixel points.
             */
            inline PointPx operator-(const PointWorldPx &p) const { return PointPx(x() - p.x(), y() - p.y()); }

        };

        /**
         * Represents a viewport (on-screen) point in pixels.
         */
        class QWIDGETMAP_EXPORT PointViewportPx : public PointPx
        {

        public:

            /**
             * PointWorldPx represents a viewport (on-screen) point in pixels.
             * @param x The x-axis in pixels.
             * @param y The y-axis in pixels.
             */
            PointViewportPx(const qreal& x, const qreal& y) : PointPx(x, y) { }

            /**
             * Adds the pixel point to the viewport pixel point.
             * @param p The pixel point to add.
             * @return the viewport pixel point moved by the pixel point value.
             */
            inline PointViewportPx operator+(const PointPx &p) const { return PointViewportPx(x() + p.x(), y() + p.y()); }

            /**
             * Removes the pixel point from the viewport pixel point.
             * @param p The pixel point to remove.
             * @return the viewport pixel point moved by the pixel point value.
             */
            inline PointViewportPx operator-(const PointPx &p) const { return PointViewportPx(x() - p.x(), y() - p.y()); }

            /**
             * Adds the viewport points together to the total number of pixel points.
             * @param p The viewport point to add.
             * @return the total number of pixel points.
             */
            inline PointPx operator+(const PointViewportPx &p) const { return PointPx(x() + p.x(), y() + p.y()); }

            /**
             * Calcaulte the difference between two viewport points.
             * @param p The viewport point to remove.
             * @return the difference in pixel points.
             */
            inline PointPx operator-(const PointViewportPx &p) const { return PointPx(x() - p.x(), y() - p.y()); }

        };

    }

}
