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

#include "Example.h"

// Qt includes.
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QVBoxLayout>

// QWidgetMap includes.
#include <QWidgetMap/draw/geometry/GeometryEllipse.h>
#include <QWidgetMap/draw/geometry/GeometryLineString.h>
#include <QWidgetMap/draw/geometry/GeometryPointArrow.h>
#include <QWidgetMap/draw/geometry/GeometryPointCircle.h>
#include <QWidgetMap/draw/map/MapGoogle.h>
#include <QWidgetMap/draw/map/MapOSM.h>

// Only include if GDAL enabled.
#ifdef QWM_GDAL
    #include <QWidgetMap/draw/other/ESRIShapefile.h>
#endif

/**
 * This is an example application, which lets you play around with QWidgetMap's features.
 * There are the following buttons configured:
 *  - Add Point: adds a Point to the coordinate you click (this point will be clickable).
 *  - Drag Rect: lets to drag a rectangular into which will be zoomed in.
 *  - Move To Click: moves the view middle to the clicked coordinate.
 *
 * A "mini" map lets you see where you are. You can even click on it to change your position.
 */
Example::Example(QWidget* parent)
    : QWidget(parent)
{
    // Setup the maps.
    setupMaps();

    // Setup the layout
    setupLayout();
}

void Example::setupMaps()
{
    // Create a new QWidgetMap.
    m_map_control = std::unique_ptr<qwm::QWidgetMap>(new qwm::QWidgetMap(QSizeF(480.0, 640.0)));

    // Connect the signal/slots are required.
    // Signal/slot to manage drawables being clicked.
    QObject::connect(&(m_map_control->layer_manager()), &qwm::LayerManager::drawableClicked, this, &Example::drawableClickEvent);
    // Signal/slot to manage geometries being selected.
    QObject::connect(&(m_map_control->event_manager()), &qwm::EventManager::geometriesSelected, this, &Example::geometriesSelectedEvent);
    // Signal/slot to manage main map control mouse press events.
    QObject::connect(&(m_map_control->event_manager()), &qwm::EventManager::mouseEventPressCoordinate, this, &Example::main_mouseEventPressCoordinate);
    // Signal/slot to manage main map control mouse move/release events and key pressed events.
    QObject::connect(&(m_map_control->event_manager()), &qwm::EventManager::mouseEventMoveCoordinate, this, &Example::main_mouseKeyEventCoordinate);
    QObject::connect(&(m_map_control->event_manager()), &qwm::EventManager::mouseEventReleaseCoordinate, this, &Example::main_mouseKeyEventCoordinate);
    QObject::connect(&(m_map_control->event_manager()), &qwm::EventManager::keyEventPressed, this, &Example::main_mouseKeyEventCoordinate);

    // Set the mouse button functionality required for the QWidgetMap.
    m_map_control->event_manager().setMouseButtonLeft(qwm::EventManager::MouseButtonMode::Pan);
    m_map_control->event_manager().setMouseButtonRight(qwm::EventManager::MouseButtonMode::SelectBox);

    // Enable the zoom controls.
    m_map_control->enableZoomControls(true, false);

    // Set focus point on Barcelona, Spain at level 3.
    m_map_control->viewport_manager().setFocusPointWorldCoord(qwm::util::PointWorldCoord(2.176234, 41.373764));
    m_map_control->viewport_manager().setZoom(3);

    // Set the event manager's preview colours/line width.
    m_map_control->event_manager().setPreviewColours(QPen(Qt::red), QBrush(Qt::red), 0.6);
    m_map_control->event_manager().setPreviewLineWidth(2);

    // Create a base map adapter to use.
    auto map_adapter_google(std::make_shared<qwm::draw::map::MapGoogle>(qwm::draw::map::MapGoogle::GoogleLayerType::MAPS));
    auto map_adapter_osm(std::make_shared<qwm::draw::map::MapOSM>());

    // Create/add a layer with the selected map adapter.
    auto layer_map(std::make_shared<qwm::Layer>("Map Layer"));
    layer_map->addDrawable(map_adapter_google);
    m_map_control->layer_manager().add(layer_map);
    m_map_control->setBackgroundColour(Qt::white);

    // Create a layer to store geometries.
    std::shared_ptr<qwm::Layer> layer_geometries(std::make_shared<qwm::Layer>("Geometry Layer"));
    m_map_control->layer_manager().add(layer_geometries);


    // Create a "mini" QWidgetMap.
    m_mini_map_control = std::unique_ptr<qwm::QWidgetMap>(new qwm::QWidgetMap(QSizeF(150.0, 150.0)));

    // Signal/slot to manage "mini" map control mouse press events.
    QObject::connect(&(m_mini_map_control->event_manager()), &qwm::EventManager::mouseEventPressCoordinate, this, &Example::mini_mouseEventPressCoordinate);

    // Set the background colour.
    m_mini_map_control->setBackgroundColour(Qt::white);

    // Disable any zoom/mouse interaction.
    m_mini_map_control->enableZoomControls(false);
    m_mini_map_control->event_manager().setMouseButtonLeft(qwm::EventManager::MouseButtonMode::None);
    m_mini_map_control->event_manager().setMouseButtonRight(qwm::EventManager::MouseButtonMode::None);

    // Set focus point on the same as the main control at level 0.
    m_mini_map_control->viewport_manager().setFocusPointWorldCoord(m_map_control->viewport_manager().focusPointWorldCoord());
    m_mini_map_control->viewport_manager().setZoom(2);

    // Create/add a layer with the selected map adapter for the "mini" QWidgetMap.
    auto layer_mini_map(std::make_shared<qwm::Layer>("Mini Map Layer"));
    layer_mini_map->addDrawable(map_adapter_osm);
    m_mini_map_control->layer_manager().add(layer_mini_map);

    // Create a GeoemtryEllipse.
    auto ellipse(std::make_shared<qwm::draw::geometry::GeometryEllipse>(qwm::util::PointWorldCoord(-2.0, 52.0), 100000, 50000, 65));
    ellipse->setPen(QPen(Qt::red));
    layer_geometries->addDrawable(ellipse);

    // Create some GeometryPoints to add to a GeometryLineString.
    QPen pen(QColor(255, 0, 0, 100));
    pen.setWidth(2);
    std::vector<std::shared_ptr<qwm::draw::geometry::GeometryPoint>> points;
    points.emplace_back(std::make_shared<qwm::draw::geometry::GeometryPointCircle>(qwm::util::PointWorldCoord(8.259959, 50.001781)));
    points.back()->setPen(pen);
    points.back()->setMetadata("name", "Mainz, Hauptbahnhof");
    points.emplace_back(std::make_shared<qwm::draw::geometry::GeometryPointCircle>(qwm::util::PointWorldCoord(8.263758, 49.998917)));
    points.back()->setPen(pen);
    points.back()->setMetadata("name", "Mainz, Münsterplatz");
    points.emplace_back(std::make_shared<qwm::draw::geometry::GeometryPointCircle>(qwm::util::PointWorldCoord(8.265812, 50.001952)));
    points.back()->setPen(pen);
    points.back()->setMetadata("name", "Mainz, Neubrunnenplatz");
    points.emplace_back(std::make_shared<qwm::draw::geometry::GeometryPointCircle>(qwm::util::PointWorldCoord(8.2688, 50.004015)));
    points.back()->setPen(pen);
    points.back()->setMetadata("name", "Mainz, Bauhofstraße LRP");
    points.emplace_back(std::make_shared<qwm::draw::geometry::GeometryPointCircle>(qwm::util::PointWorldCoord(8.272845, 50.00495)));
    points.back()->setPen(pen);
    points.back()->setMetadata("name", "Mainz, Landtag");
    points.emplace_back(std::make_shared<qwm::draw::geometry::GeometryPointCircle>(qwm::util::PointWorldCoord(8.272845, 50.00495)));
    points.back()->setPen(pen);
    points.back()->setMetadata("name", "Mainz, Brückenplatz");
    points.emplace_back(std::make_shared<qwm::draw::geometry::GeometryPointCircle>(qwm::util::PointWorldCoord(8.280349, 50.008173)));
    points.back()->setPen(pen);
    points.back()->setMetadata("name", "Mainz, Brückenkopf");
    points.emplace_back(std::make_shared<qwm::draw::geometry::GeometryPointCircle>(qwm::util::PointWorldCoord(8.273573, 50.016315)));
    points.back()->setPen(pen);
    points.back()->setMetadata("name", "Wiesbaden-Mainz-Kastel, Eleonorenstraße");
    points.emplace_back(std::make_shared<qwm::draw::geometry::GeometryPointCircle>(qwm::util::PointWorldCoord(8.275145, 50.016992)));
    points.back()->setPen(pen);
    points.back()->setMetadata("name", "Wiesbaden-Mainz-Kastel, Johannes-Goßner-Straße");
    points.emplace_back(std::make_shared<qwm::draw::geometry::GeometryPointCircle>(qwm::util::PointWorldCoord(8.270476, 50.021426)));
    points.back()->setPen(pen);
    points.back()->setMetadata("name", "Wiesbaden-Mainz-Kastel, Ruthof");
    points.emplace_back(std::make_shared<qwm::draw::geometry::GeometryPointCircle>(qwm::util::PointWorldCoord(8.266445, 50.025913)));
    points.back()->setPen(pen);
    points.back()->setMetadata("name", "Wiesbaden-Mainz-Kastel, Mudra Kaserne");
    points.emplace_back(std::make_shared<qwm::draw::geometry::GeometryPointCircle>(qwm::util::PointWorldCoord(8.260378, 50.030345)));
    points.back()->setPen(pen);
    points.back()->setMetadata("name", "Wiesbaden-Mainz-Amoneburg, Dyckerhoffstraße");

    // Add the GeometryPoints to the layer (and the coord to the raw points).
    std::vector<qwm::util::PointWorldCoord> raw_points;
    for(const auto& point : points)
    {
        // Add the coored to the raw points.
        raw_points.push_back(point->coord());

        // Also add the point to the custom layer.
        layer_geometries->addDrawable(point);
    }

    // A QPen also can use transparency (225 = opaque, 0 = transparent).
    pen = QPen(QColor(0, 0, 255, 100));
    pen.setWidth(5);

    // Create a GeometryLineString using the raw points.
    auto line_string(std::make_shared<qwm::draw::geometry::GeometryLineString>(raw_points));
    line_string->setPen(pen);
    line_string->setMetadata("name", "Busline 54");
    line_string->setMetadataDisplayed("name", 5, qwm::draw::geometry::AlignmentType::TopRight, 5.0);

    // Add the GeoemtryLineString to the layer.
    layer_geometries->addDrawable(line_string);


    // Create a GeometryArrow with a heading set.
    auto arrow_point(std::make_shared<qwm::draw::geometry::GeometryPointArrow>(qwm::util::PointWorldCoord(-20.0, 20.0), QSizeF(15.0, 15.0)));
    arrow_point->setPen(QPen(Qt::red));
    arrow_point->setRotation(92.4);
    layer_geometries->addDrawable(arrow_point);


// Only include if GDAL enabled.
#ifdef QWM_GDAL
    // ESRI Shapefile example.
    auto esri_layer(std::make_shared<qwm::Layer>("ESRI"));
    auto esri_shapefile(std::make_shared<qwm::draw::other::ESRIShapefile>("resources/esri_shapefile/countries.shp"));
    esri_shapefile->setPenPolygon(QPen(Qt::black));
    esri_layer->addDrawable(esri_shapefile);
    m_map_control->layer_manager().add(esri_layer);
#endif
}

