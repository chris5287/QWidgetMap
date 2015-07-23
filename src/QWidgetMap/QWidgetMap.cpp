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

#include "QWidgetMap.h"

// Qt includes.
#include <QtWidgets/QStyleOption>

// STL includes.
#include <cmath>

// Local includes.
#include "ui_QWidgetMap.h"
#include "util/ImageManager.h"

using namespace qwm;

QWidgetMap::QWidgetMap(const QSizeF& size_px, QWidget* parent, Qt::WindowFlags window_flags)
    : QWidget(parent, window_flags),
      m_ui(new Ui::QWidgetMap),
      m_layer_manager(std::make_shared<LayerManager>()),
      m_viewport_manager(std::make_shared<ViewportManager>(size_px, projection::EPSG::SphericalMercator, QSize(256, 256))),
      m_event_manager(m_viewport_manager, m_layer_manager),
      m_render_manager(m_viewport_manager, m_layer_manager),
      m_primary_screen_pixmap(size_px.toSize() * 2)
{
    // Setup the UI form.
    m_ui->setupUi(this);

    // Setup event manager filtering.
    QObject::connect(&m_event_manager, &EventManager::requestRedraw, this, &QWidgetMap::updateUI);
    installEventFilter(&m_event_manager);

    // Connect signals/slots to show rendering progress.
    QObject::connect(&m_render_manager, &RenderManager::renderingStarted, m_ui->m_progress_indicator, &util::QProgressIndicator::startAnimation);
    QObject::connect(&m_render_manager, &RenderManager::renderingFinished, m_ui->m_progress_indicator, &util::QProgressIndicator::stopAnimation);
    QObject::connect(&m_render_manager, &RenderManager::renderingFinished, this, &QWidgetMap::clearScaledPrimaryScreen);

    // Connect signal/slot to update the primary screen when the render manager provides an image change.
    QObject::connect(&m_render_manager, &RenderManager::imageChanged, this, &QWidgetMap::updatePrimaryScreen);

    // Connect signal/slot to update the controls when viewport changes are made.
    QObject::connect(m_viewport_manager.get(), &ViewportManager::viewportChanged, this, &QWidgetMap::updateUI);

    // Connect signal/slots for zoom controls.
    QObject::connect(m_ui->m_button_zoom_in, &QPushButton::clicked, m_viewport_manager.get(), &ViewportManager::zoomIn);
    QObject::connect(m_ui->m_slider_zoom, &QSlider::valueChanged, m_viewport_manager.get(), &ViewportManager::setZoom);
    QObject::connect(m_ui->m_button_zoom_out, &QPushButton::clicked, m_viewport_manager.get(), &ViewportManager::zoomOut);

    // Default - background colour (transparent).
    setBackgroundColour(Qt::transparent);

    // Set QWidget viewport size.
    setSize(size_px);
}

QWidgetMap::~QWidgetMap()
{
    // Destroy the image manager instance.
    util::ImageManager::destory();
}

ViewportManager& QWidgetMap::viewport_manager()
{
    // Assert that a viewport manager been set up.
    Q_ASSERT(m_viewport_manager.get() != nullptr);

    // Returns the viewport manager,
    return *(m_viewport_manager.get());
}

LayerManager& QWidgetMap::layer_manager()
{
    // Assert that a layer manager been set up.
    Q_ASSERT(m_layer_manager.get() != nullptr);

    // Returns the layer manager,
    return *(m_layer_manager.get());
}

EventManager& QWidgetMap::event_manager()
{
    // Returns the event manager.
    return m_event_manager;
}

void QWidgetMap::setSize(const QSizeF& size_px)
{
    // Set the inherited QWidget maximum size.
    QWidget::setMaximumSize(size_px.width() + 1, size_px.height() + 1);

    // Sets the viewport size.
    m_viewport_manager->setSizePx(size_px);

    // Update the UI.
    updateUI();
}

void QWidgetMap::setNetworkProxy(const QNetworkProxy& proxy)
{
    // Set the Image Manager's network proxy.
    util::ImageManager::get().setProxy(proxy);
}

