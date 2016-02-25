#ifndef QGIS_DEV_BROWSERDOCKWIGHT_H
#define QGIS_DEV_BROWSERDOCKWIGHT_H

#include <QDockWidget>
#include <ui_qgsbrowserdockwidgetbase.h>

#include <QModelIndex>
#include <QShowEvent>

class QgsLayerItem;
class QgsBrowserModel;
class QModelIndex;
class qgis_dev_dockBrowserTreeView;
class QgsDataItem;
class qgis_dev_browserTreeFilterProxyModel;

class qgis_dev_browserDockWight : public QDockWidget, private Ui::QgsBrowserDockWidgetBase
{
    Q_OBJECT

public:
    qgis_dev_browserDockWight( QString name, QWidget *parent = 0 );
    ~qgis_dev_browserDockWight();

    void addFavouriteDirectory( QString favDir );

public slots:
    void addLayerAtIndex( const QModelIndex& index );
    void showContextMenu( const QPoint& );

    void addFavourite(); // ��ӵ��ղ�
    void addFavouriteDirectory(); // ���Ŀ¼���ղ�
    void removeFavourite(); // �Ƴ��ղ�

    void refresh();

    void showFilterWidget( bool visible );
    void setFilterSyntax( QAction * );
    void setCaseSensitive( bool caseSensitive ); // ���ô�Сд����
    void setFilter();

    // layer menu items
    void addCurrentLayer();
    void addSelectedLayers();
    void showProperties();
    void toggleFastScan();

protected:
    void refreshModel( const QModelIndex& index );
    void showEvent( QShowEvent* event ) override;
    void addLayer( QgsLayerItem* layerItem );

    qgis_dev_dockBrowserTreeView* m_browserView;
    QgsBrowserModel* m_model;
    qgis_dev_browserTreeFilterProxyModel* m_proxyModel;
    QString m_initPath;
private:

};

#endif // QGIS_DEV_BROWSERDOCKWIGHT_H