void Example::setupLayout()
{
    // Create the buttons.
    m_button_add_point = new QPushButton("Enable 'Add Point'");
    m_button_add_point->setCheckable(true);
    m_button_add_point->setMaximumWidth(100);
    m_button_add_point->setMaximumHeight(20);
    m_button_add_point->setFont(QFont("Verdana", 5));
    QObject::connect(m_button_add_point, &QPushButton::toggled, this, &Example::toggleAddPoint);

    m_button_draw_rect = new QPushButton("Enable 'Drag Rect'");
    m_button_draw_rect->setCheckable(true);
    m_button_draw_rect->setMaximumWidth(100);
    m_button_draw_rect->setMaximumHeight(20);
    m_button_draw_rect->setFont(QFont("Verdana", 5));
    QObject::connect(m_button_draw_rect, &QPushButton::toggled, this, &Example::toggleDrawRect);

    m_button_move_click = new QPushButton("Enable 'Move to Click'");
    m_button_move_click->setCheckable(true);
    m_button_move_click->setMaximumWidth(100);
    m_button_move_click->setMaximumHeight(20);
    m_button_move_click->setFont(QFont("Verdana", 5));
    QObject::connect(m_button_move_click, &QPushButton::toggled, this, &Example::toggleMoveClick);

    // Set the focus policy of each button.
    m_button_add_point->setFocusPolicy(Qt::NoFocus);
    m_button_draw_rect->setFocusPolicy(Qt::NoFocus);
    m_button_move_click->setFocusPolicy(Qt::NoFocus);


    // Create an inner layout to display buttons/"mini" map control.
    auto layout_inner(new QVBoxLayout);
    layout_inner->addWidget(m_button_add_point);
    layout_inner->addWidget(m_button_draw_rect);
    layout_inner->addWidget(m_button_move_click);
    layout_inner->addSpacing(300);
    layout_inner->addWidget(m_mini_map_control.get());

    // Set the main map control to use the inner layout.
    m_map_control->setLayout(layout_inner);

    // Create a main layout for the main map control.
    auto layout_main(new QHBoxLayout);
    layout_main->addWidget(m_map_control.get());
    layout_main->setMargin(0);

    // Set the main layout to the QWidget.
    QWidget::setLayout(layout_main);
}

