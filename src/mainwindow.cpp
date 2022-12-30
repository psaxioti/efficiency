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

  int Line_Number = 0;

  Sources = create_sources();
  //   Number_of_Sources=Sources.size();
  if (Sources.size() <= 0) {
    error_code(12);
    this->close();
  }
  FFunctions = create_functions();

  Fit_Order = new int[FFunctions.size()];
  Number_of_Fits = 0;
  Number_of_Sources_used = 1;
  Selected_Function = 0;
  Plot_Window_Open = false;
  Repeat_Fit = false;

  readSettings();
  createStatusBar();

  Number_of_Sources_label = new QLabel();
  Number_of_Sources_label->setText("Number of Sources used");
  layout->addWidget(Number_of_Sources_label, Line_Number, 0);
  Number_of_Sources_input = new QSpinBox;
  Number_of_Sources_input->setMaximum(Sources.size());
  Number_of_Sources_input->setMinimum(1);
  layout->addWidget(Number_of_Sources_input, Line_Number++, 1);
  connect(Number_of_Sources_input, SIGNAL(valueChanged(int)), this,
          SLOT(source_number()));

  source_input_text.push_back("Source ");
  source_input_text.push_back("Activity (Bq)");
  source_input_text.push_back("Activity Error(Bq)");
  source_input_text.push_back("Measurement Time (sec)");
  for (size_t j = 0; j < Sources.size(); j++) {
    for (size_t i = 0; i < source_input_text.size(); i++) {
      source_input_label.push_back(new QLabel());
      if (i == 0)
        source_input_label[i + (j * source_input_text.size())]->setText(
            source_input_text[i] + QString::number(j + 1));
      else
        source_input_label[i + (j * source_input_text.size())]->setText(
            source_input_text[i]);
      layout->addWidget(source_input_label[i + (j * source_input_text.size())],
                        Line_Number, i);
      source_input_label[i + (j * source_input_text.size())]->setHidden(true);
    }

    source_input.push_back(new QComboBox());
    for (size_t i = 0; i < Sources.size(); i++)
      source_input[j]->addItem(QString::fromStdString(Sources[i].Source_Name));
    layout->addWidget(source_input[j], ++Line_Number, 0);
    connect(source_input[j], SIGNAL(activated(int)), this,
            SLOT(source_choice()));
    source_input[j]->setHidden(true);

    source_input_activity.push_back(new QDoubleSpinBox);
    source_input_activity[j]->setMaximum(5e10);
    layout->addWidget(source_input_activity[j], Line_Number, 1);
    source_input_activity[j]->setHidden(true);

    source_input_activity_error.push_back(new QDoubleSpinBox);
    source_input_activity_error[j]->setMaximum(5e10);
    layout->addWidget(source_input_activity_error[j], Line_Number, 2);
    source_input_activity_error[j]->setHidden(true);

    source_input_time.push_back(new QDoubleSpinBox);
    source_input_time[j]->setMaximum(5e10);
    layout->addWidget(source_input_time[j], Line_Number++, 3);
    source_input_time[j]->setHidden(true);
  }
  source_number();

  Data_input_button = new QPushButton(tr("Open data file"));
  connect(Data_input_button, SIGNAL(released()), this, SLOT(open()));
  layout->addWidget(Data_input_button, Line_Number, 0);

  Calculate_Efficiencies_button = new QPushButton(tr("Calculate efficiencies"));
  Calculate_Efficiencies_button->setDisabled(true);
  connect(Calculate_Efficiencies_button, SIGNAL(released()), this,
          SLOT(calc_exp_eff()));
  layout->addWidget(Calculate_Efficiencies_button, Line_Number++, 1);

  Function_Select_label = new QLabel();
  Function_Select_label->setText("Select fitting function:");
  layout->addWidget(Function_Select_label, Line_Number, 0);

  Function_Name_label = new QLabel();
  Function_Name_label->setText(
      QString::fromStdString(FFunctions[Selected_Function].Function_Name));
  layout->addWidget(Function_Name_label, Line_Number++, 1);

  Function_Select_box = new QComboBox();
  Function_Select_box->setMaximumHeight(45);
  Function_Select_box->setMinimumHeight(45);
  Function_Select_box->setIconSize(QSize(250, 45));
  for (size_t i = 0; i < FFunctions.size(); i++) {
    Function_Icon = new QIcon();
    Function_Icon->addFile(
        QString::fromStdString(FFunctions[i].Function_Picture));
    Function_Select_box->addItem("");
    Function_Select_box->setItemIcon(i, *Function_Icon);
  }
  layout->addWidget(Function_Select_box, Line_Number, 0, 1, 2);
  connect(Function_Select_box, SIGNAL(activated(int)), this,
          SLOT(func_choice()));

  Fit_button = new QPushButton(tr("Fit"));
  Fit_button->setDisabled(true);
  connect(Fit_button, SIGNAL(released()), this, SLOT(fit_func()));
  layout->addWidget(Fit_button, Line_Number, 2);
  Repeat_Fit_button = new QPushButton(tr("Fit Again"));
  Repeat_Fit_button->setDisabled(true);
  connect(Repeat_Fit_button, SIGNAL(released()), this, SLOT(fit_repeat()));
  layout->addWidget(Repeat_Fit_button, Line_Number++, 3);

  int dum = Line_Number;
  Editor_window = new QTextEdit;
  Editor_window->setMinimumHeight(300);
  Editor_window->setMaximumHeight(300);
  Editor_window->setPlainText(tr(""));
  layout->addWidget(Editor_window, Line_Number, 0, 6, 2);

  for (size_t i = 0; i < FFunctions.size(); i++) {
    Plot_Fit_select.push_back(new QCheckBox());
    Plot_Fit_select[i]->setHidden(true);
    layout->addWidget(Plot_Fit_select[i], Line_Number, 2);
    Plot_Confidence_select.push_back(new QCheckBox());
    Plot_Confidence_select[i]->setHidden(true);
    layout->addWidget(Plot_Confidence_select[i], Line_Number++, 3);
  }

  Line_Number = dum + 5;
  Plot_button = new QPushButton(tr("Plot"));
  Plot_button->setDisabled(true);
  connect(Plot_button, SIGNAL(released()), this, SLOT(plot()));
  layout->addWidget(Plot_button, Line_Number++, 2);

  Calculate_for_gamma_label = new QLabel(tr("Enter Egamma"));
  layout->addWidget(Calculate_for_gamma_label, Line_Number, 0);
  Gamma_input = new QDoubleSpinBox;
  Gamma_input->setMaximum(5000.);
  layout->addWidget(Gamma_input, Line_Number, 1);
  Calculate_button = new QPushButton(tr("Calculate"));
  Calculate_button->setDisabled(true);
  connect(Calculate_button, SIGNAL(released()), this, SLOT(calc_eff()));
  layout->addWidget(Calculate_button, Line_Number++, 2);
  for (size_t i = 0; i < FFunctions.size(); i++) {
    Calculate_Efficiency_label.push_back(new QLabel());
    Calculate_Efficiency_label[i]->setHidden(true);
    layout->addWidget(Calculate_Efficiency_label[i], Line_Number, 0);
    Calculate_Efficiency_result.push_back(new QLabel);
    Calculate_Efficiency_result[i]->setText(QString::number(0));
    Calculate_Efficiency_result[i]->setHidden(true);
    layout->addWidget(Calculate_Efficiency_result[i], Line_Number, 1);
    Calculate_Efficiency_error.push_back(new QLabel);
    Calculate_Efficiency_error[i]->setText(QString::number(0));
    Calculate_Efficiency_error[i]->setHidden(true);
    layout->addWidget(Calculate_Efficiency_error[i], Line_Number, 2);
    Calculate_Efficiency_error_label.push_back(new QLabel);
    Calculate_Efficiency_error_label[i]->setText(" error (%)");
    Calculate_Efficiency_error_label[i]->setHidden(true);
    layout->addWidget(Calculate_Efficiency_error_label[i], Line_Number++, 3);
  }

  QWidget *window = new QWidget();
  window->setLayout(layout);
  setCentralWidget(window);
  setWindowTitle(tr(XSTR(Efficiency Calculator v VERSION)));
}

