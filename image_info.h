#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define _USE_MATH_DEFINES
#include <math.h>

#define WIDTH 352
#define HEIGHT 288
#define PIXEL_DPCM_VALUE 6			// 0(Median Prediction) and 6(No prediction)
#define MV_PRED 0					// Pixed(Not used)
#define BKLEN 8
#define BKSIZE (BKLEN * BKLEN)
#define CbCr_BKLEN (BKLEN / 2)
#define CbCr_BKSIZE (CbCr_BKLEN * CbCr_BKLEN)
#define SEARCH_RANGE 33		
#define CbCr_SEARCH_RANGE ((SEARCH_RANGE - 1) / 2 + 1)
#define PADLEN ((SEARCH_RANGE - 1)/2)
#define CbCr_PADLEN ((SEARCH_RANGE - 1) / 4) 
#define FRAME 90

//#define BKLEN 8
//#define BKSIZE (BKLEN * BKLEN)
//#define CbCr_BKLEN (BKLEN / 2)
//#define CbCr_BKSIZE (CbCr_BKLEN * CbCr_BKLEN)
//
//#define MV_PRED 0					// Pixed(Not used)
//#define INTRA_PREDICTION_FLAG 1		// 0(Disable) and 1(Enable)
//#define DC_DPCM_VALUE 0				// 0(Median Prediction) and 6(No prediction)
//#define PIXEL_DPCM_VALUE 6			// 0(Median Prediction) and 6(No prediction)
//#define DC_QP 1						// 1, 8 and 16
//#define AC_QP 1					// 1 and 16
//#define SEARCH_RANGE 33		
//#define CbCr_SEARCH_RANGE ((SEARCH_RANGE - 1) / 2 + 1)
//#define INTRA_PERIOD 1				// 0 to 10
//
//#define PADLEN ((SEARCH_RANGE - 1)/2)
//#define CbCr_PADLEN ((SEARCH_RANGE - 1) / 4) 
//
#define INPUTFILE "C:\\Users\\icsp\\source\\repos\\Encoder\\Release\\football_cif(352X288)_90f.yuv"		// ENCODER
//#define INPUTFILE "Recon_Enc.YUV"		// DECODER