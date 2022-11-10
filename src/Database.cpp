#include "Database.hh"

source :: source() {
}

source :: source(std::string name, double Lt, double Lt_err, std::vector < double > en, std::vector < double > en_err, std::vector < double > ig, std::vector < double > ig_err) {
   Source_Name = name;
   Lifetime=Lt;
   Lifetime_Error=Lt_err;
   Gammas = en;
   Gammas_Errors = en_err;
   Intensities = ig;
   Intensities_Errors = ig_err;
}

fit_function::fit_function() {
}

fit_function::fit_function(std::string name, std::string picture, int param, double data[], f_function_type f1, df_function_type df1, fdf_function_type fdf1, normal_function ff1, derivative_function ffdf1) {
   Function_Name = name;
   Function_Picture = picture;
   Number_of_Parameters = param;
   Parameters_Values = new double [Number_of_Parameters];
   Parameters_Errors = new double [Number_of_Parameters];
   for( int i=0 ; i<Number_of_Parameters ; i++ ) Parameters_Values[i] = data[i];
   Covariance_Matrix = new double*[Number_of_Parameters];
   for( int i=0 ; i<Number_of_Parameters ; i++ ) Covariance_Matrix[i] = new double[Number_of_Parameters];
   f = f1;
   df = df1;
   fdf = fdf1;
   Fit_Function = ff1;
   Derivative = ffdf1;
   is_fitted = false;
   Plot_Fit = false;
   Plot_Confidence_Band = false;
}

fit_function::fit_function(std::string name, std::string picture, int param, double data[], normal_function f1, derivative_function df1) {
   Function_Name = name;
   Function_Picture = picture;
   Number_of_Parameters = param;
   Parameters_Values = new double [Number_of_Parameters];
   Parameters_Errors = new double [Number_of_Parameters];
   for( int i=0 ; i<Number_of_Parameters ; i++ ) Parameters_Values[i] = data[i];
   Covariance_Matrix = new double*[Number_of_Parameters];
   for( int i=0 ; i<Number_of_Parameters ; i++ ) Covariance_Matrix[i] = new double[Number_of_Parameters];
   Fit_Function = f1;
   Derivative = df1;
   is_fitted = false;
   Plot_Fit = false;
   Plot_Confidence_Band = false;
}

int fit_function::f1(const gsl_vector * x, void *params, gsl_vector * f) {
   size_t n = ((struct data *) params)->n;
   double *t = ((struct data *) params)->t;
   double *yy = ((struct data *) params)->yy;
   double *sigma = ((struct data *) params)->sigma;
   
   double p[Number_of_Parameters];
   double Yi;
   for ( int i = 0 ; i<Number_of_Parameters ; i++ ) p[i] = gsl_vector_get(x, i);

   size_t i;

   for ( i = 0 ; i < n ; i++ ) {
      Yi = Fit_Function(t[i],p);
      gsl_vector_set(f, i, (Yi - yy[i]) / sigma[i]);
   }
   return GSL_SUCCESS;
}

int fit_function::df1(const gsl_vector * x, void *params, gsl_matrix * J) {
   size_t n = ((struct data *) params)->n;
   double *t = ((struct data *) params)->t;
   double *sigma = ((struct data *) params)->sigma;
   
   double p[Number_of_Parameters];
   for ( int i = 0 ; i<Number_of_Parameters ; i++ ) p[i] = gsl_vector_get(x, i);
   size_t i;

   for ( i = 0 ; i < n ; i++ ) {
      for ( size_t j=0 ; j < n ;j++ ) {
         gsl_matrix_set(J, i, j, Derivative(j,t[i],p)/sigma[i]);
      }
   }
   return GSL_SUCCESS;
}

int fit_function::fdf1(const gsl_vector * x, void *params, gsl_vector * f, gsl_matrix * J) {
   fit_function::f1(x, params, f);
   fit_function::df1(x, params, J);

   return GSL_SUCCESS;
}

