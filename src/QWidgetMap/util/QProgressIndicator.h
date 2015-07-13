/**
 * @copyright 2015 Chris Stylianou
 * @copyright 2009 - 2010 Morgan Leborgne
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
#include <QtGui/QColor>
#include <QtWidgets/QWidget>

// STL includes.
#include <chrono>

// Local includes.
#include "../qwidgetmap_global.h"

/// QWidgetMap namespace.
namespace qwm
{

    /// Utilities namespace.
    namespace util
    {

        /**
         * Displays a progress indicator to show that a lengthy task is under way.
         */
        class QWIDGETMAP_EXPORT QProgressIndicator : public QWidget
        {
            Q_OBJECT
            Q_PROPERTY(std::chrono::milliseconds animationDelay READ animationDelay WRITE setAnimationDelay)
            Q_PROPERTY(bool alwaysVisible READ alwaysVisible WRITE setAlwaysVisible)
            Q_PROPERTY(QColor color READ color WRITE setColor)

        public:

            /**
             * This constructs a QProgressIndicator.
             * @param parent QWidget parent ownership.
             */
            explicit QProgressIndicator(QWidget* parent = nullptr);

            /// Disable copy constructor.
            QProgressIndicator(const QProgressIndicator&) = delete;

            /// Disable copy assignment.
            QProgressIndicator& operator=(const QProgressIndicator&) = delete;

            /// Destructor.
            ~QProgressIndicator() = default;

        public:

            /**
             * Returns the delay between animation steps.
             * @return the number of milliseconds between animation steps.
             */
            std::chrono::milliseconds animationDelay() const;

            /**
             * Returns a boolean value indicating whether the component is currently animated.
             * @return the animation state.
             */
            bool isAnimated() const;

            /**
             * Returns a boolean value indicating whether the receiver shows itself even when it is not animating.
             * @return whether the receiver shows itself even when it is not animating.
             */
            bool alwaysVisible() const;

            /**
             * Returns the color of the component.
             * @return the color of the component.
             */
            const QColor& color() const;

            /**
             * Returns the recommended size for the widget.
             * @return the recommended size.
             */
            QSize sizeHint() const final;

            /**
             * Returns the preferred height for this widget, given the width.
             * @param width The given width.
             * @return the preferred height.
             */
            int heightForWidth(int width) const final;

        public slots:

            /**
             * Starts the spin animation.
             */
            void startAnimation();

            /**
             * Stops the spin animation.
             */
            void stopAnimation();

            /**
             * Sets the delay between animation steps.
             * @param delay The delay in milliseconds.
             */
            void setAnimationDelay(const std::chrono::milliseconds& delay);

            /**
             * Sets whether the component hides itself when it is not animating.
             * @param state The display state.
             */
            void setAlwaysVisible(const bool& state);

            /**
             * Sets the color of the components to the given color.
             * @param color The color to set.
             */
            void setColor(const QColor& color);

        protected:

            /**
             * Updates the progress indicator rotation angle and then schedules a repaint.
             * @param event The QTimerEvent.
             */
            void timerEvent(QTimerEvent* event) final;

            /**
             * Paints the current scene.
             * @param event The QPaintEvent.
             */
            void paintEvent(QPaintEvent* event) final;

        private:

            /// The current rotation angle of the progress indicator.
            int m_angle { 0 };

            /// The current timer identifier.
            int m_timer_identifier { -1 };

            /// The delay between animations.
            std::chrono::milliseconds m_animation_delay { std::chrono::milliseconds(40) };

            /// Whether the progress indicator is always visible.
            bool m_always_visible { false };

            /// The brush color of the progress capsule indicators.
            QColor m_color { Qt::black };

        };

    }

}
