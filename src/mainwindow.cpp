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
#include "mainwindow.hh"

MainWindow::MainWindow() {
   QGridLayout *layout = new QGridLayout;
   
   Sources = create_sources(Number_of_Sources);
   if( Number_of_Sources<=0 ) { error_code(12); this->close(); }
   FFunctions = create_functions(Number_of_Functions);
   
   Fit_Order = new int [Number_of_Functions];
   Number_of_Fits = 0;
   Selected_Source = 0;
   Selected_Function = 0;
   Plot_Window_Open = false;
   Repeat_Fit_Button = false;
   
   gen_text[0] = "Source";
   gen_text[1] = "Activity (Bq)";
   gen_text[2] = "Activity Error(Bq)";
   gen_text[3] = "Measurement Time (sec)";
   gen_text[4] = "Select fitting function:";

   readSettings();
   createStatusBar();

   for( int i=0 ; i<4 ; i++ ) {
      gen_label[i] = new QLabel();
      gen_label[i]->setText(gen_text[i]);
      layout->addWidget(gen_label[i],0,i);
   }

   source_combo = new QComboBox();
   for ( int i=0 ; i<Number_of_Sources ; i++ ) source_combo->addItem(QString::fromStdString(Sources[i].Source_Name)); 
   layout->addWidget(source_combo,1,0);
   connect(source_combo, SIGNAL(activated(int)), this, SLOT(source_choice()));

   spin_activity = new QDoubleSpinBox;
   spin_activity->setMaximum(5e10);
   layout->addWidget(spin_activity,1,1);

   spin_erract = new QDoubleSpinBox;
   spin_erract->setMaximum(5e10);
   layout->addWidget(spin_erract,1,2);
   spin_time = new QDoubleSpinBox;
   spin_time->setMaximum(5e10);
   layout->addWidget(spin_time,1,3);

   open_button = new QPushButton(tr("Open data file"));
   connect(open_button, SIGNAL(released()), this, SLOT(open()));
   layout->addWidget(open_button,2,0);

   calc_exp_eff_button = new QPushButton(tr("Calculate efficiencies"));
   calc_exp_eff_button->setDisabled(true);
   connect(calc_exp_eff_button, SIGNAL(released()), this, SLOT(calc_exp_eff()));
   layout->addWidget(calc_exp_eff_button,2,1);

   gen_label[4] = new QLabel();
   gen_label[4]->setText(gen_text[4]);
   layout->addWidget(gen_label[4],3,0);

   gen_label[5] = new QLabel();
   gen_label[5]->setText(QString::fromStdString(FFunctions[Selected_Function].Function_Name));
   layout->addWidget(gen_label[5],3,1);

   func_combo = new QComboBox();
   func_combo->setMaximumHeight(45);
   func_combo->setMinimumHeight(45);
   func_combo->setIconSize(QSize(250,45));
   for ( int i=0 ; i<Number_of_Functions ; i++ ) {
      func_icon = new QIcon();
      func_icon->addFile(QString::fromStdString(FFunctions[i].Function_Picture));
      func_combo->addItem("");
      func_combo->setItemIcon(i,*func_icon);
   }
   layout->addWidget(func_combo,4,0,1,2);
   connect(func_combo, SIGNAL(activated(int)), this, SLOT(func_choice()));

   fit_button = new QPushButton(tr("Fit"));
   fit_button->setDisabled(true);
   connect(fit_button, SIGNAL(released()), this, SLOT(fit_func()));
   layout->addWidget(fit_button,4,2);
   fit_repeat_button = new QPushButton(tr("Fit Again"));
   fit_repeat_button->setDisabled(true);
   connect(fit_repeat_button, SIGNAL(released()), this, SLOT(fit_repeat()));
   layout->addWidget(fit_repeat_button,4,3);

   smallEditor = new QTextEdit;
   smallEditor->setMinimumHeight(300);
   smallEditor->setMaximumHeight(300);
   smallEditor->setPlainText(tr(""));
   layout->addWidget(smallEditor, 5, 0, 6,2);

   draw_button = new QPushButton(tr("Plot"));
   draw_button->setDisabled(true);
   connect(draw_button, SIGNAL(released()), this, SLOT(plot()));
   layout->addWidget(draw_button,10,2);

   for( int i=0 ; i<Number_of_Functions ; i++ ) {
      check_fit[i] = new QCheckBox();
      check_fit[i]->setHidden(true);
      layout->addWidget(check_fit[i],5+i,2);
      check_conf[i] = new QCheckBox();
      check_conf[i]->setHidden(true);
      layout->addWidget(check_conf[i],5+i,3);
   }
   label_egamma = new QLabel(tr("Enter Egamma"));
   layout->addWidget(label_egamma,11,0);
   spin_egamma = new QDoubleSpinBox;
   spin_egamma->setMaximum(5000.);
   layout->addWidget(spin_egamma,11,1);
   eff_button = new QPushButton(tr("Calculate"));
   eff_button->setDisabled(true);
   connect(eff_button, SIGNAL(released()), this, SLOT(calc_eff()));
   layout->addWidget(eff_button,11,2);
   for( int i=0 ; i<Number_of_Functions ; i++ ) {
      label_result[i] = new QLabel();
      label_result[i]->setHidden(true);
      layout->addWidget(label_result[i],12+i,0);
      text_eff[i] = new QLabel;
      text_eff[i]->setText(QString::number(0));
      text_eff[i]->setHidden(true);
      layout->addWidget(text_eff[i],12+i,1);
      text_conf[i] = new QLabel;
      text_conf[i]->setText(QString::number(0));
      text_conf[i]->setHidden(true);
      layout->addWidget(text_conf[i],12+i,2);
      text_error[i] = new QLabel;
      text_error[i]->setText(" error (%)");
      text_error[i]->setHidden(true);
      layout->addWidget(text_error[i],12+i,3);
   }

    QWidget *window = new QWidget();
    window->setLayout(layout);
    setCentralWidget(window);
    setWindowTitle(tr("Efficiency Calculator v1.0"));
}

