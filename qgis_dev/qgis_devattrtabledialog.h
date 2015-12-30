#ifndef QGIS_DEVATTRTABLEDIALOG_H
#define QGIS_DEVATTRTABLEDIALOG_H

#include <QDialog>
#include "ui_qgis_devattrtabledialog.h"

#include <QDockWidget>
#include <QSignalMapper>

#include <qgsattibutedialog.h>
#include <qgsvectorlayer.h>
#include <qgsfieldmodel.h>
#include <qgsrubberband.h>

// ���Ա�Ի���
class qgis_devattrtableDialog : public QDialog
{
    Q_OBJECT

public:
    qgis_devattrtableDialog( QgsVectorLayer* theVecLayer, QWidget *parent = 0 );
    ~qgis_devattrtableDialog();

private:
    Ui::qgis_devattrtableDialog ui;

    // Ū������Щ��Ա�Ǹ�ɶ��
    QMenu* mMenuActions;
    QAction* mActionToggleEditing;
    QDockWidget* mDock;
    QgsDistanceArea* myDa;
    QMenu* mFilterColumnsMenu;
    QSignalMapper* mFilterActionMapper;
    QgsVectorLayer* mLayer;
    QgsFieldModel* mFieldModel;

    QgsRubberBand* mRubberBand;
};

#endif // QGIS_DEVATTRTABLEDIALOG_H
