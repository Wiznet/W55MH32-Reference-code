#ifndef __WIZ_CEPHES_H
#define __WIZ_CEPHES_H

double wiz_cephes_igamc(double a, double x);
double wiz_cephes_igam(double a, double x);
double wiz_cephes_lgam(double x);
double wiz_cephes_p1evl(double x, double *coef, int N);
double wiz_cephes_polevl(double x, double *coef, int N);
double wiz_cephes_erf(double x);
double wiz_cephes_erfc(double x);
double wiz_cephes_normal(double x);

#endif /*  __WIZ_CEPHES_H  */
