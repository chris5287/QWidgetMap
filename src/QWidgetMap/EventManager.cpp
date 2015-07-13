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

#include "EventManager.h"

// STL includes.
#include <cmath>

// Local includes.
#include "draw/geometry/GeometryEllipse.h"
#include "draw/geometry/GeometryLineString.h"
#include "draw/geometry/GeometryPolygon.h"
#include "util/Algorithms.h"

using namespace qwm;

EventManager::EventManager(const std::shared_ptr<ViewportManager>& viewport_manager, const std::shared_ptr<LayerManager>& layer_manager, QObject* parent)
    : QObject(parent),
      m_viewport_manager(viewport_manager),
      m_layer_manager(layer_manager),
      m_inertia_event_manager(viewport_manager, parent)
{

}

void EventManager::setPreviewColours(const QPen& pen, const QBrush& brush, const double& opacity)
{
    // Set the preview settings.
    m_preview_pen = pen;
    m_preview_brush = brush;
    m_preview_opacity = opacity;
}

void EventManager::setPreviewLineWidth(const double& width_px)
{
    // Set the preview line width.
    m_preview_line_width_px = width_px;
}

EventManager::MouseButtonMode EventManager::mouseButtonLeftMode()
{
    // Return the left mouse button mode.
    return m_mouse_left_mode;
}

void EventManager::setMouseButtonLeft(const MouseButtonMode& mode)
{
    // Set the left mouse button settings.
    m_mouse_left_mode = mode;

    // Reset mouse points for polygon.
    m_mouse_positions_coord.clear();
}

EventManager::MouseButtonMode EventManager::mouseButtonRightMode()
{
    // Return the right mouse button mode.
    return m_mouse_right_mode;
}

void EventManager::setMouseButtonRight(const MouseButtonMode& mode)
{
    // Set the right mouse button settings.
    m_mouse_right_mode = mode;

    // Reset mouse points for polygon.
    m_mouse_positions_coord.clear();
}

bool EventManager::eventFilter(QObject* /*object*/, QEvent* event)
{
    // Switch on the event types.
    switch(event->type())
    {
        case QEvent::MouseButtonPress:
        {
            mousePressEvent(static_cast<QMouseEvent*>(event));
            break;
        }
        case QEvent::MouseButtonRelease:
        {
            mouseReleaseEvent(static_cast<QMouseEvent*>(event));
            break;
        }
        case QEvent::MouseButtonDblClick:
        {
            mouseDoubleClickEvent(static_cast<QMouseEvent*>(event));
            break;
        }
        case QEvent::MouseMove:
        {
            mouseMoveEvent(static_cast<QMouseEvent*>(event));
            break;
        }
        case QEvent::Wheel:
        {
            wheelEvent(static_cast<QWheelEvent*>(event));
            break;
        }
        case QEvent::KeyPress:
        {
            keyPressEvent(static_cast<QKeyEvent*>(event));
            break;
        }
        default:
        {
            // Ignore.
            break;
        }
    }

    // Return false to allow other objects to process the event.
    return false;
}

void EventManager::mousePressEvent(QMouseEvent* mouse_event)
{
    // Fetch the current viewport manager.
    const Viewport current_viewport(*(m_viewport_manager.get()));

    // Store the mouse location of the current/starting mouse click.
    m_mouse_position_current_px = util::PointViewportPx(mouse_event->localPos().x(), mouse_event->localPos().y());
    m_mouse_position_pressed_px = m_mouse_position_current_px;

    // Loop through each layer and pass the mouse event on.
    for(const auto& layer : m_layer_manager->layers())
    {
        // Send the mouse press event to the layer.
        layer->mousePressEvent(mouse_event, current_viewport);
    }

    // Left button pressed?
    EventManager::MouseButtonMode mouse_mode(EventManager::MouseButtonMode::None);
    if(mouse_event->button() == Qt::MouseButton::LeftButton)
    {
        // Capture the left mouse press.
        m_mouse_left_pressed = true;

        // Capture the left mouse mode.
        mouse_mode = m_mouse_left_mode;
    }
    // Right button pressed?
    else if(mouse_event->button() == Qt::MouseButton::RightButton)
    {
        // Capture the right mouse press.
        m_mouse_right_pressed = true;

        // Capture the right mouse mode.
        mouse_mode = m_mouse_right_mode;
    }

    // Are we in panning mode?
    if(mouse_mode == EventManager::MouseButtonMode::Pan)
    {
        // Inform the inertia event manager of the press event.
        m_inertia_event_manager.mouseEvent(mouse_event);
    }

    // Emit the pressed mouse event with the current mouse coordinate.
    emit mouseEventPressCoordinate(mouse_event, projection::toPointWorldCoord(current_viewport, m_mouse_position_current_px));
}