void MainWindow::source_number() {
  Number_of_Sources_used = Number_of_Sources_input->value();
  if (Selected_Source.size() != Number_of_Sources_used)
    Selected_Source.clear();
  for (size_t i = 0; i < Number_of_Sources_used; i++) {
    for (size_t j = 0; j < source_input_text.size(); j++)
      source_input_label[j + (i * source_input_text.size())]->setHidden(false);
    if (!Selected_Source.empty())
      source_input[i]->setCurrentIndex(Selected_Source[i]);
    else
      source_input[i]->setCurrentIndex(i);
    source_input[i]->setHidden(false);
    source_input_activity[i]->setHidden(false);
    source_input_activity_error[i]->setHidden(false);
    source_input_time[i]->setHidden(false);
  }
  for (size_t i = Number_of_Sources_used; i < Sources.size(); i++) {
    for (size_t j = 0; j < source_input_text.size(); j++)
      source_input_label[j + (i * source_input_text.size())]->setHidden(true);
    source_input[i]->setHidden(true);
    source_input_activity[i]->setHidden(true);
    source_input_activity_error[i]->setHidden(true);
    source_input_time[i]->setHidden(true);
  }
  if (Selected_Source.empty())
    source_choice();
  return;
}

void MainWindow::source_choice() {
  Selected_Source.clear();
  for (size_t i = 0; i < Number_of_Sources_used; i++)
    for (size_t j = 0; j < Sources.size(); j++)
      if (QString::fromStdString(Sources[j].Source_Name) ==
          source_input[i]->currentText())
        Selected_Source.push_back(j);
  source_number();
  for (size_t i = 0; i < Number_of_Sources_used; i++)
    for (size_t j = i + 1; j < Number_of_Sources_used; j++)
      if (Selected_Source[i] == Selected_Source[j]) {
        error_code(2);
        return;
      }
  return;
}

