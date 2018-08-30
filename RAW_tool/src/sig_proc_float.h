/*-----------------------------------------
Contributor: Jérôme Dubois
Version: 1.1
Date: 07/2018
Descritption:
C scrit for signal processing based on the fft algorithme from rosettacode
source from https://rosettacode.org/wiki/Fast_Fourier_transform
all other algorithmes are made by Jérôme Dubois
FFT algorithm were improve by adding a precalculation of the fft_table
IFFT was added by simply calculate the complex conjugation of fft_table.
IFFT can be normed or not (IFFT(FFT(s))=s if IFFT is normed) so we can minimise calculation time.

algorithmes are made for float precision and can be modified to double precision

The librairy has been customed to make Hilbert Transform and envelope detection.
-----------------------------------------*/

#ifndef SIG_PROC_H
#define SIG_PROC_H

#include <math.h>
#include "pointer_management.h"


//Pi
const double PI = 3.141592653589793238460;

//Structures
typedef struct fftf_table fftf_table;
typedef struct sig_proc_float sig_proc_float;

//Functions prototype
int int_pow(int x, int power);//power>= 0
int power_two(int x, int *pow);//determine the power of 2 superior and nearest to x return 0 if length(x) is a power of 2, 1 else
void fill_ftable(fftf_table *tbf);
void init_struct_fftf_table (fftf_table **tbf, int N); //N number of point for initialisation
void resize_struct_fftf_table(fftf_table *tbf, int N);
void delete_struct_fftf_table(fftf_table *tbf);
void init_struct_spf (sig_proc_float **spf, int N);
void resize_struct_spf (sig_proc_float *spf, int N);
void delete_struct_spf (sig_proc_float *spf);

void fftf_preparation(sig_proc_float *spf); //fill sigc with sig (if user want to use sig buffer)
void mean_fsignal_calculus(sig_proc_float *spf);
void paddingf (sig_proc_float *spf, float value);// value_padding, pad is filled with value from Npoint Npad
void rec_fftf(cplxf *buf, cplxf *out, int step, fftf_table *tbf);
void fftf(sig_proc_float *spf);//need a signal with length equal to a power of 2
void rec_ifftf(cplxf *buf, cplxf *out, int step, fftf_table *tbf);
void ifftf(sig_proc_float *spf);//need a signal with length equal to a power of 2
void Nifftf(sig_proc_float *spf);//normalized ifft: Nifft(fft(s))=s
void ifftf_filt(sig_proc_float *spf);//same than ifftf but use fft_filt not fft
void Nifftf_filt(sig_proc_float *spf);
void filterf_parameter(sig_proc_float *spf, float fech , float f0 , float fm , int methode);
void reset_filtf_buffer(sig_proc_float *spf);
void apply_filterf(sig_proc_float *spf);
void init_hilberttransformf(sig_proc_float *spf, float fech , float f0 , float fm , int methode);
void hilberttransformf(sig_proc_float *spf);
void Nhilberttransformf(sig_proc_float *spf); //normalized Hilbert transform
void envelope_detectionf(sig_proc_float *spf); //proportional to envelope
void Nenvelope_detectionf(sig_proc_float *spf); //true envelope


//structure definition
struct fftf_table
{
	int Npoint; //number of point of the signal
	int Npad; //number of point of the signal with zeros padding

	cplxf *fftf_table;
  	cplxf *ifftf_table;
};

struct sig_proc_float
{
  float mean_sig; //used when padding with mean of the signal

  fftf_table *tbf;
  float *sig;
  float *envelope;
  cplxf *sigc;
  cplxf *fft;
  cplxf *ifft;

  float *window_hilb; //window for filtering size not Npad but size of the window
  int n0; //position of the first point of the window
  int nf; //position of the last point
  int N_win; //number of point of window_hilb
  cplxf *fft_filt;
};



//Functions definitions
int int_pow(int x, int power)
{
	int y=1;
	int i=1;
	for (i=1 ; i<=power ; i++){y*=x;}
	return y;
}

int power_two(int x, int *pow)
{
	if (x<1) {return 2;}
	else if (x==1)
	{
		(*pow)=0;
		return 0;
	}
	else
	{
		int ret=1;
		(*pow)=0;
		double y=(double)x;
		while (y>=2.0)
		{
			(*pow)++;
			//printf("y = %f\n",y);
			if (y==2.0) {ret=0;}
			y/=2.0;
		}
		return ret;
	}
}

void fill_ftable(fftf_table *tbf)
{
	int j=0;

	for (j=0 ; j<tbf->Npad ; j++)
	{
		tbf->fftf_table[j]=cexpf(-I*PI*j/tbf->Npad);
		tbf->ifftf_table[j]=cexpf(I*PI*j/tbf->Npad);
	}
}