void EventManager::mouseReleaseEvent(QMouseEvent* mouse_event)
{
    // Fetch the current viewport manager.
    const Viewport current_viewport(*(m_viewport_manager.get()));

    // Store the mouse location of the current mouse click.
    m_mouse_position_current_px = util::PointViewportPx(mouse_event->localPos().x(), mouse_event->localPos().y());

    // Convert to world coordinates.
    const util::PointWorldCoord mouse_pressed_coord(projection::toPointWorldCoord(current_viewport, m_mouse_position_pressed_px));
    const util::PointWorldCoord mouse_released_coord(projection::toPointWorldCoord(current_viewport, m_mouse_position_current_px));

    // Left button released?
    EventManager::MouseButtonMode mouse_mode(EventManager::MouseButtonMode::None);
    if(mouse_event->button() == Qt::MouseButton::LeftButton)
    {
        // Capture the left mouse button release.
        m_mouse_left_pressed = false;

        // Capture the left mouse mode.
        mouse_mode = m_mouse_left_mode;
    }
    // Right button released?
    else if(mouse_event->button() == Qt::MouseButton::RightButton)
    {
        // Capture the right mouse button release.
        m_mouse_right_pressed = false;

        // Capture the right mouse mode.
        mouse_mode = m_mouse_right_mode;
    }

    // Are we in panning mode?
    if(mouse_mode == EventManager::MouseButtonMode::Pan)
    {
        // Inform the inertia event manager of the release event.
        m_inertia_event_manager.mouseEvent(mouse_event);
    }
    // Are we in pan ... mode?
    else if(mouse_mode == EventManager::MouseButtonMode::PanBox ||
            mouse_mode == EventManager::MouseButtonMode::PanLine)
    {
        // Capture coords to move/zoom with.
        std::vector<util::PointWorldCoord> coords;
        coords.push_back(mouse_pressed_coord);
        coords.push_back(mouse_released_coord);

        // Tell the map control to move and zoom as required by pan ... mode.
        m_viewport_manager->setFocusPointWorldMean(coords, true);
    }
    // Are we in pan circle mode?
    else if(mouse_mode == EventManager::MouseButtonMode::PanCircle)
    {
        // Calculate the distance between the points.
        const double distance(util::algorithms::distance(mouse_pressed_coord, mouse_released_coord));

        // Create ellipse geometry.
        const draw::geometry::GeometryEllipse geometry_ellipse(mouse_pressed_coord, distance, distance, 0);

        // Tell the map control to move and zoom as required by pan ... mode.
        m_viewport_manager->setFocusPointWorldMean(geometry_ellipse.boundingBox(current_viewport).toStdVector(), true);
    }
    // Else, are we in select ... mode?
    else if(mouse_mode == EventManager::MouseButtonMode::SelectBox ||
            mouse_mode == EventManager::MouseButtonMode::SelectLine ||
            mouse_mode == EventManager::MouseButtonMode::SelectCircle)
    {
        // Convert to world coordinates.
        util::PointWorldCoord bounding_top_left_coord(mouse_pressed_coord);
        util::PointWorldCoord bounding_bottom_right_coord(mouse_released_coord);

        // Construct geometry to compare against (default is polygon/rect).
        std::unique_ptr<draw::geometry::Geometry> geometry_to_compare_coord(new draw::geometry::GeometryPolygon(util::RectWorldCoord(bounding_top_left_coord, bounding_bottom_right_coord).toStdVector()));
        if(mouse_mode == EventManager::MouseButtonMode::SelectLine)
        {
            // Set the search line width to match the same value as the preview line width.
            QPen line_pen;
            line_pen.setWidthF(m_preview_line_width_px);

            // Create line string geometry.
            geometry_to_compare_coord.reset(new draw::geometry::GeometryLineString(std::vector<util::PointWorldCoord>{bounding_top_left_coord, bounding_bottom_right_coord}));
            geometry_to_compare_coord->setPen(line_pen);
        }
        else if(mouse_mode == EventManager::MouseButtonMode::SelectCircle)
        {
            // Calculate the distance between the points.
            const double distance(util::algorithms::distance(mouse_pressed_coord, mouse_released_coord));

            // Create ellipse geometry.
            geometry_to_compare_coord.reset(new draw::geometry::GeometryEllipse(mouse_pressed_coord, distance, distance, 0));

            // Recalculate top-left and bottom-right coordinates for the bounding rect fetch.
            bounding_top_left_coord = geometry_to_compare_coord->boundingBox(current_viewport).topLeftCoord();
            bounding_bottom_right_coord = geometry_to_compare_coord->boundingBox(current_viewport).bottomRightCoord();
        }
        /// SelectPolygon is handled by double click event.

        // Collection of selected geometries.
        std::map<std::string, std::vector<std::shared_ptr<draw::geometry::Geometry>>> selected_geometries;

        // Loop through each layer to check geometries touches.
        for(const auto& layer : m_layer_manager->layers())
        {
            // Is the layer visible?
            if(layer->isVisible(current_viewport))
            {
                // Loop through each geometry for the layer.
                for(const auto& geometry : layer->drawableGeometries(util::RectWorldCoord(bounding_top_left_coord, bounding_bottom_right_coord)))
                {
                    // Does the geometry touch our area rect?
                    if(geometry->touches(*(geometry_to_compare_coord.get()), current_viewport))
                    {
                        // Add the geometry to the selected collection.
                        selected_geometries[layer->name()].push_back(geometry);
                    }
                }
            }
        }

        // Emit the geometries selected.
        emit geometriesSelected(selected_geometries);
    }
    // Are we in draw/pan/selection polygon mode?
    else if(mouse_mode == EventManager::MouseButtonMode::DrawPolygon ||
            mouse_mode == EventManager::MouseButtonMode::PanPolygon ||
            mouse_mode == EventManager::MouseButtonMode::SelectPolygon)
    {
        // Should we ignore this mouse release?
        if(m_mouse_release_ignore == true)
        {
            // Unmark the release flag ready for the next release.
            m_mouse_release_ignore = false;
        }
        else
        {
            // Add the point to the current list of pressed points (used for polygons).
            m_mouse_positions_coord.push_back(mouse_released_coord);
        }
    }

    // Schedule a repaint to remove any potential screen artifacts.
    emit requestRedraw();

    // Emit the released mouse event with the press and release mouse coordinates.
    emit mouseEventReleaseCoordinate(mouse_event, mouse_pressed_coord, mouse_released_coord);
}

