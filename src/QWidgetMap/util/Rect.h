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
#include <QtCore/QRectF>

// STL includes.
#include <vector>

// Local includes.
#include "../qwidgetmap_global.h"
#include "Point.h"

/// QWidgetMap namespace.
namespace qwm
{

    /// Utilities namespace.
    namespace util
    {

        /**
         * Represents a real-world coordinate box in decimal degrees.
         */
        class QWIDGETMAP_EXPORT RectWorldCoord : public QRectF
        {

        public:

            /**
             * RectWorldCoord represents a real-world coordinate box in decimal degrees.
             */
            RectWorldCoord() : QRectF() { }

            /**
             * RectWorldCoord represents a real-world coordinate box in decimal degrees.
             * @param top_left_coord The top left point in decimal degrees.
             * @param bottom_right_coord The bottom_right in decimal degrees.
             */
            RectWorldCoord(const PointWorldCoord& top_left_coord, const PointWorldCoord& bottom_right_coord) : QRectF(top_left_coord, bottom_right_coord) { }

            /**
             * RectWorldCoord represents a real-world coordinate box in decimal degrees.
             * @param top_left_coord The top left point in decimal degrees.
             * @param size_coord The size of the box in decimal degrees.
             */
            RectWorldCoord(const PointWorldCoord& top_left_coord, const QSizeF& size_coord) : QRectF(top_left_coord, size_coord) { }

            /**
             * Returns the top-left coordinate in decimal degrees.
             * @return the top-left coordinate in decimal degrees.
             */
            inline PointWorldCoord topLeftCoord() const { return PointWorldCoord(QRectF::left(), QRectF::top()); }

            /**
             * Returns the top-right coordinate in decimal degrees.
             * @return the top-right coordinate in decimal degrees.
             */
            inline PointWorldCoord topRightCoord() const { return PointWorldCoord(QRectF::right(), QRectF::top()); }

            /**
             * Returns the bottom-right coordinate in decimal degrees.
             * @return the bottom-right coordinate in decimal degrees.
             */
            inline PointWorldCoord bottomRightCoord() const { return PointWorldCoord(QRectF::right(), QRectF::bottom()); }

            /**
             * Returns the bottom-left coordinate in decimal degrees.
             * @return the bottom-left coordinate in decimal degrees.
             */
            inline PointWorldCoord bottomLeftCoord() const { return PointWorldCoord(QRectF::left(), QRectF::bottom()); }

            /**
             * Returns the box coordiantes in decimal degrees.
             * @return the box coordiantes in decimal degrees.
             */
            inline std::vector<PointWorldCoord> toStdVector() const
            {
                // Create a vector to return the points.
                std::vector<PointWorldCoord> return_points;

                // Add the points.
                return_points.push_back(topLeftCoord());
                return_points.push_back(topRightCoord());
                return_points.push_back(bottomRightCoord());
                return_points.push_back(bottomLeftCoord());

                // Return the points.
                return return_points;
            }

        public:

            /**
             * Constructs a RectWorldCoord froma QRectF.
             * @param rect_coord The QRectF value to convert.
             * @return the constucted RectWorldCoord.
             */
            static RectWorldCoord fromQRectF(const QRectF& rect_coord) { return RectWorldCoord(rect_coord); }

        private:

            /**
             * RectWorldCoord represents a real-world coordinate box in decimal degrees.
             * @param rect_coord The QRectF to convert in decimal degrees.
             */
            RectWorldCoord(const QRectF& rect_coord) : QRectF(rect_coord) { }

        };

        /**
         * Represents a box in pixels (no relationship to viewport or world).
         */
        class QWIDGETMAP_EXPORT RectPx : public QRectF
        {

        public:

            /**
             * RectPx represents a box in pixels (no relationship to viewport or world).
             * @param top_left_px The top left point in pixels.
             * @param bottom_right_px The bottom_right in pixels.
             */
            RectPx(const PointPx& top_left_px, const PointPx& bottom_right_px) : QRectF(top_left_px, bottom_right_px) { }