void MainWindow::open() {
  QString fileName = QFileDialog::getOpenFileName(this);
  if (!fileName.isEmpty()) {
    loadFile(fileName);
    Number_of_Fits = 0;
    Data_to_fit.clear();
    Repeat_Fit_button->setDisabled(true);
    Calculate_Efficiencies_button->setDisabled(false);
    Plot_button->setDisabled(true);
    Calculate_button->setDisabled(true);
    for (size_t i = 0; i < FFunctions.size(); i++) {
      FFunctions[i].X_for_Plot.clear();
      FFunctions[i].Y_for_Plot.clear();
      FFunctions[i].Confidence_Band.clear();
      FFunctions[i].is_fitted = false;
      FFunctions[i].Plot_Fit = false;
      FFunctions[i].Plot_Confidence_Band = false;
      Plot_Fit_select[i]->setHidden(true);
      Plot_Fit_select[i]->setChecked(FFunctions[i].Plot_Fit);
      Plot_Confidence_select[i]->setHidden(true);
      Plot_Confidence_select[i]->setChecked(FFunctions[i].Plot_Confidence_Band);
      Calculate_Efficiency_label[i]->setHidden(true);
      Calculate_Efficiency_result[i]->setHidden(true);
      Calculate_Efficiency_result[i]->setText(QString::number(0));
      Calculate_Efficiency_error[i]->setHidden(true);
      Calculate_Efficiency_error[i]->setText(QString::number(0));
      Calculate_Efficiency_error_label[i]->setHidden(true);
      for (size_t j = 0; j < FFunctions[i].Number_of_Parameters; j++)
        FFunctions[i].Parameters_Values[j] = 1.;
    }
    Editor_window->clear();
  }
}

