#include "buffer.h"

void Quantization(Img_Buf* img) {
	img->Quant_blk[0] = (int)(img->DCT_blk[0] / (int)img->info.dcqp);
	img->Cb_Quant_blk[0] = (int)(img->Cb_DCT_blk[0] / (int)img->info.dcqp);
	img->Cr_Quant_blk[0] = (int)(img->Cr_DCT_blk[0] / (int)img->info.dcqp);
	for (int i = 0; i < img->info.Blk_H; i++) {
		for (int j = 0; j < img->info.Blk_W; j++) {
			if (i != 0 || j != 0)
				img->Quant_blk[i * img->info.Blk_W + j] = (int)(img->DCT_blk[i * img->info.Blk_W + j] / (int)img->info.acqp);
		}
	}
	for (int i = 0; i < img->info.CbCr_Blk_H; i++) {
		for (int j = 0; j < img->info.CbCr_Blk_W; j++) {
			if (i != 0 || j != 0) {
				img->Cb_Quant_blk[i * img->info.CbCr_Blk_W + j]
					= (int)(img->Cb_DCT_blk[i * img->info.CbCr_Blk_W + j] / (int)img->info.acqp);
				img->Cr_Quant_blk[i * img->info.CbCr_Blk_W + j]
					= (int)(img->Cr_DCT_blk[i * img->info.CbCr_Blk_W + j] / (int)img->info.acqp);
			}
		}
	}
}

void deQuantization(Img_Buf* img)
{	
	img->DCT_blk[0] = (double)((double)img->Quant_blk[0] * img->info.dcqp);
	img->Cb_DCT_blk[0] = (double)((double)img->Cb_Quant_blk[0] * img->info.dcqp);
	img->Cr_DCT_blk[0] = (double)((double)img->Cr_Quant_blk[0] * img->info.dcqp);
	for (int i = 0; i < img->info.Blk_H; i++) {
		for (int j = 0; j < img->info.Blk_W; j++) {
			if (i != 0 || j != 0)
				img->DCT_blk[i * img->info.Blk_W + j] = (double)((double)img->Quant_blk[i * img->info.Blk_W + j] * img->info.acqp);
		}
	}
	for (int i = 0; i < img->info.CbCr_Blk_H; i++) {
		for (int j = 0; j < img->info.CbCr_Blk_W; j++) {
			if (i != 0 || j != 0) {
				img->Cb_DCT_blk[i * img->info.CbCr_Blk_W + j] 
					= (double)((double)img->Cb_Quant_blk[i * img->info.CbCr_Blk_W + j] * img->info.acqp);
				img->Cr_DCT_blk[i * img->info.CbCr_Blk_W + j]
					= (double)((double)img->Cr_Quant_blk[i * img->info.CbCr_Blk_W + j] * img->info.acqp);
			}
		}
	}
} 