void EventManager::mouseDoubleClickEvent(QMouseEvent* mouse_event)
{
    // Fetch the current viewport manager.
    const Viewport current_viewport(*(m_viewport_manager.get()));

    // Store the mouse location of the current mouse click.
    m_mouse_position_current_px = util::PointViewportPx(mouse_event->localPos().x(), mouse_event->localPos().y());

    // Left button double clicked?
    EventManager::MouseButtonMode mouse_mode(EventManager::MouseButtonMode::None);
    if(mouse_event->button() == Qt::MouseButton::LeftButton)
    {
        // Capture the left mouse mode.
        mouse_mode = m_mouse_left_mode;
    }
    // Right button double clicked?
    else if(mouse_event->button() == Qt::MouseButton::RightButton)
    {
        // Capture the right mouse mode.
        mouse_mode = m_mouse_right_mode;
    }

    // Are we in draw/pan/selection polygon mode?
    if(mouse_mode == EventManager::MouseButtonMode::DrawPolygon ||
       mouse_mode == EventManager::MouseButtonMode::PanPolygon ||
       mouse_mode == EventManager::MouseButtonMode::SelectPolygon)
    {
        // Are we in pan polygon mode?
        if(mouse_mode == EventManager::MouseButtonMode::PanPolygon)
        {
            // Tell the map control to move and zoom as required by pan polygon mode.
            m_viewport_manager->setFocusPointWorldMean(m_mouse_positions_coord, true);
        }
        // Are we in selection polygon mode?
        else if(mouse_mode == EventManager::MouseButtonMode::SelectPolygon)
        {
            // Perform selection based on polygon.
            const draw::geometry::GeometryPolygon geometry_polygon(m_mouse_positions_coord);

            // Collection of selected geometries.
            std::map<std::string, std::vector<std::shared_ptr<draw::geometry::Geometry>>> selected_geometries;

            // Loop through each layer to check geometries touches.
            for(const auto& layer : m_layer_manager->layers())
            {
                // Is the layer visible?
                if(layer->isVisible(current_viewport))
                {
                    // Loop through each geometry for the layer.
                    for(const auto& geometry : layer->drawableGeometries(util::RectWorldCoord::fromQRectF(geometry_polygon.toQPolygonF().boundingRect())))
                    {
                        // Does the geometry touch our area rect?
                        if(geometry->touches(geometry_polygon, current_viewport))
                        {
                            // Add the geometry to the selected collection.
                            selected_geometries[layer->name()].push_back(geometry);
                        }
                    }
                }
            }

            // Emit the geometries selected.
            emit geometriesSelected(selected_geometries);
        }

        // Emit the mouse event with polygon details.
        emit mouseEventDoubleClickCoordinates(mouse_event, m_mouse_positions_coord);

        // Reset mouse points for polygon.
        m_mouse_positions_coord.clear();

        // Mark that the next mouse release command should be ignored during polygon modes. .
        m_mouse_release_ignore = true;
    }

    // Schedule a repaint to remove any potential screen artifacts.
    emit requestRedraw();

    // Emit the double click mouse event with the press and current mouse coordinate.
    emit mouseEventDoubleClickCoordinate(mouse_event, projection::toPointWorldCoord(current_viewport, m_mouse_position_pressed_px), projection::toPointWorldCoord(current_viewport, m_mouse_position_current_px));
}