void Example::resizeEvent(QResizeEvent* event)
{
    // Set the new viewport size.
    m_map_control->setSize(event->size());
}

void Example::toggleAddPoint(bool enable)
{
    // Should we enable it?
    if(enable)
    {
        // Update the button text.
        m_button_add_point->setText("Disable 'Add Point'");
    }
    else
    {
        // Update the button text.
        m_button_add_point->setText("Enable 'Add Point'");
    }
}

void Example::toggleDrawRect(bool enable)
{
    // Should we enable it?
    if(enable)
    {
        // Set the left mouse button to draw a rect.
        m_map_control->event_manager().setMouseButtonLeft(qwm::EventManager::MouseButtonMode::DrawBox);

        // Update the button text.
        m_button_draw_rect->setText("Disable 'Drag Rect'");
    }
    else
    {
        // Default the left mouse button to pan.
        m_map_control->event_manager().setMouseButtonLeft(qwm::EventManager::MouseButtonMode::Pan);

        // Update the button text.
        m_button_draw_rect->setText("Enable 'Drag Rect'");
    }
}

void Example::toggleMoveClick(bool enable)
{
    // Should we enable it?
    if(enable)
    {
        // Update the button text.
        m_button_move_click->setText("Disable 'Move to Click'");
    }
    else
    {
        // Update the button text.
        m_button_move_click->setText("Enable 'Move to Click'");
    }
}

