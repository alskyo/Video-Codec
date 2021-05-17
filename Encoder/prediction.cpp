#include "buffer.h"
void Prediction(Img_Buf* img, Pred_Buf* pred, int row, int col) {
	unsigned char* Ref, * Src;

	unsigned char Ref_pix[2 * BKLEN + 1] = { 0 };

	int Sad[Max_Mode_num] = { 0 };
	int Sort[Max_Mode_num] = { 0 };

	int Best_Sad, tmp;
	int Sad_Buf0, Sad_Buf1, Sad_Buf2, Sad_Buf3;

	int DC_Avr = 0;
	int temp = 0;

	int Best_intraMode = 0;
	Src = img->Recon_img;
	Ref = img->Recon_img + (row * img->info.Blk_H) * img->info.width + col * img->info.Blk_W;

	if (row == 0) // 영상의 경계
	{
		for (int i = 0; i < img->info.Blk_W; i++)
			Ref_pix[i] = 128;

		Ref_pix[2 * BKLEN] = 128; //16번
	}
	else
	{
		for (int i = 0; i < img->info.Blk_W; i++)
			Ref_pix[i] = Ref[-img->info.width + i];

		Ref_pix[2 * BKLEN] = Ref[-img->info.width - 1];
	}

	if (col == 0) //영상의 경계
		for (int i = 0; i < img->info.Blk_H; i++)
			Ref_pix[BKLEN + i] = 128;
	else
		for (int i = 0; i < img->info.Blk_H; i++)
			Ref_pix[BKLEN + i] = Ref[i * img->info.width - 1];	// 여기 고쳐보는중

	// DC --------------------------------------------------------------
	if (row != 0 && col != 0) {
		for (int i = 0; i < 2 * BKLEN + 1; i++)
			DC_Avr += Ref_pix[i];
		DC_Avr /= (2 * BKLEN + 1);
	}
	else if (row == 0) {
		for (int i = 8; i < 2 * BKLEN; i++)
			DC_Avr += Ref_pix[i];
		DC_Avr /= (BKLEN);
	}
	else if (col == 0) {
		for (int i = 0; i < BKLEN; i++)
			DC_Avr += Ref_pix[i];
		DC_Avr /= (BKLEN);
	}
	for (int i = 0; i < BKLEN; i++)
		for (int j = 0; j < BKLEN; j++)
			pred->Dc[i * img->info.Blk_W + j] = DC_Avr; // DC값 저장
	// -----------------------------------------------------------------
	
	// Vertical --------------------------------------------------------
	for (int i = 0; i < BKLEN; i++)
		for (int j = 0; j < BKLEN; j++)
			pred->Ver[i * img->info.Blk_W + j] = Ref_pix[j];
	// -----------------------------------------------------------------

	// Horizontal ------------------------------------------------------
	for (int i = 0; i < BKLEN; i++)
		for (int j = 0; j < BKLEN; j++)
			pred->Hor[i * img->info.Blk_W + j] = Ref_pix[BKLEN + i];	// 살짝 애매
	// -----------------------------------------------------------------

	// Sum of Absolute Difference
	for (int i = 0; i < img->info.Blk_H; i++) {
		for (int j = 0; j < img->info.Blk_W; j++) {
			Sad[2] += abs(pred->Resi_Dc[i * img->info.Blk_W + j] = (img->Cur_blk[i * img->info.Blk_W + j] - pred->Dc[i * img->info.Blk_W + j]));
			Sad[0] += abs(pred->Resi_Ver[i * img->info.Blk_W + j] = (img->Cur_blk[i * img->info.Blk_W + j] - pred->Ver[i * img->info.Blk_W + j]));
			Sad[1] += abs(pred->Resi_Hor[i * img->info.Blk_W + j] = (img->Cur_blk[i * img->info.Blk_W + j] - pred->Hor[i * img->info.Blk_W + j]));
		}
	}

	// Sorting 오름차순
	Sad_Buf0 = Sad[0];
	Sad_Buf1 = Sad[1];
	Sad_Buf2 = Sad[2];
	for (int i = 0; i < 3; i++) {        
		for (int j = 0; j < 2 - i; j++) {
			if (Sad[j] > Sad[j + 1]) {
				temp = Sad[j];
				Sad[j] = Sad[j + 1];
				Sad[j + 1] = temp;
			}
		}
	}
	if (Sad[0] == Sad_Buf0)
		Best_intraMode = 0;
	else if (Sad[0] == Sad_Buf1)
		Best_intraMode = 1;
	else if (Sad[0] == Sad_Buf2)
		Best_intraMode = 2;

	img->Best_IntraMode[row * (img->info.width / BKLEN) + col] = Best_intraMode;

	switch (Best_intraMode)
	{
	case 0:
		memcpy(img->Resi_blk, pred->Resi_Ver, img->info.Blk_H * img->info.Blk_W * sizeof(int));
		break;
	case 1:
		memcpy(img->Resi_blk, pred->Resi_Hor, img->info.Blk_H * img->info.Blk_W * sizeof(int));
		break;
	case 2:
		memcpy(img->Resi_blk, pred->Resi_Dc, img->info.Blk_H * img->info.Blk_W * sizeof(int));
		break;
	}

}