void init_struct_fftf_table (fftf_table **tbf, int N)
{
	fftf_table *tmp=NULL;
	tmp=(fftf_table *)malloc(sizeof(fftf_table));
	int res=0, powt=0;
	tmp->Npoint=N;
	res=power_two(N, &powt);
	powt+=res;
	tmp->Npad=int_pow(2,powt);
	create_vector_cplxf (&tmp->fftf_table, tmp->Npad);
	create_vector_cplxf (&tmp->ifftf_table, tmp->Npad);
	fill_ftable(tmp);
	*tbf=tmp;
}

void resize_struct_fftf_table(fftf_table *tbf, int N)
{
	int res=0, powt=0;
	tbf->Npoint=N;
	res=power_two(N, &powt);
	powt+=res;
	tbf->Npad=int_pow(2,powt);
	resize_vector_cplxf (&tbf->fftf_table, tbf->Npad);
	resize_vector_cplxf (&tbf->ifftf_table, tbf->Npad);
	fill_ftable(tbf);
}

void delete_struct_fftf_table(fftf_table *tbf)
{
	delete_vector_cplxf(&tbf->fftf_table);
	delete_vector_cplxf(&tbf->ifftf_table);
	free(tbf);
}

void init_struct_spf (sig_proc_float **spf, int N)
{
  sig_proc_float *tmp=NULL;
  tmp=(sig_proc_float *)malloc(sizeof(sig_proc_float));
  init_struct_fftf_table (&tmp->tbf, N);
  create_vector_float(&tmp->sig, tmp->tbf->Npoint);
  create_vector_float(&tmp->envelope, tmp->tbf->Npoint);
  create_vector_float(&tmp->window_hilb, tmp->tbf->Npoint);
  create_0_vector_cplxf (&tmp->sigc, tmp->tbf->Npad);
  create_vector_cplxf (&tmp->fft, tmp->tbf->Npad);
  create_vector_cplxf (&tmp->fft_filt, tmp->tbf->Npad);
  create_vector_cplxf (&tmp->ifft, tmp->tbf->Npad);
  *spf=tmp;
}

void resize_struct_spf (sig_proc_float *spf, int N)
{
  resize_struct_fftf_table (spf->tbf, N);
  resize_vector_float(&spf->sig, spf->tbf->Npoint);
  resize_vector_float(&spf->envelope, spf->tbf->Npoint);
  resize_vector_float(&spf->window_hilb, spf->tbf->Npoint);
  resize_vector_cplxf(&spf->sigc, spf->tbf->Npad);
  resize_vector_cplxf(&spf->fft, spf->tbf->Npad);
  resize_vector_cplxf(&spf->fft_filt, spf->tbf->Npad);
  resize_vector_cplxf(&spf->ifft, spf->tbf->Npad);
}

void delete_struct_spf (sig_proc_float *spf)
{
  delete_struct_fftf_table (spf->tbf);
  delete_vector_float(&spf->sig);
  delete_vector_float(&spf->envelope);
  delete_vector_float(&spf->window_hilb);
  delete_vector_cplxf(&spf->sigc);
  delete_vector_cplxf(&spf->fft);
  delete_vector_cplxf(&spf->fft_filt);
  delete_vector_cplxf(&spf->ifft);
  free(spf);
}

void fftf_preparation(sig_proc_float *spf)
{
  int i=0;
  for (i=0 ; i<spf->tbf->Npoint ; i++) {spf->sigc[i]=(cplxf)spf->sig[i];}
}

void mean_fsignal_calculus(sig_proc_float *spf)
{
	int i=0;
	float mean=0.0;
	for (i=0 ; i<spf->tbf->Npoint ; i++) {mean+=spf->sig[i];}
	mean/=spf->tbf->Npoint;
	spf->mean_sig=mean;
}

void paddingf (sig_proc_float *spf, float value)
{
	int i=0;
	for (i=spf->tbf->Npoint ; i<spf->tbf->Npad ; i++) {spf->sigc[i]=(cplxf)value;}
}

void rec_fftf(cplxf *buf, cplxf *out, int step, fftf_table *tbf)
{
	cplxf t;
        if (step < tbf->Npad)
				{
                rec_fftf(out, buf, step * 2, tbf);
                rec_fftf(out + step, buf + step, step * 2, tbf);

                for (int i = 0; i < tbf->Npad; i += 2 * step)
								{
                        t = tbf->fftf_table[i] * out[i + step];
                        buf[i / 2]     = out[i] + t;
                        buf[(i + tbf->Npad)/2] = out[i] - t;
                }
        }
}

void fftf(sig_proc_float *spf)
{
  int i=0;
  for (i=0 ; i<spf->tbf->Npad ; i++) {spf->fft[i]=spf->sigc[i];}

	rec_fftf(spf->fft, spf->sigc, 1, spf->tbf);
}

