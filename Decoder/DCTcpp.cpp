#include "buffer.h"

double cos_table[BKSIZE];
double CbCr_cos_table[CbCr_BKSIZE];

void Make_Cosine_Array(Img_Buf* img) {
	for (int i = 0; i < BKLEN; i++)
		for (int j = 0; j < BKLEN; j++)
			cos_table[i * BKLEN + j] = (double)cos(M_PI * ((double)j + 0.5) * (double)i / (double)BKLEN);

	for (int i = 0; i < CbCr_BKLEN; i++)
		for (int j = 0; j < CbCr_BKLEN; j++)
			CbCr_cos_table[i * CbCr_BKLEN + j] = (double)cos(M_PI * ((double)j + 0.5) * (double)i / (double)CbCr_BKLEN);
}

void DCT(Img_Buf* img) {

	double Cu, Temp, Temp2;
	double* Dct_Buf = (double*)malloc(sizeof(double) * BKSIZE);
	double* Cb_Dct_Buf = (double*)malloc(sizeof(double) * CbCr_BKSIZE);
	double* Cr_Dct_Buf = (double*)malloc(sizeof(double) * CbCr_BKSIZE);

	// row dct
	for (int dct_row = 0; dct_row < BKLEN; dct_row++) {

		for (int u = 0; u < BKLEN; u++) {
			Temp = 0.0;
			if (u == 0) Cu = sqrt(1.0 / (double)BKLEN);
			else Cu = sqrt(2.0 / (double)BKLEN);

			// sigma
			for (int x = 0; x < BKLEN; x++) {
				Temp += Cu * (double)img->Resi_blk[dct_row * BKLEN + x] * cos_table[u * BKLEN + x];
				//(double)cos(M_PI * ((double)x + 0.5) * (double)u / (double)BKLEN);
			}
			Dct_Buf[dct_row * BKLEN + u] = Temp;
		}
	}

	// col dct
	for (int dct_col = 0; dct_col < BKLEN; dct_col++) {

		for (int u = 0; u < BKLEN; u++) {
			Temp = 0.0;
			if (u == 0) Cu = sqrt(1.0 / (double)BKLEN);
			else Cu = sqrt(2.0 / (double)BKLEN);

			// sigma
			for (int x = 0; x < BKLEN; x++) {
				Temp += Cu * (double)Dct_Buf[dct_col + x * BKLEN] * cos_table[u * BKLEN + x];
			}
			img->DCT_blk[dct_col + u * BKLEN] = Temp;
		}
	}
	// Chroma
	// row dct
	for (int dct_row = 0; dct_row < CbCr_BKLEN; dct_row++) {

		for (int u = 0; u < CbCr_BKLEN; u++) {
			Temp = 0.0;
			Temp2 = 0.0;
			if (u == 0) Cu = sqrt(1.0 / (double)CbCr_BKLEN);
			else Cu = sqrt(2.0 / (double)CbCr_BKLEN);

			// sigma
			for (int x = 0; x < CbCr_BKLEN; x++) {
				Temp += Cu * (double)img->Cb_Resi_blk[dct_row * CbCr_BKLEN + x] * CbCr_cos_table[u * CbCr_BKLEN + x];
				Temp2 += Cu * (double)img->Cr_Resi_blk[dct_row * CbCr_BKLEN + x] * CbCr_cos_table[u * CbCr_BKLEN + x];
				//(double)cos(M_PI * ((double)x + 0.5) * (double)u / (double)BKLEN);
			}
			Cb_Dct_Buf[dct_row * CbCr_BKLEN + u] = Temp;
			Cr_Dct_Buf[dct_row * CbCr_BKLEN + u] = Temp2;
		}
	}

	// col dct
	for (int dct_col = 0; dct_col < CbCr_BKLEN; dct_col++) {

		for (int u = 0; u < CbCr_BKLEN; u++) {
			Temp = 0.0;
			Temp2 = 0.0;
			if (u == 0) Cu = sqrt(1.0 / (double)CbCr_BKLEN);
			else Cu = sqrt(2.0 / (double)CbCr_BKLEN);

			// sigma
			for (int x = 0; x < CbCr_BKLEN; x++) {
				Temp += Cu * (double)Cb_Dct_Buf[dct_col + x * CbCr_BKLEN] * CbCr_cos_table[u * CbCr_BKLEN + x];
				Temp2 += Cu * (double)Cr_Dct_Buf[dct_col + x * CbCr_BKLEN] * CbCr_cos_table[u * CbCr_BKLEN + x];
			}
			img->Cb_DCT_blk[dct_col + u * CbCr_BKLEN] = Temp;
			img->Cr_DCT_blk[dct_col + u * CbCr_BKLEN] = Temp2;
		}
	}
}

