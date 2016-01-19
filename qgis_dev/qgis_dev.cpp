#include "qgis_dev.h"

#include <QDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QWidget>
#include <QGridLayout>
#include <QFont>
#include <QLineEdit>
#include <QToolButton>
#include <QMargins>
#include <QFile>
#include <QDir>
#include <Qt>
#include <QBitmap>

// QGis include
#include <qgsvectorlayer.h>
#include <qgsmaplayerregistry.h>
#include <qgsrasterlayer.h>
#include <qgsproject.h>
#include <qgslayertreemodel.h>
#include <qgsapplication.h>
#include <qgslayertreelayer.h>
#include <qgslayertreegroup.h>
#include <qgslayertreeregistrybridge.h>
#include <qgslayertreeviewdefaultactions.h>
#include <qgsattributedialog.h>
#include <qgscursors.h>

#include "qgis_devlayertreeviewmenuprovider.h"
#include "qgis_devattrtabledialog.h"

// for attribute table
#include <qgsfeaturelistview.h>
#include <qgsattributetableview.h>
#include <qgsattributetablemodel.h>
#include <qgsfeaturelistmodel.h>
#include <qgsvectorlayercache.h>

#include <qgsattributedialog.h>
#include <qgseditorwidgetfactory.h>


// for layer symbol
#include <qgssymbollayerv2.h>
#include <qgssymbolv2.h>
#include <qgsmarkersymbollayerv2.h>
#include <qgsvectorlayerrenderer.h>
#include <qgsrendercontext.h>
#include <qgssinglesymbolrendererv2.h>
#include <qgssymbollayerv2.h>
#include <qgsmapcanvas.h>
#include <qgsmapoverviewcanvas.h>

qgis_dev* qgis_dev::sm_instance = 0;

qgis_dev::qgis_dev( QWidget *parent, Qt::WindowFlags flags )
    : QMainWindow( parent, flags ),
      m_MousePrecisionDecimalPlaces( 0 )
{
    if ( sm_instance )
    {
        QMessageBox::critical(
            this,
            tr( "Multiple Instances of QgisApp" ),
            tr( "Multiple instances of QGIS application object detected.\nPlease contact the developers.\n" ) );
        // abort();
    }
    sm_instance = this;

    ui.setupUi( this );

    //! ��ʼ��map canvas
    m_mapCanvas = new QgsMapCanvas();
    m_mapCanvas->enableAntiAliasing( true );
    m_mapCanvas->setCanvasColor( QColor( 255, 255, 255 ) );

    //! ������ӡ��ͼ��ͼ
    m_composer = new qgis_devComposer( this );
    createComposer();

    //! ��ʼ��ͼ�������
    m_layerTreeView = new QgsLayerTreeView( this );
    initLayerTreeView();

    //! ����
    QWidget* centralWidget = this->centralWidget();
    QGridLayout* centralLayout = new QGridLayout( centralWidget );

    m_stackedWidget = new QStackedWidget( this );
    m_stackedWidget->setLayout( new QHBoxLayout() );
    m_stackedWidget->addWidget( m_mapCanvas );
    m_stackedWidget->addWidget( m_composer );

    centralLayout->addWidget( m_stackedWidget, 0, 0, 1, 1 );

    //! ��ʼ��status bar
    createStatusBar();

    // connections
    connect( ui.actionAdd_Vector, SIGNAL( triggered() ), this, SLOT( addVectorLayers() ) );
    connect( ui.actionAdd_Raster, SIGNAL( triggered() ), this, SLOT( addRasterLayers() ) );
    connect( ui.actionToggle_Overview, SIGNAL( triggered() ), this, SLOT( createOverview() ) );
}

qgis_dev::~qgis_dev()
{
    delete sm_instance;
}

