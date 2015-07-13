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

#include "ESRIShapefile.h"

// Qt includes.
#include <QtCore/QDebug>
#include <QtCore/QFile>

// GDAL includes.
#include <gdal/ogrsf_frmts.h>

// Local includes.
#include "../../projection/Projection.h"

using namespace qwm;
using namespace qwm::draw::other;

namespace
{
    /**
     * Validates layer names for common mistakes.
     * @param layer_names The layer names to validate.
     * @return the layer names that are valid.
     */
    std::vector<std::string> validLayerNames(const std::vector<std::string>& layer_names)
    {
        // Default return layer names.
        std::vector<std::string> return_layer_names;

        // For each layer name.
        for(const auto& layer_name : layer_names)
        {
            // Check the layer name is not empty.
            if(layer_name.empty() == false)
            {
                // Add the valid layer name.
                return_layer_names.push_back(layer_name);
            }
        }

        // Return the valid layer names.
        return return_layer_names;
    }
}

ESRIShapefile::ESRIShapefile(const std::string& file_path, const std::vector<std::string>& layer_names, QObject* parent)
    : Drawable(DrawableType::ESRIShapefile, parent),
      m_ogr_layer_names(validLayerNames(layer_names))
{
    // Register OGR drivers.
    OGRRegisterAll();

    // Check the file exists.
    if(QFile(file_path.c_str()).exists())
    {
        // Open the file.
        m_ogr_data_set = OGRSFDriverRegistrar::Open(file_path.c_str(), FALSE);
    }
    else
    {
        // Warn that the file does not exist.
        qDebug() << "ESRI Shapefile does not exist: " << file_path.c_str();
    }
}

ESRIShapefile::ESRIShapefile(const std::string& file_path, const std::string& layer_name, QObject* parent)
    : ESRIShapefile(file_path, validLayerNames(std::vector<std::string>{layer_name}), parent)
{

}

ESRIShapefile::~ESRIShapefile()
{
    // Do we have a dataset open?
    if(m_ogr_data_set != nullptr)
    {
        // Close the data set.
        OGRDataSource::DestroyDataSource(m_ogr_data_set);
    }
}

const QPen& ESRIShapefile::penPolygon() const
{
    // Do we have a pen?
    if(m_pen_polygon == nullptr)
    {
        // Create a default pen.
        m_pen_polygon = std::make_shared<QPen>();
    }

    // Get the pen to draw with.
    return *(m_pen_polygon.get());
}

void ESRIShapefile::setPenPolygon(const std::shared_ptr<QPen>& pen)
{
    // Set the pen to draw with.
    m_pen_polygon = pen;

    // Emit that we need to redraw to display this change.
    emit requestRedraw();
}

void ESRIShapefile::setPenPolygon(const QPen& pen)
{
    // Set the pen to draw with.
    m_pen_polygon = std::make_shared<QPen>(pen);

    // Emit that we need to redraw to display this change.
    emit requestRedraw();
}

const QBrush& ESRIShapefile::brushPolygon() const
{
    // Do we have a brush?
    if(m_brush_polygon == nullptr)
    {
        // Create a default brush.
        m_brush_polygon = std::make_shared<QBrush>();
    }

    // Get the brush to draw with.
    return *(m_brush_polygon.get());
}

void ESRIShapefile::setBrushPolygon(const std::shared_ptr<QBrush>& brush)
{
    // Set the brush to draw with.
    m_brush_polygon = brush;

    // Emit that we need to redraw to display this change.
    emit requestRedraw();
}

void ESRIShapefile::setBrushPolygon(const QBrush& brush)
{
    // Set the brush to draw with.
    m_brush_polygon = std::make_shared<QBrush>(brush);

    // Emit that we need to redraw to display this change.
    emit requestRedraw();
}

const QPen& ESRIShapefile::penLineString() const
{
    // Do we have a pen?
    if(m_pen_linestring == nullptr)
    {
        // Create a default pen.
        m_pen_linestring = std::make_shared<QPen>();
    }

    // Get the pen to draw with.
    return *(m_pen_linestring.get());
}

void ESRIShapefile::setPenLineString(const std::shared_ptr<QPen>& pen)
{
    // Set the pen to draw with.
    m_pen_linestring = pen;

    // Emit that we need to redraw to display this change.
    emit requestRedraw();
}

void ESRIShapefile::setPenLineString(const QPen& pen)
{
    // Set the pen to draw with.
    m_pen_linestring = std::make_shared<QPen>(pen);

    // Emit that we need to redraw to display this change.
    emit requestRedraw();
}

