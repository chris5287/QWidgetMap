/**
 * @copyright 2015 Chris Stylianou
 * @copyright 2015 Jake Wade
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

#include "InertiaEventManager.h"

// Qt includes.
#include <QtGui/QCursor>

using namespace qwm;
using namespace qwm::util;

InertiaEventManager::InertiaEventManager(const std::shared_ptr<ViewportManager>& viewport_manager, QObject* parent)
    : QObject(parent),
      m_viewport_manager(viewport_manager)
{

}

void InertiaEventManager::mouseEvent(QMouseEvent* mouse_event)
{
    // Only handle MouseButtonPress/ButtonRelease/MouseMove events.
    if(mouse_event->type() == QEvent::MouseButtonPress ||
       mouse_event->type() == QEvent::MouseButtonRelease ||
       mouse_event->type() == QEvent::MouseMove)
    {
        // Then using kinetic state and the mouse event types, decide what to do...
        switch(m_kinetic_state)
        {
            case KineticState::Steady:
            {
                // If Steady && MousePress events...
                if(mouse_event->type() == QEvent::MouseButtonPress)
                {
                    // Alter the kinetic state to Pressed.
                    m_kinetic_state = KineticState::Pressed;
                }

                // Finished.
                break;
            }
            case KineticState::Pressed:
            {
                // If Pressed && MouseRelease events...
                if(mouse_event->type() == QEvent::MouseButtonRelease)
                {
                    // Alter the kinetic state to Steady, no scrolling performed.
                    m_kinetic_state = KineticState::Steady;
                }
                // If Pressed && MouseMove events...
                else if(mouse_event->type() == QEvent::MouseMove)
                {
                    // Record the current mouse position as the press point and the current drag point (used for calculating speed).
                    m_mouse_position_pressed_px = util::PointViewportPx(mouse_event->localPos().x(), mouse_event->localPos().y());
                    m_mouse_position_dragged_px = m_mouse_position_pressed_px;

                    // Alter the kinetic state to ManualScroll, allowing 1:1 mouse:scroll movement.
                    m_kinetic_state = KineticState::ManualScroll;

                    // Start the timer if it's not active.
                    if(m_kinetic_ticker.isActive() == false)
                    {
                        m_kinetic_ticker.start(20, this);
                    }
                }

                // Finished.
                break;
            }
            case KineticState::ManualScroll:
            {
                // If ManualScroll && MouseMove events...
                if(mouse_event->type() == QEvent::MouseMove)
                {
                    // Fetch the current mouse position.
                    const util::PointViewportPx current_pos_px(mouse_event->localPos().x(), mouse_event->localPos().y());

                    // Move the map by the offset between the last mouse pressed position and the current position.
                    m_viewport_manager->scroll(m_mouse_position_pressed_px - current_pos_px);

                    // Record the current mouse position as the press point.
                    m_mouse_position_pressed_px = current_pos_px;
                }
                // If ManualScroll && MouseRelease events...
                else if(mouse_event->type() == QEvent::MouseButtonRelease)
                {
                    // Alter the kinetic state to AutoScroll, allowing automatic scroll movement.
                    m_kinetic_state = KineticState::AutoScroll;
                }

                // Finished.
                break;
            }
            case KineticState::AutoScroll:
            {
                // If ManualScroll && MousePress events...
                if(mouse_event->type() == QEvent::MouseButtonPress)
                {
                    // Alter the kinetic state to Steady.
                    m_kinetic_state = KineticState::Steady;
                }

                // Finished.
                break;
            }
        }
    }
}

void InertiaEventManager::timerEvent(QTimerEvent* event)
{
    // Are we in ManualScroll mode.
    if(m_kinetic_state == KineticState::ManualScroll)
    {
        // Fetch the current mouse position.
        const QPoint current_mouse_position(QCursor::pos());
        const util::PointViewportPx current_mouse_position_px(current_mouse_position.x(), current_mouse_position.y());

        // Set the current kinetic speed of mouse.
        m_kinetic_speed = m_mouse_position_dragged_px - current_mouse_position_px;

        // Reset the number of steps to use when calculating deceleration.
        m_kinetic_deceleration_steps = 20;

        // Update the mouse dragged position.
        m_mouse_position_dragged_px = current_mouse_position_px;
    }
    // Are we in AutoScroll mode.
    else if(m_kinetic_state == KineticState::AutoScroll)
    {
        // Decelerate the speed.
        decelerateSpeed();

        // Move the map by the calculated speed.
        m_viewport_manager->scroll(m_kinetic_speed);

        // If the speed is 0.0, 0.0 (ie: not moving).
        if(m_kinetic_speed == PointPx(0.0, 0.0))
        {
            // Alter the kinetic state to Steady.
            m_kinetic_state = KineticState::Steady;
        }
    }

    // Call inherited QObject timer event.
    QObject::timerEvent(event);
}

void InertiaEventManager::decelerateSpeed()
{
    // Check we have a valid deceleration step to process.
    if(m_kinetic_deceleration_steps > 0)
    {
        // Maximum speed allowed in pixels.
        const double max_speed_px = 100.0;

        // Use qBounds to restrict the speed to the maximum kinetic speed allowed.
        const double x_speed(qBound(-max_speed_px, m_kinetic_speed.x(), max_speed_px));
        const double y_speed(qBound(-max_speed_px, m_kinetic_speed.y(), max_speed_px));

        // Calculate the current step distance required for the current deceleration steps remaining.
        const double x_step_distance(x_speed / m_kinetic_deceleration_steps);
        const double y_step_distance(y_speed / m_kinetic_deceleration_steps);

        // Calculate the new kinetic speed.
        m_kinetic_speed = PointPx(x_speed - x_step_distance, y_speed - y_step_distance);

        // Reduce the deceleration steps required for the next calculation.
        m_kinetic_deceleration_steps--;
    }
    else
    {
        // Set kinectic speed to not moving instead.
        m_kinetic_speed = PointPx(0.0, 0.0);
    }
}
