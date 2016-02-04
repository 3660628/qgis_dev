#ifndef QGIS_DEVMAPTOOLIDENTIFYACTION_H
#define QGIS_DEVMAPTOOLIDENTIFYACTION_H

#include <QObject>
#include <QPointer>
#include <QMouseEvent>

#include <qgsmaptoolidentify.h>
#include <qgspoint.h>
#include <qgsfeature.h>
#include <qgsfeaturestore.h>
#include <qgsfield.h>
#include <qgsdistancearea.h>


class QgsIdentifyResultsDialog;
class QgsMapLayer;
class QgsRasterLayer;
class QgsVectorLayer;

class qgis_devMapToolIdentifyAction : public QgsMapToolIdentify
{
    Q_OBJECT

public:
    qgis_devMapToolIdentifyAction( QgsMapCanvas * canvas );
    ~qgis_devMapToolIdentifyAction();

    ////! ��д����ƶ��¼�
    //virtual void canvasMoveEvent( QMouseEvent * e );

    ////! ��д���������¼�
    //virtual void canvasPressEvent( QMouseEvent * e );

    //! ��д�����ͷ��¼�
    virtual void canvasReleaseEvent( QMouseEvent * e );

    //virtual void activate();

    //virtual void deactivate();

public slots:
    //void handleCopyToClipboard( QgsFeatureStore & );
    //void handleChangedRasterResults( QList<IdentifyResult>& results );

signals:
    //void identifyProgress( int, int );
    //void identifyMessage( QString );
    // void copyToClipboard( QgsFeatureStore & );

private slots:
    //void showAttributeTable( QgsMapLayer* layer, const QList<QgsFeature> featureList );

private:
    ////! Pointer to the identify results dialog for name/value pairs
    //QPointer<QgsIdentifyResultsDialog> mResultsDialog;

    //QgsIdentifyResultsDialog *resultsDialog();

    //virtual QGis::UnitType displayUnits();

    //// pointers to the custom actions for identify menu
    //QAction* mAttributeTableAction;

};

#endif // QGIS_DEVMAPTOOLIDENTIFYACTION_H