void MainWindow::source_choice() {
   Selected_Source = source_combo->currentIndex();
}

void MainWindow::open() {
   QString fileName = QFileDialog::getOpenFileName(this);
   if ( !fileName.isEmpty() ) {
      loadFile(fileName);
      Number_of_Fit_Points = 0;
      Number_of_Fits = 0;
      Data_to_fit.clear();
      fit_repeat_button->setDisabled(true);
      calc_exp_eff_button->setDisabled(false);
      draw_button->setDisabled(true);
      eff_button->setDisabled(true);
      for( int i=0 ; i<Number_of_Functions ; i++ ) {
         FFunctions[i].X_for_Plot.clear();
         FFunctions[i].Y_for_Plot.clear();
         FFunctions[i].Confidence_Band.clear();
         FFunctions[i].is_fitted = false;
         FFunctions[i].Plot_Fit = false;
         FFunctions[i].Plot_Confidence_Band = false;
         check_fit[i]->setHidden(true);
         check_fit[i]->setChecked(FFunctions[i].Plot_Fit);
         check_conf[i]->setHidden(true);
         check_conf[i]->setChecked(FFunctions[i].Plot_Confidence_Band);
         label_result[i]->setHidden(true);
         text_eff[i]->setHidden(true);
         text_eff[i]->setText(QString::number(0));
         text_conf[i]->setHidden(true);
         text_conf[i]->setText(QString::number(0));
         text_error[i]->setHidden(true);
         for( int j=0 ; j<FFunctions[i].Number_of_Parameters ; j++ ) FFunctions[i].Parameters_Values[j]=1.;
      }
      smallEditor->clear();
   }
}