std::vector < source > create_sources() {
   double Lifetime=0., Lifetime_error=0.;
   std::string Source_Name;
   std::vector < source > test;
   std::vector < double > energies, energies_err, Intensity, Intensity_err;
   
   std::string line, datapath, filename;
   std::ifstream sources;
   #if defined(_WIN32) || defined(CUSTOM) || defined(__APPLE__)
      datapath=QApplication::applicationDirPath().toStdString().c_str();
      datapath.append("/Data/");
   #elif defined(__linux__)
      datapath="/usr/local/share/Efficiency/";
   #endif
   filename=datapath+"Sources_DB.lib";

   sources.open(filename.c_str());
   if( !sources.is_open() ) {
      QString fileName = QFileDialog::getOpenFileName(0,"Open file with sources");
      sources.open(fileName.toUtf8().constData());
      if( !sources.is_open() ) return test;
   }
   
   while(!sources.eof()){
      char c=sources.peek();
      double en, en_err, ig, ig_err;
      if(c=='#' || c=='\n' || c==EOF){
         sources.ignore(512, '\n');
         if(energies.size()>0.){
            source test1(Source_Name,Lifetime,Lifetime_error,energies,energies_err,Intensity,Intensity_err);
            test.push_back(test1);
            Lifetime=0.;
            energies.clear();
            energies_err.clear();
            Intensity.clear();
            Intensity_err.clear();
         }
         continue;
      }
      getline(sources, line);
      std::istringstream iss(line);
      if(Lifetime==0.) iss>>Source_Name>>Lifetime>>Lifetime_error;
      else {
         iss>>en>>en_err>>ig>>ig_err;
         energies.push_back(en);
         energies_err.push_back(en_err);
         Intensity.push_back(100*ig);
         Intensity_err.push_back(100*ig_err);
      }
   }

   sources.close();
   return test;
}

std::vector < fit_function > create_functions() {
   std::vector < fit_function > test;
   std::string name,picture,datapath;
   int no_parameters;
   #if defined(_WIN32) || defined(CUSTOM) || defined(__APPLE__)
      datapath=QApplication::applicationDirPath().toStdString().c_str();
      datapath.append("/Data/");
   #elif defined(__linux__)
      datapath="/usr/local/share/Efficiency/";
   #endif
   datapath.append("Icons/");

   {
      name = "Debertin";
      picture = datapath+name+".png";
      no_parameters = 5;
      double xinit[] = { 1. , 1. , 1. , 1. , 1. };
      int Debertin_f (const gsl_vector * x, void *params, gsl_vector * f);;
      f_function_type f = &Debertin_f;
      int Debertin_df (const gsl_vector * x, void *params, gsl_matrix * J);
      df_function_type df = &Debertin_df;
      int Debertin_fdf (const gsl_vector * x, void *params, gsl_vector * f, gsl_matrix * J);
      fdf_function_type fdf = &Debertin_fdf;
      double Debertin (double x, double * param);
      normal_function ff = &Debertin;
      double Debertin_Derivative (int i, double x, double * param);
      derivative_function ffdf1 = &Debertin_Derivative;
      fit_function test1(name,picture,no_parameters,xinit,f,df,fdf,ff,ffdf1);
      test.push_back(test1);
   }
   {
      name = "IAEA";
      picture = datapath+name+".png";
      no_parameters = 4;
      double xinit[] = { 1. , 1. , 1. , 1. };
      int IAEA_f (const gsl_vector * x, void *params, gsl_vector * f);
      f_function_type f = &IAEA_f;
      int IAEA_df (const gsl_vector * x, void *params, gsl_matrix * J);
      df_function_type df = &IAEA_df;
      int IAEA_fdf (const gsl_vector * x, void *params, gsl_vector * f, gsl_matrix * J);
      fdf_function_type fdf = &IAEA_fdf;
      double IAEA (double x, double * param);
      normal_function ff = &IAEA;
      double IAEA_Derivative (int i, double x, double * param);
      derivative_function ffdf1 = &IAEA_Derivative;
      fit_function test1(name,picture,no_parameters,xinit,f,df,fdf,ff,ffdf1);
      test.push_back(test1);
   }
   {
      name = "Kalfas";
      picture = datapath+name+".png";
      no_parameters = 4;
      double xinit[] = { 1. , 1. , 1. , 1. };
      int Kalfas_f (const gsl_vector * x, void *params, gsl_vector * f);
      f_function_type f = &Kalfas_f;
      int Kalfas_df (const gsl_vector * x, void *params, gsl_matrix * J);
      df_function_type df = &Kalfas_df;
      int Kalfas_fdf (const gsl_vector * x, void *params, gsl_vector * f, gsl_matrix * J);
      fdf_function_type fdf = &Kalfas_fdf;
      double Kalfas (double x, double * param);
      normal_function ff = &Kalfas;
      double Kalfas_Derivative (int i, double x, double * param);
      derivative_function ffdf1 = &Kalfas_Derivative;
      fit_function test1(name,picture,no_parameters,xinit,f,df,fdf,ff,ffdf1);
      test.push_back(test1);
   }
   return test;
}