void MainWindow::loadFile(const QString &fileName) {
  std::ifstream file(fileName.toUtf8().constData());
  std::string line;
  Experimental_Data.clear();
  while (getline(file, line)) {
    std::vector<double> data;
    double value;
    std::istringstream iss(line);
    while (iss >> value)
      data.push_back(value);
    Experimental_Data.push_back(data);
  }
  std::sort(Experimental_Data.begin(), Experimental_Data.end());

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
  Number_of_Fits = 0;
  Data_to_fit.clear();
  Repeat_Fit_button->setDisabled(true);
  Plot_button->setDisabled(true);
  Calculate_button->setDisabled(true);
  for (size_t i = 0; i < FFunctions.size(); i++) {
    FFunctions[i].X_for_Plot.clear();
    FFunctions[i].Y_for_Plot.clear();
    FFunctions[i].Confidence_Band.clear();
    FFunctions[i].is_fitted = false;
    FFunctions[i].Plot_Fit = false;
    FFunctions[i].Plot_Confidence_Band = false;
    Plot_Fit_select[i]->setHidden(true);
    Plot_Fit_select[i]->setChecked(FFunctions[i].Plot_Fit);
    Plot_Confidence_select[i]->setHidden(true);
    Plot_Confidence_select[i]->setChecked(FFunctions[i].Plot_Confidence_Band);
    Calculate_Efficiency_label[i]->setHidden(true);
    Calculate_Efficiency_result[i]->setHidden(true);
    Calculate_Efficiency_result[i]->setText(QString::number(0));
    Calculate_Efficiency_error[i]->setHidden(true);
    Calculate_Efficiency_error[i]->setText(QString::number(0));
    Calculate_Efficiency_error_label[i]->setHidden(true);
    for (size_t j = 0; j < FFunctions[i].Number_of_Parameters; j++)
      FFunctions[i].Parameters_Values[j] = 1.;
  }
  Editor_window->clear();

  std::vector<double> activity, activity_error, measurement_time;
  for (size_t i = 0; i < Selected_Source.size(); i++) {
    activity.push_back(source_input_activity[i]->value());
    activity_error.push_back(source_input_activity_error[i]->value());
    measurement_time.push_back(source_input_time[i]->value());
    if (activity[i] == 0) {
      error_code(0);
      return;
    }
    if (measurement_time[i] == 0) {
      error_code(1);
      return;
    }
  }
  if (Experimental_Data[0][0] == 0) {
    error_code(10);
    return;
  }
  for (size_t i = 0; i < Experimental_Data.size(); i++) {
    std::vector<int> found_in_sources, number_of_gamma_in_source;
    for (size_t k = 0; k < Selected_Source.size(); k++) {
      for (size_t j = 0; j < Sources[Selected_Source[k]].Gammas.size(); j++) {
        if (abs(Experimental_Data[i][0] -
                Sources[Selected_Source[k]].Gammas[j]) < 2.) {
          found_in_sources.push_back(k);
          number_of_gamma_in_source.push_back(j);
        }
        if (Experimental_Data[i][0] <
            (Sources[Selected_Source[k]].Gammas[j] + 2.))
          break;
      }
    }
    if (found_in_sources.size() > 1) {
      QStringList items;
      bool ok;
      QString tex = "Data point of energy " +
                    QString::number(Experimental_Data[i][0]) +
                    " keV could belong to more than one source.\nPlease select "
                    "the source that it was measured for.";
      for (size_t j = 0; j < found_in_sources.size(); j++)
        items << QString::fromStdString(
            Sources[Selected_Source[found_in_sources[j]]].Source_Name);
      QString ret = QInputDialog::getItem(this, tr("Source selection"), tex,
                                          items, 0, false, &ok);
      if (ok) {
        for (size_t j = 0; j < found_in_sources.size(); j++)
          if (QString::fromStdString(
                  Sources[Selected_Source[found_in_sources[j]]].Source_Name) ==
              ret) {
            found_in_sources.clear();
            found_in_sources.push_back(j);
            int dummy = number_of_gamma_in_source[j];
            number_of_gamma_in_source.clear();
            number_of_gamma_in_source.push_back(dummy);
            break;
          }
      }
    }
    if (found_in_sources.size() == 1) {
      std::vector<double> data;
      data.push_back(Sources[Selected_Source[found_in_sources[0]]]
                         .Gammas[number_of_gamma_in_source[0]]);
      data.push_back(Experimental_Data[i][1] /
                     (measurement_time[found_in_sources[0]] *
                      activity[found_in_sources[0]] *
                      (Sources[Selected_Source[found_in_sources[0]]]
                           .Intensities[number_of_gamma_in_source[0]] /
                       100.)));
      data.push_back(
          data[1] *
          sqrt((pow(Experimental_Data[i][2] / Experimental_Data[i][1], 2)) +
               (pow(Sources[Selected_Source[found_in_sources[0]]]
                            .Intensities_Errors[number_of_gamma_in_source[0]] /
                        Sources[Selected_Source[found_in_sources[0]]]
                            .Intensities[number_of_gamma_in_source[0]],
                    2)) +
               (pow(activity_error[found_in_sources[0]] /
                        activity[found_in_sources[0]],
                    2))));
      Data_to_fit.push_back(data);
    }
  }

  if (Experimental_Data.size() > Data_to_fit.size())
    error_code(3);
  Fit_button->setDisabled(false);
  Repeat_Fit_button->setDisabled(true);
  return;
}