void MainWindow::loadFile(const QString &fileName) {
   std::ifstream file(fileName.toUtf8().constData());
   std::string line;
   Number_of_Exp_Points = 0;
   while ( getline(file, line) ) {
      std::vector < double > data;
      double value;
      std::istringstream iss(line);
      while ( iss >> value ) data.push_back(value);
      Experimental_Data.push_back(data);
      Number_of_Exp_Points++;
   }
   file.close();
   #ifndef QT_NO_CURSOR
      QApplication::setOverrideCursor(Qt::WaitCursor);
   #endif
   #ifndef QT_NO_CURSOR
      QApplication::restoreOverrideCursor();
   #endif
   statusBar()->showMessage(tr("File loaded"), 6000);
}

void MainWindow::calc_exp_eff() {
   Number_of_Fit_Points = 0;
   Number_of_Fits = 0;
   Data_to_fit.clear();
   fit_repeat_button->setDisabled(true);
   draw_button->setDisabled(true);
   eff_button->setDisabled(true);
   for( int i=0 ; i<Number_of_Functions ; i++ ) {
      FFunctions[i].X_for_Plot.clear();
      FFunctions[i].Y_for_Plot.clear();
      FFunctions[i].Confidence_Band.clear();
      FFunctions[i].is_fitted = false;
      FFunctions[i].Plot_Fit = false;
      FFunctions[i].Plot_Confidence_Band = false;
      check_fit[i]->setHidden(true);
      check_fit[i]->setChecked(FFunctions[i].Plot_Fit);
      check_conf[i]->setHidden(true);
      check_conf[i]->setChecked(FFunctions[i].Plot_Confidence_Band);
      label_result[i]->setHidden(true);
      text_eff[i]->setHidden(true);
      text_eff[i]->setText(QString::number(0));
      text_conf[i]->setHidden(true);
      text_conf[i]->setText(QString::number(0));
      text_error[i]->setHidden(true);
      for( int j=0 ; j<FFunctions[i].Number_of_Parameters ; j++ ) FFunctions[i].Parameters_Values[j]=1.;
   }
   smallEditor->clear();
   
   double activity = spin_activity->value();
   double activity_error = spin_erract->value();
   double measurement_time = spin_time->value();

   if( activity==0 ) { error_code(0) ; return ; }
   if( measurement_time==0 ) { error_code(1) ; return ; }
   if( Experimental_Data[0][0]==0 ) { error_code(10) ; return ; }

   for( int i=0 ; i<Sources[Selected_Source].Number_of_Gammas ; i++ ) {
      for ( int j=0 ; j<Number_of_Exp_Points ; j++ ) {
         std::vector < double > data;
         double value;
         if( abs(Experimental_Data[j][0]-Sources[Selected_Source].Gammas[i])<2. ) {
            value = Sources[Selected_Source].Gammas[i];
            data.push_back(value);
            value = Experimental_Data[j][1]/(measurement_time*activity*(Sources[Selected_Source].Intensities[i]/100.));
            data.push_back(value);
            value = value * sqrt( (pow(Experimental_Data[j][2]/Experimental_Data[j][1],2)) + (pow(Sources[Selected_Source].Intensities_Errors[i]/Sources[Selected_Source].Intensities[i],2)) + (pow(activity_error/activity,2)));
            data.push_back(value);
            Data_to_fit.push_back(data);
            Number_of_Fit_Points++;
         }
      }
   }
   fit_button->setDisabled(false);
   fit_repeat_button->setDisabled(true);
   return;
}

void MainWindow::func_choice() {
   Selected_Function = func_combo->currentIndex();
   gen_label[5]->setText(QString::fromStdString(FFunctions[Selected_Function].Function_Name));
   fit_button->setDisabled(FFunctions[Selected_Function].is_fitted);
}