///////////////////////////////////////////////////////////////////////

double Debertin (double x, double * param) {
   double lnt = log(x);
   double Yi = param[0]*lnt + (param[1]*lnt + param[2] * pow(lnt,2.)+param[3]*pow(lnt,4.)+param[4]*pow(lnt,5.))/x;
   return Yi;
}

///////////////////////////////////////////////////////////////////////

double Debertin_Derivative (int i, double x, double * param) {
   double lnt = log(x);
   double Yi;
   if( i==0 )      Yi = lnt;
   else if( i==1 ) Yi = lnt/x;
   else if( i==2 ) Yi = pow(lnt,2)/x;
   else if( i==3 ) Yi = pow(lnt,4)/x;
   else if( i==4 ) Yi = pow(lnt,5)/x;
   return Yi;
}

///////////////////////////////////////////////////////////////////////

int Debertin_f (const gsl_vector * x, void *params, gsl_vector * f) {
   size_t n = ((struct data *) params)->n;
   double *t = ((struct data *) params)->t;
   double *yy = ((struct data *) params)->yy;
   double *sigma = ((struct data *) params)->sigma;

   double A = gsl_vector_get (x, 0);
   double B = gsl_vector_get (x, 1);
   double C = gsl_vector_get (x, 2);
   double D = gsl_vector_get (x, 3);
   double E = gsl_vector_get (x, 4);

   size_t i;

   for ( i = 0 ; i < n ; i++ ) {
      double lnt = log(t[i]);
      double Yi = A*lnt + 1./t[i]*(B*lnt + C * pow(lnt,2.)+D*pow(lnt,4.)+E*pow(lnt,5.));
      gsl_vector_set (f, i, (Yi - yy[i]) / sigma[i]);
   }
   return GSL_SUCCESS;
}

///////////////////////////////////////////////////////////////////////

int Debertin_df (const gsl_vector * x, void *params, gsl_matrix * J) {
   size_t n = ((struct data *) params)->n;
   double *t = ((struct data *) params)->t;
   double *sigma = ((struct data *) params)->sigma;
 
   size_t i;

   for ( i = 0 ; i < n ; i++ ) {
      double ener = t[i];
      double s = sigma[i];
      double lnt = log(ener);

      gsl_matrix_set (J, i, 0, lnt/s);
      gsl_matrix_set (J, i, 1, lnt/ener/s);
      gsl_matrix_set (J, i, 2, pow(lnt,2)/ener/s);
      gsl_matrix_set (J, i, 3, pow(lnt,4)/ener/s);
      gsl_matrix_set (J, i, 4, pow(lnt,5)/ener/s);

   }
   return GSL_SUCCESS;
}

///////////////////////////////////////////////////////////////////////

int Debertin_fdf(const gsl_vector * x, void *params, gsl_vector * f, gsl_matrix * J) {
   Debertin_f(x, params, f);
   Debertin_df(x, params, J);

   return GSL_SUCCESS;
}

///////////////////////////////////////////////////////////////////////

double IAEA (double x, double * param) {
   double Yi = param[0] + param[1] /x + param[2] /pow(x,2.0) + param[3] /pow(x,3.0);
   return Yi;
}

///////////////////////////////////////////////////////////////////////

double IAEA_Derivative (int i, double x, double * param) {
   double Yi;
   if( i==0 )      Yi = 1;
   else if( i==1 ) Yi = 1/x;
   else if( i==2 ) Yi = 1/(x*x);
   else if( i==3 ) Yi = 1/(x*x*x);
   return Yi;
}

///////////////////////////////////////////////////////////////////////

int IAEA_f(const gsl_vector * x, void *params, gsl_vector * f) {
   size_t n = ((struct data *) params)->n;
   double *t = ((struct data *) params)->t;
   double *yy = ((struct data *) params)->yy;
   double *sigma = ((struct data *) params)->sigma;

   double A = gsl_vector_get (x, 0);
   double B = gsl_vector_get (x, 1);
   double C = gsl_vector_get (x, 2);
   double D = gsl_vector_get (x, 3);

   size_t i;

   for ( i = 0 ; i < n ; i++ ) {
      double Yi = A + B /t[i] + C /pow(t[i],2.0) + D /pow(t[i],3.0);
      gsl_vector_set (f, i, (Yi - yy[i]) / sigma[i]);
   }
  return GSL_SUCCESS;
}