void EventManager::mouseMoveEvent(QMouseEvent* mouse_event)
{
    // Fetch the current viewport manager.
    const Viewport current_viewport(*(m_viewport_manager.get()));

    // Update the current mouse position.
    m_mouse_position_current_px = util::PointViewportPx(mouse_event->localPos().x(), mouse_event->localPos().y());

    // Left button still pressed?
    EventManager::MouseButtonMode mouse_mode(EventManager::MouseButtonMode::None);
    if(mouse_event->buttons() & Qt::MouseButton::LeftButton)
    {
        // Capture the left mouse mode.
        mouse_mode = m_mouse_left_mode;
    }
    // Right button still pressed?
    else if(mouse_event->buttons() & Qt::MouseButton::RightButton)
    {
        // Capture the right mouse mode.
        mouse_mode = m_mouse_right_mode;
    }

    // Are we in panning mode?
    if(mouse_mode == EventManager::MouseButtonMode::Pan)
    {
        // Inform the inertia event manager of the move event.
        m_inertia_event_manager.mouseEvent(mouse_event);

        // Update the left mouse pressed location.
        m_mouse_position_pressed_px = m_mouse_position_current_px;
    }

    // Schedule a repaint to remove any potential screen artifacts.
    emit requestRedraw();

    // Emit the moved mouse event with the press and current mouse coordinates.
    emit mouseEventMoveCoordinate(mouse_event, projection::toPointWorldCoord(current_viewport, m_mouse_position_pressed_px), projection::toPointWorldCoord(current_viewport, m_mouse_position_current_px));
}

