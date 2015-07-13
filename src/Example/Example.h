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
#include <QtWidgets/QPushButton>

// QWidgetMap includes.
#include <QWidgetMap/QWidgetMap.h>

class Example : public QWidget
{
    Q_OBJECT

public:

    /**
     * This is used to construct a Example QWidget.
     * @param parent QWidget parent ownership.
     */
    Example(QWidget* parent = nullptr);

    /// Disable copy constructor.
    Example(const Example&) = delete;

    /// Disable copy assignment.
    Example& operator=(const Example&) = delete;

    /// Destructor.
    ~Example() = default;

private:

    /**
     * Setup map control.
     */
    void setupMaps();

    /**
     * Setups layout.
     */
    void setupLayout();

    /**
     * Handles QWidget resizing.
     * @param resize_event The QResizeEvent that occured.
     */
    void resizeEvent(QResizeEvent* event);

private slots:

    /**
     * Toggle whether to add points
     * @param enable Whether to add points.
     */
    void toggleAddPoint(bool enable);

    /**
     * Toggle between draw rect and pan modes.
     * @param enable Whether to enable draw rect mode.
     */
    void toggleDrawRect(bool enable);

    /**
     * Toggle whether a click moves the map.
     * @param enable Whether a click moves the map.
     */
    void toggleMoveClick(bool enable);

    /**
     * Slot to handle geometry selection events.
     * @param selected_geometries The geometries that have been selected.
     */
    void geometriesSelectedEvent(std::map<std::string, std::vector<std::shared_ptr<qwm::draw::geometry::Geometry>>> selected_geometries);

    /**
     * Slot to handle a drawable item that has been clicked.
     * @param drawable The drawable item that has been clicked.
     */
    void drawableClickEvent(const std::shared_ptr<qwm::draw::Drawable>& drawable);

    /**
     * Slot to handle mouse move/release events and key pressed events in the main map control.
     */
    void main_mouseKeyEventCoordinate();

    /**
     * Slot to handle mouse release events in the main map control.
     * @param mouse_event The QMouseEvent that occured.
     * @param press_coordinate The corresponding world coordinate of the mouse press.
     */
    void main_mouseEventPressCoordinate(QMouseEvent* mouse_event, qwm::util::PointWorldCoord press_coordinate);

    /**
     * Slot to handle mouse press events in the "mini" map control.
     * @param mouse_event The QMouseEvent that occured.
     * @param press_coordinate The corresponding world coordinate of the mouse press.
     */
    void mini_mouseEventPressCoordinate(QMouseEvent* mouse_event, qwm::util::PointWorldCoord press_coordinate);

private:

    /// The main map control.
    std::unique_ptr<qwm::QWidgetMap> m_map_control;

    /// The "mini" map control.
    std::unique_ptr<qwm::QWidgetMap> m_mini_map_control;

    /// Push button to toggle new points being added on a mouse click.
    QPushButton* m_button_add_point;

    /// Push button to toggle the mouse button drawing a rect.
    QPushButton* m_button_draw_rect;

    /// Push button to toggle the mouse click moving the map.
    QPushButton* m_button_move_click;

};
