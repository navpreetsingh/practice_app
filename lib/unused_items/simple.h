#ifndef SIMPLE_H
#define SIMPLE_H

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <iostream>
#include <QTimer>
#include <fstream>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>                                                              
#include <time.h>  
#include <string.h>
#include <QApplication>
#include <QDateEdit>
#include <QScrollBar>
#include <QDateTime>
#include <vector>
#include <sstream>
#include <algorithm>
#include <QHBoxLayout>
#include <QObjectList>
#include <QDateTime>
#include <mysql++.h>
#include <typeinfo>


#include "qchartviewer.h"
#include "chartdir.h" 

class Simple : public QDialog
{
    Q_OBJECT
public:
    Simple(QWidget *parent = 0);
    ~Simple();

private:
    // TimeStamps, volume, high, low, open and close data
    QString m_tickerKey;
    double *date;
    double *open;
    double *high;
    double *low;
    double *close;
    double *volume;    
    int data_len;       

    // An extra data series to compare with the close data
    QString m_compareKey;
    double *m_compareData;
    int m_compareDataLen;

    // The resolution of the data in seconds. 1 day = 86400 seconds.
    int m_duration;

    // The moving average periods
    int m_avgPeriod1;
    int m_avgPeriod2;

    //The moving average periods of indicators
    int m_indicator1_p1;
    int m_indicator1_p2;
    int m_indicator1_p3;    
    int m_indicator2_p1;
    int m_indicator2_p2;
    int m_indicator2_p3;    
    int m_indicator3_p1;
    int m_indicator3_p2;
    int m_indicator3_p3;
    int m_indicator4_p1;
    int m_indicator4_p2;
    int m_indicator4_p3;
    int m_band_period;
    double m_band_width;
    double m_ParabolicSAR_int;
    double m_ParabolicSAR_inc;
    double m_ParabolicSAR_max;


   
    // The user interface
    QLineEdit *m_TickerSymbol;
    QLineEdit *m_CompareWith;
    QComboBox *m_TimeRange;
    QComboBox *m_ChartSize;
    QCheckBox *m_VolumeBars;
    QCheckBox *m_ParabolicSAR;
    QCheckBox *m_LogScale;
    QCheckBox *m_PercentageScale;
    QComboBox *m_ChartType;
    QComboBox *m_PriceBand;
    QComboBox *m_AvgType1;
    QComboBox *m_AvgType2;
    QLineEdit *m_MovAvg1;
    QLineEdit *m_MovAvg2;
    QComboBox *m_Indicator1;
    QComboBox *m_Indicator2;
    QComboBox *m_Indicator3;
    QComboBox *m_Indicator4;
    QChartViewer *m_ChartViewer;
    QScrollBar *m_HScrollBar;
    QDateEdit *m_StartDate;
    QDateEdit *m_EndDate;
    QLineEdit *m_Indicator1_pd1;
    QLineEdit *m_Indicator1_pd2;
    QLineEdit *m_Indicator1_pd3;
    QLineEdit *m_Indicator2_pd1;
    QLineEdit *m_Indicator2_pd2;
    QLineEdit *m_Indicator2_pd3;
    QLineEdit *m_Indicator3_pd1;
    QLineEdit *m_Indicator3_pd2;
    QLineEdit *m_Indicator3_pd3;
    QLineEdit *m_Indicator4_pd1;
    QLineEdit *m_Indicator4_pd2;
    QLineEdit *m_Indicator4_pd3;
    QLineEdit *m_band_pd;
    QLineEdit *m_band_wd;
    QLineEdit *m_ParabolicSAR_initial;
    QLineEdit *m_ParabolicSAR_increment;
    QLineEdit *m_ParabolicSAR_maximum;


    void drawChart(QChartViewer *viewer);            // Draw chart
	void simple(MultiChart *m, int mouseX);    // Draw Track Cursor
	void initChartViewer(QChartViewer *viewer);     // Initialize the QChartViewer
	void read_data(int durationOfStock);
    void read_data_compare(int durationOfStock);
	void updateControls(QChartViewer *viewer);      // Update other controls
	void updateImageMap(QChartViewer *viewer);      // Update the image map	

private slots:
    void onComboBoxChanged(int);
    void onCheckBoxChanged();
    void onLineEditChanged();
    void onMouseUsageChanged(int mouseUsage);
	void onViewPortChanged();	
	void onMouseMovePlotArea(QMouseEvent *event);
	void onHScrollBarChanged(int value);
	void onMouseWheelChart(QWheelEvent *event);
};

#endif // SIMPLE_H
