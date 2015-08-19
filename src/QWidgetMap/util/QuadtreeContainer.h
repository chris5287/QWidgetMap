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
#include <QtCore/QDebug>

// STD includes.
#include <cstddef>
#include <memory>
#include <set>
#include <utility>
#include <vector>

// Local includes.
#include "../qwidgetmap_global.h"
#include "../util/Point.h"

/// QWidgetMap namespace.
namespace qwm
{

    /// Utilities namespace.
    namespace util
    {

        /// Quadtree container (based on http://en.wikipedia.org/wiki/Quadtree).
        template <class T>
        class QWIDGETMAP_EXPORT QuadtreeContainer
        {

        public:

            /**
             * Quadtree Container constructor.
             * @param capacity The number of items this quadtree container can store before it's children are created/used.
             * @param boundary_coord The bounding box area that this quadtree container covers in coordinates.
             */
            QuadtreeContainer(const std::size_t& capacity, const RectWorldCoord& boundary_coord)
                : m_capacity(capacity),
                  m_boundary_coord(boundary_coord)
            {
                // Reserve the container size.
                m_points.reserve(capacity);
            }

            /// Disable copy constructor.
            QuadtreeContainer(const QuadtreeContainer&) = delete;

            /// Disable copy assignment.
            QuadtreeContainer& operator=(const QuadtreeContainer&) = delete;

            /// Destructor.
            ~QuadtreeContainer() = default;

        public:

            /**
             * Fetches objects within the specified bounding box range.
             * @param return_points The objects that are within the specified range are added to this.
             * @param range_coord The bounding box range.
             */
            void query(std::set<T>& return_points, const RectWorldCoord& range_coord) const
            {
                // Does the range intersect with our boundary.
                if(range_coord.intersects(m_boundary_coord))
                {
                    // Check whether any of our points are contained in the range.
                    for(const auto& point : m_points)
                    {
                        // Is the point contained by the query range.
                        if(range_coord.contains(point.first))
                        {
                            // Add to the return points.
                            return_points.insert(point.second);
                        }
                    }

                    // Do we have any child quadtree nodes?
                    if(m_child_north_east != nullptr)
                    {
                        // Search each child and add the points they return.
                        m_child_north_east->query(return_points, range_coord);
                        m_child_north_west->query(return_points, range_coord);
                        m_child_south_east->query(return_points, range_coord);
                        m_child_south_west->query(return_points, range_coord);
                    }
                }
            }

            /**
             * Inserts an object into the quadtree container.
             * @param point_coord The objects's point in coordinates.
             * @param object The object to insert.
             * @return whether the object was inserted into this quadtree container.
             */
            bool insert(const PointWorldCoord& point_coord, const T& object)
            {
                // Keep track of our success.
                bool success(false);

                // Does this boundary contain the point?
                if(m_boundary_coord.contains(point_coord))
                {
                    // Have we reached our capacity?
                    if(m_points.size() < m_capacity)
                    {
                        // Add the point.
                        m_points.emplace_back(point_coord, object);

                        // Update our success.
                        success = true;
                    }
                    else
                    {
                        // Do we already have child quadtree nodes?
                        if(m_child_north_east == nullptr)
                        {
                            // We need to create the child quadtree nodes before we continue.
                            subdivide();
                        }

                        // Try inserting into north east.
                        if(m_child_north_east->insert(point_coord, object))
                        {
                            // Update our success.
                            success = true;
                        }
                        // Try inserting into north west.
                        else if(m_child_north_west->insert(point_coord, object))
                        {
                            // Update our success.
                            success = true;
                        }
                        // Try inserting into south east.
                        else if(m_child_south_east->insert(point_coord, object))
                        {
                            // Update our success.
                            success = true;
                        }
                        // Try inserting into south west.
                        else if(m_child_south_west->insert(point_coord, object))
                        {
                            // Update our success.
                            success = true;
                        }
                        else
                        {
                            // Warn that we are unable to insert point into quadtree container.
                            qDebug() << "Unable to insert point into quadtree container.";
                        }
                    }
                }

                // Return our success.
                return success;
            }

            /**
             * Removes an object from the quadtree container.
             * @param point_coord The objects's point in coordinates.
             * @param object The object to remove.
             */
            void erase(const PointWorldCoord& point_coord, const T& object)
            {
                // Does this boundary contain the point?
                if(m_boundary_coord.contains(point_coord))
                {
                    // Check whether any of our points are contained in the range.
                    auto itr_point(m_points.begin());
                    while(itr_point != m_points.end())
                    {
                        // Have we found the object?
                        if(itr_point->second == object)
                        {
                            // Remove the object from the container.
                            itr_point = m_points.erase(itr_point);
                        }
                        else
                        {
                            // Move on to the next point.
                            ++itr_point;
                        }
                    }

                    // Do we have child quadtree nodes?
                    if(m_child_north_east != nullptr)
                    {
                        // Search each child and remove the object if found.
                        m_child_north_east->erase(point_coord, object);
                        m_child_north_west->erase(point_coord, object);
                        m_child_south_east->erase(point_coord, object);
                        m_child_south_west->erase(point_coord, object);
                    }
                }
            }

            /**
             * Removes all objects from the quadtree container.
             */
            void clear()
            {
                // Clear the points.
                m_points.clear();

                // Reset the child nodes.
                m_child_north_east.reset(nullptr);
                m_child_north_west.reset(nullptr);
                m_child_south_east.reset(nullptr);
                m_child_south_west.reset(nullptr);
            }

        private:

            /**
             * Creates the child nodes.
             */
            void subdivide()
            {
                // Calculate half the size of the boundary.
                const QSizeF half_size(m_boundary_coord.size() / 2.0);

                // Construct the north east child.
                const RectWorldCoord north_east(PointWorldCoord(m_boundary_coord.left() + half_size.width(), m_boundary_coord.top()), half_size);
                m_child_north_east.reset(new QuadtreeContainer<T>(m_capacity, north_east));

                // Construct the north west child.
                const RectWorldCoord north_west(PointWorldCoord(m_boundary_coord.left(), m_boundary_coord.top()), half_size);
                m_child_north_west.reset(new QuadtreeContainer<T>(m_capacity, north_west));

                // Construct the south east child.
                const RectWorldCoord south_east(PointWorldCoord(m_boundary_coord.left() + half_size.width(), m_boundary_coord.top() + half_size.height()), half_size);
                m_child_south_east.reset(new QuadtreeContainer<T>(m_capacity, south_east));

                // Construct the south west child.
                const RectWorldCoord south_west(PointWorldCoord(m_boundary_coord.left(), m_boundary_coord.top() + half_size.height()), half_size);
                m_child_south_west.reset(new QuadtreeContainer<T>(m_capacity, south_west));
            }

        private:

            /// Quadtree node capacity.
            const std::size_t m_capacity;

            /// Boundary of this quadtree node.
            const RectWorldCoord m_boundary_coord;

            /// Points in this quadtree node.
            std::vector<std::pair<PointWorldCoord, T>> m_points;

            /// Child: north east quadtree node.
            std::unique_ptr<QuadtreeContainer> m_child_north_east;

            /// Child: north west quadtree node.
            std::unique_ptr<QuadtreeContainer> m_child_north_west;

            /// Child: south east quadtree node.
            std::unique_ptr<QuadtreeContainer> m_child_south_east;

            /// Child: south west quadtree node.
            std::unique_ptr<QuadtreeContainer> m_child_south_west;

        };

    }

}