///////////////////////////////////////////////////////////////////////

int IAEA_df(const gsl_vector * x, void *params, gsl_matrix * J) {
   size_t n = ((struct data *) params)->n;
   double *t = ((struct data *) params)->t;
   double *sigma = ((struct data *) params)->sigma;

   size_t i;

   for ( i = 0 ; i < n ; i++ ) {
      double tas = t[i];
      double s = sigma[i];

      gsl_matrix_set (J, i, 0, 1 / s);
      gsl_matrix_set (J, i, 1, 1 / (s*tas));
      gsl_matrix_set (J, i, 2, 1 / (s*tas*tas));
      gsl_matrix_set (J, i, 3, 1 / (s*tas*tas*tas));
   }
   return GSL_SUCCESS;
}

///////////////////////////////////////////////////////////////////////

int IAEA_fdf(const gsl_vector * x, void *params, gsl_vector * f, gsl_matrix * J) {
   IAEA_f (x, params, f);
   IAEA_df (x, params, J);

   return GSL_SUCCESS;
}

///////////////////////////////////////////////////////////////////////

double Kalfas (double x, double * param) {
   double Yi = param[0]*pow(x,param[1])/(1000*param[2]+pow(x,param[3]));
   return Yi;
}

///////////////////////////////////////////////////////////////////////

double Kalfas_Derivative (int i, double x, double * param) {
   double Yi;
   if( i==0 )      Yi = pow(x,param[1])/(1000*param[2]+pow(x,param[3]));
   else if( i==1 ) Yi = param[0]*pow(x,param[1])*log(x)/(1000*param[2]+pow(x,param[3]));
   else if( i==2 ) Yi = -1000*param[0]*pow(x,param[1])/pow((1000*param[2]+pow(x,param[3])),2);
   else if( i==3 ) Yi = -param[0]*pow(x,param[1])*pow(x,param[3])*log(x)/pow(1000*param[2]+pow(x,param[3]),2);
   return Yi;
}

///////////////////////////////////////////////////////////////////////

int Kalfas_f(const gsl_vector * x, void *params, gsl_vector * f) {
   size_t n = ((struct data *) params)->n;
   double *t = ((struct data *) params)->t;
   double *yy = ((struct data *) params)->yy;
   double *sigma = ((struct data *) params)->sigma;

   double A = gsl_vector_get (x, 0);
   double B = gsl_vector_get (x, 1);
   double C = gsl_vector_get (x, 2);
   double D = gsl_vector_get (x, 3);

   size_t i;

   for ( i = 0 ; i < n ; i++ ) {
      double Yi = A*pow(t[i],B)/(1000*C+pow(t[i],D));
      gsl_vector_set (f, i, (Yi - yy[i]) / sigma[i]);
   }
   return GSL_SUCCESS;
}
///////////////////////////////////////////////////////////////////////

int Kalfas_df(const gsl_vector * x, void *params, gsl_matrix * J) {
   size_t n = ((struct data *) params)->n;
   double *t = ((struct data *) params)->t;
   double *sigma = ((struct data *) params)->sigma;

   double A = gsl_vector_get (x, 0);
   double B = gsl_vector_get (x, 1);
   double C = gsl_vector_get (x, 2);
   double D = gsl_vector_get (x, 3);

   size_t i;

   for ( i = 0 ; i < n ; i++ ) {
      double ener = t[i];
      double s = sigma[i];

      gsl_matrix_set (J, i, 0, pow(ener,B)/(1000*C+pow(ener,D))/s);
      gsl_matrix_set (J, i, 1, A*pow(ener,B)*log(ener)/(1000*C+pow(ener,D))/s);
      gsl_matrix_set (J, i, 2, -1000*A*pow(ener,B)/pow((1000*C+pow(ener,D)),2)/s);
      gsl_matrix_set (J, i, 3, -A*pow(ener,B)*pow(ener,D)*log(ener)/pow(1000*C+pow(ener,D),2)/s);
   }
   return GSL_SUCCESS;
}

///////////////////////////////////////////////////////////////////////

int Kalfas_fdf(const gsl_vector * x, void *params, gsl_vector * f, gsl_matrix * J) {
   Kalfas_f(x, params, f);
   Kalfas_df(x, params, J);

   return GSL_SUCCESS;
}



