#include <iostream>
#include <fstream>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>                                                              
#include <time.h>  
#include <string.h>
#include <QApplication>
#include <vector>
#include <sstream>
#include <algorithm>
#include <QHBoxLayout>
#include <QObjectList>
#include <QDateTime>
#include <QPushButton>
#include <QButtonGroup>
#include <QMouseEvent>
#include <mysql++.h>
#include <typeinfo>
#include "chartdir.h"
#include "FinanceChart.h"
#include "simple.h"

using namespace std;
using namespace mysqlpp;

int main(int argc, char *argv[])
{    
    cout << "main \n";
    QApplication app(argc, argv);
    Simple demo;
    demo.show();
    return app.exec();
}

Simple::Simple(QWidget *parent) :
        QDialog(parent, Qt::Window)
{
    cout << "simple(Qwidget *parent) \n";    
        
    //
    // Set up the GUI
    //

    m_duration = 1;
    m_compareDataLen = 0;

    setWindowTitle("Technical Analysis");
    resize(954, 520);
    QFont defaultFont("Arial");
    defaultFont.setPixelSize(11);
    setFont(defaultFont);
    setPalette(QPalette(QColor(0, 0, 0xff)));

    // Use horizontal layout to put the tree view on the left and the charts on the right
    QHBoxLayout *horizontalLayout = new QHBoxLayout(this);
    horizontalLayout->setMargin(0);
    horizontalLayout->setSpacing(0);

    // The frame on the left side
    QWidget *leftPanel = new QWidget(this);
    leftPanel->setAutoFillBackground(true);
    leftPanel->setPalette(QPalette(QColor(186, 221, 255)));
    leftPanel->setFixedWidth(156);
    horizontalLayout->addWidget(leftPanel);

    QFrame *separator = new QFrame(this);
    separator->setFrameStyle(QFrame::VLine | QFrame::Plain);
    horizontalLayout->addWidget(separator);

    QWidget *rightPanel = new QWidget(this);
    horizontalLayout->addWidget(rightPanel);
    rightPanel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // Ticker Symbol
    (new QLabel("Ticker Symbol", leftPanel))->setGeometry(8, 10, 140, 18);
    m_TickerSymbol = new QLineEdit("ABAN", leftPanel);
    m_TickerSymbol->setGeometry(8, 26, 140, 20);

    // Compare With
    (new QLabel("Compare With", leftPanel))->setGeometry(8, 52, 140, 18);
    m_CompareWith = new QLineEdit(leftPanel);
    m_CompareWith->setGeometry(8, 68, 140, 20);

    // Time Period
    (new QLabel("Time Period", leftPanel))->setGeometry(8, 94, 140, 18);
    m_TimeRange = new QComboBox(leftPanel);
    m_TimeRange->setGeometry(8, 110, 140, 20);

    // Chart Size
    (new QLabel("Chart Size", leftPanel))->setGeometry(8, 136, 140, 18);
    m_ChartSize = new QComboBox(leftPanel);
    m_ChartSize->setGeometry(8, 152, 140, 20);

    // Value bars/Log Scale/Grid Lines
    m_VolumeBars = new QCheckBox("Show Volume Bars", leftPanel);
    m_ParabolicSAR = new QCheckBox("Parabolic SAR", leftPanel);
    m_ParabolicSAR_initial = new QLineEdit("0.02", leftPanel);
    m_ParabolicSAR_increment = new QLineEdit("0.02", leftPanel);
    m_ParabolicSAR_maximum = new QLineEdit("0.2", leftPanel);
    m_LogScale = new QCheckBox("Log Scale", leftPanel);
    m_PercentageScale = new QCheckBox("Percentage Grid", leftPanel);
    m_VolumeBars->setChecked(true);
    m_VolumeBars->setGeometry(8, 176, 140, 20);
    m_ParabolicSAR->setGeometry(8, 196, 140, 20);
    m_ParabolicSAR_initial->setGeometry(8, 216, 35, 20);
    m_ParabolicSAR_increment->setGeometry(58, 216, 35, 20);
    m_ParabolicSAR_maximum->setGeometry(108, 216, 35, 20);
    m_LogScale->setGeometry(8, 236, 140, 20);
    m_PercentageScale->setGeometry(8, 256, 140, 20);

    // Chart Type
    (new QLabel("Chart Type", leftPanel))->setGeometry(8, 282, 140, 18);
    m_ChartType = new QComboBox(leftPanel);
    m_ChartType->setGeometry(8, 298, 140, 20);

    // Price Bands
    (new QLabel("Price Bands", leftPanel))->setGeometry(8, 324, 140, 18);
    m_PriceBand = new QComboBox(leftPanel);
    m_band_pd = new QLineEdit("20", leftPanel);
    m_band_wd =  new QLineEdit("2", leftPanel);
    m_PriceBand->setGeometry(8, 340, 140, 20);
    m_band_pd->setGeometry(8, 360, 35, 20);
    m_band_wd->setGeometry(58,360,35,20);

    // Moving Averages
    (new QLabel("Moving Averages", leftPanel))->setGeometry(8, 400, 140, 18);
    m_AvgType1 = new QComboBox(leftPanel);
    m_AvgType2 = new QComboBox(leftPanel);
    m_MovAvg1 = new QLineEdit("10", leftPanel);
    m_MovAvg2 = new QLineEdit("25", leftPanel);
    m_AvgType1->setGeometry(8, 420, 105, 20);
    m_AvgType2->setGeometry(8, 440, 105, 20);
    m_MovAvg1->setGeometry(113, 420, 35, 20);
    m_MovAvg2->setGeometry(113, 440, 35, 20);

    // Technical Indicators
    (new QLabel("Technical Indicators", leftPanel))->setGeometry(8, 475, 140, 18);
    m_Indicator1 = new QComboBox(leftPanel);    
    m_Indicator2 = new QComboBox(leftPanel);
    m_Indicator3 = new QComboBox(leftPanel);
    m_Indicator4 = new QComboBox(leftPanel);
    m_Indicator1->setGeometry(8, 490, 140, 20);
    m_Indicator2->setGeometry(8, 540, 140, 20);
    m_Indicator3->setGeometry(8, 590, 140, 20);
    m_Indicator4->setGeometry(8, 640, 140, 20);

    //Technical Indicators periods
    m_Indicator1_pd1 = new QLineEdit("26", leftPanel);
    m_Indicator1_pd2 = new QLineEdit("12", leftPanel);
    m_Indicator1_pd3 = new QLineEdit("9", leftPanel);
    m_Indicator2_pd1 = new QLineEdit("26", leftPanel);
    m_Indicator2_pd2 = new QLineEdit("12", leftPanel);
    m_Indicator2_pd3 = new QLineEdit("9", leftPanel);
    m_Indicator3_pd1 = new QLineEdit("26", leftPanel);
    m_Indicator3_pd2 = new QLineEdit("12", leftPanel);
    m_Indicator3_pd3 = new QLineEdit("9", leftPanel);
    m_Indicator4_pd1 = new QLineEdit("26", leftPanel);
    m_Indicator4_pd2 = new QLineEdit("12", leftPanel);
    m_Indicator4_pd3 = new QLineEdit("9", leftPanel);

    m_Indicator1_pd1->setGeometry(8, 510,35,20);
    m_Indicator1_pd2->setGeometry(58,510,35,20);
    m_Indicator1_pd3->setGeometry(108,510,35,20);
    m_Indicator2_pd1->setGeometry(8,560,35,20);
    m_Indicator2_pd2->setGeometry(58,560,35,20);
    m_Indicator2_pd3->setGeometry(108,560,35,20);
    m_Indicator3_pd1->setGeometry(8,610,35,20);
    m_Indicator3_pd2->setGeometry(58,610,35,20);
    m_Indicator3_pd3->setGeometry(108,610,35,20);
    m_Indicator4_pd1->setGeometry(8,660,35,20);
    m_Indicator4_pd2->setGeometry(58,660,35,20);
    m_Indicator4_pd3->setGeometry(108,660,35,20);

    QFont labelFont(defaultFont);
    labelFont.setBold(true);
    const QObjectList& children = leftPanel->children();
    for (int i = 0; i < children.size(); ++i)
    {
        QLabel *label = qobject_cast<QLabel *>(children[i]);
        if (0 != label)
            label->setFont(labelFont);
    }

    // Chart Viewer
    m_ChartViewer = new QChartViewer(rightPanel);
    m_ChartViewer->move(8, 12);

    //
    // Fill the contents of the combo boxes
    //

    /*m_TimeRange->addItem("1 minute", 1);
    m_TimeRange->addItem("2 minutes ", 1);
    m_TimeRange->addItem("5 minutes", 1);
    m_TimeRange->addItem("15 minutes", 1);
    m_TimeRange->addItem("30 minutes", 1);
    m_TimeRange->addItem("1 hour", 1);
    m_TimeRange->addItem("2 hours", 1);*/
    
    m_TimeRange->addItem("1 day", 1);
    m_TimeRange->addItem("1 week", 7);
    m_TimeRange->addItem("1 month", 30);
    m_TimeRange->setCurrentIndex(0);

    m_ChartSize->addItem("Small", "S");
    m_ChartSize->addItem("Medium", "M");
    m_ChartSize->addItem("Large", "L");
    m_ChartSize->addItem("Huge", "H");
    m_ChartSize->setCurrentIndex(2);

    m_ChartType->addItem("None", "None");
    m_ChartType->addItem("CandleStick", "CandleStick");
    m_ChartType->addItem("Closing Price", "Close");
    m_ChartType->addItem("Median Price", "Median");
    m_ChartType->addItem("OHLC", "OHLC");
    m_ChartType->addItem("Typical Price", "TP");
    m_ChartType->addItem("Weighted Close", "WC");
    m_ChartType->setCurrentIndex(1);

    m_PriceBand->addItem("None", "None");
    m_PriceBand->addItem("Bollinger Band", "BB");
    m_PriceBand->addItem("Donchain Channel", "DC");
    m_PriceBand->addItem("Envelop (SMA 20 +/- 10%)", "Envelop");
    m_PriceBand->setCurrentIndex(1);

    m_AvgType1->addItem("None", "None");
    m_AvgType1->addItem("Simple", "SMA");
    m_AvgType1->addItem("Exponential", "EMA");
    m_AvgType1->addItem("Triangular", "TMA");
    m_AvgType1->addItem("Weighted", "WMA");
    m_AvgType1->setCurrentIndex(1);

    for (int i = 0; i < m_AvgType1->count(); ++i)
        m_AvgType2->addItem(m_AvgType1->itemText(i), m_AvgType1->itemData(i));
    m_AvgType2->setCurrentIndex(1);

    //m_Indicator1->addItem("None", "None");
    m_Indicator1->addItem("Accumulation/Distribution", "AccDist");
    m_Indicator1->addItem("Aroon Oscillator", "AroonOsc");
    m_Indicator1->addItem("Aroon Up/Down", "Aroon");
    m_Indicator1->addItem("Avg Directional Index", "ADX");
    m_Indicator1->addItem("Avg True Range", "ATR");
    m_Indicator1->addItem("Bollinger Band Width", "BBW");
    m_Indicator1->addItem("Chaikin Money Flow", "CMF");
    m_Indicator1->addItem("Chaikin Oscillator", "COscillator");
    m_Indicator1->addItem("Chaikin Volatility", "CVolatility");
    m_Indicator1->addItem("Close Location Value", "CLV");
    m_Indicator1->addItem("Commodity Channel Index", "CCI");
    m_Indicator1->addItem("Detrended Price Osc", "DPO");
    m_Indicator1->addItem("Donchian Channel Width", "DCW");
    m_Indicator1->addItem("Ease of Movement", "EMV");
    m_Indicator1->addItem("Fast Stochastic", "FStoch");
    m_Indicator1->addItem("MACD", "MACD");
    m_Indicator1->addItem("Mass Index", "MDX");
    m_Indicator1->addItem("Momentum", "Momentum");
    m_Indicator1->addItem("Money Flow Index", "MFI");
    m_Indicator1->addItem("Neg Volume Index", "NVI");
    m_Indicator1->addItem("On Balance Volume", "OBV");
    m_Indicator1->addItem("Performance", "Performance");
    m_Indicator1->addItem("% Price Oscillator", "PPO");
    m_Indicator1->addItem("% Volume Oscillator", "PVO");
    m_Indicator1->addItem("Pos Volume Index", "PVI");
    m_Indicator1->addItem("Price Volume Trend", "PVT");
    m_Indicator1->addItem("Rate of Change", "ROC");
    m_Indicator1->addItem("RSI", "RSI");
    m_Indicator1->addItem("Slow Stochastic", "SStoch");
    m_Indicator1->addItem("StochRSI", "StochRSI");
    m_Indicator1->addItem("TRIX", "TRIX");
    m_Indicator1->addItem("Ultimate Oscillator", "UO");
    m_Indicator1->addItem("Volume", "Vol");
    m_Indicator1->addItem("William's %R", "WilliamR");

    for (int i = 0; i < m_Indicator1->count(); ++i)
    {
        m_Indicator2->addItem(m_Indicator1->itemText(i), m_Indicator1->itemData(i));
        m_Indicator3->addItem(m_Indicator1->itemText(i), m_Indicator1->itemData(i));
        m_Indicator4->addItem(m_Indicator1->itemText(i), m_Indicator1->itemData(i));
    }

    m_Indicator1->setCurrentIndex(m_Indicator1->findData("RSI"));
    m_Indicator2->setCurrentIndex(m_Indicator2->findData("MACD"));
    m_Indicator3->setCurrentIndex(m_Indicator2->findData("CCI"));
    m_Indicator4->setCurrentIndex(m_Indicator2->findData("StochRSI"));
    
    // Horizontal scroll bar
    m_HScrollBar = new QScrollBar(Qt::Horizontal, this);
    m_HScrollBar->setGeometry(165, 728, 1135, 17);
    connect(m_HScrollBar, SIGNAL(valueChanged(int)), SLOT(onHScrollBarChanged(int)));
    
    // Pointer push button
    QPushButton *pointerPB = new QPushButton(QIcon(":/pointer.png"), "Pointer");
    
    // The Pointer/Zoom In/Zoom Out buttons form a button group
    QButtonGroup *mouseUsage = new QButtonGroup(separator);
    mouseUsage->addButton(pointerPB, Chart::MouseUsageScroll);
    connect(mouseUsage, SIGNAL(buttonPressed(int)), SLOT(onMouseUsageChanged(int)));
    
    // Use the same action hanlder for all controls
    const QObjectList &allControls = leftPanel->children();
    for (int i = 0; i < allControls.count(); ++i)
    {
        QObject *obj;
        if ((obj = qobject_cast<QComboBox *>(allControls[i])) != 0)
            connect(obj, SIGNAL(currentIndexChanged(int)), SLOT(onComboBoxChanged(int)));
        else if ((obj = qobject_cast<QCheckBox *>(allControls[i])) != 0)
            connect(obj, SIGNAL(clicked()), SLOT(onCheckBoxChanged()));
        else if ((obj = qobject_cast<QLineEdit *>(allControls[i])) != 0)
            connect(obj, SIGNAL(editingFinished()), SLOT(onLineEditChanged()));
        else if ((obj = qobject_cast<QScrollBar *>(allControls[i])) != 0)
            connect(obj, SIGNAL(valueChanged(int)), SLOT(onHScrollBarChanged(int)));
        else if ((obj = qobject_cast<QButtonGroup *>(allControls[i])) != 0)
            connect(obj, SIGNAL(buttonPressed(int)), SLOT(onMouseUsageChanged(int)));
    }
    connect(m_ChartViewer, SIGNAL(viewPortChanged()), SLOT(onViewPortChanged()));
    connect(m_ChartViewer, SIGNAL(mouseMovePlotArea(QMouseEvent*)), SLOT(onMouseMovePlotArea(QMouseEvent*)));
    connect(m_ChartViewer, SIGNAL(mouseWheel(QWheelEvent*)), SLOT(onMouseWheelChart(QWheelEvent*)));
    
    // Load the data    
    read_data(m_duration);   

    // Update the chart
    drawChart(m_ChartViewer);
        
    // Initialize the QChartViewer
    initChartViewer(m_ChartViewer);

    // Initially set the mouse to drag to scroll mode
    pointerPB->click();

    // Trigger the ViewPortChanged event to draw the chart
    m_ChartViewer->updateViewPort(true, true);
}

