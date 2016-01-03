#ifndef QGIS_DEVATTRTABLEDIALOG_H
#define QGIS_DEVATTRTABLEDIALOG_H

#include <QDialog>
#include "ui_qgis_devattrtabledialog.h"
#include "ui_qgsattributetabledialog.h"

#include <QDockWidget>
#include <QSignalMapper>

#include <qgsvectorlayer.h>
#include <qgsfieldmodel.h>
#include <qgsrubberband.h>

// ���Ա�Ի���
class qgis_devattrtableDialog : public QDialog, public Ui::QgsAttributeTableDialog
{
    Q_OBJECT

public:
    qgis_devattrtableDialog( QgsVectorLayer* theVecLayer, QWidget *parent = 0, Qt::WindowFlags flags = Qt::Window );
    ~qgis_devattrtableDialog();

    /**
    * Sets the filter expression to filter visible features
    * @param filterString filter query string. QgsExpression compatible.
    */
    void setFilterExpression( QString filterString );

public slots:
    void editingToggled(); // Toggles editing mode

private slots:
    void updateTitle(); // update title of this window

    void filterExpressionBuilder();
    void filterShowAll();
    void filterSelected();
    void filterVisible();
    void filterEdited();
    void filterColumnChanged( QObject* filterAction );
    void filterQueryChanged( const QString& query );
    void filterQueryAccepted();

    void columnBoxInit(); // �ֶ����Ƴ�ʼ���ֶ���ʾ�б�

    // ִ���ֶμ�����
    void runFieldCalculation( QgsVectorLayer* layer, QString fieldName,
                              QString expression, QgsFeatureIds filteredIds = QgsFeatureIds() );
    void updateFieldFromExpression(); // �����û������Expression�����ֶ�
    void updateFieldFromExpressionSelected(); // �����û�ѡ���feature�������Expression�����ֶ�
    void updateButtonStatus( QString fieldName, bool isValid );
private:

    QMenu* mMenuActions; // �˵�����
    QAction* mActionToggleEditing; // �༭����

    QDockWidget* mDock; // ʹ�����Ա��ڿ�Dock
    QgsDistanceArea* myDa;

    QMenu* mFilterColumnsMenu;
    QSignalMapper* mFilterActionMapper;

    QgsVectorLayer* mLayer; // ��ʾ��ͼ���������Ϣ
    QgsFieldModel* mFieldModel; // �ֶ�����ģ��

    QgsRubberBand* mRubberBand;
};

#endif // QGIS_DEVATTRTABLEDIALOG_H
