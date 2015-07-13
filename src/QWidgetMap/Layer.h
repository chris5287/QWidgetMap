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
#include <QtCore/QObject>
#include <QtCore/QReadWriteLock>
#include <QtCore/QVariant>
#include <QtGui/QMouseEvent>
#include <QtGui/QPainter>

// STL includes.
#include <map>
#include <memory>
#include <string>
#include <vector>

// Local includes.
#include "qwidgetmap_global.h"
#include "Viewport.h"
#include "draw/Drawable.h"
#include "draw/geometry/Geometry.h"
#include "draw/geometry/GeometryFixed.h"
#include "util/Rect.h"
#include "util/QuadtreeContainer.h"

/// QWidgetMap namespace.
namespace qwm
{

    /**
     * Captures the drawables to display within a layer.
     */
    class QWIDGETMAP_EXPORT Layer : public QObject
    {
        Q_OBJECT

    public:

        /**
         * This is used to construct a layer.
         * @param name The name of the layer.
         * @param parent QObject parent ownership.
         */
        Layer(const std::string& name, QObject* parent = nullptr);

        /// Disable copy constructor.
        Layer(const Layer&) = delete;

        /// Disable copy assignment.
        Layer& operator=(const Layer&) = delete;

        /// Destructor.
        virtual ~Layer() = default;

    public:

        /**
         * Returns the layer's name.
         * @return the name of this layer.
         */
        const std::string& name() const;

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
         * Fetches whether the layer is visible at a specific zoom level.
         * @param viewport The viewport to check.
         * @return whether the layer is visible at a specific zoom level.
         */
        bool isVisible(const Viewport& viewport) const;

        /**
         * Set the visibility of the layer.
         * @param enabled Whether to make the layer visible.
         */
        void setVisible(const bool& enabled);

        /**
         * Set the minimum zoom level to show this layer at.
         * @param zoom_minimum The minimum zoom level to show this layer at.
         */
        void setZoomMinimum(const int& zoom_minimum = 0);

        /**
         * Set the maximum zoom level to show this layer at.
         * @param zoom_maximum The maximum zoom level to show this layer at.
         */
        void setZoomMaximum(const int& zoom_maximum = 17);

        /**
         * Whether mouse events are currently enabled.
         * @return whether mouse events are enabled.
         */
        bool isMouseEventsEnabled() const;

        /**
         * Set whether the layer should handle mouse events.
         * @param enable Whether the layer should handle mouse events.
         */
        void setMouseEventsEnabled(const bool& enable);

    public:

        /**
         * Returns the drawable items (non-geometries) in this layer.
         * @return the drawable items (non-geometries) in this layer.
         */
        std::vector<std::shared_ptr<draw::Drawable>> drawableItems() const;

        /**
         * Returns the drawable geoemtries in this layer.
         * @param range_coord The bounding box range to limit the geometries that are fetched in coordinates.
         * @return the drawable geoemtries in this layer.
         */
        std::vector<std::shared_ptr<draw::geometry::Geometry>> drawableGeometries(const util::RectWorldCoord& range_coord) const;

        /**
         * Adds a drawable item/geometry to this Layer.
         * @param drawable The drawable item/geometry to add.
         * @param disable_redraw Whether to disable the redraw call after the drawable item/geometry is added.
         * @return whether the drawable item/geometry was added into this layer.
         */
        bool addDrawable(const std::shared_ptr<draw::Drawable>& drawable, const bool& disable_redraw = false);

        /**
         * Removes a drawable item/geometry from this Layer.
         * @param drawable The drawable item/geometry to remove.
         * @param disable_redraw Whether to disable the redraw call after the drawable item/geometry is removed.
         * @return whether the drawable item/geometry was removed into this layer.
         */
        bool removeDrawable(const std::shared_ptr<draw::Drawable>& drawable, const bool& disable_redraw = false);

        /**
         * Removes all drawable items from this Layer.
         * @param disable_redraw Whether to disable the redraw call after all drawable items are removed.
         */
        void clearDrawables(const bool& disable_redraw = false);

    public:

        /**
         * Handles mouse press events (such as left-clicking an item on the layer).
         * @param mouse_event The mouse event.
         * @param viewport The current viewport to use.
         * @param fuzzy_factor_px The "fuzzy-factor" in pixels allowed when searching for nearby drawable geometries.
         */
        void mousePressEvent(const QMouseEvent* mouse_event, const Viewport& viewport, const qreal& fuzzy_factor_px = 5.0) const;

        /**
         * Draws each map/geometry to a pixmap using the provided painter.
         * @param painter The painter to draw on.
         * @param drawing_rect_world_coord The drawing rect in world coordinates.
         * @param viewport The current viewport to use.
         */
        void draw(QPainter& painter, const util::RectWorldCoord& drawing_rect_world_coord, const Viewport& viewport) const;

    signals:

        /**
         * Signal emitted when a drawable item is clicked.
         * @param drawable The drawable item clicked.
         */
        void drawableClicked(const std::shared_ptr<draw::Drawable>& drawable) const;

        /**
         * Signal emitted when a change has occurred that requires the layer to be redrawn.
         */
        void requestRedraw() const;

    private:

        /// The layer name.
        const std::string m_name;

        /// Meta-data storage.
        std::map<std::string, QVariant> m_metadata;

        /// Whether the layer is visible.
        bool m_visible { true };

        /// Minimum zoom level to show this layer.
        int m_zoom_minimum { 0 };

        /// Maximum zoom level to show this layer.
        int m_zoom_maximum { 17 };

        /// Whether mouse events are enabled.
        bool m_mouse_events_enabled { true };

    private:

        /// List of drawable items drawn by this layer.
        std::vector<std::shared_ptr<draw::Drawable>> m_drawable_items;

        /// Mutex to protect drawable items.
        mutable QReadWriteLock m_drawable_items_mutex;

    private:

        /// List of drawable geometries (point) drawn by this layer.
        util::QuadtreeContainer<std::shared_ptr<draw::geometry::Geometry>> m_drawable_geometries_points { 50, util::RectWorldCoord(util::PointWorldCoord(-180.0, 90.0), util::PointWorldCoord(180.0, -90.0)) };

        /// List of drawable geometries (fixed) drawn by this layer.
        std::vector<std::shared_ptr<draw::geometry::GeometryFixed>> m_drawable_geometries_fixed;

        /// Mutex to protect drawable geometries.
        mutable QReadWriteLock m_drawable_geometries_mutex;

    };

}