void Example::geometriesSelectedEvent(std::map<std::string, std::vector<std::shared_ptr<qwm::draw::geometry::Geometry>>> selected_geometries)
{
    // Loop through the layers.
    for(const auto& layers : selected_geometries)
    {
        // Loop through the geometries selected in the layer.
        for(const auto& geometry : layers.second)
        {
            // Process the clicked geometry.
            drawableClickEvent(geometry);
        }
    }
}

void Example::drawableClickEvent(const std::shared_ptr<qwm::draw::Drawable>& drawable)
{
    // Is it a geometry drawable item.
    if(drawable->drawableType() == qwm::draw::DrawableType::Geometry)
    {
        // Cast to a geomtery point.
        const auto geometry(std::static_pointer_cast<qwm::draw::geometry::Geometry>(drawable));

        // Is it a GeometryPoint.
        if(geometry->geometryType() == qwm::draw::geometry::GeometryType::GeometryPoint)
        {
            // Display a message box with the point's details.
            QMessageBox::information(this, geometry->metadata("name").toString(), geometry->metadata("name").toString());
        }
    }
}

void Example::main_mouseEventPressCoordinate(QMouseEvent* mouse_event, qwm::util::PointWorldCoord press_coordinate)
{
    // Was there a left mouse button press?
    if(mouse_event->button() == Qt::MouseButton::LeftButton)
    {
        // Is the "Move to click" button checked?
        if(m_button_move_click->isChecked())
        {
            // Move both the "mini" and main map controls to the click coordinate.
            m_mini_map_control->viewport_manager().setFocusPointWorldAnimated(press_coordinate);
            m_map_control->viewport_manager().setFocusPointWorldAnimated(press_coordinate);
        }

        // Is the "Add point" button checked?
        if(m_button_add_point->isChecked())
        {
            // Add a GeometryPointCircle with a radius of 10.0 pixels.
            m_map_control->layer_manager().layer("Geometry Layer")->addDrawable(std::make_shared<qwm::draw::geometry::GeometryPointCircle>(press_coordinate, QSizeF(10.0, 10.0)));
        }
    }
}

void Example::main_mouseKeyEventCoordinate()
{
    // Update the location of the "mini" map control.
    m_mini_map_control->viewport_manager().setFocusPointWorldCoord(m_map_control->viewport_manager().focusPointWorldCoord());
}

void Example::mini_mouseEventPressCoordinate(QMouseEvent* mouse_event, qwm::util::PointWorldCoord press_coordinate)
{
    // Was there a left mouse button press?
    if(mouse_event->button() == Qt::MouseButton::LeftButton)
    {
        // Move both the "mini" and main map controls to the click coordinate.
        m_mini_map_control->viewport_manager().setFocusPointWorldAnimated(press_coordinate);
        m_map_control->viewport_manager().setFocusPointWorldAnimated(press_coordinate);
    }
}