void qgis_dev::addVectorLayers()
{
    QString filename = QFileDialog::getOpenFileName( this, tr( "open vector" ), "", "*.shp" );
    if ( filename == QString::null ) { return;}

    // test attribute table
    // QString filename = "D:\\Data\\qgis_sample_data\\shapefiles\\airports.shp" ;

    QStringList temp = filename.split( QDir::separator() );
    QString basename = temp.at( temp.size() - 1 );
    QgsVectorLayer* vecLayer = new QgsVectorLayer( filename, basename, "ogr", false );
    if ( !vecLayer->isValid() )
    {
        QMessageBox::critical( this, "error", "layer is invalid" );
        return;
    }

    QgsMapLayerRegistry::instance()->addMapLayer( vecLayer );
    mapCanvasLayerSet.append( vecLayer );
    m_mapCanvas->setExtent( vecLayer->extent() );
    m_mapCanvas->setLayerSet( mapCanvasLayerSet );
    m_mapCanvas->setVisible( true );
    m_mapCanvas->freeze( false );
    m_mapCanvas->refresh();
}

void qgis_dev::addRasterLayers()
{
    QString filename = QFileDialog::getOpenFileName( this, tr( "open vector" ), "", "*.tif" );
    QStringList temp = filename.split( QDir::separator() );
    QString basename = temp.at( temp.size() - 1 );
    QgsRasterLayer* rasterLayer = new QgsRasterLayer( filename, basename, "gdal", false );
    if ( !rasterLayer->isValid() )
    {
        QMessageBox::critical( this, "error", "layer is invalid" );
        return;
    }

    QgsMapLayerRegistry::instance()->addMapLayer( rasterLayer );
    mapCanvasLayerSet.append( rasterLayer );
    m_mapCanvas->setExtent( rasterLayer->extent() );
    m_mapCanvas->setLayerSet( mapCanvasLayerSet );
    m_mapCanvas->setVisible( true );
    m_mapCanvas->freeze( false );
    m_mapCanvas->refresh();
}

void qgis_dev::initLayerTreeView()
{
    QgsLayerTreeModel* model = new QgsLayerTreeModel( QgsProject::instance()->layerTreeRoot(), this );
    model->setFlag( QgsLayerTreeModel::AllowNodeRename );
    model->setFlag( QgsLayerTreeModel::AllowNodeReorder );
    model->setFlag( QgsLayerTreeModel::AllowNodeChangeVisibility );
    model->setFlag( QgsLayerTreeModel::ShowLegendAsTree );
    model->setAutoCollapseLegendNodes( 10 );

    m_layerTreeView->setModel( model );

    // ����Ҽ��˵�
    m_layerTreeView->setMenuProvider( new qgis_devLayerTreeViewMenuProvider( m_layerTreeView, m_mapCanvas ) );

    connect( QgsProject::instance()->layerTreeRegistryBridge(), SIGNAL( addedLayersToLayerTree( QList<QgsMapLayer*> ) ),
             this, SLOT( autoSelectAddedLayer( QList<QgsMapLayer*> ) ) );

    // add group tool button
    QToolButton * btnAddGroup = new QToolButton();
    btnAddGroup->setAutoRaise( true );
    btnAddGroup->setIcon( qgis_dev::getThemeIcon( "mActionAdd.png" ) );

    btnAddGroup->setToolTip( tr( "Add Group" ) );
    connect( btnAddGroup, SIGNAL( clicked() ), m_layerTreeView->defaultActions(), SLOT( addGroup() ) );

    // expand / collapse tool buttons
    QToolButton* btnExpandAll = new QToolButton();
    btnExpandAll->setAutoRaise( true );
    btnExpandAll->setIcon( qgis_dev::getThemeIcon( "mActionExpandTree.png" ) );
    btnExpandAll->setToolTip( tr( "Expand All" ) );
    connect( btnExpandAll, SIGNAL( clicked() ), m_layerTreeView, SLOT( expandAll() ) );

    QToolButton* btnCollapseAll = new QToolButton();
    btnCollapseAll->setAutoRaise( true );
    btnCollapseAll->setIcon( qgis_dev::getThemeIcon( "mActionCollapseTree.png" ) );
    btnCollapseAll->setToolTip( tr( "Collapse All" ) );
    connect( btnCollapseAll, SIGNAL( clicked() ), m_layerTreeView, SLOT( collapseAll() ) );

    // remove item button
    QToolButton* btnRemoveItem = new QToolButton();
    btnRemoveItem->setIcon( qgis_dev::getThemeIcon( "mActionRemoveLayer.svg" ) );
    btnRemoveItem->setAutoRaise( true );
    connect( btnRemoveItem, SIGNAL( clicked() ), this, SLOT( removeLayer() ) );

    // ��ť����
    QHBoxLayout* toolbarLayout = new QHBoxLayout();
    toolbarLayout->setContentsMargins( QMargins( 5, 0, 5, 0 ) );
    toolbarLayout->addWidget( btnAddGroup );
    toolbarLayout->addWidget( btnCollapseAll );
    toolbarLayout->addWidget( btnExpandAll );
    toolbarLayout->addWidget( btnRemoveItem );
    toolbarLayout->addStretch();

    QVBoxLayout* vboxLayout = new QVBoxLayout();
    vboxLayout->setMargin( 0 );
    vboxLayout->addLayout( toolbarLayout );
    vboxLayout->addWidget( m_layerTreeView );

    // װ��dock widget��
    m_layerTreeDock = new QDockWidget( tr( "Layers" ), this );
    m_layerTreeDock->setObjectName( "Layers" );
    m_layerTreeDock->setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );

    QWidget* w = new QWidget();
    w->setLayout( vboxLayout );
    m_layerTreeDock->setWidget( w );
    addDockWidget( Qt::LeftDockWidgetArea, m_layerTreeDock );

    // ���ӵ�ͼ������ͼ�������
    m_layerTreeCanvasBridge = new QgsLayerTreeMapCanvasBridge( QgsProject::instance()->layerTreeRoot(), m_mapCanvas, this );
    connect( QgsProject::instance(), SIGNAL( writeProject( QDomDocument& ) ),
             m_layerTreeCanvasBridge, SLOT( writeProject( QDomDocument& ) ) );
    connect( QgsProject::instance(), SIGNAL( readProject( QDomDocument ) ),
             m_layerTreeCanvasBridge, SLOT( readProject( QDomDocument ) ) );

    connect( m_mapCanvas, SIGNAL( xyCoordinates( const QgsPoint& ) ), this, SLOT( showMouseCoordinate( const QgsPoint& ) ) );

}

