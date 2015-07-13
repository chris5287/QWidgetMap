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
#include <QtCore/QObject>
#include <QtCore/QVariant>
#include <QtGui/QPainter>

// Local includes.
#include "../qwidgetmap_global.h"
#include "../Viewport.h"
#include "../util/Rect.h"

/// QWidgetMap namespace.
namespace qwm
{

    /// Drawable namespace.
    namespace draw
    {

        /// Drawable types.
        enum class DrawableType
        {
            /// Geoemtry.
            Geometry,

            /// Map.
            Map,

            /// ESRI Shapefile.
            ESRIShapefile
        };

        /**
         * Interface to be implemented to make an item drawable.
         */
        class QWIDGETMAP_EXPORT Drawable : public QObject
        {
            Q_OBJECT

        protected:

            /**
             * This constructs a drawable item.
             * @param drawable_type The drawable type.
             * @param parent QObject parent ownership.
             */
            explicit Drawable(const DrawableType& drawable_type, QObject* parent = nullptr);

        public:

            /// Disable copy constructor.
            Drawable(const Drawable&) = delete;

            /// Disable copy assignment.
            Drawable& operator=(const Drawable&) = delete;

            /// Destructor.
            virtual ~Drawable() = default;

        public:

            /**
             * Fetches the drawable type.
             * @return the drawable type.
             */
            const DrawableType& drawableType() const;

            /**
             * Fetches a meta-data value.
             * @param key The meta-data key.
             * @return the meta-data value.
             */
            QVariant metadata(const std::string& key) const;

            /**
             * Set a meta-data key/value.
             * @param key The meta-data key.
             * @param value The meta-data value.
             */
            void setMetadata(const std::string& key, const QVariant& value);

            /**
             * Fetches whether the drawable item is visible.
             * @return whether the drawable item is visible.
             */
            bool visible() const;

            /**
             * Fetches whether the drawable item is visible at a specific zoom level.
             * @param viewport The viewport to check.
             * @return whether the drawable item is visible at a specific zoom level.
             */
            virtual bool isVisible(const Viewport& viewport) const;

            /**
             * Set the visibility of the drawable item.
             * @param enabled Whether to make the drawable item visible.
             */
            void setVisible(const bool& enabled);

            /**
             * Set the minimum zoom level to show this drawable item at.
             * @param zoom_minimum The minimum zoom level to show this drawable item at.
             */
            void setZoomMinimum(const int& zoom_minimum = 0);

            /**
             * Set the maximum zoom level to show this drawable item at.
             * @param zoom_maximum The maximum zoom level to show this drawable item at.
             */
            void setZoomMaximum(const int& zoom_maximum = 17);

        public:

            /**
             * Draws the item to the provided painter.
             * @param painter The painter to draw on.
             * @param drawing_rect_world_coord The drawing rect in world coordinates.
             * @param viewport The current viewport to use.
             */
            virtual void draw(QPainter& painter, const util::RectWorldCoord& drawing_rect_world_coord, const Viewport& viewport) const = 0;

        signals:

            /**
             * Signal emitted when a drawable item is clicked.
             */
            void drawableClicked() const;

            /**
             * Signal emitted when a change has occurred that requires the drawable item to be redrawn.
             */
            void requestRedraw() const;

        private:

            /// The drawable type.
            const DrawableType m_drawable_type;

            /// Meta-data storage.
            std::map<std::string, QVariant> m_metadata;

            /// Whether the drawable item is visible.
            bool m_visible { true };

            /// Minimum zoom level to show this drawable item.
            int m_zoom_minimum = { 0 };

            /// Maximum zoom level to show this drawable item.
            int m_zoom_maximum = { 17 };

        };

    }

}