void CbCr_Prediction(Img_Buf* img, int row, int col) {
	unsigned char* Cb_Ref, * Cb_Src, * Cr_Ref, * Cr_Src;

	unsigned char Cb_Ref_pix[2 * CbCr_BKLEN + 1] = { 0 };
	unsigned char Cr_Ref_pix[2 * CbCr_BKLEN + 1] = { 0 };
	unsigned char Cb_Best[CbCr_BKSIZE];
	unsigned char Cr_Best[CbCr_BKSIZE];

	int Cb_DC_Avr = 0;
	int Cr_DC_Avr = 0;

	Cb_Src = img->Cb_Recon_img;
	Cb_Ref = img->Cb_Recon_img + (row * img->info.CbCr_Blk_H) * img->info.CbCr_width + col * img->info.CbCr_Blk_W;
	Cr_Src = img->Cr_Recon_img;
	Cr_Ref = img->Cr_Recon_img + (row * img->info.CbCr_Blk_H) * img->info.CbCr_width + col * img->info.CbCr_Blk_W;

	if (row == 0) // 영상의 경계
	{
		for (int i = 0; i < img->info.CbCr_Blk_W; i++) {
			Cb_Ref_pix[i] = 128;		// 0 ~ 7
			Cr_Ref_pix[i] = 128;
		}
		Cb_Ref_pix[2 * CbCr_BKLEN] = 128; //8번
		Cr_Ref_pix[2 * CbCr_BKLEN] = 128;
	}
	else
	{
		for (int i = 0; i < img->info.CbCr_Blk_W; i++) {
			Cb_Ref_pix[i] = Cb_Ref[-img->info.CbCr_width + i];
			Cr_Ref_pix[i] = Cr_Ref[-img->info.CbCr_width + i];
		}
		Cb_Ref_pix[2 * CbCr_BKLEN] = Cb_Ref[-img->info.CbCr_width - 1];
		Cr_Ref_pix[2 * CbCr_BKLEN] = Cr_Ref[-img->info.CbCr_width - 1];
	}
	if (col == 0) //영상의 경계
		for (int i = 0; i < img->info.CbCr_Blk_H; i++) {
			Cb_Ref_pix[CbCr_BKLEN + i] = 128;
			Cr_Ref_pix[CbCr_BKLEN + i] = 128;
		}	
	else {
		for (int i = 0; i < img->info.CbCr_Blk_H; i++) {
			Cb_Ref_pix[CbCr_BKLEN + i] = Cb_Ref[i * img->info.CbCr_width - 1];
			Cr_Ref_pix[CbCr_BKLEN + i] = Cr_Ref[i * img->info.CbCr_width - 1];
		}
	}
	if (img->Best_IntraMode[row * (img->info.width / BKLEN) + col] == 2) {
		if (row != 0 && col != 0) {
			for (int i = 0; i < 2 * CbCr_BKLEN + 1; i++) {
				Cb_DC_Avr += Cb_Ref_pix[i];
				Cr_DC_Avr += Cr_Ref_pix[i];
			}
			Cb_DC_Avr /= (2 * CbCr_BKLEN + 1);
			Cr_DC_Avr /= (2 * CbCr_BKLEN + 1);
		}
		else if (row == 0) {
			for (int i = CbCr_BKLEN; i < 2 * CbCr_BKLEN; i++) {
				Cb_DC_Avr += Cb_Ref_pix[i];
				Cr_DC_Avr += Cr_Ref_pix[i];
			}
			Cb_DC_Avr /= (CbCr_BKLEN);
			Cr_DC_Avr /= (CbCr_BKLEN);
		}
		else if (col == 0) {
			for (int i = 0; i < CbCr_BKLEN; i++) {
				Cb_DC_Avr += Cb_Ref_pix[i];
				Cr_DC_Avr += Cr_Ref_pix[i];
			}	
			Cb_DC_Avr /= (CbCr_BKLEN);
			Cr_DC_Avr /= (CbCr_BKLEN);
		}
		for (int i = 0; i < CbCr_BKLEN; i++)
			for (int j = 0; j < CbCr_BKLEN; j++) {
				Cb_Best[i * img->info.CbCr_Blk_W + j] = Cb_DC_Avr;
				Cr_Best[i * img->info.CbCr_Blk_W + j] = Cr_DC_Avr;
			}	
	}
	else if (img->Best_IntraMode[row * (img->info.width / BKLEN) + col] == 0) {
		for (int i = 0; i < CbCr_BKLEN; i++)
			for (int j = 0; j < CbCr_BKLEN; j++) {
				Cb_Best[i * img->info.CbCr_Blk_W + j] = Cb_Ref_pix[j];
				Cr_Best[i * img->info.CbCr_Blk_W + j] = Cr_Ref_pix[j];
			}
	}
	else if (img->Best_IntraMode[row * (img->info.width / BKLEN) + col] == 1) {
		for (int i = 0; i < CbCr_BKLEN; i++)
			for (int j = 0; j < CbCr_BKLEN; j++) {
				Cb_Best[i * img->info.CbCr_Blk_W + j] = Cb_Ref_pix[CbCr_BKLEN + i];
				Cr_Best[i * img->info.CbCr_Blk_W + j] = Cr_Ref_pix[CbCr_BKLEN + i];
			}
	}
	// Residual 저장
	for (int i = 0; i < img->info.CbCr_Blk_H; i++) {
		for (int j = 0; j < img->info.CbCr_Blk_W; j++) {
			img->Cb_Resi_blk[i * img->info.CbCr_Blk_W + j] 
				= (img->Cb_Cur_blk[i * img->info.CbCr_Blk_W + j] - Cb_Best[i * img->info.CbCr_Blk_W + j]);
			img->Cr_Resi_blk[i * img->info.CbCr_Blk_W + j]
				= (img->Cr_Cur_blk[i * img->info.CbCr_Blk_W + j] - Cr_Best[i * img->info.CbCr_Blk_W + j]);
		}
	}
}