void qgis_dev::createStatusBar()
{
    statusBar()->setStyleSheet( "QStatusBar::item {border: none;}" );

    //! ��ӵ�ͼ�ʹ�ӡ��ͼ�л���ComboBox
    pageViewComboBox = new QComboBox( statusBar() );
    pageViewComboBox->addItem( tr( "Map" ) );
    pageViewComboBox->addItem( tr( "Composer" ) );
    connect( pageViewComboBox, SIGNAL( activated( int ) ), m_stackedWidget, SLOT( setCurrentIndex( int ) ) );
    statusBar()->addPermanentWidget( pageViewComboBox );

    //! ��ӽ�����
    m_progressBar = new QProgressBar( statusBar() );
    m_progressBar->setObjectName( "m_progressBar" );
    m_progressBar->setMaximum( 100 );
    m_progressBar->hide();
    statusBar()->addPermanentWidget( m_progressBar, 1 );
    connect( m_mapCanvas, SIGNAL( renderStarting() ), this, SLOT( canvasRefreshStarted() ) );
    connect( m_mapCanvas, SIGNAL( mapCanvasRefreshed() ), this, SLOT( canvasRefreshFinished() ) );

    QFont myFont( "Arial", 9 );
    statusBar()->setFont( myFont );

    //! ���������ʾ��ǩ
    m_coordsLabel = new QLabel( QString(), statusBar() );
    m_coordsLabel->setObjectName( "m_coordsLabel" );
    m_coordsLabel->setFont( myFont );
    m_coordsLabel->setMinimumWidth( 10 );
    m_coordsLabel->setMargin( 3 );
    m_coordsLabel->setAlignment( Qt::AlignCenter );
    m_coordsLabel->setFrameStyle( QFrame::NoFrame );
    m_coordsLabel->setText( tr( "Coordinate:" ) );
    m_coordsLabel->setToolTip( tr( "Current map coordinate" ) );
    statusBar()->addPermanentWidget( m_coordsLabel, 0 );

    //! ����༭��ǩ
    m_coordsEdit = new QLineEdit( QString(), statusBar() );
    m_coordsEdit->setObjectName( "m_coordsEdit" );
    m_coordsEdit->setFont( myFont );
    m_coordsEdit->setMinimumWidth( 10 );
    m_coordsEdit->setMaximumWidth( 300 );
    m_coordsEdit->setContentsMargins( 0, 0, 0, 0 );
    m_coordsEdit->setAlignment( Qt::AlignCenter );
    statusBar()->addPermanentWidget( m_coordsEdit, 0 );
    //m_coordsEdit->setReadOnly( true );
    connect( m_coordsEdit, SIGNAL( returnPressed() ), this, SLOT( userCenter() ) );

    //! �����߱�ǩ
    m_scaleLabel = new QLabel( QString(), statusBar() );
    m_scaleLabel->setObjectName( "m_scaleLabel" );
    m_scaleLabel->setFont( myFont );
    m_scaleLabel->setMinimumWidth( 10 );
    m_scaleLabel->setMargin( 3 );
    m_scaleLabel->setAlignment( Qt::AlignCenter );
    m_scaleLabel->setFrameStyle( QFrame::NoFrame );
    m_scaleLabel->setText( tr( "Scale" ) );
    m_scaleLabel->setToolTip( tr( "Current map scale" ) );
    statusBar()->addPermanentWidget( m_scaleLabel, 0 );

    m_scaleEdit = new QgsScaleComboBox( statusBar() );
    m_scaleEdit->setObjectName( "m_scaleEdit" );
    m_scaleEdit->setFont( myFont );
    m_scaleEdit->setMinimumWidth( 10 );
    m_scaleEdit->setMaximumWidth( 100 );
    m_scaleEdit->setContentsMargins( 0, 0, 0, 0 );
    m_scaleEdit->setToolTip( tr( "Current map scale (formatted as x:y)" ) );
    statusBar()->addPermanentWidget( m_scaleEdit, 0 );
    connect( m_scaleEdit,  SIGNAL( scaleChanged() ), this, SLOT( userScale() ) );

}