//
// Destructor
//
Simple::~Simple()
{
    cout << "~simple \n";
    delete m_ChartViewer->getChart();
}

//
// Initialize the QChartViewer
//
void Simple::initChartViewer(QChartViewer *viewer)
{
    cout << "InitChartViewer \n";
    // Set the full x range to be the duration of the data
    viewer->setFullRange("x", date[0], date[data_len]);

    // Initialize the view port to show the latest 20% of the time range
    viewer->setViewPortWidth(0.04);
    viewer->setViewPortLeft(1 - viewer->getViewPortWidth());

    // Set the maximum zoom to 10 points
    viewer->setZoomInWidthLimit(50.0 / data_len);
}

void Simple::onCheckBoxChanged()
{
    cout << "onCheckBoxChanged \n";    
    drawChart(m_ChartViewer);
}

void Simple::onComboBoxChanged(int)
{
    cout << "onComboBoxChanged \n";
    // The duration selected by the user
    int duration = m_TimeRange->itemData(m_TimeRange->currentIndex()).toInt();
    cout << "Duration: " << duration << "\n";
    if(duration != m_duration)
    {
        read_data(duration);
        if(m_compareDataLen != 0)
            read_data_compare(duration);
        m_duration = duration;
    }
    drawChart(m_ChartViewer);
}