void Reconstruct(Img_Buf* img, Pred_Buf* pred, int Row, int Col) {
	unsigned char Best[BKSIZE];
	unsigned char *Ref, *Src;

	unsigned char Ref_pix[2 * BKLEN + 1];

	int DC_Avr = 0;

	Src = img->Recon_img;
	Ref = img->Recon_img + (Row * img->info.Blk_H) * img->info.width + Col * img->info.Blk_W;  

	if (Row == 0) {
		for (int i = 0; i < img->info.Blk_W; i++)
			Ref_pix[i] = 128;

		Ref_pix[2 * BKLEN] = 128;
	}
	else {
		for(int i = 0; i < img->info.Blk_W; i++)
			Ref_pix[i] = Ref[-img->info.width + i];
		Ref_pix[2 * BKLEN] = Ref[-img->info.width - 1];
	}

	if (Col == 0) {
		for (int i = 0; i < img->info.Blk_H; i++)
			Ref_pix[BKLEN + i] = 128;
	}
	else
		for (int i = 0; i < img->info.Blk_H; i++)
			Ref_pix[BKLEN + i] = Ref[i * img->info.width - 1];

	if (img->Best_IntraMode[Row * (img->info.width / BKLEN) + Col] == 2) {
		if (Row != 0 && Col != 0) {
			for (int i = 0; i < 2 * BKLEN + 1; i++)
				DC_Avr += Ref_pix[i];
			DC_Avr /= (2 * BKLEN + 1);
		}
		else if (Row == 0) {
			for (int i = 8; i < 2 * BKLEN; i++)
				DC_Avr += Ref_pix[i];
			DC_Avr /= (BKLEN);
		}
		else if (Col == 0) {
			for (int i = 0; i < BKLEN; i++)
				DC_Avr += Ref_pix[i];
			DC_Avr /= (BKLEN);
		}
		for (int i = 0; i < BKLEN; i++)
			for (int j = 0; j < BKLEN; j++)
				Best[i * img->info.Blk_W + j] = DC_Avr;
	}
	else if (img->Best_IntraMode[Row * (img->info.width / BKLEN) + Col] == 0) {
		for (int i = 0; i < BKLEN; i++)
			for (int j = 0; j < BKLEN; j++)
				Best[i * img->info.Blk_W + j] = Ref_pix[j];
	}
	else if (img->Best_IntraMode[Row * (img->info.width / BKLEN) + Col] == 1) {
		for (int i = 0; i < BKLEN; i++)
			for (int j = 0; j < BKLEN; j++)
				Best[i * img->info.Blk_W + j] = Ref_pix[BKLEN + i];
	}

	for (int i = 0; i < img->info.Blk_H; i++)
	{
		for (int j = 0; j < img->info.Blk_W; j++)
		{
			if (img->Resi_blk[i * img->info.Blk_W + j] + Best[i * img->info.Blk_W + j] > 255)
				img->Recon_blk[i * img->info.Blk_W + j] = 255;
			else if (img->Resi_blk[i * img->info.Blk_W + j] + Best[i * img->info.Blk_W + j] < 0)
				img->Recon_blk[i * img->info.Blk_W + j] = 0;
			else
				img->Recon_blk[i * img->info.Blk_W + j] = img->Resi_blk[i * img->info.Blk_W + j] + Best[i * img->info.Blk_W + j];
		}
	}
}