void MainWindow::fit_func() {
   if ( Data_to_fit.size()==0 ) { error_code(10) ; return ; }
   if ( Number_of_Fit_Points<FFunctions[Selected_Function].Number_of_Parameters ) { error_code(11) ; return ; }
   const gsl_multifit_fdfsolver_type *T;
   gsl_multifit_fdfsolver *s;
   int status;
   unsigned int iter = 0;
   const size_t n = Number_of_Fit_Points;
   const size_t p = FFunctions[Selected_Function].Number_of_Parameters;
   
   gsl_matrix *covar = gsl_matrix_alloc(p,p);
   double t[n], y[n], sigma[n];
   for ( int i=0 ; i<Number_of_Fit_Points ; i++ ) {
      t[i] = Data_to_fit[i][0];
      y[i] = Data_to_fit[i][1];
      sigma[i] = Data_to_fit[i][2];
   }
   struct data d = { n, t, y, sigma };
   gsl_multifit_function_fdf f;
   double x_init[p];
   for ( int i=0 ; i<FFunctions[Selected_Function].Number_of_Parameters ; i++ ) {
      if ( Repeat_Fit_Button ) x_init[i] = FFunctions[Selected_Function].Parameters_Values[i];
      else x_init[i] = 1.;
   }
   FFunctions[Selected_Function].X_for_Plot.clear();
   FFunctions[Selected_Function].Y_for_Plot.clear();
   FFunctions[Selected_Function].Confidence_Band.clear();

   f.f = FFunctions[Selected_Function].f;
//   f.f = boost::bind(&fit_function::f1,&FFunctions[Selected_Function],_1,_2,_3);
   f.df = FFunctions[Selected_Function].df;
   f.fdf = FFunctions[Selected_Function].fdf;

//   smallEditor->clear();
   f.n = n;
   f.p = p;
   f.params = &d;
   T = gsl_multifit_fdfsolver_lmsder;
   s = gsl_multifit_fdfsolver_alloc(T, n, p);

   gsl_vector *x = gsl_vector_alloc(p);
   for ( size_t kola=0 ; kola<p ; kola++ ) gsl_vector_set(x,kola,x_init[kola]);

   gsl_multifit_fdfsolver_set(s, &f, x);
   do {
      iter++;
      status = gsl_multifit_fdfsolver_iterate(s);
      if ( status ) break;
      status = gsl_multifit_test_delta(s->dx, s->x, 1e-4, 1e-4);
   }
   while ( status == GSL_CONTINUE && iter < 500 );
   if( status !=0 ) {
      smallEditor->append("Fit results for "+QString::fromStdString(FFunctions[Selected_Function].Function_Name)+" function.\n");
      smallEditor->append(tr(gsl_strerror(status)));
      fit_repeat_button->setDisabled(false);
   }
   else if( status==0 ) {
      smallEditor->append("Fit results for "+QString::fromStdString(FFunctions[Selected_Function].Function_Name)+" function.\n");
      smallEditor->append("Fit "+tr(gsl_strerror(status))+" in "+QString::number(iter)+" iterations.\n");
      if( FFunctions[Selected_Function].is_fitted==false ) {
         check_fit[Number_of_Fits]->setText(QString::fromStdString(FFunctions[Selected_Function].Function_Name));
         FFunctions[Selected_Function].is_fitted = true;
         Fit_Order[Number_of_Fits] = Selected_Function;
         check_fit[Number_of_Fits]->setHidden(false);
         check_conf[Number_of_Fits]->setText(tr("95% Confidence Band"));
         check_conf[Number_of_Fits]->setHidden(false);
         label_result[Number_of_Fits]->setText(QString::fromStdString(FFunctions[Selected_Function].Function_Name)+" efficiency:");
         label_result[Number_of_Fits]->setHidden(false);
         text_eff[Number_of_Fits]->setHidden(false);
         text_conf[Number_of_Fits]->setHidden(false);
         text_error[Number_of_Fits]->setHidden(false);
         Number_of_Fits++;
         fit_button->setDisabled(true);
         draw_button->setDisabled(false);
         eff_button->setDisabled(false);
      }
   }

   gsl_multifit_covar (s->J, 0.0, covar);

   for (size_t k = 0; k < p ; k++) {
      FFunctions[Selected_Function].Parameters_Values[k] = gsl_vector_get (s->x, k);
      for ( size_t kk = 0; kk < p ; kk++ ) FFunctions[Selected_Function].Covariance_Matrix[k][kk] = gsl_matrix_get(covar,k,kk);
   }

   FFunctions[Selected_Function].Chi2 = gsl_blas_dnrm2(s->f);
   FFunctions[Selected_Function].Degrees_of_Freedom = n - p;

   QString result_line;
   for( int i=0 ; i<FFunctions[Selected_Function].Number_of_Parameters ; i++ ) {
      result_line = "A"+QString::number(i+1)+"= "+ QString::number(FFunctions[Selected_Function].Parameters_Values[i]);
      result_line = result_line+" +/- "+QString::number(sqrt(FFunctions[Selected_Function].Covariance_Matrix[i][i]));
      smallEditor->append(result_line);
   }

   result_line = "\n chisq/dof= "+ QString::number(pow(FFunctions[Selected_Function].Chi2, 2.0) / FFunctions[Selected_Function].Degrees_of_Freedom);
   smallEditor->append(result_line);
   Number_of_Plot_Points = (t[Number_of_Fit_Points-1]*1.2-t[0]*0.8)/5.;
   for( int i=0 ; i<Number_of_Plot_Points ; i++ ) {
      FFunctions[Selected_Function].X_for_Plot.push_back((i*5.)+(t[0]*0.8));
      FFunctions[Selected_Function].Y_for_Plot.push_back(FFunctions[Selected_Function].Fit_Function(FFunctions[Selected_Function].X_for_Plot[i],FFunctions[Selected_Function].Parameters_Values));
      FFunctions[Selected_Function].Confidence_Band.push_back(conf_band(Selected_Function,p,FFunctions[Selected_Function].X_for_Plot[i]));
   }

//Ypologismos R^2
   double sum = 0;
   for( int i=0 ; i<Number_of_Fit_Points ; i++ ) sum += Data_to_fit[i][1];
   double yaver = sum/Number_of_Fit_Points;
   sum = 0;
   for( int i=0 ; i<Number_of_Fit_Points ; i++ ) sum += pow((Data_to_fit[i][1]-yaver),2);

   double sum1 = 0;
   for( int i=0 ; i<Number_of_Fit_Points ; i++ ) sum1 += pow((Data_to_fit[i][1]-FFunctions[Selected_Function].Fit_Function(Data_to_fit[i][0],FFunctions[Selected_Function].Parameters_Values)),2);

   result_line = "\n RSq= "+ QString::number(1-sum1/sum);
   smallEditor->append(result_line);
   smallEditor->append("");
   smallEditor->append("------------------------------------");
   smallEditor->append("");
   Repeat_Fit_Button = false;
   gsl_multifit_fdfsolver_free(s);
}

