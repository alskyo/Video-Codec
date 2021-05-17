#include "buffer.h"

void deQuantization(Img_Buf* img)
{
	img->DCT_blk[0] = (double)((double)img->Quant_blk[0] * img->info.qp_dc);
	img->Cb_DCT_blk[0] = (double)((double)img->Cb_Quant_blk[0] * img->info.qp_dc);
	img->Cr_DCT_blk[0] = (double)((double)img->Cr_Quant_blk[0] * img->info.qp_dc);
	for (int i = 0; i < img->info.Blk_H; i++) {
		for (int j = 0; j < img->info.Blk_W; j++) {
			if (i != 0 || j != 0)
				img->DCT_blk[i * img->info.Blk_W + j] = (double)((double)img->Quant_blk[i * img->info.Blk_W + j] * img->info.qp_ac);
		}
	}
	for (int i = 0; i < img->info.CbCr_Blk_H; i++) {
		for (int j = 0; j < img->info.CbCr_Blk_W; j++) {
			if (i != 0 || j != 0) {
				img->Cb_DCT_blk[i * img->info.CbCr_Blk_W + j]
					= (double)((double)img->Cb_Quant_blk[i * img->info.CbCr_Blk_W + j] * img->info.qp_ac);
				img->Cr_DCT_blk[i * img->info.CbCr_Blk_W + j]
					= (double)((double)img->Cr_Quant_blk[i * img->info.CbCr_Blk_W + j] * img->info.qp_ac);
			}
		}
	}
}