void Simple::onLineEditChanged()
{
    cout << "onLineEditChanged \n";

    /*int new_avgPeriod1 = m_MovAvg1->text().toInt();
    int new_avgPeriod2 = m_MovAvg2->text().toInt();*/

    QString tickerKey = m_TickerSymbol->text();
    
    QString compareKey = m_CompareWith->text();

    if (m_tickerKey != tickerKey)
    {
        m_tickerKey = m_TickerSymbol->text();
        read_data(m_duration);
    } 
        

    if (m_compareKey != compareKey)
    {
        m_compareKey =  m_CompareWith ->text();
        read_data_compare(m_duration);
    }
       

    /*if ((new_avgPeriod1 != m_avgPeriod1) || (new_avgPeriod2 != m_avgPeriod2) ||
        (m_tickerKey != tickerKey) || (m_compareKey != compareKey))
    { */      
    drawChart(m_ChartViewer);
    //}
        
}

//
// User clicks on the the horizontal scroll bar
//
void Simple::onHScrollBarChanged(int value)
{
    cout << "onHScrollBarChanged \n";
    
    if (!m_ChartViewer->isInViewPortChangedEvent())
    {        
        // Set the view port based on the scroll bar
        int scrollBarLen = m_HScrollBar->maximum() + m_HScrollBar->pageStep();
        m_ChartViewer->setViewPortLeft(value / (double)scrollBarLen);

        // Update the chart display without updating the image maps. (We can delay updating
        // the image map until scrolling is completed and the chart display is stable.)
        m_ChartViewer->updateViewPort(true, false);
    }
}

