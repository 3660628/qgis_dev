#ifndef QGIS_DEV_H
#define QGIS_DEV_H

#include <QtGui/QMainWindow>
#include "ui_qgis_dev.h"

#include <QList>
#include <QDockWidget>
#include <QProgressBar>
#include <QLabel>
#include <QDockWidget>
#include <QIcon>

// QGis include
#include <qgsmapcanvas.h>
#include <qgsmaplayer.h>
#include <qgslayertreeview.h>
#include <qgslayertreemapcanvasbridge.h>
#include <qgsscalecombobox.h>
#include <qgsrendererv2.h>

class qgis_dev : public QMainWindow
{
    Q_OBJECT

public:
    qgis_dev( QWidget *parent = 0, Qt::WindowFlags flags = Qt::Window );
    ~qgis_dev();

    void addDockWidget( Qt::DockWidgetArea area, QDockWidget* dockwidget );

    //! ���ֵ�һʵ��
    static qgis_dev* instance() { return sm_instance; }

    static QIcon getThemeIcon( const QString &theName );

    QgsMapCanvas* mapCanvas() {return m_mapCanvas; }
public slots:
    //! ���ʸ��ͼ��
    void addVectorLayers();
    //! ���դ��ͼ��
    void addRasterLayers();

    void autoSelectAddedLayer( QList<QgsMapLayer*> layers );
    //! �����Ա�
    void openAttributeTableDialog();
    //! �Ƴ�ͼ��
    void removeLayer();
    //! �ı�ʸ��ͼ�����ʾ��ʽ
    void changeSymbol( QgsVectorLayer* layer, QgsFeatureRendererV2* featureRenderer );
private:
    Ui::qgis_devClass ui;
    static qgis_dev* sm_instance;

    QgsMapCanvas* m_mapCanvas; // ��ͼ����

    //! ͼ�����
    QgsLayerTreeView* m_layerTreeView;
    QgsLayerTreeMapCanvasBridge *m_layerTreeCanvasBridge;


    QDockWidget *m_layerTreeDock;
    QDockWidget *m_layerOrderDock;
    /*QDockWidget *m_OverviewDock;
    QDockWidget *m_pGpsDock;
    QDockWidget *m_logDock;*/

    QLabel* m_scaleLabel; // ��״̬������ʾ��scale 1:��
    QgsScaleComboBox* m_scaleEdit; //! ��״̬������ʾ������ֵ
    QProgressBar* m_progressBar;
    QLabel* m_coordsLabel; //! ��״̬����ʾ"Coordinate / Extent"
    QLineEdit* m_coordsEdit; //! ��״̬����ʾ��������

    QList<QgsMapCanvasLayer> mapCanvasLayerSet; // ��ͼ�������õ�ͼ�㼯��

//=== Private Member Functions ===

    void initLayerTreeView();
    void createStatusBar();

    //! ��ȡ��ǰѡ��ͼ��
    QgsMapLayer* activeLayer();

    //! ���ڻ�ȡimages��Դ�⵱�е�Icon·��
    static const QString activeThemePath();
    static const QString defaultThemePath();
};

#endif // QGIS_DEV_H