void MainWindow::func_choice() {
  Selected_Function = Function_Select_box->currentIndex();
  Function_Name_label->setText(
      QString::fromStdString(FFunctions[Selected_Function].Function_Name));
  Fit_button->setDisabled(FFunctions[Selected_Function].is_fitted);
}

void MainWindow::fit_func() {
  if (Data_to_fit.size() == 0) {
    error_code(10);
    return;
  }
  if (Data_to_fit.size() < FFunctions[Selected_Function].Number_of_Parameters) {
    error_code(11);
    return;
  }
  const gsl_multifit_fdfsolver_type *T;
  gsl_multifit_fdfsolver *s;
  int status;
  unsigned int iter = 0;
  const size_t n = Data_to_fit.size();
  const size_t p = FFunctions[Selected_Function].Number_of_Parameters;

  gsl_matrix *covar = gsl_matrix_alloc(p, p);
  double t[n], y[n], sigma[n];
  for (size_t i = 0; i < Data_to_fit.size(); i++) {
    t[i] = Data_to_fit[i][0];
    y[i] = Data_to_fit[i][1];
    sigma[i] = Data_to_fit[i][2];
  }
  struct data d = {n, t, y, sigma};
  gsl_multifit_function_fdf f;
  double x_init[p];
  for (size_t i = 0; i < FFunctions[Selected_Function].Number_of_Parameters;
       i++) {
    if (Repeat_Fit)
      x_init[i] = FFunctions[Selected_Function].Parameters_Values[i];
    else
      x_init[i] = 1.;
  }
  FFunctions[Selected_Function].X_for_Plot.clear();
  FFunctions[Selected_Function].Y_for_Plot.clear();
  FFunctions[Selected_Function].Confidence_Band.clear();

  f.f = FFunctions[Selected_Function].f;
  //   f.f =
  //   boost::bind(&fit_function::f1,&FFunctions[Selected_Function],_1,_2,_3);
  f.df = FFunctions[Selected_Function].df;
  f.fdf = FFunctions[Selected_Function].fdf;

  //   Editor_window->clear();
  f.n = n;
  f.p = p;
  f.params = &d;
  T = gsl_multifit_fdfsolver_lmsder;
  s = gsl_multifit_fdfsolver_alloc(T, n, p);

  gsl_vector *x = gsl_vector_alloc(p);
  for (size_t kola = 0; kola < p; kola++)
    gsl_vector_set(x, kola, x_init[kola]);

  gsl_multifit_fdfsolver_set(s, &f, x);
  do {
    iter++;
    status = gsl_multifit_fdfsolver_iterate(s);
    if (status != GSL_SUCCESS && status != GSL_ENOPROG)
      break;
    status = gsl_multifit_test_delta(s->dx, s->x, 1e-4, 1e-4);
  } while (status == GSL_CONTINUE && iter < 500);
  if (status != GSL_SUCCESS && status != GSL_ETOLF && status != GSL_ETOLX &&
      status != GSL_ETOLG) {
    Editor_window->append(
        "Fit results for " +
        QString::fromStdString(FFunctions[Selected_Function].Function_Name) +
        " function.\n");
    Editor_window->append(tr(gsl_strerror(status)));
    Repeat_Fit_button->setDisabled(false);
  } else {
    Editor_window->append(
        "Fit results for " +
        QString::fromStdString(FFunctions[Selected_Function].Function_Name) +
        " function.\n");
    if (status == GSL_SUCCESS)
      Editor_window->append("Fit " + tr(gsl_strerror(status)) + " in " +
                            QString::number(iter) + " iterations.\n");
    else
      Editor_window->append(
          "The solution has converged to within machine precision, but " +
          tr(gsl_strerror(status)) + " after " + QString::number(iter) +
          " iterations.\n");
    if (FFunctions[Selected_Function].is_fitted == false) {
      Plot_Fit_select[Number_of_Fits]->setText(
          QString::fromStdString(FFunctions[Selected_Function].Function_Name));
      FFunctions[Selected_Function].is_fitted = true;
      Fit_Order[Number_of_Fits] = Selected_Function;
      Plot_Fit_select[Number_of_Fits]->setHidden(false);
      Plot_Confidence_select[Number_of_Fits]->setText(
          tr("95% Confidence Band"));
      Plot_Confidence_select[Number_of_Fits]->setHidden(false);
      Calculate_Efficiency_label[Number_of_Fits]->setText(
          QString::fromStdString(FFunctions[Selected_Function].Function_Name) +
          " efficiency:");
      Calculate_Efficiency_label[Number_of_Fits]->setHidden(false);
      Calculate_Efficiency_result[Number_of_Fits]->setHidden(false);
      Calculate_Efficiency_error[Number_of_Fits]->setHidden(false);
      Calculate_Efficiency_error_label[Number_of_Fits]->setHidden(false);
      Number_of_Fits++;
      Fit_button->setDisabled(true);
      Plot_button->setDisabled(false);
      Calculate_button->setDisabled(false);
    }
  }
#if GSL_MAJOR_VERSION >= 2
  gsl_matrix *J = gsl_matrix_alloc(s->fdf->n, s->fdf->p);
  gsl_multifit_fdfsolver_jac(s, J);
  gsl_multifit_covar(J, 0.0, covar);
#else
  gsl_multifit_covar(s->J, 0.0, covar);
#endif

  for (size_t k = 0; k < p; k++) {
    FFunctions[Selected_Function].Parameters_Values[k] =
        gsl_vector_get(s->x, k);
    for (size_t kk = 0; kk < p; kk++)
      FFunctions[Selected_Function].Covariance_Matrix[k][kk] =
          gsl_matrix_get(covar, k, kk);
  }

  FFunctions[Selected_Function].Chi2 = gsl_blas_dnrm2(s->f);
  FFunctions[Selected_Function].Degrees_of_Freedom = n - p;

  QString result_line;
  for (size_t i = 0; i < FFunctions[Selected_Function].Number_of_Parameters;
       i++) {
    result_line =
        "A" + QString::number(i + 1) + "= " +
        QString::number(FFunctions[Selected_Function].Parameters_Values[i]);
    result_line = result_line + " +/- " +
                  QString::number(sqrt(
                      FFunctions[Selected_Function].Covariance_Matrix[i][i]));
    Editor_window->append(result_line);
  }

  result_line =
      "\n chisq/dof= " +
      QString::number(pow(FFunctions[Selected_Function].Chi2, 2.0) /
                      FFunctions[Selected_Function].Degrees_of_Freedom);
  Editor_window->append(result_line);
  do {
    if (FFunctions[Selected_Function].X_for_Plot.size() > 0)
      FFunctions[Selected_Function].X_for_Plot.push_back(
          5. +
          FFunctions[Selected_Function]
              .X_for_Plot[FFunctions[Selected_Function].X_for_Plot.size() - 1]);
    else
      FFunctions[Selected_Function].X_for_Plot.push_back(t[0] * 0.8);
    FFunctions[Selected_Function].Y_for_Plot.push_back(
        FFunctions[Selected_Function].Fit_Function(
            FFunctions[Selected_Function]
                .X_for_Plot[FFunctions[Selected_Function].X_for_Plot.size() -
                            1],
            FFunctions[Selected_Function].Parameters_Values));
    FFunctions[Selected_Function].Confidence_Band.push_back(conf_band(
        Selected_Function, p,
        FFunctions[Selected_Function]
            .X_for_Plot[FFunctions[Selected_Function].X_for_Plot.size() - 1]));

  } while (
      FFunctions[Selected_Function]
          .X_for_Plot[FFunctions[Selected_Function].X_for_Plot.size() - 1] <
      t[Data_to_fit.size() - 1] * 1.2);

  // Ypologismos R^2
  double sum = 0;
  for (size_t i = 0; i < Data_to_fit.size(); i++)
    sum += Data_to_fit[i][1];
  double yaver = sum / Data_to_fit.size();
  sum = 0;
  for (size_t i = 0; i < Data_to_fit.size(); i++)
    sum += pow((Data_to_fit[i][1] - yaver), 2);

  double sum1 = 0;
  for (size_t i = 0; i < Data_to_fit.size(); i++)
    sum1 += pow((Data_to_fit[i][1] -
                 FFunctions[Selected_Function].Fit_Function(
                     Data_to_fit[i][0],
                     FFunctions[Selected_Function].Parameters_Values)),
                2);

  result_line = "\n RSq= " + QString::number(1 - sum1 / sum);
  Editor_window->append(result_line);
  Editor_window->append("");
  Editor_window->append("------------------------------------");
  Editor_window->append("");
  Repeat_Fit = false;
  gsl_multifit_fdfsolver_free(s);
}

