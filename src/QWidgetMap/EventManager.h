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

// Local includes.
#include "qwidgetmap_global.h"
#include "LayerManager.h"
#include "ViewportManager.h"
#include "util/InertiaEventManager.h"

/// QWidgetMap namespace.
namespace qwm
{

    /**
     * Manages the mouse, wheel and key events.
     */
    class QWIDGETMAP_EXPORT EventManager : public QObject
    {
        Q_OBJECT

    public:

        /// Mouse buttons mode types.
        enum class MouseButtonMode
        {
            /// No action.
            None,

            /// Move the map.
            Pan,

            /// Draw a box.
            DrawBox,

            /// Pan box.
            PanBox,

            /// Selection box.
            SelectBox,

            /// Draw a line.
            DrawLine,

            /// Pan line.
            PanLine,

            /// Selection line.
            SelectLine,

            /// Draw a circle.
            DrawCircle,

            /// Pan circle.
            PanCircle,

            /// Selection circle.
            SelectCircle,

            /// Draw a polygon.
            DrawPolygon,

            /// Pan polygon.
            PanPolygon,

            /// Selection polygon.
            SelectPolygon
        };

    public:

        /**
         * EventManager constructor.
         * @param viewport_manager The viewport manager to use.
         * @param layer_manager The layer manager to use.
         * @param parent QObject parent ownership.
         */
        EventManager(const std::shared_ptr<ViewportManager>& viewport_manager, const std::shared_ptr<LayerManager>& layer_manager, QObject* parent = nullptr);

        /// Disable copy constructor.
        EventManager(const EventManager&) = delete;

        /// Disable copy assignment.
        EventManager& operator=(const EventManager&) = delete;

        /// Destructor.
        ~EventManager() = default;

    public:

        /**
         * Set the colours used when displaying previews (pan/draw/selection shapes).
         * @param pen The pen to use.
         * @param brush The brush to use.
         * @param opacity The opacity to use (between 0.0 - transparent and 1.0 - opaque).
         */
        void setPreviewColours(const QPen& pen = QPen(QColor(66, 132, 253)), const QBrush& brush = QBrush(QColor(66, 132, 253)), const double& opacity = 0.4);

        /**
         * Set the line width to use when displaying line previews (pan/draw/selection).
         * @param width_px The width to use in pixels.
         */
        void setPreviewLineWidth(const double& width_px = 5.0);

        /**
         * Fetches the left mouse button mode.
         * @return the left mouse button mode.
         */
        EventManager::MouseButtonMode mouseButtonLeftMode();

        /**
         * Sets the left mouse button mode.
         * @param mode The mouse mode required.
         */
        void setMouseButtonLeft(const MouseButtonMode& mode);

        /**
         * Fetches the right mouse button mode.
         * @return the right mouse button mode.
         */
        EventManager::MouseButtonMode mouseButtonRightMode();

        /**
         * Sets the right mouse button mode.
         * @param mode The mouse mode required.
         */
        void setMouseButtonRight(const MouseButtonMode& mode);

    public:

        /**
         * Event filter.
         * @param object Object that sent the event.
         * @param event The event.
         * @return whether to stop processing the event.
         */
        bool eventFilter(QObject* object, QEvent* event);

        /**
         * Called when a mouse button is pressed down.
         * @param mouse_event The mouse event.
         */
        void mousePressEvent(QMouseEvent* mouse_event);

        /**
         * Called when a mouse button is released.
         * @param mouse_event The mouse event.
         */
        void mouseReleaseEvent(QMouseEvent* mouse_event);

        /**
         * Called when a mouse button is double clicked.
         * @param mouse_event The mouse event.
         */
        void mouseDoubleClickEvent(QMouseEvent* mouse_event);

        /**
         * Called when a mouse is moved.
         * @param mouse_event The mouse event.
         */
        void mouseMoveEvent(QMouseEvent* mouse_event);

        /**
         * Called when a mouse's wheel is scrolled.
         * @param wheel_event The mouse wheel event.
         */
        void wheelEvent(QWheelEvent* wheel_event);

        /**
         * Called when a keyboard key is pressed.
         * @param key_event The keyboard key event.
         */
        void keyPressEvent(QKeyEvent* key_event);

        /**
         * Called during the QWidgetMap's paintEvent to allow any preview events to be drawn.
         * @param viewport The viewport to use.
         * @param painter The painter to draw to.
         */
        void previewEvent(const Viewport& viewport, QPainter& painter);

    signals:

        /**
         * Signal emitted when a change has occurred that requires the widget to be redrawn.
         */
        void requestRedraw() const;

        /**
         * Signal emitted when geometries are selected (see MouseButtonModes).
         * @param selected_geometries The selected geometries in each layer.
         */
        void geometriesSelected(std::map<std::string, std::vector<std::shared_ptr<draw::geometry::Geometry>>> selected_geometries);

        /**
         * Signal emitted on MousePressEvents with the additional map coordinates of the mouse press.
         * @param mouse_event The QMouseEvent that occured.
         * @param press_coordinate The corresponding world coordinate of the mouse press.
         */
        void mouseEventPressCoordinate(QMouseEvent* mouse_event, util::PointWorldCoord press_coordinate);

        /**
         * Signal emitted on MouseReleaseEvents with the additional map coordinates of the mouse press/release.
         * @param mouse_event The QMouseEvent that occured.
         * @param press_coordinate The corresponding world coordinate of the mouse press.
         * @param release_coordinate The corresponding world coordinate of the mouse release.
         */
        void mouseEventReleaseCoordinate(QMouseEvent* mouse_event, util::PointWorldCoord press_coordinate, util::PointWorldCoord release_coordinate);

        /**
         * Signal emitted on MouseDoubleClickEvents with the additional map coordinates of the mouse press/double press.
         * @param mouse_event The QMouseEvent that occured.
         * @param press_coordinate The corresponding world coordinate of the mouse press.
         * @param double_press_coordinate The corresponding world coordinate of the mouse double press.
         */
        void mouseEventDoubleClickCoordinate(QMouseEvent* mouse_event, util::PointWorldCoord press_coordinate, util::PointWorldCoord double_press_coordinate);

        /**
         * Signal emitted on MouseDoubleClickEvents with the additional map coordinates of the mouse presses (polygon usage).
         * @param mouse_event The QMouseEvent that occured.
         * @param pressed_coordinates The corresponding world coordinate of the mouse presses.
         */
        void mouseEventDoubleClickCoordinates(QMouseEvent* mouse_event, std::vector<util::PointWorldCoord> pressed_coordinates);

        /**
         * Signal emitted on MouseMoveEvents with the additional map coordinates of the mouse press/current.
         * @param mouse_event The QMouseEvent that occured.
         * @param press_coordinate The corresponding world coordinate of the mouse press.
         * @param current_coordinate The corresponding world coordinate of the current mouse.
         */
        void mouseEventMoveCoordinate(QMouseEvent* mouse_event, util::PointWorldCoord press_coordinate, util::PointWorldCoord current_coordinate);

        /**
         * Signal emitted when a keyboard key is pressed.
         * @param key_event The keyboard key event.
         */
        void keyEventPressed(QKeyEvent* key_event);

    private:

        /// The viewport manager to use.
        std::shared_ptr<ViewportManager> m_viewport_manager;

        /// The layer manager to use.
        std::shared_ptr<LayerManager> m_layer_manager;

        /// The inertia event manager to use.
        util::InertiaEventManager m_inertia_event_manager;

    private:

        /// The pen to use for previews (pan/draw/selection shapes).
        QPen m_preview_pen { QColor(66, 132, 253) };

        /// The brush to use for previews (pan/draw/selection shapes).
        QBrush m_preview_brush { QColor(66, 132, 253) };

        /// The opacoty of the previews (pan/draw/selection shapes).
        double m_preview_opacity { 0.4 };

        /// The pen width for line previews in pixels (pan/draw/selection lines).
        double m_preview_line_width_px { 5.0 };

        /// Whether the left mouse button is currently pressed.
        bool m_mouse_left_pressed { false };

        /// The left mouse button mode.
        MouseButtonMode m_mouse_left_mode { MouseButtonMode::Pan };

        /// Whether the right mouse button is currently pressed.
        bool m_mouse_right_pressed { false };

        /// The right mouse button mode.
        MouseButtonMode m_mouse_right_mode { MouseButtonMode::DrawBox };

        /// The mouse position when a mouse button was initially pressed in pixels.
        util::PointViewportPx m_mouse_position_pressed_px { 0.0, 0.0 };

        /// The current mouse position in pixels (set after every mouse event).
        util::PointViewportPx m_mouse_position_current_px { 0.0, 0.0 };

        /// The mouse positions in coord (set after every mouse event when in draw/pan/selection polygon).
        std::vector<util::PointWorldCoord> m_mouse_positions_coord;

        /// Whether the next mouse release should be ignored (polygon only).
        bool m_mouse_release_ignore { false };

    };

}
