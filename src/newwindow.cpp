#include "newwindow.hh"
#include "qcustomplot.h"

newwindow::newwindow(MainWindow *testing) {

   setWindowTitle(tr("Efficiency Plot"));

   readSettings();

   QCustomPlot *customPlot = new QCustomPlot();
   QColor Colours[] = {QColor("black"), QColor("red"), QColor("blue"),
                       QColor("green"), QColor("yellow"), QColor("darkRed")};
   int Number_of_Plots = 0;
   QVector<double> x(testing->Data_to_fit.size()), y(testing->Data_to_fit.size()), yerr(testing->Data_to_fit.size());
   for (size_t i = 0; i < testing->Data_to_fit.size(); i++) {
      x[i] = testing->Data_to_fit[i][0];
      y[i] = testing->Data_to_fit[i][1];
      yerr[i] = testing->Data_to_fit[i][2];
   }

   // Plot for the experimental Data graph(0)
   customPlot->addGraph();
   customPlot->xAxis->setBasePen(QPen(Qt::black, 2));
   customPlot->graph(Number_of_Plots)->setLineStyle(QCPGraph::lsNone);
   customPlot->graph(Number_of_Plots)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 6));
   auto yerrors = new QCPErrorBars(customPlot->xAxis, customPlot->yAxis);
   yerrors->setDataPlottable(customPlot->graph(Number_of_Plots));
   yerrors->setErrorType(QCPErrorBars::ErrorType::etValueError);
   yerrors->setPen(QPen(QColor(180, 180, 180)));
   //   customPlot->graph(Number_of_Plots)->setErrorType(QCPGraph::etValue);
   //   customPlot->graph(Number_of_Plots)->setErrorPen(QPen(QColor(180,180,180)));
   customPlot->graph(Number_of_Plots)->setName("Experimental");
   //   customPlot->graph(Number_of_Plots)->setDataValueError(x, y, yerr);
   customPlot->graph(Number_of_Plots)->setData(x, y);
   yerrors->setData(yerr);
   Number_of_Plots++;
   // give the axes some labels:
   customPlot->xAxis->setLabel("Energy" + QString::fromUtf8("γ") + "(keV)");
   customPlot->yAxis->setLabel("Efficiency");
   // set axes ranges, so we see all data:
   customPlot->xAxis->setRange(
       0.8 * (testing->FFunctions[testing->Fit_Order[0]].X_for_Plot[0]),
       1.2 * (testing->FFunctions[testing->Fit_Order[0]].X_for_Plot[(testing->FFunctions[testing->Fit_Order[0]].X_for_Plot.size()) - 1]));
   customPlot->yAxis->setRange(
       (*std::min_element(
           &(testing->FFunctions[testing->Fit_Order[0]].Y_for_Plot[0]),
           &(testing->FFunctions[testing->Fit_Order[0]].Y_for_Plot[(testing->FFunctions[testing->Fit_Order[0]].X_for_Plot.size()) - 1]))) *
           0.8,
       (*std::max_element(
           &(testing->FFunctions[testing->Fit_Order[0]].Y_for_Plot[0]),
           &(testing->FFunctions[testing->Fit_Order[0]].Y_for_Plot[(testing->FFunctions[testing->Fit_Order[0]].X_for_Plot.size()) - 1]))) *
           1.2);
   // End of plot for the experimental Data graph(0)
   customPlot->legend->setVisible(true);
   customPlot->legend->setFont(QFont("Helvetica", 9));

   for (int ijk = 0; ijk < (testing->Number_of_Fits);
        ijk++) {                                                           // ijk runs for each "theoretical curve
      if (testing->FFunctions[testing->Fit_Order[ijk]].Plot_Fit == true) { // if the checkbox is true (checked)
         QPen pen;
         customPlot->addGraph();
         pen.setWidth(2);
         pen.setColor(Colours[ijk + 1]);
         customPlot->graph(Number_of_Plots)->setName(QString::fromStdString(testing->FFunctions[testing->Fit_Order[ijk]].Function_Name));
         customPlot->graph(Number_of_Plots)->setPen(pen);
#if defined(_WIN32)
         customPlot->graph(Number_of_Plots)->setData(QVector<double>::fromStdVector(testing->FFunctions[testing->Fit_Order[ijk]].X_for_Plot), QVector<double>::fromStdVector(testing->FFunctions[testing->Fit_Order[ijk]].Y_for_Plot));
#else
         customPlot->graph(Number_of_Plots)->setData(QVector<double>((testing->FFunctions[testing->Fit_Order[ijk]].X_for_Plot).begin(), (testing->FFunctions[testing->Fit_Order[ijk]].X_for_Plot).end()), QVector<double>((testing->FFunctions[testing->Fit_Order[ijk]].Y_for_Plot).begin(), (testing->FFunctions[testing->Fit_Order[ijk]].Y_for_Plot).end()));
#endif
         Number_of_Plots++;
      }
      if (testing->FFunctions[testing->Fit_Order[ijk]].Plot_Confidence_Band == true) {
         for (int j = 0; j < 2; j++) {
            std::vector<double> cb(testing->FFunctions[testing->Fit_Order[ijk]].Y_for_Plot);
            for (size_t k = 0; k < testing->FFunctions[testing->Fit_Order[ijk]].Y_for_Plot.size(); k++)
               cb[k] = cb[k] + (pow(-1, j) * testing->FFunctions[testing->Fit_Order[ijk]].Confidence_Band[k]);
            QPen pen;
            pen.setWidth(2);
            pen.setStyle(Qt::DotLine);
            pen.setColor(Colours[ijk + 1]);
            customPlot->addGraph();
            customPlot->graph(Number_of_Plots)->setPen(pen);
#if defined(_WIN32)
            customPlot->graph(Number_of_Plots)->setData(QVector<double>::fromStdVector(testing->FFunctions[testing->Fit_Order[ijk]].X_for_Plot), QVector<double>::fromStdVector(testing->FFunctions[testing->Fit_Order[ijk]].Y_for_Plot));
#else
            customPlot->graph(Number_of_Plots)->setData(QVector<double>((testing->FFunctions[testing->Fit_Order[ijk]].X_for_Plot).begin(), (testing->FFunctions[testing->Fit_Order[ijk]].X_for_Plot).end()), QVector<double>(cb.begin(), cb.end()));
#endif
            customPlot->legend->removeItem(customPlot->legend->itemCount() - 1);
            Number_of_Plots++;
         }
      }
   }
   customPlot->replot();
   QGridLayout *lay = new QGridLayout;
   lay->addWidget(customPlot, 0, 0);
   QWidget *window = new QWidget();
   window->setLayout(lay);
   setCentralWidget(window);
}

void newwindow::closeEvent(QCloseEvent *) {
   writeSettings();
}

void newwindow::resizeEvent(QResizeEvent *) {
   //   writeSettings();
}

void newwindow::readSettings() {
   QSettings settings("Troll", "Efficiency Plot");
   QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
   QSize size = settings.value("size", QSize(400, 400)).toSize();
   //   size = settings.
   resize(size);
   move(pos);
}

void newwindow::writeSettings() {
   QSettings settings("Troll", "Efficiency Plot");
   settings.setValue("pos", pos());
   settings.setValue("size", size());
}