//
// Update controls in the user interface when the view port changed
//
void Simple::updateControls(QChartViewer *viewer)
{
    cout << "updateControls \n";
    // The logical length of the scrollbar. It can be any large value. The actual value does
    // not matter.
    const int scrollBarLen = 1000000000;

    // Update the horizontal scroll bar
    m_HScrollBar->setEnabled(viewer->getViewPortWidth() < 1);
    m_HScrollBar->setPageStep((int)ceil(viewer->getViewPortWidth() * scrollBarLen));
    m_HScrollBar->setSingleStep(min(scrollBarLen / 100, m_HScrollBar->pageStep()));
    m_HScrollBar->setRange(0, scrollBarLen - m_HScrollBar->pageStep());
    m_HScrollBar->setValue((int)(0.5 + viewer->getViewPortLeft() * scrollBarLen));

}

//
// Draw track cursor when mouse is moving over plotarea
//
void Simple::onMouseMovePlotArea(QMouseEvent *)
{
    cout << "onMouseMovePlotArea \n";
    simple((MultiChart *)m_ChartViewer->getChart(), m_ChartViewer->getPlotAreaMouseX());
    m_ChartViewer->updateDisplay();
}

//
// The Pointer, Zoom In or Zoom out button is pressed
//
void Simple::onMouseUsageChanged(int mouseUsage)
{
    cout << "onMouseUsageChanged \n";
    m_ChartViewer->setMouseUsage(mouseUsage);
}

//
// The ViewPortChanged event handler. This event occurs if the user scrolls or zooms in
// or out the chart by dragging or clicking on the chart. It can also be triggered by
// calling WinChartViewer.updateViewPort.
//

void Simple::onViewPortChanged()
{
    cout << "onViewPortChanged \n";
	updateControls(m_ChartViewer);
    
    if (m_ChartViewer->needUpdateChart())
        drawChart(m_ChartViewer);
    // We need to update the track line too. If the mouse is moving on the chart (eg. if
    // the user drags the mouse on the chart to scroll it), the track line will be updated
    // in the MouseMovePlotArea event. Otherwise, we need to update the track line here.
    if (m_ChartViewer->needUpdateImageMap())
        updateImageMap(m_ChartViewer);
}

void Simple::updateImageMap(QChartViewer *viewer)
{
    cout << "updateImageMap \n";
    // Include tool tip for the chart
    if (0 == viewer->getImageMapHandler())
    {
        viewer->setImageMap(viewer->getChart()->getHTMLImageMap("", "",
            "title='[{dataSetName}] {x|mmm dd, yyyy}: USD {value|2}'"));
    }
}

//
// When the mouse enters the chart, we will generate an image map for hot spots and tooltips
// support if it has not already been generated.
//
void Simple::onMouseWheelChart(QWheelEvent *event)
{
    cout << "onMouseWheelChart \n";
    // Process the mouse wheel only if the mouse is over the plot area
    if (!m_ChartViewer->isMouseOnPlotArea())
    {
        event->ignore();
        return;
    }

    // We zoom in or out by 10% depending on the mouse wheel direction.
    double newVpWidth = m_ChartViewer->getViewPortWidth() * (event->delta() > 0 ? 0.9 : 1 / 0.9);
    double newVpHeight = m_ChartViewer->getViewPortHeight() * (event->delta() > 0 ? 0.9 : 1 / 0.9);

    // We do not zoom beyond the zoom width or height limits.
    newVpWidth = max(m_ChartViewer->getZoomInWidthLimit(), min(newVpWidth,
        m_ChartViewer->getZoomOutWidthLimit()));
    newVpHeight = max(m_ChartViewer->getZoomInHeightLimit(), min(newVpWidth,
        m_ChartViewer->getZoomOutHeightLimit()));

    if ((newVpWidth != m_ChartViewer->getViewPortWidth()) ||
            (newVpHeight != m_ChartViewer->getViewPortHeight()))
    {
        // Set the view port position and size so that the point under the mouse remains under
        // the mouse after zooming.

        double deltaX = (m_ChartViewer->getPlotAreaMouseX() - m_ChartViewer->getPlotAreaLeft()) *
            (m_ChartViewer->getViewPortWidth() - newVpWidth) / m_ChartViewer->getPlotAreaWidth();
        m_ChartViewer->setViewPortLeft(m_ChartViewer->getViewPortLeft() + deltaX);
        m_ChartViewer->setViewPortWidth(newVpWidth);

        double deltaY = (m_ChartViewer->getPlotAreaMouseY() - m_ChartViewer->getPlotAreaTop()) *
            (m_ChartViewer->getViewPortHeight() - newVpHeight) / m_ChartViewer->getPlotAreaHeight();
        m_ChartViewer->setViewPortTop(m_ChartViewer->getViewPortTop() + deltaY);
        m_ChartViewer->setViewPortHeight(newVpHeight);

        m_ChartViewer->updateViewPort(true, false);
    }
}