void MainWindow::fit_repeat() {
  Repeat_Fit = true;
  fit_func();
}

void MainWindow::calc_eff() {
  if (Gamma_input->value() == 0) {
    error_code(0);
    return;
  }
  for (int i = 0; i < Number_of_Fits; i++) {
    double eff, conf;
    eff = FFunctions[Fit_Order[i]].Fit_Function(
        Gamma_input->value(), FFunctions[Fit_Order[i]].Parameters_Values);
    conf =
        conf_band(Fit_Order[i], FFunctions[Fit_Order[i]].Number_of_Parameters,
                  Gamma_input->value());
    conf = conf / eff * 100;
    Calculate_Efficiency_result[i]->setText(QString::number(eff));
    Calculate_Efficiency_error[i]->setText(QString::number(conf));
  }
  return;
}

void MainWindow::plot() {
  for (int i = 0; i < Number_of_Fits; i++) {
    if (Plot_Fit_select[i]->isChecked() == false &&
        Plot_Confidence_select[i]->isChecked() == true) {
      Plot_Confidence_select[i]->setChecked(false);
    }
  }
  for (int i = 0; i < Number_of_Fits; i++) {
    if (Plot_Fit_select[i]->isChecked() == true) {
      FFunctions[Fit_Order[i]].Plot_Fit = true;
    } else {
      FFunctions[Fit_Order[i]].Plot_Fit = false;
    }
    if (Plot_Confidence_select[i]->isChecked() == true) {
      FFunctions[Fit_Order[i]].Plot_Confidence_Band = true;
    } else {
      FFunctions[Fit_Order[i]].Plot_Confidence_Band = false;
    }
  }

  if (Plot_Window_Open == true) {
    lila->close();
  }
  lila = new newwindow(this); // Be sure to destroy you window somewhere
  lila->show();
  Plot_Window_Open = true;
}

