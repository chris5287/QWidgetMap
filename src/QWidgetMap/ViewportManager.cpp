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

#include "ViewportManager.h"

// Qt includes.
#include <QtCore/QDebug>
#include <QtCore/QTimer>

// Local includes.
#include "util/Algorithms.h"

using namespace qwm;

ViewportManager::ViewportManager(const QSizeF& size_px, const projection::EPSG& projection_epsg, const QSize& tile_size_px, QObject* parent)
    : QObject(parent),
      Viewport(size_px, projection_epsg, tile_size_px)
{
    // Nothing else to do.
}

void ViewportManager::setSizePx(const QSizeF& size_px)
{
    // Set the viewport size.
    Viewport::setSizePx(size_px);

    // Emit that the viewport has changed.
    emit viewportChanged();
}

void ViewportManager::setProjection(const projection::EPSG& projection_epsg)
{
    // Set the viewport projection.
    Viewport::setProjection(projection_epsg);

    // Emit that the viewport has changed.
    emit viewportChanged();
}

void ViewportManager::setFocusPointWorldCoord(const util::PointWorldCoord& world_coord)
{
    // Set the viewport world focus point.
    Viewport::setFocusPointWorldCoord(world_coord);

    // Emit that the viewport has changed.
    emit viewportChanged();
}

void ViewportManager::setFocusPointWorldMean(const std::vector<util::PointWorldCoord>& world_coords, const bool& auto_zoom)
{
    // Calculate the mean of the points and set it as the focus point.
    setFocusPointWorldCoord(util::algorithms::mean(world_coords));

    // Has auto zoom been requested?
    if(auto_zoom)
    {
        // Auto zoom go-go-go!

        // While the view cannot fit all the coordinates in and we have not reached the minimum zoom level.
        while(viewportContains(world_coords) == false && Viewport::zoom() > m_zoom_minimum)
        {
            // Zoom out to the next level.
            zoomOut();
        }

        // While the view fits all the coordinates in and we have not reached the maximum zoom level.
        while(viewportContains(world_coords) == true && Viewport::zoom() < m_zoom_maximum)
        {
            // Zoom in to the next level.
            zoomIn();
        }

        // Finally, check we haven't accidentally zoomed in too far, meaning the coordinates do not actually fit the display.
        if(viewportContains(world_coords) == false && Viewport::zoom() > m_zoom_minimum)
        {
            // Zoom out to the next level.
            zoomOut();
        }
    }
}

void ViewportManager::setFocusPointWorldAnimated(const util::PointWorldCoord& world_coord, const int& steps, const std::chrono::milliseconds& step_interval)
{
    // Is animation already taking place?
    if(m_animated_mutex.try_lock())
    {
        // Set the new focus point to scroll to.
        m_animated_focus_world_coord = world_coord;

        // Set the number of steps it should take.
        m_animated_steps = steps;

        // Set the interval between each step.
        m_animated_interval = step_interval;

        // Set the animation tick to start.
        QTimer::singleShot(m_animated_interval.count(), this, SLOT(animatedTick()));
    }
    else
    {
        // Warn that we cannot animate as we are already mid-animating.
        qDebug() << "Unable to animate to new focus point, already animating!";
    }
}

void ViewportManager::setScrollRestriction(const util::RectWorldCoord& rect_world_coord)
{
    // Set the scroll restriction.
    m_scroll_restriction = rect_world_coord;

    // Reset the focus point.
    setFocusPointWorldCoord(util::PointWorldCoord(0.0, 0.0));
}

void ViewportManager::scroll(const util::PointPx& delta_px)
{
    // Calculate the new focus point with the delta change.
    const util::PointWorldCoord new_focus_world_coord(projection::toPointWorldCoord(*this, focusPointWorldPx() + delta_px));
    const util::PointWorldPx new_focus_world_px(projection::toPointWorldPx(*this, new_focus_world_coord));

    // Calculate the rect world coordinates that the new focus point will provide.
    const util::RectWorldCoord new_rect_world_coord
    {
        projection::toPointWorldCoord(*this, toPointWorldPx(util::PointViewportPx(0.0, 0.0), new_focus_world_px)),
        projection::toPointWorldCoord(*this, toPointWorldPx(util::PointViewportPx(sizePx().width(), sizePx().height()), new_focus_world_px))
    };

    // If no scroll restriction is set, or if the new focus point rect intersects with the scroll restriction...
    if(m_scroll_restriction.isNull() || m_scroll_restriction.intersects(new_rect_world_coord))
    {
        // Update focus point with the delta change.
        setFocusPointWorldCoord(new_focus_world_coord);
    }
}

void ViewportManager::scrollLeft(const double& delta_px)
{
    // Scroll the focus point left by required pixels.
    scroll(util::PointPx(-delta_px, 0.0));
}

void ViewportManager::scrollRight(const double& delta_px)
{
    // Scroll the focus point right by required pixels.
    scroll(util::PointPx(delta_px, 0.0));
}

