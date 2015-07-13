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

#include "Layer.h"

// Local includes.
#include "draw/geometry/GeometryPoint.h"
#include "draw/geometry/GeometryPointShape.h"

using namespace qwm;

Layer::Layer(const std::string& name, QObject* parent)
    : QObject(parent),
      m_name(name)
{

}

const std::string& Layer::name() const
{
    // Return the layer's name.
    return m_name;
}

QVariant Layer::metadata(const std::string& key) const
{
    // Default return value.
    QVariant return_value;

    // Find the key.
    const auto itr_find(m_metadata.find(key));
    if(itr_find != m_metadata.end())
    {
        // Fetch the value.
        return_value = itr_find->second;
    }

    // Return the value.
    return return_value;
}

void Layer::setMetadata(const std::string& key, const QVariant& value)
{
    // Set the metadata key/value.
    m_metadata[key] = value;
}

bool Layer::visible() const
{
    // Return the visibility.
    return m_visible;
}

bool Layer::isVisible(const Viewport& viewport) const
{
    // Default visibility.
    bool visibility(m_visible);

    // Check whether the zoom level is within range.
    if(m_zoom_minimum > viewport.zoom() || m_zoom_maximum < viewport.zoom())
    {
        // Outside of supported zoom levels.
        visibility = false;
    }

    // Return the visibility.
    return visibility;
}

void Layer::setVisible(const bool& enabled)
{
    // Only update visibility if it has changed.
    if(m_visible != enabled)
    {
        // Set the visibility.
        m_visible = enabled;

        // Emit that we need to redraw to display this change.
        emit requestRedraw();
    }
}

void Layer::setZoomMinimum(const int& zoom_minimum)
{
    // Only update zoom minimum if it has changed.
    if(m_zoom_minimum != zoom_minimum)
    {
        // Set the zoom minimum.
        m_zoom_minimum = zoom_minimum;

        // Emit that we need to redraw to display this change.
        emit requestRedraw();
    }
}

void Layer::setZoomMaximum(const int& zoom_maximum)
{
    // Only update zoom maximum if it has changed.
    if(m_zoom_maximum != zoom_maximum)
    {
        // Set the zoom maximum.
        m_zoom_maximum = zoom_maximum;

        // Emit that we need to redraw to display this change.
        emit requestRedraw();
    }
}

bool Layer::isMouseEventsEnabled() const
{
    // Return the enabled value.
    return m_mouse_events_enabled;
}

void Layer::setMouseEventsEnabled(const bool& enable)
{
    // Set whether to enable mouse events.
    m_mouse_events_enabled = enable;
}

std::vector<std::shared_ptr<draw::Drawable>> Layer::drawableItems() const
{
    // Gain a read lock to protect the drawable items.
    QReadLocker locker(&m_drawable_items_mutex);

    // Return the drawable items.
    return m_drawable_items;
}

std::vector<std::shared_ptr<draw::geometry::Geometry>> Layer::drawableGeometries(const util::RectWorldCoord& range_coord) const
{
    // Gain a read lock to protect the geometries container.
    QReadLocker locker(&m_drawable_geometries_mutex);

    // Populate the geometries container with geometry points.
    std::set<std::shared_ptr<draw::geometry::Geometry>> geometry_points;
    m_drawable_geometries_points.query(geometry_points, range_coord);

    // The geometries container to return, populate with geometry points.
    std::vector<std::shared_ptr<draw::geometry::Geometry>> return_geometries(geometry_points.begin(), geometry_points.end());

    // Loop through the fixed geometries types (ellipse, line string, polygon).
    for(const auto& geometry : m_drawable_geometries_fixed)
    {
        // Does the query and geometry bounding box intersect?
        if(range_coord.intersects(geometry->boundingBoxFixed()))
        {
            // Add to the return geometries.
            return_geometries.push_back(geometry);
        }
    }

    // Return the list of geometries.
    return return_geometries;
}

bool Layer::addDrawable(const std::shared_ptr<draw::Drawable>& drawable, const bool& disable_redraw)
{
    // Keep track of our success.
    bool success(false);

    // Check that the drawable item is valid.
    if(drawable != nullptr)
    {
        // Is this a drawable item (non-geometry)?
        if(drawable->drawableType() != draw::DrawableType::Geometry)
        {
            // Gain a write lock to protect the drawable items.
            QWriteLocker locker(&m_drawable_items_mutex);

            // Add the drawable item.
            m_drawable_items.push_back(drawable);

            // Update our success.
            success = true;
        }
        else
        {
            // Handle the different drawable geometry types.
            const auto drawable_geometry(std::static_pointer_cast<draw::geometry::Geometry>(drawable));
            if(drawable_geometry->geometryType() == draw::geometry::GeometryType::GeometryPoint)
            {
                // Gain a write lock to protect the geometries container.
                QWriteLocker locker(&m_drawable_geometries_mutex);

                // Add the geometry point container.
                m_drawable_geometries_points.insert(std::static_pointer_cast<draw::geometry::GeometryPoint>(drawable_geometry)->coord(), drawable_geometry);

                // Update our success.
                success = true;
            }
            else
            {
                // Gain a write lock to protect the geometries container.
                QWriteLocker locker(&m_drawable_geometries_mutex);

                // Add the geometry.
                m_drawable_geometries_fixed.push_back(std::static_pointer_cast<draw::geometry::GeometryFixed>(drawable_geometry));

                // Update our success.
                success = true;
            }
        }
    }

    // Was we successful?
    if(success)
    {
        // Should we redraw?
        if(disable_redraw == false)
        {
            // Emit to redraw layer.
            emit requestRedraw();
        }

        // Connect signal/slot to pass on redraw reuqests.
        QObject::connect(drawable.get(), &draw::Drawable::requestRedraw, this, &Layer::requestRedraw);
    }

    // Return our success.
    return success;
}