//
// Draw finance chart track line with legend
//
void Simple::simple(MultiChart *m, int mouseX)
{
    cout << "simple: \n";
    
    // Clear the current dynamic layer and get the DrawArea object to draw on it.
    DrawArea *d = m->initDynamicLayer();

    // It is possible for a FinanceChart to be empty, so we need to check for it.
    if (m->getChartCount() == 0)
        return ;

    // Get the data x-value that is nearest to the mouse
    int xValue = (int)(((XYChart *)m->getChart(0))->getNearestXValue(mouseX));//layer

    // Iterate the XY charts (main price chart and indicator charts) in the FinanceChart
    XYChart *c = 0;
    for(int i = 0; i < m->getChartCount(); ++i) 
    {       
        c = (XYChart *)m->getChart(i);   

        // Variables to hold the legend entries
        ostringstream ohlcLegend;
        vector<string> legendEntries;

        // Iterate through all layers to find the highest data point
        for(int j = 0; j < c->getLayerCount(); ++j) 
        {
            Layer *layer = c->getLayerByZ(j);            
            int xIndex = layer->getXIndexOf(xValue);            
            int dataSetCount = layer->getDataSetCount();            

            // In a FinanceChart, only layers showing OHLC data can have 4 data sets
            if (dataSetCount == 4) 
            {
                double highValue = layer->getDataSet(0)->getValue(xIndex);                
                double lowValue = layer->getDataSet(1)->getValue(xIndex);                
                double openValue = layer->getDataSet(2)->getValue(xIndex);                
                double closeValue = layer->getDataSet(3)->getValue(xIndex);
                			
                if (closeValue != Chart::NoValue) 
                {
                    // Build the OHLC legend
					ohlcLegend << "      <*block*>";
					ohlcLegend << "Open: " << c->formatValue(openValue, "{value|P4}");
					ohlcLegend << ", High: " << c->formatValue(highValue, "{value|P4}"); 
					ohlcLegend << ", Low: " << c->formatValue(lowValue, "{value|P4}"); 
					ohlcLegend << ", Close: " << c->formatValue(closeValue, "{value|P4}");

                    // We also draw an upward or downward triangle for up and down days and the %
                    // change
                    double lastCloseValue = layer->getDataSet(3)->getValue(xIndex - 1);                    
                    if (lastCloseValue != Chart::NoValue) 
                    {
                        double change = closeValue - lastCloseValue;
                        string symbol = (change >= 0) ?
                            "<*font,color=008800*><*img=@triangle,width=8,color=008800*>" :
                            "<*font,color=CC0000*><*img=@invertedtriangle,width=8,color=CC0000*>";

                        ohlcLegend << "  " << symbol << " " << c->formatValue(change, "{value|P4}");
                    }
					ohlcLegend << "<*/*>";
                }
            } 
            else 
            {
                // Iterate through all the data sets in the layer
                for(int k = 0; k < layer->getDataSetCount(); ++k)
                {
                    DataSet *dataSet = layer->getDataSetByZ(k);
                    string name = dataSet->getDataName();
                    double value = dataSet->getValue(xIndex);
                    if ((0 != name.size()) && (value != Chart::NoValue)) 
                    {
                        // In a FinanceChart, the data set name consists of the indicator name and its
                        // latest value. It is like "Vol: 123M" or "RSI (14): 55.34". As we are
                        // generating the values dynamically, we need to extract the indictor name
                        // out, and also the volume unit (if any).

						// The volume unit
						string unitChar;						

                        // The indicator name is the part of the name up to the colon character.
						int delimiterPosition = (int)name.find(':');
                        if ((int)name.npos != delimiterPosition) 
                        {							
							// The unit, if any, is the trailing non-digit character(s).
							int lastDigitPos = (int)name.find_last_of("0123456789");							
                            if (((int)name.npos != lastDigitPos) && (lastDigitPos + 1 < (int)name.size())
                                && (lastDigitPos > delimiterPosition))
								unitChar = name.substr(lastDigitPos + 1);
							name.resize(delimiterPosition);
                        }

                        // In a FinanceChart, if there are two data sets, it must be representing a
                        // range.
                        if (dataSetCount == 2) 
                        {
                            // We show both values in the range in a single legend entry
                            value = layer->getDataSet(0)->getValue(xIndex);                            
                            double value2 = layer->getDataSet(1)->getValue(xIndex);                            
                            name = name + ": " + c->formatValue(min(value, value2), "{value|P3}");
							name = name + " - " + c->formatValue(max(value, value2), "{value|P3}");
                        } 
                        else 
                        {
                            // In a FinanceChart, only the layer for volume bars has 3 data sets for
                            // up/down/flat days
                            if (dataSetCount == 3) 
                            {
                                // The actual volume is the sum of the 3 data sets.
                                value = layer->getDataSet(0)->getValue(xIndex) + layer->getDataSet(1
                                    )->getValue(xIndex) + layer->getDataSet(2)->getValue(xIndex);
                                }

                            // Create the legend entry
                            name = name + ": " + c->formatValue(value, "{value|P3}") + unitChar;                            
                        }

                        // Build the legend entry, consist of a colored square box and the name (with
                        // the data value in it).
						ostringstream legendEntry;
						legendEntry << "<*block*><*img=@square,width=8,edgeColor=000000,color=" 
							<< hex << dataSet->getDataColor() << "*> " << name << "<*/*>";						
                        legendEntries.push_back(legendEntry.str());
                    }
                }
            }
        }

        // Get the plot area position relative to the entire FinanceChart
        PlotArea *plotArea = c->getPlotArea();
        //cout << "plot area: " << plotArea << "\n";
        int plotAreaLeftX = plotArea->getLeftX() + c->getAbsOffsetX();
        //cout << "plot area leftx: " << plotAreaLeftX << "\n";
        int plotAreaTopY = plotArea->getTopY() + c->getAbsOffsetY();
        //cout << "plot area topy: " << plotAreaTopY << "\n";

		// The legend begins with the date label, then the ohlcLegend (if any), and then the
		// entries for the indicators.
		ostringstream legendText;
		legendText << "<*block,valign=top,maxWidth=" << (plotArea->getWidth() - 5) 
			<< "*><*font=arialbd.ttf*>[" << c->xAxis()->getFormattedLabel(xValue, "mmm dd, yyyy")
			<< "]<*/font*>" << ohlcLegend.str();
		for (int i = ((int)legendEntries.size()) - 1; i >= 0; --i)
		{
			legendText << "      " << legendEntries[i];
			//cout << "legend text: " << legendText << "\n";
		}
		legendText << "<*/*>";

        // Draw a vertical track line at the x-position
        d->vline(plotAreaTopY, plotAreaTopY + plotArea->getHeight(), c->getXCoor(xValue) +
            c->getAbsOffsetX(), d->dashLineColor(0x000000, 0x0101));

        // Display the legend on the top of the plot area
        TTFText *t = d->text(legendText.str().c_str(), "arial.ttf", 8);
        t->draw(plotAreaLeftX + 5, plotAreaTopY + 3, 0x000000, Chart::TopLeft);
		t->destroy();
    }
}

/////////////////////////////////////////////////////////////////////////////
// Chart Creation

/// <summary>
/// Add a moving average line to the FinanceChart object.
/// </summary>
/// <param name="m">The FinanceChart object to add the line to.</param>
/// <param name="avgType">The moving average type (SMA/EMA/TMA/WMA).</param>
/// <param name="avgPeriod">The moving average period.</param>
/// <param name="color">The color of the line.</param>
static LineLayer* addMovingAvg(FinanceChart *m, QString avgType, int avgPeriod, int color)
{
    cout << "addMovingAvg \n";
    if (avgPeriod > 1)
    {
        if (avgType == "SMA")
            return m->addSimpleMovingAvg(avgPeriod, color);
        else if (avgType == "EMA")
            return m->addExpMovingAvg(avgPeriod, color);
        else if (avgType == "TMA")
            return m->addTriMovingAvg(avgPeriod, color);
        else if (avgType == "WMA")
            return m->addWeightedMovingAvg(avgPeriod, color);
    }

    return 0;
}