void rec_ifftf(cplxf *buf, cplxf *out, int step, fftf_table *tbf)
{
	cplxf t;
        if (step < tbf->Npad)
				{
                rec_ifftf(out, buf, step * 2, tbf);
                rec_ifftf(out + step, buf + step, step * 2, tbf);

                for (int i = 0; i < tbf->Npad; i += 2 * step)
								{
                        t = tbf->ifftf_table[i] * out[i + step];
                        buf[i / 2]     = out[i] + t;
                        buf[(i + tbf->Npad)/2] = out[i] - t;
                }
        }
}

void ifftf(sig_proc_float *spf)
{
  int i=0;
  for (i=0 ; i<spf->tbf->Npad ; i++) {spf->ifft[i]=spf->fft[i];}

	rec_ifftf(spf->ifft, spf->fft, 1, spf->tbf);
}

void Nifftf(sig_proc_float *spf)
{
  int i=0;
  for (i=0 ; i<spf->tbf->Npad ; i++) {spf->ifft[i]=spf->fft[i];}

	rec_ifftf(spf->ifft, spf->fft, 1, spf->tbf);
	for (int i = 0 ; i < spf->tbf->Npad ; i++){spf->ifft[i] /= spf->tbf->Npad;}
}

void ifftf_filt(sig_proc_float *spf)
{
  int i=0;
  for (i=0 ; i<spf->tbf->Npad ; i++) {spf->ifft[i]=spf->fft_filt[i];}

	rec_ifftf(spf->ifft, spf->fft_filt, 1, spf->tbf);
}

void Nifftf_filt(sig_proc_float *spf)
{
  int i=0;
  for (i=0 ; i<spf->tbf->Npad ; i++) {spf->ifft[i]=spf->fft_filt[i];}

	rec_ifftf(spf->ifft, spf->fft_filt, 1, spf->tbf);
	for (int i = 0 ; i < spf->tbf->Npad ; i++){spf->ifft[i] /= spf->tbf->Npad;}
}

void Hilb_filterf_parameter(sig_proc_float *spf, float fech , float f0 , float fm , int methode)
{
  int N=0,i=0;
	spf->n0=(int)(f0*spf->tbf->Npad/fech);
	spf->nf=(int)(fm*spf->tbf->Npad/fech);
	N=spf->nf-spf->n0+1;
  if (N!=spf->N_win) {resize_vector_float(&spf->window_hilb, N);}
  spf->N_win=N;

  switch(methode)
  {
    case 0 : //door
      for (int i=0 ; i<N ; i++){spf->window_hilb[i]=1.0;}
      break; // optional

    case 1 : //Hanning: 0.5*(1-cos(2*pi*t/T))
      for (int i=0 ; i<N ; i++){spf->window_hilb[i]=(0.5*(1.0-cos(2.0*PI*i/N)));}
      break;

    case 2 : //Blackmann: 0.42-0.5*cos(2*pi*t/T)+0.08*cos(4*pi*t/T)
      for (int i=0 ; i<N ; i++){spf->window_hilb[i]=(0.42-0.5*cos(2.0*PI*i/N)+0.08*cos(4.0*PI*i/N));}
      break;
  }
}

void reset_filtf_buffer(sig_proc_float *spf)
{
  int i=0;
  for (i=0 ; i<spf->n0 ; i++) {spf->fft_filt[i]=(complex)(0.0);}
  for (i=spf->nf ; i<spf->tbf->Npad ; i++) {spf->fft_filt[i]=(complex)(0.0);}
}

void apply_filterf(sig_proc_float *spf)
{
  int i=0;
  for (i=0 ; i<spf->N_win ; i++) {spf->fft_filt[spf->n0+i]=spf->fft[spf->n0+i]*spf->window_hilb[i];}
}

void init_hilberttransformf(sig_proc_float *spf, float fech , float f0 , float fm , int methode)
{
  Hilb_filterf_parameter(spf, fech , f0 , fm , methode);
  reset_filtf_buffer(spf);
}

void hilberttransformf(sig_proc_float *spf)
{
  fftf(spf);
  reset_filtf_buffer(spf);
  apply_filterf(spf);
  ifftf_filt(spf);
}

void Nhilberttransformf(sig_proc_float *spf)
{
  fftf(spf);
  reset_filtf_buffer(spf);
  apply_filterf(spf);
  Nifftf_filt(spf);
}

void envelope_detectionf(sig_proc_float *spf)
{
  int i=0;
  hilberttransformf(spf);
	for (i=0 ; i<spf->tbf->Npoint ; i++){spf->envelope[i]=cabsf(spf->ifft[i]);}
}

void Nenvelope_detectionf(sig_proc_float *spf)
{
  int i=0;
  Nhilberttransformf(spf);
	for (i=0 ; i<spf->tbf->Npoint ; i++){spf->envelope[i]=cabsf(spf->ifft[i])*2.0;}
}

#endif
