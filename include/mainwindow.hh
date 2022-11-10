/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef MAINWINDOW_H
   #define MAINWINDOW_H 1
   
//   #include <iomanip>
//   #include <fstream>
//   #include <iostream>
   #include <algorithm>

   #include "Database.hh"
   #include "newwindow.hh"

//   #include <QMainWindow>
//   #include <QtGui>
//   #include <QStatusBar>
//   #if QT_VERSION >= 0x050000
//      #include <QtWidgets>
//   #endif

//   #include <gsl/gsl_blas.h>
//   #include <gsl/gsl_multifit_nlin.h>

//   QT_BEGIN_NAMESPACE
//      class QAction;
//      class QDialogButtonBox;
//      class QGroupBox;
//      class QLabel;
//      class QLineEdit;
//      class QMenu;
//      class QMenuBar;
//      class QPushButton;
//      class QTextEdit;
//      class QSpinBox;
//      class QStatusBar;
//      class QComboBox;
//      class QMessageBox;
//      class QIcon;
//      class QSize;
//      class QDialog;
//      class QCheckBox;
//      class QDoubleSpinBox;
//   QT_END_NAMESPACE

   class newwindow;

   class MainWindow : public QMainWindow {
      Q_OBJECT
      
      private:
         std::vector < source > Sources;
//         int Number_of_Sources;
         int Number_of_Sources_used;
         std::vector < int > Selected_Source;
//int Selected_source;
//to be included         std::vector < int > Selected_Source;
         int Selected_Function;
         std::vector < std::vector < double > > Experimental_Data;
//         int Number_of_Exp_Points;
         bool Plot_Window_Open;
         bool Repeat_Fit;

      public:
         int Number_of_Fits;
//         int Number_of_Functions;
         std::vector < fit_function > FFunctions;
         MainWindow();
         int * Fit_Order;
         double conf_band(int, int , double);
//         int Number_of_Plot_Points;
//         int Number_of_Fit_Points;
         std::vector < std::vector < double > > Data_to_fit;

      private slots:
         void open();
         void fit_func();
         void func_choice();
         void source_number();
         void source_choice();
         void plot();
         void calc_eff();
         void calc_exp_eff();
         void fit_repeat();
      protected:
         void closeEvent(QCloseEvent *event);

      private:
         void error_code(int);
         void setStatusBar();
         void createMenu();
         void createHorizontalGroupBox();
         void createGridGroupBox();
         void createFormGroupBox();
         void readSettings();
         void writeSettings();
         void loadFile(const QString &fileName);
         void createStatusBar();


      enum { NumGridRows = 3, NumButtons = 4 };

      std::vector < QString > source_input_text;
      QLabel *Number_of_Sources_label, *Calculate_for_gamma_label;
      QSpinBox *Number_of_Sources_input;
      QDoubleSpinBox *Gamma_input;
      std::vector < QLabel* > source_input_label, Calculate_Efficiency_label, Calculate_Efficiency_result, Calculate_Efficiency_error, Calculate_Efficiency_error_label;
      std::vector < QComboBox* > source_input;
      std::vector < QDoubleSpinBox* > source_input_activity, source_input_activity_error, source_input_time;
      QLabel *Function_Select_label, *Function_Name_label;
      QComboBox *Function_Select_box;
      QIcon *Function_Icon;
      QPushButton *Data_input_button, *Calculate_Efficiencies_button, *Fit_button, *Repeat_Fit_button, *Plot_button, *Calculate_button;
      QTextEdit *Editor_window;
      std::vector < QCheckBox* > Plot_Fit_select, Plot_Confidence_select;
      
      
      QString gen_text[6];
      QGroupBox *horizontalGroupBox;
      QGroupBox *gridGroupBox;
      QLabel *text_eff[5],*text_conf[5],*text_error[5];
      QLabel *gen_label[100];
      QLabel *label_egamma,*label_result[5];
      QPushButton *butt, *draw_button, *eff_button;

      QDialogButtonBox *buttonBox;
      QDoubleSpinBox *spin_egamma;
      QMenu *fileMenu;
      QMessageBox *msgBox;
      QDialog *plotWin;
      newwindow *lila;
      QCheckBox *check_fit[5];
      QCheckBox *check_conf[5];
   };

#endif
