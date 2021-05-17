#include "buffer.h"

void Inverse_DC_dpcm(Img_Buf* img) {

	int left, left_upper, upper, median;
	int Cb_left, Cb_left_upper, Cb_upper, Cb_median;
	int Cr_left, Cr_left_upper, Cr_upper, Cr_median;

	for (int Blk_Row = 0; Blk_Row < img->info.height / img->info.Blk_H; Blk_Row++) {
		for (int Blk_Col = 0; Blk_Col < img->info.width / img->info.Blk_W; Blk_Col++) {
			img->DC_buffer[Blk_Row * (img->info.width / img->info.Blk_W) + Blk_Col]
				= img->Quant_img[(Blk_Row * img->info.Blk_H) * img->info.width + Blk_Col * img->info.Blk_W];
			// chroma
			img->Cb_DC_buffer[Blk_Row * (img->info.width / img->info.Blk_W) + Blk_Col]
				= img->Cb_Quant_img[(Blk_Row * img->info.CbCr_Blk_H) * img->info.CbCr_width + Blk_Col * img->info.CbCr_Blk_W];
			img->Cr_DC_buffer[Blk_Row * (img->info.width / img->info.Blk_W) + Blk_Col]
				= img->Cr_Quant_img[(Blk_Row * img->info.CbCr_Blk_H) * img->info.CbCr_width + Blk_Col * img->info.CbCr_Blk_W];
			if (Blk_Row == 0 && Blk_Col != 0) {
				img->DC_buffer[Blk_Col] += img->DC_buffer[(Blk_Col - 1)];
				// chroma
				img->Cb_DC_buffer[Blk_Col] += img->Cb_DC_buffer[(Blk_Col - 1)];
				img->Cr_DC_buffer[Blk_Col] += img->Cr_DC_buffer[(Blk_Col - 1)];
			}
			else if (Blk_Row != 0 && Blk_Col != 0) {
				left = img->DC_buffer[Blk_Row * (img->info.width / img->info.Blk_W) + Blk_Col - 1];
				upper = img->DC_buffer[(Blk_Row - 1) * (img->info.width / img->info.Blk_W) + Blk_Col];
				left_upper = img->DC_buffer[(Blk_Row - 1) * (img->info.width / img->info.Blk_W) + Blk_Col - 1];

				Cb_left = img->Cb_DC_buffer[Blk_Row * (img->info.width / img->info.Blk_W) + Blk_Col - 1];
				Cb_upper = img->Cb_DC_buffer[(Blk_Row - 1) * (img->info.width / img->info.Blk_W) + Blk_Col];
				Cb_left_upper = img->Cb_DC_buffer[(Blk_Row - 1) * (img->info.width / img->info.Blk_W) + Blk_Col - 1];

				Cr_left = img->Cr_DC_buffer[Blk_Row * (img->info.width / img->info.Blk_W) + Blk_Col - 1];
				Cr_upper = img->Cr_DC_buffer[(Blk_Row - 1) * (img->info.width / img->info.Blk_W) + Blk_Col];
				Cr_left_upper = img->Cr_DC_buffer[(Blk_Row - 1) * (img->info.width / img->info.Blk_W) + Blk_Col - 1];

				if (left > upper) {
					if (left > left_upper) {
						if (left_upper > upper)
							img->DC_buffer[Blk_Row * (img->info.width / img->info.Blk_W) + Blk_Col] + left_upper;
						else
							img->DC_buffer[Blk_Row * (img->info.width / img->info.Blk_W) + Blk_Col] + upper;
					}
					else {
						img->DC_buffer[Blk_Row * (img->info.width / img->info.Blk_W) + Blk_Col] + left;
					}
				}
				else {
					if (left > left_upper) {
						img->DC_buffer[Blk_Row * (img->info.width / img->info.Blk_W) + Blk_Col] + left;
					}
					else {
						if (upper > left_upper)
							img->DC_buffer[Blk_Row * (img->info.width / img->info.Blk_W) + Blk_Col] + left_upper;
						else
							img->DC_buffer[Blk_Row * (img->info.width / img->info.Blk_W) + Blk_Col] + upper;
					}
				}
				// Cb
				if (Cb_left > Cb_upper) {
					if (Cb_left > Cb_left_upper) {
						if (Cb_left_upper > Cb_upper)
							img->Cb_DC_buffer[Blk_Row * (img->info.width / img->info.Blk_W) + Blk_Col] + Cb_left_upper;
						else
							img->Cb_DC_buffer[Blk_Row * (img->info.width / img->info.Blk_W) + Blk_Col] + Cb_upper;
					}
					else {
						img->Cb_DC_buffer[Blk_Row * (img->info.width / img->info.Blk_W) + Blk_Col] + Cb_left;
					}
				}
				else {
					if (Cb_left > Cb_left_upper) {
						img->Cb_DC_buffer[Blk_Row * (img->info.width / img->info.Blk_W) + Blk_Col] + Cb_left;
					}
					else {
						if (Cb_upper > Cb_left_upper)
							img->Cb_DC_buffer[Blk_Row * (img->info.width / img->info.Blk_W) + Blk_Col] + Cb_left_upper;
						else
							img->Cb_DC_buffer[Blk_Row * (img->info.width / img->info.Blk_W) + Blk_Col] + Cb_upper;
					}
				}
				// Cr
				if (Cr_left > Cr_upper) {
					if (Cr_left > Cr_left_upper) {
						if (Cr_left_upper > Cr_upper)
							img->Cr_DC_buffer[Blk_Row * (img->info.width / img->info.Blk_W) + Blk_Col] + Cr_left_upper;
						else
							img->Cr_DC_buffer[Blk_Row * (img->info.width / img->info.Blk_W) + Blk_Col] + Cr_upper;
					}
					else {
						img->Cr_DC_buffer[Blk_Row * (img->info.width / img->info.Blk_W) + Blk_Col] + Cr_left;
					}
				}
				else {
					if (Cr_left > Cr_left_upper) {
						img->Cr_DC_buffer[Blk_Row * (img->info.width / img->info.Blk_W) + Blk_Col] + Cr_left;
					}
					else {
						if (Cr_upper > Cr_left_upper)
							img->Cr_DC_buffer[Blk_Row * (img->info.width / img->info.Blk_W) + Blk_Col] + Cr_left_upper;
						else
							img->Cr_DC_buffer[Blk_Row * (img->info.width / img->info.Blk_W) + Blk_Col] + Cr_upper;
					}
				}
			}
		}
		if (Blk_Row != 0) {
			img->DC_buffer[Blk_Row * (img->info.width / img->info.Blk_W)] += img->DC_buffer[(Blk_Row - 1) * (img->info.width / img->info.Blk_W)];
			img->Cb_DC_buffer[Blk_Row * (img->info.width / img->info.Blk_W)] += img->Cb_DC_buffer[(Blk_Row - 1) * (img->info.CbCr_width / img->info.CbCr_Blk_W)];
			img->Cr_DC_buffer[Blk_Row * (img->info.width / img->info.Blk_W)] += img->Cr_DC_buffer[(Blk_Row - 1) * (img->info.CbCr_width / img->info.CbCr_Blk_W)];
		}
	}
	for (int Blk_Row = 0; Blk_Row < img->info.height / img->info.Blk_H; Blk_Row++)
		for (int Blk_Col = 0; Blk_Col < img->info.width / img->info.Blk_H; Blk_Col++) {
			img->Quant_img[(Blk_Row * img->info.Blk_H) * img->info.width + Blk_Col * img->info.Blk_W]
				= img->DC_buffer[Blk_Row * (img->info.width / img->info.Blk_W) + Blk_Col];
			img->Cb_Quant_img[(Blk_Row * img->info.CbCr_Blk_H) * img->info.CbCr_width + Blk_Col * img->info.CbCr_Blk_W]
				= img->Cb_DC_buffer[Blk_Row * (img->info.width / img->info.Blk_W) + Blk_Col];
			img->Cr_Quant_img[(Blk_Row * img->info.CbCr_Blk_H) * img->info.CbCr_width + Blk_Col * img->info.CbCr_Blk_W]
				= img->Cr_DC_buffer[Blk_Row * (img->info.width / img->info.Blk_W) + Blk_Col];
		}
}