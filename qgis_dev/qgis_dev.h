#ifndef QGIS_DEV_H
#define QGIS_DEV_H

#include <QtGui/QMainWindow>
#include "ui_qgis_dev.h"

#include "qgis_devComposer.h"

#include <QList>
#include <QDockWidget>
#include <QProgressBar>
#include <QLabel>
#include <QDockWidget>
#include <QIcon>
#include <QStackedWidget>
#include <QTimer>

// QGis include
#include <qgsmapcanvas.h>
#include <qgsmaplayer.h>
#include <qgslayertreeview.h>
#include <qgslayertreemapcanvasbridge.h>
#include <qgsscalecombobox.h>
#include <qgsrendererv2.h>
#include <qgscomposermap.h>
#include <qgscomposerview.h>

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

    //! �ı�ʸ��ͼ�����ʾ��ʽ
    void changeSymbol( QgsVectorLayer* layer, QgsFeatureRendererV2* featureRenderer );
    //! ��ʾ���ʹ��ʸ��ͼ����ʾ��ʽ
    void layerSymbolTest();
    //! �Ƴ�ͼ��
    void removeAllLayers();
    void removeLayer();

private slots:
    //! ��ʾ���λ�õ�������
    void showMouseCoordinate( const QgsPoint & );
    //! ����ӥ��ͼ
    void createOverview();
    //! ��ʾ��ǰ��ͼ����
    void showScale( double theScale );
    //! �����û�ָ���������ŵ�ͼ
    void userScale();
    //! ������ʾ�û���������ʾ�������������
    void userCenter();

    //! ˢ�µ�ͼ��ʾ
    void refreshMapCanvas();

    //! ���ĵ�ͼ��λ , Ŀǰ������������ʾ�Ŀؼ��ϣ�δ��Ӧ�û�����ӿ�
    void mapUnitChange( int i );
    //! ��ѣЧ��
    void dizzy(); // ���Ч����qgis����Ա���ص����ַ�ʽ�ɣ�just for fun


private:
    Ui::qgis_devClass ui;
    static qgis_dev* sm_instance;

    QStackedWidget* m_stackedWidget;
    QComboBox* pageViewComboBox;

    QgsMapCanvas* m_mapCanvas; // ��ͼ����

    //! ͼ�����
    QgsLayerTreeView* m_layerTreeView;
    QgsLayerTreeMapCanvasBridge *m_layerTreeCanvasBridge;

    QDockWidget *m_layerTreeDock; // װͼ�������
    QDockWidget *m_layerOrderDock;
    QDockWidget *m_OverviewDock; // װӥ��ͼ
    QDockWidget *m_pGpsDock;
    QDockWidget *m_logDock;

    QLabel* m_scaleLabel; // ��״̬������ʾ��scale 1:��
    QgsScaleComboBox* m_scaleEdit; //! ��״̬������ʾ������ֵ
    QProgressBar* m_progressBar;
    QLabel* m_coordsLabel; // ��״̬����ʾ"Coordinate / Extent"
    QLineEdit* m_coordsEdit; // ��״̬����ʾ��������

    QList<QgsMapCanvasLayer> mapCanvasLayerSet; // ��ͼ�������õ�ͼ�㼯��

    QCursor* m_overviewMapCursor; // ӥ��ͼ�õ����ָ��

    unsigned int m_MousePrecisionDecimalPlaces; // ָ���������С����λ��

    qgis_devComposer* m_composer; // ��ͼ��ͼģ��

    //! Helps to make people dizzy
    QTimer *m_dizzyTimer;

//=== Private Member Functions ===

    //! ��ʼ��ͼ�������
    void initLayerTreeView();
    //! ����״̬��
    void createStatusBar();


    //! ������ӡ��ͼ��ͼ
    void createComposer();
    //! ��ȡ��ǰѡ��ͼ��
    QgsMapLayer* activeLayer();

    //! ���ڻ�ȡimages��Դ�⵱�е�Icon·��
    static const QString activeThemePath();
    static const QString defaultThemePath();

};

#endif // QGIS_DEV_H