void EventManager::wheelEvent(QWheelEvent* wheel_event)
{
    // Fetch the current viewport manager.
    const Viewport current_viewport(*(m_viewport_manager.get()));

    // Is the vertical angle delta positive? (Zoom in)
    if(wheel_event->angleDelta().y() > 0)
    {
        // Check the current zoom is less than maximum zoom.
        if(current_viewport.zoom() < m_viewport_manager->zoomMaximum())
        {
            // Capture the current wheel point at the current zoom level.
            const util::PointViewportPx wheel_px(wheel_event->posF().x(), wheel_event->posF().y());
            const util::PointWorldCoord wheel_coord(projection::toPointWorldCoord(current_viewport, wheel_px));
            const util::PointPx wheel_delta(current_viewport.focusPointWorldPx() - current_viewport.toPointWorldPx(wheel_px));

            // Zoom in.
            m_viewport_manager->zoomIn();

            // Fetch the updated viewport.
            const Viewport updated_viewport(*(m_viewport_manager.get()));

            // Google-style zoom...
            m_viewport_manager->setFocusPointWorldCoord(projection::toPointWorldCoord(updated_viewport, projection::toPointWorldPx(updated_viewport, wheel_coord) + wheel_delta));

            // Tell parents we have accepted this events.
            wheel_event->accept();
        }
        else
        {
            // Tell parents we have ignored this events.
            wheel_event->ignore();
        }
    }
    // Is the vertical angle delta negative? (Zoom out)
    else if(wheel_event->angleDelta().y() < 0)
    {
        // Check the current zoom is greater than minimum zoom.
        if(current_viewport.zoom() > m_viewport_manager->zoomMinimum())
        {
            // Capture the current wheel point at the current zoom level.
            const util::PointViewportPx wheel_px(wheel_event->posF().x(), wheel_event->posF().y());
            const util::PointWorldCoord wheel_coord(projection::toPointWorldCoord(current_viewport, wheel_px));
            const util::PointPx wheel_delta(current_viewport.focusPointWorldPx() - current_viewport.toPointWorldPx(wheel_px));

            // Zoom out.
            m_viewport_manager->zoomOut();

            // Fetch the updated viewport.
            const Viewport updated_viewport(*(m_viewport_manager.get()));

            // Google-style zoom...
            m_viewport_manager->setFocusPointWorldCoord(projection::toPointWorldCoord(updated_viewport, projection::toPointWorldPx(updated_viewport, wheel_coord) + wheel_delta));

            // Tell parents we have accepted this events.
            wheel_event->accept();
        }
        else
        {
            // Tell parents we have ignored this events.
            wheel_event->ignore();
        }
    }
}

void EventManager::keyPressEvent(QKeyEvent* key_event)
{
    // Handle the cases we want.
    switch(key_event->key())
    {
        // Up arrow key.
        case Qt::Key_Up:
        {
            m_viewport_manager->scrollUp();
            break;
        }
        // Down arrow key.
        case Qt::Key_Down:
        {
            m_viewport_manager->scrollDown();
            break;
        }
        // Left arrow key.
        case Qt::Key_Left:
        {
            m_viewport_manager->scrollLeft();
            break;
        }
        // Right arrow key.
        case Qt::Key_Right:
        {
            m_viewport_manager->scrollRight();
            break;
        }
        // Zoom in.
        case Qt::Key_Plus:
        {
            m_viewport_manager->zoomIn();
            break;
        }
        // Zoom out.
        case Qt::Key_Minus:
        {
            m_viewport_manager->zoomOut();
            break;
        }
        // Else... do nothing.
        default:
        {
            break;
        }
    }

    // Emit the key pressed event.
    emit keyEventPressed(key_event);
}