void MainWindow::closeEvent(QCloseEvent *) {
  if (Plot_Window_Open == true) {
    lila->close();
  }
}

void MainWindow::createStatusBar() { statusBar()->showMessage(tr("Ready")); }

void MainWindow::readSettings() { resize(QSize(600, 500)); }

void MainWindow::writeSettings() {
  QSettings settings("Trll", "Applicati");
  settings.setValue("pos", pos());
  settings.setValue("size", size());
}

void MainWindow::error_code(int code) {
  QMessageBox msgBox;
  switch (code) {
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
  case 2:
    msgBox.setWindowTitle("Source Error");
    msgBox.setText("You have multiple times the same source");
    msgBox.setIcon(QMessageBox::Critical);
    break;
  case 3:
    msgBox.setWindowTitle("Data Warning");
    msgBox.setText(
        "Warning: There are experimental points that will not be fitted!!!");
    msgBox.setIcon(QMessageBox::Warning);
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
    msgBox.setText(
        "Please choose correct source or add data points to be fitted");
    msgBox.setIcon(QMessageBox::Critical);
    break;
  case 12:
    msgBox.setWindowTitle("No sources file");
    msgBox.setText("Place sources file in directory and start again");
    msgBox.setIcon(QMessageBox::Critical);
  }
  msgBox.exec();
}