void qgis_dev::autoSelectAddedLayer( QList<QgsMapLayer*> layers )
{
    if ( layers.count() )
    {
        QgsLayerTreeLayer* nodeLayer = QgsProject::instance()->layerTreeRoot()->findLayer( layers[0]->id() );

        if ( !nodeLayer )
        {
            return;
        }

        QModelIndex index = m_layerTreeView->layerTreeModel()->node2index( nodeLayer );
        m_layerTreeView->setCurrentIndex( index );
    }
}

void qgis_dev::addDockWidget( Qt::DockWidgetArea area, QDockWidget* dockwidget )
{
    QMainWindow::addDockWidget( area, dockwidget );
    setCorner( Qt::TopLeftCorner, Qt::LeftDockWidgetArea );
    setCorner( Qt::BottomLeftCorner, Qt::LeftDockWidgetArea );
    setCorner( Qt::TopRightCorner, Qt::RightDockWidgetArea );
    setCorner( Qt::BottomRightCorner, Qt::RightDockWidgetArea );

    dockwidget->show();
    m_mapCanvas->refresh();
}

void qgis_dev::openAttributeTableDialog()
{
    QgsVectorLayer* mylayer = qobject_cast<QgsVectorLayer*>( activeLayer() );
    if ( !mylayer ) { return; }
    qgis_devattrtableDialog* d = new qgis_devattrtableDialog( mylayer, this );
    d->show();

    QgsVectorLayerCache * lc = new QgsVectorLayerCache( mylayer, mylayer->featureCount() );
    QgsAttributeTableView* tv = new QgsAttributeTableView();

    QgsAttributeTableModel* tm = new QgsAttributeTableModel( lc, this );

    QgsAttributeTableFilterModel* tfm = new QgsAttributeTableFilterModel( m_mapCanvas, tm, tm );

    tfm->setFilterMode( QgsAttributeTableFilterModel::ShowAll );
    tm->loadLayer();
    tv->setModel( tfm );
    tv->show();
}

QgsMapLayer* qgis_dev::activeLayer()
{
    return m_layerTreeView ? m_layerTreeView->currentLayer() : 0;
}

void qgis_dev::removeLayer()
{
    if ( !m_layerTreeView ) {return;}
    foreach( QgsMapLayer* layer, m_layerTreeView->selectedLayers() )
    {
        QgsVectorLayer* veclayer = qobject_cast<QgsVectorLayer*>( layer );
        if ( veclayer && veclayer->isEditable() ) {return;}
    }

    QList<QgsLayerTreeNode*> selectedNodes = m_layerTreeView->selectedNodes( true );

    // validate selection
    if ( selectedNodes.isEmpty() )
    {
        QMessageBox::critical(
            this,
            tr( "Error" ),
            tr( "No selection valid" ) );
        return;
    }

    foreach ( QgsLayerTreeNode* node, selectedNodes )
    {
        QgsLayerTreeGroup* parentGroup = qobject_cast<QgsLayerTreeGroup*>( node->parent() );
        if ( parentGroup )
        {
            parentGroup->removeChildNode( node );
        }
    }
    m_mapCanvas->refresh();
}