/// <summary>
/// Add an indicator chart to the FinanceChart object. In this demo example, the indicator
/// parameters (such as the period used to compute RSI, colors of the lines, etc.) are hard
/// coded to commonly used values. You are welcome to design a more complex user interface
/// to allow users to set the parameters.
/// </summary>
/// <param name="m">The FinanceChart object to add the line to.</param>
/// <param name="indicator">The selected indicator.</param>
/// <param name="height">Height of the chart in pixels</param>
static XYChart* addIndicator(FinanceChart *m, QString indicator, int height,
    int p1, int p2, int p3)
{
    cout << "addIndicator \n";
    if (indicator == "RSI")
        return m->addRSI(height, p1, 0x800080, p2, 0xff6666, 0x6666ff);
    else if (indicator == "StochRSI")
        return m->addStochRSI(height, p1, 0x800080, p2, 0xff6666, 0x6666ff);
    else if (indicator == "MACD")
        return m->addMACD(height, p1, p2, p3, 0xff, 0xff00ff, 0x8000);
    else if (indicator == "FStoch")
        return m->addFastStochastic(height, p1, p2, 0x6060, 0x606000);
    else if (indicator == "SStoch")
        return m->addSlowStochastic(height, p1, p2, 0x6060, 0x606000);
    else if (indicator == "ATR")
        return m->addATR(height, p1, 0x808080, 0xff);
    else if (indicator == "ADX")
        return m->addADX(height, p1, 0x8000, 0x800000, 0x80);
    else if (indicator == "DCW")
        return m->addDonchianWidth(height, p1, 0xff);
    else if (indicator == "BBW")
        return m->addBollingerWidth(height, p1, p2, 0xff);
    else if (indicator == "DPO")
        return m->addDPO(height, p1, 0xff);
    else if (indicator == "PVT")
        return m->addPVT(height, 0xff);
    else if (indicator == "Momentum")
        return m->addMomentum(height, p1, 0xff);
    else if (indicator == "Performance")
        return m->addPerformance(height, 0xff);
    else if (indicator == "ROC")
        return m->addROC(height, p1, 0xff);
    else if (indicator == "OBV")
        return m->addOBV(height, 0xff);
    else if (indicator == "AccDist")
        return m->addAccDist(height, 0xff);
    else if (indicator == "CLV")
        return m->addCLV(height, 0xff);
    else if (indicator == "WilliamR")
        return m->addWilliamR(height, p1, 0x800080, p2, 0xff6666, 0x6666ff);
    else if (indicator == "Aroon")
        return m->addAroon(height, p1, 0x339933, 0x333399);
    else if (indicator == "AroonOsc")
        return m->addAroonOsc(height, p1, 0xff);
    else if (indicator == "CCI")
        return m->addCCI(height, p1, 0x800080, p2, 0xff6666, 0x6666ff);
    else if (indicator == "EMV")
        return m->addEaseOfMovement(height, p1, 0x6060, 0x606000);
    else if (indicator == "MDX")
        return m->addMassIndex(height, 0x800080, 0xff6666, 0x6666ff);
    else if (indicator == "CVolatility")
        return m->addChaikinVolatility(height, p1, p2, 0xff);
    else if (indicator == "COscillator")
        return m->addChaikinOscillator(height, 0xff);
    else if (indicator == "CMF")
        return m->addChaikinMoneyFlow(height, p1, 0x8000);
    else if (indicator == "NVI")
        return m->addNVI(height, p1, 0xff, 0x883333);
    else if (indicator == "PVI")
        return m->addPVI(height, p1, 0xff, 0x883333);
    else if (indicator == "MFI")
        return m->addMFI(height, p1, 0x800080, p2, 0xff6666, 0x6666ff);
    else if (indicator == "PVO")
        return m->addPVO(height, p1, p2, p3, 0xff, 0xff00ff, 0x8000);
    else if (indicator == "PPO")
        return m->addPPO(height, p1, p2, p3, 0xff, 0xff00ff, 0x8000);
    else if (indicator == "UO")
        return m->addUltimateOscillator(height, p1, p2, p3, 0x800080, 20, 0xff6666, 0x6666ff);
    else if (indicator == "Vol")
        return m->addVolIndicator(height, 0x99ff99, 0xff9999, 0xc0c0c0);
    else if (indicator == "TRIX")
        return m->addTRIX(height, p1, 0xff);
    else
        return 0;
}

