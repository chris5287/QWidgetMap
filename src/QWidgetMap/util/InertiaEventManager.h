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

#pragma once

// Qt includes.
#include <QtCore/QBasicTimer>
#include <QtGui/QMouseEvent>

// STL includes.
#include <memory>

// Local includes.
#include "../qwidgetmap_global.h"
#include "../ViewportManager.h"

/// QWidgetMap namespace.
namespace qwm
{

    /// Utilities namespace.
    namespace util
    {

        /// Kinetic state types.
        enum class KineticState
        {
            /// Steady, no movement.
            Steady,

            /// Mouse pressed.
            Pressed,

            /// Manually scroll based on mouse move actions.
            ManualScroll,

            /// Automatically scroll based on speed of mouse during ManualScroll state (deceleration applied).
            AutoScroll
        };

        /**
         * Manages the mouse panning events to simulate inertia.
         */
        class QWIDGETMAP_EXPORT InertiaEventManager : public QObject
        {
            Q_OBJECT

        public:

            /**
             * This constructs a Inertia Event Manager.
             * @param viewport_manager The viewport manager to use.
             * @param parent QObject parent ownership.
             */
            InertiaEventManager(const std::shared_ptr<ViewportManager>& viewport_manager, QObject* parent = nullptr);

            /// Disable copy constructor.
            InertiaEventManager(const InertiaEventManager&) = delete;

            /// Disable copy assignment.
            InertiaEventManager& operator=(const InertiaEventManager&) = delete;

            /// Destructor.
            ~InertiaEventManager() = default;

        public:

            /**
             * Processes the mouse event for inertia actions.
             * @param mouse_event The mouse event.
             */
            void mouseEvent(QMouseEvent* mouse_event);

        protected:

            /**
             * During ManualScroll state, updated the current mouse speed.
             * During AutoScroll state, performs the automatic scrolling.
             * @param event The QTimerEvent.
             */
            void timerEvent(QTimerEvent* event) final;

        private:

            /**
             * Decelerates the current speed, ready for the next automatic scroll event.
             */
            void decelerateSpeed();

        private:

            /// Viewport manager to use.
            std::shared_ptr<ViewportManager> m_viewport_manager;

            /// Current mouse press position.
            util::PointViewportPx m_mouse_position_pressed_px { 0.0, 0.0 };

            /// Current mouse dragged position.
            util::PointViewportPx m_mouse_position_dragged_px { 0.0, 0.0 };

            /// Current kinetic state.
            KineticState m_kinetic_state { KineticState::Steady };

            /// Current kinetic mouse speed (captured during ManualScroll state).
            util::PointPx m_kinetic_speed { 0.0, 0.0 };

            /// Current deceleration steps to apply when calculating speed deceleration.
            int m_kinetic_deceleration_steps { 0 };

            /// Timer for actioning kinetic automatic scrolling.
            QBasicTimer m_kinetic_ticker;

        };

    }

}