void ESRIShapefile::draw(QPainter& painter, const util::RectWorldCoord& drawing_rect_world_coord, const Viewport& viewport) const
{
    // Do we have a data set open?
    if(m_ogr_data_set != nullptr)
    {
        // Do we have a list of layer names to use?
        if(m_ogr_layer_names.empty() == false)
        {
            // For each layer name.
            for(const auto& ogr_layer_name : m_ogr_layer_names)
            {
                // Get layer.
                const auto ogr_layer(m_ogr_data_set->GetLayerByName(ogr_layer_name.c_str()));
                if(ogr_layer == nullptr)
                {
                    // Invalid layer name!
                }
                else
                {
                    // Reset reading.
                    ogr_layer->ResetReading();

                    // Set the Spatial Filter.
                    ogr_layer->SetSpatialFilterRect(drawing_rect_world_coord.left(), drawing_rect_world_coord.top(), drawing_rect_world_coord.right(), drawing_rect_world_coord.bottom());

                    // Loop through features.
                    OGRFeature* ogr_feature;
                    while((ogr_feature = ogr_layer->GetNextFeature()) != nullptr)
                    {
                        // Draw the feature.
                        drawFeature(ogr_feature, painter, viewport);

                        // Destroy the feature.
                        OGRFeature::DestroyFeature(ogr_feature);
                    }
                }
            }
        }
        else
        {
            // Loop through and draw each layer.
            for(int i = 0; i < m_ogr_data_set->GetLayerCount(); ++i)
            {
                // Get layer.
                const auto ogr_layer(m_ogr_data_set->GetLayer(i));
                if(ogr_layer != nullptr)
                {
                    // Reset reading.
                    ogr_layer->ResetReading();

                    // Set the Spatial Filter.
                    ogr_layer->SetSpatialFilterRect(drawing_rect_world_coord.left(), drawing_rect_world_coord.top(), drawing_rect_world_coord.right(), drawing_rect_world_coord.bottom());

                    // Loop through features.
                    OGRFeature* ogr_feature;
                    while((ogr_feature = ogr_layer->GetNextFeature()) != nullptr)
                    {
                        // Draw the feature.
                        drawFeature(ogr_feature, painter, viewport);

                        // Destroy the feature.
                        OGRFeature::DestroyFeature(ogr_feature);
                    }
                }
            }
        }
    }
}

void ESRIShapefile::drawFeature(OGRFeature* ogr_feature, QPainter& painter, const Viewport& viewport) const
{
    // Fetch geometries.
    const auto ogr_geometry(ogr_feature->GetGeometryRef());
    if(ogr_geometry == nullptr)
    {
        // No geometry to fetch!
    }
    // Is it a polygon.
    else if(wkbFlatten(ogr_geometry->getGeometryType()) == wkbPolygon)
    {
        // Cast to a polygon.
        const auto ogr_polygon(static_cast<OGRPolygon*>(ogr_geometry));

        // Fetch the exterior ring.
        const auto ogr_exterior_ring(ogr_polygon->getExteriorRing());
        if(ogr_exterior_ring == nullptr)
        {
            // No exterior ring!
        }
        else
        {
            // Prepare storage for point.
            OGRPoint ogr_point;

            // Create a polygon of the points.
            QPolygonF polygon_px;

            // Loop through the points.
            for(int i = 0; i < ogr_exterior_ring->getNumPoints(); ++i)
            {
                // Fetch the point.
                ogr_exterior_ring->getPoint(i, &ogr_point);

                // Add the point to be drawn.
                polygon_px.append(projection::toPointWorldPx(viewport, util::PointWorldCoord(ogr_point.getX(), ogr_point.getY())));
            }

            // Set the pen to use.
            painter.setPen(penPolygon());

            // Set the brush to use.
            painter.setBrush(brushPolygon());

            // Draw the polygon line.
            painter.drawPolygon(polygon_px);
        }
    }
    else if(wkbFlatten(ogr_geometry->getGeometryType()) == wkbMultiPolygon)
    {
        // Cast to a multi polygon.
        const auto ogr_multi_polygon(static_cast<OGRMultiPolygon*>(ogr_geometry));
        if(ogr_multi_polygon == nullptr)
        {
            // No multi polygons!
        }
        else
        {
            // Loop through each polygon.
            for(int i = 0; i < ogr_multi_polygon->getNumGeometries(); ++i)
            {
                // Cast to a polygon.
                const auto ogr_polygon(static_cast<OGRPolygon*>(ogr_multi_polygon->getGeometryRef(i)));

                // Fetch the exterior ring.
                const auto ogr_exterior_ring(ogr_polygon->getExteriorRing());
                if(ogr_exterior_ring == nullptr)
                {
                    // No exterior ring!
                }
                else
                {
                    // Prepare storage for point.
                    OGRPoint ogr_point;

                    // Create a polygon of the points.
                    QPolygonF polygon_px;

                    // Loop through the points.
                    for(int i = 0; i < ogr_exterior_ring->getNumPoints(); ++i)
                    {
                        // Fetch the point.
                        ogr_exterior_ring->getPoint(i, &ogr_point);

                        // Add the point to be drawn.
                        polygon_px.append(projection::toPointWorldPx(viewport, util::PointWorldCoord(ogr_point.getX(), ogr_point.getY())));
                    }

                    // Set the pen to use.
                    painter.setPen(penPolygon());

                    // Set the brush to use.
                    painter.setBrush(brushPolygon());

                    // Draw the polygon line.
                    painter.drawPolygon(polygon_px);
                }
            }
        }
    }
    else if(wkbFlatten(ogr_geometry->getGeometryType()) == wkbLineString) // wkbLineString
    {
        // Cast to a line string.
        const auto ogr_line_string(static_cast<OGRLineString*>(ogr_geometry));

        // Prepare storage for point.
        OGRPoint ogr_point;

        // Create a polygon of the points.
        QPolygonF polygon_line_px;

        // Loop through the points.
        for(int i = 0; i < ogr_line_string->getNumPoints(); ++i)
        {
            // Fetch the point.
            ogr_line_string->getPoint(i, &ogr_point);

            // Add the point to be drawn.
            polygon_line_px.append(projection::toPointWorldPx(viewport, util::PointWorldCoord(ogr_point.getX(), ogr_point.getY())));
        }

        // Set the pen to use.
        painter.setPen(penLineString());

        // Draw the polygon line.
        painter.drawPolyline(polygon_line_px);
    }
}