/// <summary>
/// Draw the chart according to user selection and display it in the ChartViewer.
/// </summary>
/// <param name="viewer">The ChartViewer object to display the chart.</param>
void Simple::drawChart(QChartViewer *viewer)
{
   cout << "drawChart \n";
   delete m_ChartViewer->getChart();
       
    // Get the start date and end date that are visible on the chart.
    double viewPortStartDate = viewer->getValueAtViewPort("x", viewer->getViewPortLeft());
    double viewPortEndDate = viewer->getValueAtViewPort("x", viewer->getViewPortLeft() +
        viewer->getViewPortWidth());

    // Get the array indexes that corresponds to the visible start and end dates
    int startIndex = (int)floor(Chart::bSearch(DoubleArray(date, data_len), viewPortStartDate));
    int endIndex = (int)ceil(Chart::bSearch(DoubleArray(date, data_len), viewPortEndDate)) + 1;
    int noOfPoints = endIndex - startIndex + 1;        

    cout << "Start Index: " << startIndex << "End Index: " << endIndex << 
    "No. of Points: " << noOfPoints << "\n";

    // The first moving average period selected by the user.
    m_avgPeriod1 = m_MovAvg1->text().toInt();
    if (m_avgPeriod1 < 0)
        m_avgPeriod1 = 0;
    if (m_avgPeriod1 > 300)
        m_avgPeriod1 = 300;

    // The second moving average period selected by the user.
    m_avgPeriod2 = m_MovAvg2->text().toInt();
    if (m_avgPeriod2 < 0)
        m_avgPeriod2 = 0;
    if (m_avgPeriod2 > 300)
        m_avgPeriod2 = 300;

    QString parabolic_initial = m_ParabolicSAR_initial->text();
    QString parabolic_increment = m_ParabolicSAR_increment->text();
    QString parabolic_max = m_ParabolicSAR_maximum->text();
    m_ParabolicSAR_int = parabolic_initial.toDouble();
    m_ParabolicSAR_inc = parabolic_increment.toDouble();
    m_ParabolicSAR_max = parabolic_max.toDouble();

    m_band_period = m_band_pd->text().toInt();
    QString band_width = m_band_wd->text();
    m_band_width = band_width.toDouble();


    m_indicator1_p1 = m_Indicator1_pd1->text().toInt();
    m_indicator1_p2 = m_Indicator1_pd2->text().toInt();
    m_indicator1_p3 = m_Indicator1_pd3->text().toInt();

    m_indicator2_p1 = m_Indicator2_pd1->text().toInt();
    m_indicator2_p2 = m_Indicator2_pd2->text().toInt();
    m_indicator2_p3 = m_Indicator2_pd3->text().toInt();


    m_indicator3_p1 = m_Indicator3_pd1->text().toInt();
    m_indicator3_p2 = m_Indicator3_pd2->text().toInt();
    m_indicator3_p3 = m_Indicator3_pd3->text().toInt();


    m_indicator4_p1 = m_Indicator4_pd1->text().toInt();
    m_indicator4_p2 = m_Indicator4_pd2->text().toInt();
    m_indicator4_p3 = m_Indicator4_pd3->text().toInt();

    // We need extra leading data points in order to compute moving averages.
    int extraPoints = (m_avgPeriod1 > m_avgPeriod2) ? m_avgPeriod1 : m_avgPeriod2;
    if (extraPoints < 25)
        extraPoints = 25;

    //
    // Determine the chart size. In this demo, user can select 4 different chart sizes.
    // Default is the large chart size.
    //
    int width = 1135;
    int mainHeight = 255;
    int indicatorHeight = 100;

    QString selectedSize = m_ChartSize->itemData(m_ChartSize->currentIndex()).toString();
    if (selectedSize == "S")
    {
        // Small chart size
        width = 450;
        mainHeight = 160;
        indicatorHeight = 60;
    }
    else if (selectedSize == "M")
    {
        // Medium chart size
        width = 620;
        mainHeight = 215;
        indicatorHeight = 70;
    }
    else if (selectedSize == "H")
    {
        // Huge chart size
        width = 1000;
        mainHeight = 320;
        indicatorHeight = 90;
    }

    // Create the chart object using the selected size
    FinanceChart *m = new FinanceChart(width);

    // Add a title to the chart
    if(data_len <= 0)
        m->addTitle("INVALID SYMBOL");    
    else
        m->addTitle(Chart::TopCenter, m_TickerSymbol->text().toLocal8Bit().data());

    // Disable default legend box, as we are using dynamic legend
    m->setLegendStyle("normal", 8, Chart::Transparent, Chart::Transparent);
    
    // Set the data into the finance chart object
    m->setData(DoubleArray(date + startIndex, noOfPoints), 
               DoubleArray(high + startIndex, noOfPoints), 
               DoubleArray(low + startIndex, noOfPoints), 
               DoubleArray(open + startIndex, noOfPoints), 
               DoubleArray(close + startIndex, noOfPoints), 
               DoubleArray(volume + startIndex, noOfPoints), 30);

    
    // A copyright message at the bottom left corner the title area
    m->addPlotAreaTitle(Chart::BottomRight,
        "<*font=arial.ttf,size=8*>(c) Advanced Software Engineering");
    
    //
    // Add the first techical indicator according. In this demo, we draw the first
    // indicator on top of the main chart.
    //
    addIndicator(m, m_Indicator1->itemData(m_Indicator1->currentIndex()).toString(),
                 indicatorHeight, m_indicator1_p1, m_indicator1_p2, m_indicator1_p3);

        //
    // Add the main chart
        //
    m->addMainChart(mainHeight);

    //
    // Set log or linear scale according to user preference
    //
    m->setLogScale(m_LogScale->isChecked());

        //
        // Set axis labels to show data values or percentage change to user preference
        //
        if (m_PercentageScale->isChecked())
                m->setPercentageAxis();

        //
    // Draw the main chart depending on the chart type the user has selected
    //
    QString selectedType = m_ChartType->itemData(m_ChartType->currentIndex()).toString();
    if (selectedType == "Close")
        m->addCloseLine(0x000040);
    else if (selectedType == "TP")
        m->addTypicalPrice(0x000040);
    else if (selectedType == "WC")
        m->addWeightedClose(0x000040);
    else if (selectedType == "Median")
        m->addMedianPrice(0x000040);

    //
    // Add comparison line if there is data for comparison
    //
    if (m_compareDataLen > 30) {
        cout << "for comparison \n";    
            m->addComparison(DoubleArray(m_compareData + startIndex, noOfPoints), 0x0000ff,
                m_compareKey.toUtf8().data());
    }
 

    //
    // Add moving average lines.
    //
    addMovingAvg(m, m_AvgType1->itemData(m_AvgType1->currentIndex()).toString(),
                 m_avgPeriod1, 0x663300);
    addMovingAvg(m, m_AvgType2->itemData(m_AvgType2->currentIndex()).toString(),
                 m_avgPeriod2, 0x9900ff);

    //
    // Draw the main chart if the user has selected CandleStick or OHLC. We
    // draw it here to make sure it is drawn behind the moving average lines
    // (that is, the moving average lines stay on top.)
    //
    if (selectedType == "CandleStick")
        m->addCandleStick(0x33ff33, 0xff3333);
    else if (selectedType == "OHLC")
        m->addHLOC(0x8000, 0x800000);

    //
    // Add parabolic SAR if necessary
    //
    if (m_ParabolicSAR->isChecked())
        m->addParabolicSAR(m_ParabolicSAR_int, m_ParabolicSAR_inc, m_ParabolicSAR_max, 
            Chart::DiamondShape, 5, 0x008800, 0x000000);

    //
    // Add price band/channel/envelop to the chart according to user selection
    //
    QString selectedBand = m_PriceBand->itemData(m_PriceBand->currentIndex()).toString();
    if (selectedBand == "BB")
        m->addBollingerBand(m_band_period, m_band_width, 0x9999ff, 0xc06666ff);
    else if (selectedBand == "DC")
        m->addDonchianChannel(m_band_period, 0x9999ff, 0xc06666ff);
    else if (selectedBand == "Envelop")
        m->addEnvelop(m_band_period, m_band_width, 0x9999ff, 0xc06666ff);

    //
    // Add volume bars to the main chart if necessary
    //
    if (m_VolumeBars->isChecked())
        m->addVolBars(indicatorHeight, 0x99ff99, 0xff9999, 0xc0c0c0);

    //
    // Add additional indicators as according to user selection.
    //
    addIndicator(m, m_Indicator2->itemData(m_Indicator2->currentIndex()).toString(),
                 indicatorHeight, m_indicator2_p1, m_indicator2_p2, m_indicator2_p3);
    addIndicator(m, m_Indicator3->itemData(m_Indicator3->currentIndex()).toString(),
                 indicatorHeight, m_indicator3_p1, m_indicator3_p2, m_indicator3_p3);
    addIndicator(m, m_Indicator4->itemData(m_Indicator4->currentIndex()).toString(),
                 indicatorHeight, m_indicator4_p1, m_indicator4_p2, m_indicator4_p3);

    // Include track line with legend for the latest data values
    simple(m, ((XYChart *)m->getChart(0))->getPlotArea()->getRightX());

    // Set the chart to the viewer
    viewer->setChart(m);
    
}