void ViewportManager::scrollUp(const double& delta_px)
{
    // Scroll the focus point up by required pixels.
    scroll(util::PointPx(0.0, -delta_px));
}

void ViewportManager::scrollDown(const double& delta_px)
{
    // Scroll the focus point down by required pixels.
    scroll(util::PointPx(0.0, delta_px));
}

int ViewportManager::zoomMinimum() const
{
    // Returns the zoom minimum.
    return m_zoom_minimum;
}

void ViewportManager::setZoomMinimum(const int& zoom)
{
    // Set the new zoom minimum.
    m_zoom_minimum = zoom;

    // Check the zoom.
    validateZoom();
}

int ViewportManager::zoomMaximum() const
{
    // Returns the zoom maximum.
    return m_zoom_maximum;
}

void ViewportManager::setZoomMaximum(const int& zoom)
{
    // Set the new zoom maximum.
    m_zoom_maximum = zoom;

    // Check the zoom.
    validateZoom();
}

void ViewportManager::zoomIn()
{
    // Check the current zoom is less than the maximum zoom
    if(Viewport::zoom() < m_zoom_maximum)
    {
        // Increase the zoom!
        Viewport::setZoom(Viewport::zoom() + 1);

        // Emit that the viewport has changed.
        emit viewportChanged();
    }
}

void ViewportManager::zoomOut()
{
    // Check the current zoom is greater than the minimum zoom.
    if(Viewport::zoom() > m_zoom_minimum)
    {
        // Decrease the zoom!
        Viewport::setZoom(Viewport::zoom() - 1);

        // Emit that the viewport has changed.
        emit viewportChanged();
    }
}

void ViewportManager::setZoom(int zoom)
{
    // Check the requested zoom is with the zoom range allowed.
    if(zoom < m_zoom_minimum)
    {
        // Default to minimum zoon instead.
        zoom = m_zoom_minimum;
    }
    else if(zoom > m_zoom_maximum)
    {
        // Default to the maximum zoom instead.
        zoom = m_zoom_maximum;
    }

    // Check the requested zoom isn't already the current zoom.
    if(Viewport::zoom() != zoom)
    {
        // Is the required zoom less than current zoom?
        if(Viewport::zoom() > zoom)
        {
            // Keep zooming out until zoom == current zoom.
            for(int i = Viewport::zoom(); i > zoom; i--)
            {
                zoomOut();
            }
        }
        else
        {
            // Keep zooming in until zoom == current zoom.
            for(int i = Viewport::zoom(); i < zoom; i++)
            {
                zoomIn();
            }
        }
    }
}

bool ViewportManager::viewportContains(const std::vector<util::PointWorldCoord>& world_coords) const
{
    // Default return value.
    bool return_contains_all(true);

    // Get the current viewport rect.
    const util::RectWorldCoord viewport_rect_coord(
                projection::toPointWorldCoord(*this, focusPointWorldPx() - sizePointCenterPx()),
                projection::toPointWorldCoord(*this, focusPointWorldPx() + sizePointCenterPx()));

    // Loop through each coordinate and check it is contained by the viewport rect, stop if we find one outside the rect.
    for(size_t i = 0; return_contains_all == true && i < world_coords.size(); i++)
    {
        // Is the point within the rect.
        return_contains_all = viewport_rect_coord.contains(world_coords.at(i));
    }

    // Return the result.
    return return_contains_all;
}

void ViewportManager::validateZoom()
{
    // Quick check to ensure min <= max.
    if(m_zoom_maximum > m_zoom_minimum)
    {
        // Swap the zooms.
        std::swap(m_zoom_minimum, m_zoom_maximum);
    }

    // Check the current zoom is not outside the zoom range.
    if(Viewport::zoom() < m_zoom_minimum)
    {
        // Current zoom is less than the minimum, set to the minimum.
        setZoom(m_zoom_minimum);
    }
    else if(Viewport::zoom() > m_zoom_maximum)
    {
        // Current zoom is greater than the maximum, set to the maximum.
        setZoom(m_zoom_maximum);
    }
}

void ViewportManager::animatedTick()
{
    // Do we still have steps to complete?
    if(m_animated_steps > 0)
    {
        // Calculate the delta between the current focus point and the target animated focus point.
        const util::PointWorldPx start_px(focusPointWorldPx());
        const util::PointWorldPx dest_px(projection::toPointWorldPx(*this, m_animated_focus_world_coord));
        const util::PointPx delta_px(dest_px - start_px);

        // Scroll to the next point in the step.
        scroll(delta_px / m_animated_steps);

        // Reduce the number of steps left.
        m_animated_steps--;

        // Schedule the next animation tick.
        QTimer::singleShot(m_animated_interval.count(), this, SLOT(animatedTick()));
    }
    else
    {
        // We have finished our animation, unlock the mutex.
        m_animated_mutex.unlock();
    }
}