void CbCr_Reconstruct(Img_Buf* img, int Row, int Col) {
	unsigned char Cb_Best[CbCr_BKSIZE];
	unsigned char Cr_Best[CbCr_BKSIZE];
	unsigned char* Cb_Ref, * Cb_Src, * Cr_Ref, * Cr_Src;

	unsigned char Cb_Ref_pix[2 * CbCr_BKLEN + 1];
	unsigned char Cr_Ref_pix[2 * CbCr_BKLEN + 1];

	int Cb_DC_Avr = 0;
	int Cr_DC_Avr = 0;

	Cb_Src = img->Cb_Recon_img;
	Cr_Src = img->Cr_Recon_img;
	Cb_Ref = img->Cb_Recon_img + (Row * img->info.CbCr_Blk_H) * img->info.CbCr_width + Col * img->info.CbCr_Blk_W;
	Cr_Ref = img->Cr_Recon_img + (Row * img->info.CbCr_Blk_H) * img->info.CbCr_width + Col * img->info.CbCr_Blk_W;

	if (Row == 0) {
		for (int i = 0; i < img->info.CbCr_Blk_W; i++) {
			Cb_Ref_pix[i] = 128;
			Cr_Ref_pix[i] = 128;
		}
		Cb_Ref_pix[2 * CbCr_BKLEN] = 128;
		Cr_Ref_pix[2 * CbCr_BKLEN] = 128;
	}
	else {
		for (int i = 0; i < img->info.CbCr_Blk_W; i++) {
			Cb_Ref_pix[i] = Cb_Ref[-img->info.CbCr_width + i];
			Cr_Ref_pix[i] = Cr_Ref[-img->info.CbCr_width + i];
		}
		Cb_Ref_pix[2 * CbCr_BKLEN] = Cb_Ref[-img->info.CbCr_width - 1];
		Cr_Ref_pix[2 * CbCr_BKLEN] = Cr_Ref[-img->info.CbCr_width - 1];
	}

	if (Col == 0) {
		for (int i = 0; i < img->info.CbCr_Blk_H; i++) {
			Cb_Ref_pix[CbCr_BKLEN + i] = 128;
			Cr_Ref_pix[CbCr_BKLEN + i] = 128;
		}
	}
	else {
		for (int i = 0; i < img->info.CbCr_Blk_H; i++) {
			Cb_Ref_pix[CbCr_BKLEN + i] = Cb_Ref[i * img->info.CbCr_width - 1];
			Cr_Ref_pix[CbCr_BKLEN + i] = Cr_Ref[i * img->info.CbCr_width - 1];
		}
	}

	if (img->Best_IntraMode[Row * (img->info.width / BKLEN) + Col] == 2) {
		if (Row != 0 && Col != 0) {
			for (int i = 0; i < 2 * CbCr_BKLEN + 1; i++) {
				Cb_DC_Avr += Cb_Ref_pix[i];
				Cr_DC_Avr += Cr_Ref_pix[i];
			}
			Cb_DC_Avr /= (2 * CbCr_BKLEN + 1);
			Cr_DC_Avr /= (2 * CbCr_BKLEN + 1);
		}
		else if (Row == 0) {
			for (int i = CbCr_BKLEN; i < 2 * CbCr_BKLEN; i++) {
				Cb_DC_Avr += Cb_Ref_pix[i];
				Cr_DC_Avr += Cr_Ref_pix[i];
			}
			Cb_DC_Avr /= (CbCr_BKLEN);
			Cr_DC_Avr /= (CbCr_BKLEN);
		}
		else if (Col == 0) {
			for (int i = 0; i < CbCr_BKLEN; i++) {
				Cb_DC_Avr += Cb_Ref_pix[i];
				Cr_DC_Avr += Cr_Ref_pix[i];
			}
			Cb_DC_Avr /= (CbCr_BKLEN);
			Cr_DC_Avr /= (CbCr_BKLEN);
		}
		for (int i = 0; i < CbCr_BKLEN; i++)
			for (int j = 0; j < CbCr_BKLEN; j++) {
				Cb_Best[i * img->info.CbCr_Blk_W + j] = Cb_DC_Avr;
				Cr_Best[i * img->info.CbCr_Blk_W + j] = Cr_DC_Avr;
			}
	}
	else if (img->Best_IntraMode[Row * (img->info.width / BKLEN) + Col] == 0) {
		for (int i = 0; i < CbCr_BKLEN; i++)
			for (int j = 0; j < CbCr_BKLEN; j++) {
				Cb_Best[i * img->info.CbCr_Blk_W + j] = Cb_Ref_pix[j];
				Cr_Best[i * img->info.CbCr_Blk_W + j] = Cr_Ref_pix[j];
			}
	}
	else if (img->Best_IntraMode[Row * (img->info.width / BKLEN) + Col] == 1) {
		for (int i = 0; i < CbCr_BKLEN; i++)
			for (int j = 0; j < CbCr_BKLEN; j++) {
				Cb_Best[i * img->info.CbCr_Blk_W + j] = Cb_Ref_pix[CbCr_BKLEN + i];
				Cr_Best[i * img->info.CbCr_Blk_W + j] = Cr_Ref_pix[CbCr_BKLEN + i];
			}
	}

	for (int i = 0; i < img->info.CbCr_Blk_H; i++)
	{
		for (int j = 0; j < img->info.CbCr_Blk_W; j++)
		{
			// Chroma Blue
			if (img->Cb_Resi_blk[i * img->info.CbCr_Blk_W + j] + Cb_Best[i * img->info.CbCr_Blk_W + j] > 255)
				img->Cb_Recon_blk[i * img->info.CbCr_Blk_W + j] = 255;
			else if (img->Cb_Resi_blk[i * img->info.CbCr_Blk_W + j] + Cb_Best[i * img->info.CbCr_Blk_W + j] < 0)
				img->Cb_Recon_blk[i * img->info.CbCr_Blk_W + j] = 0;
			else
				img->Cb_Recon_blk[i * img->info.CbCr_Blk_W + j] 
					= img->Cb_Resi_blk[i * img->info.CbCr_Blk_W + j] + Cb_Best[i * img->info.CbCr_Blk_W + j];

			// Chroma Red
			if (img->Cr_Resi_blk[i * img->info.CbCr_Blk_W + j] + Cr_Best[i * img->info.CbCr_Blk_W + j] > 255)
				img->Cr_Recon_blk[i * img->info.CbCr_Blk_W + j] = 255;
			else if (img->Cr_Resi_blk[i * img->info.CbCr_Blk_W + j] + Cr_Best[i * img->info.CbCr_Blk_W + j] < 0)
				img->Cr_Recon_blk[i * img->info.CbCr_Blk_W + j] = 0;
			else
				img->Cr_Recon_blk[i * img->info.CbCr_Blk_W + j] 
					= img->Cr_Resi_blk[i * img->info.CbCr_Blk_W + j] + Cr_Best[i * img->info.CbCr_Blk_W + j];
		}
	}
}