void Simple::read_data(int durationOfStock)
{
    cout << "read_data \n";
    struct tm tm1;
    try {
        Connection conn(false);
        conn.connect("indian_stocks", "localhost", "root", "waheguru13");        
        Query query = conn.query();             
        char *s = m_TickerSymbol->text().toLocal8Bit().data();
        cout << "stock: " <<  s << "\n";
        /* Now SELECT */             
        switch(durationOfStock)
        {
            case 1:                
                query << "SELECT date, open, high, low, close, volume from nse_dumps WHERE nse_stock_id = (SELECT id from nse_stocks where stock_name = " << quote_only << s << " ) ORDER by date ASC";                
                break;
            case 7:                
                query << "SELECT MAX(date) AS date,  SUBSTRING_INDEX( GROUP_CONCAT( CAST( OPEN AS CHAR ) ORDER BY DATE ) ,  ',', 1 ) AS OPEN, max(high) AS high, min(low) AS low, SUBSTRING_INDEX(GROUP_CONCAT(CAST(close AS CHAR) ORDER BY date DESC), ',', 1) AS close, sum(volume) AS volume FROM `nse_dumps` WHERE nse_stock_id = (SELECT id from nse_stocks WHERE stock_name = " << quote_only << s << " ) GROUP BY DATE_FORMAT(date, \"%X Week: %V\") ORDER by date ASC";
                break;
            case 30:                
                query << "SELECT MAX(date) AS date,  SUBSTRING_INDEX( GROUP_CONCAT( CAST( OPEN AS CHAR ) ORDER BY DATE ) ,  ',', 1 ) AS OPEN, max(high) AS high, min(low) AS low, SUBSTRING_INDEX(GROUP_CONCAT(CAST(close AS CHAR) ORDER BY date DESC), ',', 1) AS close, sum(volume) AS volume FROM `nse_dumps` WHERE nse_stock_id = (SELECT id from nse_stocks WHERE stock_name = " << quote_only << s << ") GROUP BY DATE_FORMAT(date, \"%M, %Y\") ORDER by date ASC";

        }        
        StoreQueryResult ares = query.store();
        cout << "ares: " << ares.num_rows();
        if (ares.num_rows() == 0)
        {
            data_len = 0;
            return;
        }  
        data_len = ares.num_rows() - 1 ;
        cout << "data len: " << data_len;
        date = new double[data_len + 1];
        open = new double[data_len + 1];
        high = new double[data_len + 1];
        low = new double[data_len + 1];
        close = new double[data_len + 1];
        volume = new double[data_len + 1];
        for (size_t i = 0; i < ares.num_rows(); i++)
        {            
            sscanf(ares[i]["date"].c_str(),"%4d-%2d-%2d",&tm1.tm_year,&tm1.tm_mon,&tm1.tm_mday);
            date[i] = Chart::chartTime(tm1.tm_year , tm1.tm_mon, tm1.tm_mday);
            open[i] = ares[i]["open"];
            high[i] = ares[i]["high"];
            low[i] = ares[i]["low"];
            close[i] = atof(ares[i]["close"]);
            volume[i] = ares[i]["volume"];  
        }      
        //cout << "date: "<<  ares[ares.num_rows()]["date"].c_str;  
    } catch (BadQuery er) { // handle any connection or
        // query errors that may come up        
        cerr << "Error: " << er.what() << endl;        
    } catch (const BadConversion& er) {
        // Handle bad conversions        
        cerr << "Conversion error: " << er.what() << endl <<
                "\tretrieved data size: " << er.retrieved <<
                ", actual size: " << er.actual_size << endl;        
    } catch (const Exception& er) {
        // Catch-all for any other MySQL++ exceptions        
        cerr << "Error: " << er.what() << endl;        
    } 

}

void Simple::read_data_compare(int durationOfStock)
{
    cout << "read_data_compare \n";
    
    try {
        Connection conn(false);
        conn.connect("indian_stocks", "localhost", "root", "waheguru13");        
        Query query = conn.query();         
        char *s =  m_CompareWith ->text().toLocal8Bit().data();
        
        /* Now SELECT */            
        switch(durationOfStock)
        {
            case 1:                
                query << "SELECT date, open, high, low, close, volume from nse_dumps WHERE stock_id = (SELECT id from nse_stocks where stock_name = " << quote_only << s << " ) ORDER by date ASC";
                break;
            case 7:                
                query << "SELECT MAX(date) AS date, COALESCE(open) AS open, max(high) AS high, min(low) AS low, SUBSTRING_INDEX(GROUP_CONCAT(close), ',', -1) AS close, sum(volume) AS volume FROM `nse_dumps` WHERE stock_id = (SELECT id from nse_stocks WHERE stock_name = " << quote_only << s << " ) GROUP BY DATE_FORMAT(date, \"%X Week: %V\") ORDER by date ASC";
                break;
            case 30:                
                query << "SELECT MAX(date) AS date, COALESCE(open) AS open, max(high) AS high, min(low) AS low, SUBSTRING_INDEX(GROUP_CONCAT(close), ',', -1) AS close, sum(volume) AS volume FROM `nse_dumps` WHERE stock_id = (SELECT id from nse_stocks WHERE stock_name = " << quote_only << s << ") GROUP BY DATE_FORMAT(date, \"%M, %Y\") ORDER by date ASC";

        }

        StoreQueryResult ares = query.store();
        if (ares.num_rows() == 0)
        {
           m_compareDataLen = 0;
            return;
        }
        m_compareDataLen = ares.num_rows() - 1;

        m_compareData = new double[m_compareDataLen + 1];

        for (size_t i = 0; i < ares.num_rows(); i++)
        {            
            m_compareData[i] = ares[i]["close"];            
        }        
    } catch (BadQuery er) { // handle any connection or
        // query errors that may come up
        cerr << "Error: " << er.what() << endl;        
    } catch (const BadConversion& er) {
        // Handle bad conversions        
        cerr << "Conversion error: " << er.what() << endl <<
                "\tretrieved data size: " << er.retrieved <<
                ", actual size: " << er.actual_size << endl;        
    } catch (const Exception& er) {
        // Catch-all for any other MySQL++ exceptions        
        cerr << "Error: " << er.what() << endl;        
    } 
}


