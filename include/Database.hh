#ifndef DB_H
#define DB_H 1

#include <fstream>
#include <sstream>
#include <string>
#include <vector>
//   #include <iostream>

#include <gsl/gsl_blas.h>
#include <gsl/gsl_multifit_nlin.h>

#include <QApplication>
#include <QDebug>
#include <QFileDialog>
#include <QString>
//   #include <boost/function.hpp>
//   #include <boost/bind.hpp>
#define XSTR(x) STR(x)
#define STR(x) #x

struct data {
   size_t n;
   double *t;
   double *yy;
   double *sigma;
};

typedef int (*f_function_type)(const gsl_vector *x, void *params, gsl_vector *f);
typedef int (*df_function_type)(const gsl_vector *x, void *params, gsl_matrix *J);
typedef int (*fdf_function_type)(const gsl_vector *x, void *params, gsl_vector *f, gsl_matrix *J);
typedef double (*normal_function)(double, double *);
typedef double (*derivative_function)(int, double, double *);

class source {
public:
   std::string Source_Name;
   double Lifetime;
   double Lifetime_Error;
   std::vector<double> Gammas;
   std::vector<double> Gammas_Errors;
   std::vector<double> Intensities;
   std::vector<double> Intensities_Errors;

public:
   source();
   source(std::string, double, double, std::vector<double>, std::vector<double>, std::vector<double>, std::vector<double>);
};

std::vector<source> create_sources();

class fit_function {
public:
   std::string Function_Name;
   std::string Function_Picture;
   bool is_fitted;
   bool Plot_Fit;
   bool Plot_Confidence_Band;
   unsigned int Number_of_Parameters;
   double *Parameters_Values;
   double *Parameters_Errors;
   std::vector<double> X_for_Plot;
   std::vector<double> Y_for_Plot;
   std::vector<double> Confidence_Band;
   double **Covariance_Matrix;
   int Degrees_of_Freedom;
   double Chi2;

   f_function_type f;
   df_function_type df;
   fdf_function_type fdf;
   int f1(const gsl_vector *x, void *params, gsl_vector *f);
   int df1(const gsl_vector *x, void *params, gsl_matrix *J);
   int fdf1(const gsl_vector *x, void *params, gsl_vector *f, gsl_matrix *J);
   normal_function Fit_Function;
   derivative_function Derivative;

   fit_function();
   fit_function(std::string, std::string, int, double data[], f_function_type, df_function_type, fdf_function_type, normal_function, derivative_function);
   fit_function(std::string, std::string, int, double data[], normal_function, derivative_function);
};

std::vector<fit_function> create_functions();

#endif