void MainWindow::fit_repeat(){
   Repeat_Fit_Button = true;
   fit_func();
}

void MainWindow::calc_eff(){
   double eff,conf;
   if( spin_egamma->value()==0 ){
      error_code(0);
      return;
   }
   for( int i=0 ; i<Number_of_Fits ; i++ ) {
      eff = FFunctions[Fit_Order[i]].Fit_Function(spin_egamma->value(),FFunctions[Fit_Order[i]].Parameters_Values);
      conf = conf_band(Fit_Order[i],FFunctions[Fit_Order[i]].Number_of_Parameters,spin_egamma->value());
      conf = conf/eff*100;
      text_eff[i]->setText(QString::number(eff));
      text_conf[i]->setText(QString::number(conf));
   }
   return;
}

void MainWindow::plot(){
   for ( int i=0 ; i<Number_of_Fits ; i++) {
      if( check_fit[i]->isChecked()==false && check_conf[i]->isChecked()==true ) {
         check_conf[i]->setChecked(false);
      }
   }
   for ( int i=0 ; i<Number_of_Fits ; i++ ) {
      if( check_fit[i]->isChecked()==true ) {
         FFunctions[Fit_Order[i]].Plot_Fit = true;}
      else {
         FFunctions[Fit_Order[i]].Plot_Fit = false;
      }
      if( check_conf[i]->isChecked()==true ) {
         FFunctions[Fit_Order[i]].Plot_Confidence_Band = true;}
      else {
         FFunctions[Fit_Order[i]].Plot_Confidence_Band = false;
      }
   }

   if( Plot_Window_Open==true ){ lila->close() ; }
   lila = new newwindow(this); // Be sure to destroy you window somewhere
   lila->show();
   Plot_Window_Open = true;
}