bool Layer::removeDrawable(const std::shared_ptr<draw::Drawable>& drawable, const bool& disable_redraw)
{
    // Keep track of our success.
    bool success(false);

    // Check that the drawable item is valid.
    if(drawable != nullptr)
    {
        // Is this a drawable item (non-geometry)?
        if(drawable->drawableType() != draw::DrawableType::Geometry)
        {
            // Gain a write lock to protect the drawable items.
            QWriteLocker locker(&m_drawable_items_mutex);

            // Find the drawable item in the container.
            const auto itr_find(std::find(m_drawable_items.begin(), m_drawable_items.end(), drawable));
            if(itr_find != m_drawable_items.end())
            {
                // Remove the drawable item.
                m_drawable_items.erase(itr_find);

                // Update our success.
                success = true;
            }
        }
        else
        {
            // Handle the different geometry types.
            const auto drawable_geometry(std::static_pointer_cast<draw::geometry::Geometry>(drawable));
            if(drawable_geometry->geometryType() == draw::geometry::GeometryType::GeometryPoint)
            {
                // Gain a write lock to protect the geometries container.
                QWriteLocker locker(&m_drawable_geometries_mutex);

                // Remove the geometry from the list.
                m_drawable_geometries_points.erase(std::static_pointer_cast<draw::geometry::GeometryPoint>(drawable_geometry)->coord(), drawable_geometry);

                // Update our success.
                success = true;
            }
            else
            {
                // Gain a write lock to protect the geometries container.
                QWriteLocker locker(&m_drawable_geometries_mutex);

                // Find the object in the container.
                const auto itr_find(std::find(m_drawable_geometries_fixed.begin(), m_drawable_geometries_fixed.end(), std::static_pointer_cast<draw::geometry::GeometryFixed>(drawable_geometry)));
                if(itr_find != m_drawable_geometries_fixed.end())
                {
                    // Remove the geometry from the list.
                    m_drawable_geometries_fixed.erase(itr_find);

                    // Update our success.
                    success = true;
                }
            }
        }
    }

    // Was we successful?
    if(success)
    {
        // Disconnect any signals that were previously connected.
        QObject::disconnect(drawable.get(), 0, this, 0);

        // Should we redraw?
        if(disable_redraw == false)
        {
            // Emit to redraw layer.
            emit requestRedraw();
        }
    }

    // Return our success.
    return success;
}

void Layer::clearDrawables(const bool& disable_redraw)
{
    // Gain a write lock to protect the drawable items and geometries.
    QWriteLocker locker_items(&m_drawable_items_mutex);
    QWriteLocker locker_geometries(&m_drawable_geometries_mutex);

    // Remove all drawable items and geometries from the list.
    m_drawable_items.clear();
    m_drawable_geometries_points.clear();
    m_drawable_geometries_fixed.clear();

    // Should we redraw?
    if(disable_redraw == false)
    {
        // Emit to redraw layer.
        emit requestRedraw();
    }
}

void Layer::mousePressEvent(const QMouseEvent* mouse_event, const Viewport& viewport, const qreal& fuzzy_factor_px) const
{
    // Are mouse events enabled and is this layer visible?
    if(isMouseEventsEnabled() && isVisible(viewport))
    {
        // Is this a mouse press event?
        if(mouse_event->type() == QEvent::MouseButtonPress)
        {
            // Is this a left-click event?
            if(mouse_event->button() == Qt::LeftButton)
            {
                // Calculate the mouse press world point in coords.
                const util::PointWorldCoord mouse_point_coord(projection::toPointWorldCoord(viewport,  util::PointViewportPx(mouse_event->localPos().x(), mouse_event->localPos().y())));

                // Calcaulte the comparison touches geometry area to use.
                const draw::geometry::GeometryPointShape touches_area_coord(mouse_point_coord, QSizeF(fuzzy_factor_px, fuzzy_factor_px));

                // Check each drawable geometry to see it is contained in our touches geometry area.
                for(const auto& drawable_geometry : drawableGeometries(touches_area_coord.boundingBox(viewport)))
                {
                    // Does they touch?
                    if(drawable_geometry->touches(touches_area_coord, viewport))
                    {
                        // Emit that the geometry has been clicked.
                        drawable_geometry->drawableClicked();
                        drawableClicked(drawable_geometry);
                    }
                }
            }
        }
    }
}

void Layer::draw(QPainter& painter, const util::RectWorldCoord& drawing_rect_world_coord, const Viewport& viewport) const
{
    // Loop through each drawable item.
    for(const auto& drawable : drawableItems())
    {
        // Save the current painter's state.
        painter.save();

        // Check the drawable item is visible.
        if(drawable->isVisible(viewport))
        {
            // Draw the drawable item.
            drawable->draw(painter, drawing_rect_world_coord, viewport);
        }

        // Restore the painter's state.
        painter.restore();
    }

    // Save the current painter's state.
    painter.save();

    // Loop through each drawable geometry and draw it.
    for(const auto& drawable_geometry : drawableGeometries(drawing_rect_world_coord))
    {
        // Check the drawable geometry is visible.
        if(drawable_geometry->isVisible(viewport))
        {
            // Draw the drawable geometry and its meta-data displayed (if set).
            drawable_geometry->draw(painter, drawing_rect_world_coord, viewport);
            drawable_geometry->drawMetadataDisplayed(painter, viewport);
        }
    }

    // Restore the painter's state.
    painter.restore();
}