double MainWindow::conf_band(int function, int p, double egamma) {
  double t_test[99] = {
      12.706204736174698, 4.302652729749464,  3.182446305283708,
      2.7764451051977934, 2.570581835636314,  2.4469118511449666,
      2.3646242515927853, 2.306004135204168,  2.262157162798205,
      2.2281388519862735, 2.2009851600916384, 2.178812829667226,
      2.1603686564627917, 2.1447866879178012, 2.131449545559774,
      2.1199052992212533, 2.1098155778333156, 2.100922040241039,
      2.093024054408307,  2.0859634472658626, 2.0796138447276835,
      2.073873067904019,  2.0686576104190477, 2.0638985616280254,
      2.0595385527532963, 2.05552943864287,   2.051830516480281,
      2.048407141795243,  2.0452296421327034, 2.042272456301236,
      2.039513446396408,  2.0369333434600976, 2.0345152974493392,
      2.032244509317719,  2.030107928250338,  2.0280940009804462,
      2.0261924630291066, 2.024394163911966,  2.022690920036762,
      2.0210753903062715, 2.0195409704413745, 2.018081702818439,
      2.016692199227822,  2.0153675744437627, 2.0141033888808457,
      2.0128955989194246, 2.011740513729764,  2.0106347576242314,
      2.0095752371292335, 2.0085591121007527, 2.007583770315835,
      2.0066468050616857, 2.005745995317864,  2.0048792881880577,
      2.004044783289136,  2.0032407188478696, 2.002465459291016,
      2.001717484145232,  2.000995378088259,  2.0002978220142578,
      1.9996235849949402, 1.998971517033376,  1.9983405425207483,
      1.997729654317692,  1.9971379083920013, 1.9965644189523084,
      1.996008354025304,  1.9954689314298386, 1.994945415107228,
      1.9944371117711894, 1.9939433678456229, 1.993463566661884,
      1.9929971258898527, 1.9925434951809258, 1.992102154002232,
      1.9916726096446793, 1.9912543953883763, 1.9908470688116922,
      1.9904502102301198, 1.990063421254452,  1.989686323456895,
      1.9893185571365664, 1.9889597801751728, 1.9886096669757192,
      1.9882679074772156, 1.9879342062390228, 1.9876082815890748,
      1.9872898648311672, 1.9869786995062702, 1.986674540703777,
      1.986377154418625,  1.9860863169510985, 1.9858018143458114,
      1.9855234418666061, 1.9852510035054973, 1.9849843115224508,
      1.9847231860139618, 1.98446745450849,   1.9842169515863888};

  gsl_matrix *sigy = gsl_matrix_alloc(1, 1);

  gsl_matrix *cov = gsl_matrix_alloc(p, p);
  for (int k = 0; k < p; k++) {
    for (int kk = 0; kk < p; kk++) {
      gsl_matrix_set(cov, k, kk, FFunctions[function].Covariance_Matrix[k][kk]);
    }
  }
  gsl_matrix *fro = gsl_matrix_alloc(1, p);
  gsl_matrix *gamma = gsl_matrix_alloc(1, p);

  for (size_t i = 0; i < FFunctions[function].Number_of_Parameters; i++) {
    double *ParVal = new double[FFunctions[function].Number_of_Parameters];
    for (size_t j = 0; j < FFunctions[function].Number_of_Parameters; j++) {
      if (i == j)
        ParVal[j] = 1.;
      else
        ParVal[j] = 0.;
    }
    gsl_matrix_set(gamma, 0, i,
                   FFunctions[function].Derivative(
                       i, egamma, FFunctions[function].Parameters_Values));
    delete[] ParVal;
  }

  gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, gamma, cov, 0.0, fro);
  gsl_blas_dgemm(CblasNoTrans, CblasTrans, 1.0, fro, gamma, 0.0, sigy);

  double llalla = gsl_matrix_get(sigy, 0, 0);
  gsl_matrix_free(gamma);
  gsl_matrix_free(cov);
  gsl_matrix_free(fro);
  gsl_matrix_free(sigy);
  return t_test[FFunctions[function].Degrees_of_Freedom] *
         sqrt(pow(FFunctions[function].Chi2, 2) * sqrt(pow(llalla, 2)));
}