void QWidgetMap::enablePersistentCache(const std::chrono::minutes& expiry, const QDir& path)
{
    // Set the Image Manager's persistent cache settings.
    util::ImageManager::get().enablePersistentCache(expiry, path);
}

void QWidgetMap::setBackgroundColour(const QColor& colour)
{
    // Set the background colour.
    QPalette palette_background(QWidget::palette());
    palette_background.setColor(QPalette::Window, colour);
    QWidget::setAutoFillBackground(true);
    QWidget::setPalette(palette_background);
}

void QWidgetMap::enableScaledPrimaryScreen(const bool& visible)
{
    // Set whether the scaled primary screen show be visible.
    m_scaled_primary_screen_enabled = visible;
}

void QWidgetMap::enableFocusPointCrosshairs(const bool& visible)
{
    // Set whether the crosshairs should be visible.
    m_crosshairs_enabled = visible;
}

void QWidgetMap::enableZoomControls(const bool& enable, const bool& align_left)
{
    // Update the required aligment.
    m_control_zoom_align_left = align_left;

    // Set the zoom controls visibility.
    m_ui->m_widget_zoom->setVisible(enable);

    // Update zoom controls.
    updateUI();
}

QPixmap QWidgetMap::primaryScreen() const
{
    // Fetch the current viewport manager.
    const Viewport current_viewport(*(m_viewport_manager.get()));

    // Construct a QPixmap of the current viewport size.
    QPixmap return_pixmap(current_viewport.sizePx().toSize());
    return_pixmap.fill(Qt::transparent);

    // Construct a QPainter for the QPixmap.
    QPainter painter(&return_pixmap);

    // Draw the current primary screen to the pixmap's painter.
    drawPrimaryScreen(painter, current_viewport);

    // Return the pixmap.
    return return_pixmap;
}

void QWidgetMap::updateUI()
{
    // Fetch the current viewport manager.
    const Viewport current_viewport(*(m_viewport_manager.get()));

    // Schedule a repaint of the existing UI.
    QWidget::update();

    // Check whether the primary screen is still valid.
    checkPrimaryScreen(current_viewport);

    // Update the zoom slider with the current zoom value.
    m_ui->m_slider_zoom->setMinimum(m_viewport_manager->zoomMinimum());
    m_ui->m_slider_zoom->setMaximum(m_viewport_manager->zoomMaximum());
    m_ui->m_slider_zoom->setValue(current_viewport.zoom());

    // Default values.
    static const int margin(10);

    // Zoom controls on the left?
    if(m_control_zoom_align_left)
    {
        // Place the zoom controls on the left.
        m_ui->m_widget_zoom->setGeometry(margin, margin, m_ui->m_widget_zoom->width(), m_ui->m_widget_zoom->height());

        // Place the progress indicator on the right.
        m_ui->m_progress_indicator->setGeometry(size().width() - m_ui->m_progress_indicator->width() - margin, margin, m_ui->m_progress_indicator->width(), m_ui->m_progress_indicator->height());
    }
    else
    {
        // Place the zoom controls on the right.
        m_ui->m_widget_zoom->setGeometry(size().width() - m_ui->m_widget_zoom->width() - margin, margin, m_ui->m_widget_zoom->width(), m_ui->m_widget_zoom->height());

        // Place the progress indicator on the left.
        m_ui->m_progress_indicator->setGeometry(margin, margin, m_ui->m_progress_indicator->width(), m_ui->m_progress_indicator->height());
    }
}

