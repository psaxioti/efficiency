#ifndef NEWWINDOW_H
#define NEWWINDOW_H 1

#include <QDialog>
#include <QMainWindow>
#include <QStatusBar>
#include <QtGui>

#include <fstream>
#include <iomanip>
#include <iostream>
#include <vector>

#include <gsl/gsl_blas.h>
#include <gsl/gsl_multifit_nlin.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_version.h>

#include "mainwindow.hh"
#include <algorithm>

QT_BEGIN_NAMESPACE
class QAction;
class QDialogButtonBox;
class QGroupBox;
class QLabel;
class QLineEdit;
class QMenu;
class QMenuBar;
class QPushButton;
class QTextEdit;
class QSpinBox;
class QStatusBar;
class QComboBox;
class QMessageBox;
class QIcon;
class QSize;
class QDialog;
class QColor;
QT_END_NAMESPACE

class MainWindow;

class newwindow : public QMainWindow {
   Q_OBJECT

public:
   explicit newwindow(class MainWindow *testkk);
   void resizeEvent(QResizeEvent *event);

protected:
   void closeEvent(QCloseEvent *event);

private slots:

private:
   void file_error();
   void setStatusBar();
   void createMenu();
   void createHorizontalGroupBox();
   void createGridGroupBox();
   void createFormGroupBox();
   void writeSettings();
   void readSettings();
   void loadFile(const QString &fileName);
   void setCurrentFile(const QString &fileName);
   void createStatusBar();

   enum { NumGridRows = 3,
          NumButtons = 4 };

   QString curFile;
   QMenuBar *menuBar;
   QGroupBox *horizontalGroupBox;
   QGroupBox *gridGroupBox;
   QGroupBox *formGroupBox;
   QTextEdit *smallEditor;
   QTextEdit *bigEditor;
   QLabel *labels[NumGridRows];
   QLineEdit *lineEdits[NumGridRows];
   QPushButton *butt, *open_button, *fit_button, *draw_button;
   QDialogButtonBox *buttonBox;
   QSpinBox *lala, *eff_spin;
   QMenu *fileMenu;
   QAction *exitAction;
   QComboBox *func_combo;
   QMessageBox *msgBox;
   QIcon *func_icon;
   QDialog *plotWin;
   QColor *Colours[5];
};
#endif // NEWWINDOW_H