void MainWindow::closeEvent(QCloseEvent *event){
   if( Plot_Window_Open ==true ) { lila->close() ; }
}

void MainWindow::createStatusBar(){
   statusBar()->showMessage(tr("Ready"));
}

void MainWindow::readSettings(){
     resize( QSize(600,500));
}

void MainWindow::writeSettings(){
   QSettings settings("Trll", "Applicati");
   settings.setValue("pos", pos());
   settings.setValue("size", size());
}

void MainWindow::error_code (int code){
   QMessageBox msgBox;
   switch (code){
      case 0:
         msgBox.setWindowTitle("Activity Error");
         msgBox.setText("Please enter non zero activity");
         msgBox.setIcon(QMessageBox::Critical);
         break;
      case 1:
         msgBox.setWindowTitle("Time Error");
         msgBox.setText("Please enter non zero measurement time");
         msgBox.setIcon(QMessageBox::Critical);
         break;
      case 9:
         msgBox.setWindowTitle("Fitting Data");
         msgBox.setText("There is a problem with the data file (zero values)");
         msgBox.setIcon(QMessageBox::Critical);
         break;
      case 10:
         msgBox.setWindowTitle("No Fitting Data");
         msgBox.setText("Please choose a Data file or the correct source");
         msgBox.setIcon(QMessageBox::Critical);
         break;
      case 11:
         msgBox.setWindowTitle("Data points less than fitting parameters");
         msgBox.setText("Please choose correct source or add data points to be fitted");
         msgBox.setIcon(QMessageBox::Critical);
         break;
      case 12:
         msgBox.setWindowTitle("No sources file");
         msgBox.setText("Place sources file in directory and start again");
         msgBox.setIcon(QMessageBox::Critical);
   }
   msgBox.exec();
}

double MainWindow::conf_band(int function, int p, double egamma){
   double t_test[18] = {12.706,4.303,3.182,2.776,2.571,2.447,2.365,2.306,2.262,2.228,
                       2.201,2.179,2.160,2.145,2.131,2.120,2.110,2.101};

   gsl_matrix *sigy = gsl_matrix_alloc (1, 1);

   gsl_matrix *cov = gsl_matrix_alloc (p, p);
   for ( int k=0 ; k<p ; k++ ) {
      for( int kk=0 ; kk<p ; kk++ ) {
         gsl_matrix_set(cov,k,kk,FFunctions[function].Covariance_Matrix[k][kk]);
      }
   }
   gsl_matrix *fro = gsl_matrix_alloc(1,p);
   gsl_matrix *gamma = gsl_matrix_alloc (1, p);
   
   for ( int i=0 ; i<FFunctions[function].Number_of_Parameters ; i++ ) {
      double * ParVal = new double [FFunctions[function].Number_of_Parameters] ;
      for ( int j=0 ; j<FFunctions[function].Number_of_Parameters ; j++ ) {
         if( i==j ) ParVal[j] = 1. ;
         else  ParVal[j] = 0. ;
      }
      gsl_matrix_set (gamma, 0, i, FFunctions[function].Derivative(i,egamma,FFunctions[function].Parameters_Values));
      delete[] ParVal;
   }

   gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, gamma, cov, 0.0, fro);
   gsl_blas_dgemm(CblasNoTrans, CblasTrans, 1.0, fro, gamma, 0.0, sigy);

   double llalla = gsl_matrix_get(sigy,0,0);
   gsl_matrix_free(gamma);
   gsl_matrix_free(cov);
   gsl_matrix_free(fro);
   gsl_matrix_free(sigy);

   return t_test[FFunctions[function].Degrees_of_Freedom]*sqrt(pow(FFunctions[function].Chi2,2)*sqrt(pow(llalla,2)));
}