void QWidgetMap::drawPrimaryScreen(QPainter& painter, const Viewport& viewport) const
{
    // Get access to the primary screen mutex.
    std::lock_guard<std::mutex> locker(m_mutex_primary_screen);

    // Do we have an invalid primary screen for the current viewport.
    if(m_primary_screen_zoom != viewport.zoom())
    {
        // Calculate the scale difference from the zoom levels.
        const double scale(std::pow(2, viewport.zoom() - m_primary_screen_zoom));

        // Save the scaled screen pixmap and rect coords.
        m_scaled_primary_screen_pixmap = m_primary_screen_pixmap.scaledToWidth(m_primary_screen_pixmap.width() * scale);
        m_scaled_primary_screen_rect_world_coord = m_primary_screen_rect_world_coord;
    }

    // Draw the scaled primary screen is allowed and available.
    if(m_scaled_primary_screen_enabled && m_scaled_primary_screen_pixmap.isNull() == false)
    {
        painter.drawPixmap(projection::toPointViewportPx(viewport, m_scaled_primary_screen_rect_world_coord.topLeftCoord()), m_scaled_primary_screen_pixmap);
    }

    // Do we have a valid primary screen for the current viewport.
    if(m_primary_screen_zoom == viewport.zoom())
    {
        // Draws the primary screen image to the pixmap.
        // Note: viewport.sizeCenterPx() is the same as (viewport.sizePx() / 2)
        painter.drawPixmap(projection::toPointViewportPx(viewport, m_primary_screen_rect_world_coord.topLeftCoord()), m_primary_screen_pixmap);
    }
}

void QWidgetMap::checkPrimaryScreen(const Viewport& viewport)
{
    // Get access to the primary screen mutex.
    std::lock_guard<std::mutex> locker(m_mutex_primary_screen);

    // Do we have an invalid primary screen?
    if(m_primary_screen_rect_world_coord.contains(viewport.rectWorldCoord()) == false || m_primary_screen_zoom != viewport.zoom())
    {
        // Invalid primary screen detected, request a redraw from the render manager.
        m_render_manager.requestRedraw();
    }
}

void QWidgetMap::updatePrimaryScreen(QPixmap pixmap, util::RectWorldCoord rect_world_coord, int zoom)
{
    // Get access to the primary screen mutex.
    std::lock_guard<std::mutex> locker(m_mutex_primary_screen);

    // Update the primary screen's details.
    m_primary_screen_pixmap = pixmap;
    m_primary_screen_rect_world_coord = rect_world_coord;
    m_primary_screen_zoom = zoom;

    // Schedule a repaint.
    QWidget::update();
}

void QWidgetMap::clearScaledPrimaryScreen()
{
    // Reset the scaled primary screen.
    m_scaled_primary_screen_pixmap = QPixmap();
    m_scaled_primary_screen_rect_world_coord = util::RectWorldCoord { util::PointWorldCoord { 0.0, 0.0 },  util::PointWorldCoord { 0.0, 0.0 } };

    // Schedule a repaint.
    QWidget::update();
}

void QWidgetMap::paintEvent(QPaintEvent* paint_event)
{
    // Fetch the current viewport manager.
    const Viewport current_viewport(*(m_viewport_manager.get()));

    // Call inherited QWidgets paint event first.
    QWidget::paintEvent(paint_event);

    // Create a painter for this QWidget to draw on.
    QPainter painter(this);

    // Ensure antialiasing is enabled (primitives and pixmaps).
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform, true);

    // Apply style sheet options to painter.
    QStyleOption style_options;
    style_options.initFrom(this);
    QWidget::style()->drawPrimitive(QStyle::PE_Widget, &style_options, &painter, this);

    // Draw the current primary screen to the widget.
    drawPrimaryScreen(painter, current_viewport);

    // Draw a box around the edge of the viewport (useful for debugging).
    painter.drawRect(util::RectViewportPx(util::PointViewportPx(0.0, 0.0), util::PointViewportPx(current_viewport.sizePx().width(), current_viewport.sizePx().height())));

    // Should we draw the crosshairs?
    if(m_crosshairs_enabled)
    {
        // Draw the crosshair at the viewport center.
        // |
        painter.drawLine(current_viewport.sizePointCenterPx().x(), current_viewport.sizePointCenterPx().y() - 10.0,
                         current_viewport.sizePointCenterPx().x(), current_viewport.sizePointCenterPx().y() + 10.0);
        // -
        painter.drawLine(current_viewport.sizePointCenterPx().x() - 10.0, current_viewport.sizePointCenterPx().y(),
                         current_viewport.sizePointCenterPx().x() + 10.0, current_viewport.sizePointCenterPx().y());
    }

    // Draw the event manager's preview event detail.
    m_event_manager.previewEvent(current_viewport, painter);
}