            /**
             * RectPx represents a box in pixels (no relationship to viewport or world).
             * @param top_left_px The top left point in pixels.
             * @param size_px The size of the box in pixels.
             */
            RectPx(const PointPx& top_left_px, const QSizeF& size_px) : QRectF(top_left_px, size_px) { }

            /**
             * Returns the left point in pixels.
             * @return the left point in pixels.
             */
            inline qreal leftPx() const { return QRectF::left(); }

            /**
             * Returns the top point in pixels.
             * @return the top point in pixels.
             */
            inline qreal topPx() const { return QRectF::top(); }

            /**
             * Returns the right point in pixels.
             * @return the right point in pixels.
             */
            inline qreal rightPx() const { return QRectF::right(); }

            /**
             * Returns the bottom point in pixels.
             * @return the bottom point in pixels.
             */
            inline qreal bottomPx() const { return QRectF::bottom(); }

        };

        /**
         * Represents a real-world box in pixels.
         */
        class QWIDGETMAP_EXPORT RectWorldPx : public RectPx
        {

        public:

            /**
             * RectWorldPx represents a real-world box in pixels.
             * @param top_left_px The top left point in pixels.
             * @param bottom_right_px The bottom right in pixels.
             */
            RectWorldPx(const PointWorldPx& top_left_px, const PointWorldPx& bottom_right_px) : RectPx(top_left_px, bottom_right_px) { }

            /**
             * RectWorldPx represents a real-world box in pixels.
             * @param top_left_px The top left point in pixels.
             * @param size_px The size in pixels.
             */
            RectWorldPx(const PointWorldPx& top_left_px, const QSizeF& size_px) : RectPx(top_left_px, size_px) { }

            /**
             * Returns the center point in pixels.
             * @return the center point in pixels.
             */
            inline PointWorldPx centerPx() const { return PointWorldPx(QRectF::center().x(), QRectF::center().y()); }

            /**
             * Returns the top-left point in pixels.
             * @return the top-left point in pixels.
             */
            inline PointWorldPx topLeftPx() const { return PointWorldPx(leftPx(), topPx()); }

            /**
             * Returns the bottom-right point in pixels.
             * @return the bottom-right point in pixels.
             */
            inline PointWorldPx bottomRightPx() const { return PointWorldPx(rightPx(), bottomPx()); }

        };

        /**
         * Represents a viewport (on-screen) box in pixels.
         */
        class QWIDGETMAP_EXPORT RectViewportPx : public RectPx
        {

        public:

            /**
             * RectViewportPx represents a viewport (on-screen) box in pixels.
             * @param top_left_px The top left point in pixels.
             * @param bottom_right_px The bottom right in pixels.
             */
            RectViewportPx(const PointViewportPx& top_left_px, const PointViewportPx& bottom_right_px) : RectPx(top_left_px, bottom_right_px) { }

            /**
             * RectViewportPx represents a viewport (on-screen) box in pixels.
             * @param top_left_px The top left point in pixels.
             * @param size_px The size in pixels.
             */
            RectViewportPx(const PointViewportPx& top_left_px, const QSizeF& size_px) : RectPx(top_left_px, size_px) { }

            /**
             * Returns the center point in pixels.
             * @return the center point in pixels.
             */
            inline PointViewportPx centerPx() const { return PointViewportPx(QRectF::center().x(), QRectF::center().y()); }

            /**
             * Returns the top-left point in pixels.
             * @return the top-left point in pixels.
             */
            inline PointViewportPx topLeftPx() const { return PointViewportPx(leftPx(), topPx()); }

            /**
             * Returns the bottom-right point in pixels.
             * @return the bottom-right point in pixels.
             */
            inline PointViewportPx bottomRightPx() const { return PointViewportPx(rightPx(), bottomPx()); }

        };

    }

}