QIcon qgis_dev::getThemeIcon( const QString &theName )
{
    QString myPreferredPath = activeThemePath() + QDir::separator() + theName;
    QString myDefaultPath = defaultThemePath() + QDir::separator() + theName;

    if ( QFile::exists( myPreferredPath ) )
    {
        return QIcon( myPreferredPath );
    }
    else if ( QFile::exists( myDefaultPath ) )
    {
        //could still return an empty icon if it
        //doesnt exist in the default theme either!
        return QIcon( myDefaultPath );
    }
    else
    {
        return QIcon();
    }
}

const QString qgis_dev::activeThemePath()
{
    return "";
}

const QString qgis_dev::defaultThemePath()
{
    return ":/images/themes/default/";
}

void qgis_dev::changeSymbol( QgsVectorLayer* layer, QgsFeatureRendererV2* featureRenderer )
{
    if ( !layer->isValid() ) {return;}
}

void qgis_dev::layerSymbolTest()
{
    // ��ȡ��ǰѡ�е�ͼ��
    QgsVectorLayer* veclayer = qobject_cast<QgsVectorLayer*>( this->activeLayer() );
    if( !veclayer->isValid() ) { return; }

    if ( veclayer->geometryType() == QGis::Point )
    {
        // ���� svgMarkerSymbolLayer
        QgsSvgMarkerSymbolLayerV2* svgMarker = new QgsSvgMarkerSymbolLayerV2( "money/money_bank2.svg" );

        QgsSymbolLayerV2List symList;
        symList.append( svgMarker );

        QgsMarkerSymbolV2* markSym = new QgsMarkerSymbolV2( symList );

        QgsSingleSymbolRendererV2* symRenderer = new QgsSingleSymbolRendererV2( markSym );

        svgMarker->setSize( 10 );
        veclayer->setRendererV2( symRenderer );
    }

}

void qgis_dev::showMouseCoordinate( const QgsPoint &p )
{
    m_coordsEdit->setText( p.toDegreesMinutes( m_MousePrecisionDecimalPlaces ) );
}

void qgis_dev::removeAllLayers()
{
    QgsMapLayerRegistry::instance()->removeAllMapLayers();
}

void qgis_dev::createOverview()
{
    QgsMapOverviewCanvas* overviewCanvas = new QgsMapOverviewCanvas( NULL, m_mapCanvas );
    overviewCanvas->setWhatsThis( tr( "Map overview canvas." ) );

    QBitmap overviewPanBmp = QBitmap::fromData( QSize( 16, 16 ), pan_bits ); // ������qgscursors.h�ļ���
    QBitmap overviewPanBmpMask = QBitmap::fromData( QSize( 16, 16 ), pan_mask_bits );
    m_overviewMapCursor = new QCursor( overviewPanBmp, overviewPanBmpMask, 0, 0 );
    overviewCanvas->setCursor( *m_overviewMapCursor );

    // װ��ӥ��ͼ��
    m_OverviewDock = new QDockWidget( tr( "Overview" ), this );
    m_OverviewDock->setObjectName( "Overview" );
    m_OverviewDock->setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
    m_OverviewDock->setWidget( overviewCanvas );
    addDockWidget( Qt::LeftDockWidgetArea, m_OverviewDock );

    m_OverviewDock->setMinimumSize( 200, 200 );

    m_mapCanvas->enableOverviewMode( overviewCanvas );

    // ������ͼ������������ͼ�㶼���뵽ӥ��ͼ��
    if ( m_layerTreeView )
    {
        foreach( QgsLayerTreeLayer* nodeLayer, m_layerTreeView->layerTreeModel()->rootGroup()->findLayers() )
        {
            nodeLayer->setCustomProperty( "overview", 1 );
        }
    }

}

void qgis_dev::createComposer()
{

}

