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
#include <QtCore/QtGlobal>
#if (QT_VERSION < QT_VERSION_CHECK(5, 5, 0))
#include <QtDesigner/QDesignerCustomWidgetInterface>
#else
#include <QtUiPlugin/QDesignerCustomWidgetInterface>
#endif

class PluginQWidgetMap : public QObject, public QDesignerCustomWidgetInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "PluginQWidgetMap")
    Q_INTERFACES(QDesignerCustomWidgetInterface)

public:

    /**
     * Constructor.
     * @param parent QObject parent ownership.
     */
    PluginQWidgetMap(QObject* parent = nullptr);

    /**
     * Called when initialising the plugin.
     * @param core The core form interface.
     */
    void initialize(QDesignerFormEditorInterface* core);

    /**
     * Check whether the plugin has been initialised.
     * @return whether the plugin is initialised.
     */
    bool isInitialized() const;

    /**
     * Creates an instance of the widget.
     * @param parent QWidget parent ownership.
     * @return the widget.
     */
    QWidget* createWidget(QWidget* parent);

    /**
     * Fetches the name of the widget.
     * @return the name of the widget.
     */
    QString name() const;

    /**
     * Fetches the grouping name.
     * @return the grouping name.
     */
    QString group() const;

    /**
     * Fetches the icon to display.
     * @return the icon to display.
     */
    QIcon icon() const;

    /**
     * Fetches the tool tip.
     * @return the tool tip.
     */
    QString toolTip() const;

    /**
     * Fetches the what's this text.
     * @return the what's this text.
     */
    QString whatsThis() const;

    /**
     * Fetches whether the widget can contain other widgets.
     * @return whether the widget can contain other widgets.
     */
    bool isContainer() const;

    /**
     * Fetches the default property elements.
     * @return the default property elements.
     */
    QString domXml() const;

    /**
     * Fetches the header include file to insert during UIC generation.
     * @return the header include file to insert during UIC generation.
     */
    QString includeFile() const;

private:

    /// Whether the plugin has been initialised.
    bool m_initialised { false };

};