void IDCT(Img_Buf* img) {

	double Cu, Temp, Temp2;

	double* Dct_Buf = (double*)malloc(sizeof(double) * BKSIZE);
	double* Cb_Dct_Buf = (double*)malloc(sizeof(double) * CbCr_BKSIZE);
	double* Cr_Dct_Buf = (double*)malloc(sizeof(double) * CbCr_BKSIZE);

	// row IDCT
	for (int idct_row = 0; idct_row < BKLEN; idct_row++) {
		for (int u = 0; u < BKLEN; u++) {
			Temp = 0.0;

			for (int x = 0; x < BKLEN; x++) {
				if (x == 0) Cu = sqrt(1.0 / (double)BKLEN);
				else Cu = sqrt(2.0 / (double)BKLEN);
				Temp += Cu * (double)img->DCT_blk[idct_row * BKLEN + x] * cos_table[x * BKLEN + u];
			}
			Dct_Buf[idct_row * BKLEN + u] = Temp;
		}
	}

	// col IDCT
	for (int idct_col = 0; idct_col < BKLEN; idct_col++) {

		for (int u = 0; u < BKLEN; u++) {
			Temp = 0.0;

			// sigma

			for (int x = 0; x < BKLEN; x++) {
				if (x == 0) Cu = sqrt(1.0 / (double)BKLEN);
				else Cu = sqrt(2.0 / (double)BKLEN);
				Temp += Cu * (double)Dct_Buf[idct_col + x * BKLEN] * cos_table[x * BKLEN + u];
			}
			img->DCT_blk[idct_col + u * BKLEN] = Temp;
		}
	}

	for (int row = 0; row < BKLEN; row++)
		for (int col = 0; col < BKLEN; col++) {
			img->Resi_blk[row * BKLEN + col] = (int)img->DCT_blk[row * BKLEN + col];
		}
	//Chroma
	// row IDCT
	for (int idct_row = 0; idct_row < CbCr_BKLEN; idct_row++) {
		for (int u = 0; u < CbCr_BKLEN; u++) {
			Temp = 0.0;
			Temp2 = 0.0;
			for (int x = 0; x < CbCr_BKLEN; x++) {
				if (x == 0) Cu = sqrt(1.0 / (double)CbCr_BKLEN);
				else Cu = sqrt(2.0 / (double)CbCr_BKLEN);
				Temp += Cu * (double)img->Cb_DCT_blk[idct_row * CbCr_BKLEN + x] * CbCr_cos_table[x * CbCr_BKLEN + u];
				Temp2 += Cu * (double)img->Cr_DCT_blk[idct_row * CbCr_BKLEN + x] * CbCr_cos_table[x * CbCr_BKLEN + u];
			}
			Cb_Dct_Buf[idct_row * CbCr_BKLEN + u] = Temp;
			Cr_Dct_Buf[idct_row * CbCr_BKLEN + u] = Temp2;
		}
	}

	// col IDCT
	for (int idct_col = 0; idct_col < CbCr_BKLEN; idct_col++) {
		for (int u = 0; u < CbCr_BKLEN; u++) {
			Temp = 0.0;
			Temp2 = 0.0;
			// sigma

			for (int x = 0; x < CbCr_BKLEN; x++) {
				if (x == 0) Cu = sqrt(1.0 / (double)CbCr_BKLEN);
				else Cu = sqrt(2.0 / (double)CbCr_BKLEN);
				Temp += Cu * (double)Cb_Dct_Buf[idct_col + x * CbCr_BKLEN] * CbCr_cos_table[x * CbCr_BKLEN + u];
				Temp2 += Cu * (double)Cr_Dct_Buf[idct_col + x * CbCr_BKLEN] * CbCr_cos_table[x * CbCr_BKLEN + u];
			}
			img->Cb_DCT_blk[idct_col + u * CbCr_BKLEN] = Temp;
			img->Cr_DCT_blk[idct_col + u * CbCr_BKLEN] = Temp2;
		}
	}

	for (int row = 0; row < CbCr_BKLEN; row++)
		for (int col = 0; col < CbCr_BKLEN; col++) {
			img->Cb_Resi_blk[row * CbCr_BKLEN + col] = (int)img->Cb_DCT_blk[row * CbCr_BKLEN + col];
			img->Cr_Resi_blk[row * CbCr_BKLEN + col] = (int)img->Cr_DCT_blk[row * CbCr_BKLEN + col];
		}
}