void EventManager::previewEvent(const Viewport& viewport, QPainter& painter)
{
    // Save the current painter's state.
    painter.save();

    // Set the pen and brush colours.
    painter.setPen(m_preview_pen);
    painter.setBrush(m_preview_brush);
    painter.setOpacity(m_preview_opacity);

    // Is the mouse pressed (left or right)?
    if(m_mouse_left_pressed || m_mouse_right_pressed)
    {
        // Default to the left mouse button options.
        MouseButtonMode mouse_mode(m_mouse_left_mode);

        // Has the right mouse button been pressed?
        if(m_mouse_right_pressed)
        {
            // Set to the right mouse button options.
            mouse_mode = m_mouse_right_mode;
        }

        // Handle the mouse mode.
        switch(mouse_mode)
        {
            // Is the mouse mode set to draw/pan/select a box.
            case MouseButtonMode::DrawBox:
            case MouseButtonMode::PanBox:
            case MouseButtonMode::SelectBox:
            {
                // Draw rect.
                painter.drawRect(util::RectViewportPx(m_mouse_position_pressed_px, m_mouse_position_current_px));

                // Finished.
                break;
            }
            // Is the mouse mode set to draw/pan/select a line.
            case MouseButtonMode::DrawLine:
            case MouseButtonMode::PanLine:
            case MouseButtonMode::SelectLine:
            {
                // Capture the pen and set the line width.
                QPen line_pen(painter.pen());
                line_pen.setWidthF(m_preview_line_width_px);
                painter.setPen(line_pen);

                // Draw line.
                painter.drawLine(m_mouse_position_pressed_px, m_mouse_position_current_px);

                // Finished.
                break;
            }
            // Is the mouse mode set to draw/pan/select a circle.
            case MouseButtonMode::DrawCircle:
            case MouseButtonMode::PanCircle:
            case MouseButtonMode::SelectCircle:
            {
                // Draw the crosshair at the mouse start point.
                painter.setPen(Qt::black);
                // |
                painter.drawLine(m_mouse_position_pressed_px.x(), m_mouse_position_pressed_px.y() - 1.0,
                                 m_mouse_position_pressed_px.x(), m_mouse_position_pressed_px.y() + 1.0);
                // -
                painter.drawLine(m_mouse_position_pressed_px.x() - 1.0, m_mouse_position_pressed_px.y(),
                                 m_mouse_position_pressed_px.x() + 1.0, m_mouse_position_pressed_px.y());
                painter.setPen(m_preview_pen);

                // Convert to world coordinates.
                const util::PointWorldCoord mouse_pressed_coord(projection::toPointWorldCoord(viewport, m_mouse_position_pressed_px));
                const util::PointWorldCoord mouse_released_coord(projection::toPointWorldCoord(viewport, m_mouse_position_current_px));

                // Calculate the distance between the points.
                const double distance(util::algorithms::distance(mouse_pressed_coord, mouse_released_coord));

                // Draw the projected circle.
                draw::geometry::GeometryEllipse(mouse_pressed_coord, distance, distance, 0).drawPreview(painter, viewport, m_mouse_position_pressed_px);

                // Finished.
                break;
            }
            // Is the mouse mode set to draw/pan/select a polygon.
            case MouseButtonMode::DrawPolygon:
            case MouseButtonMode::PanPolygon:
            case MouseButtonMode::SelectPolygon:
            {
                // Handle outside mouse pressing (polygons stay on screen until double click/cancel).
                break;
            }
            // Else nothing to do.
            case MouseButtonMode::None:
            case MouseButtonMode::Pan:
            {
                // Ignore.
                break;
            }
        }
    }

    // Are there points available (should only be when in draw/pan/select polygon mouse mode).
    if(m_mouse_positions_coord.empty() == false)
    {
        // Draw lines between all points available.
        for(std::size_t i = 1; i < m_mouse_positions_coord.size(); ++i)
        {
            // Convert point into viewport point.
            painter.drawLine(projection::toPointViewportPx(viewport, m_mouse_positions_coord.at(i - 1)),
                             projection::toPointViewportPx(viewport, m_mouse_positions_coord.at(i)));
        }

        // Draw points for each polygon click.
        QPolygonF polygon;
        const double point_size(5.0);
        for(const auto& point : m_mouse_positions_coord)
        {
            // Convert point into viewport point.
            const auto viewpoint_px(projection::toPointViewportPx(viewport, point));

            // Draw the point.
            painter.drawRect(viewpoint_px.x() - (point_size / 2.0), viewpoint_px.y() - (point_size / 2.0), point_size, point_size);

            // Add the point to polygon.
            polygon.push_back(viewpoint_px);
        }

        // Draw the polygon.
        painter.drawPolygon(polygon);
    }

    // Restore the painter's state.
    painter.restore();
}
