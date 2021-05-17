#include "buffer.h"

void PSNR(Img_Buf* img, FILE* oFile)
{
	fread(img->Org_img, sizeof(unsigned char), (img->info.width * img->info.height), oFile);
	fread(img->Cb_Org_img, sizeof(unsigned char), (img->info.CbCr_width * img->info.CbCr_height), oFile);
	fread(img->Cr_Org_img, sizeof(unsigned char), (img->info.CbCr_width * img->info.CbCr_height), oFile);

	int buff = 0;
	int Cb_buff = 0;
	int Cr_buff = 0;
	double mse_sum = 0.0;
	double mse = 0.0, psnr = 0.0, Cb_mse = 0.0, Cb_psnr = 0.0, Cr_mse = 0.0, Cr_psnr = 0.0;

	for (int Blk_Row = 0; Blk_Row < img->info.height / img->info.Blk_H; Blk_Row++) {
		for (int Blk_Col = 0; Blk_Col < img->info.width / img->info.Blk_W; Blk_Col++) {
			for (int i = 0; i < img->info.Blk_H; i++) {
				for (int j = 0; j < img->info.Blk_W; j++) {
					buff = img->Org_img[(i + Blk_Row * img->info.Blk_H) * (img->info.width) + (j + Blk_Col * img->info.Blk_W)]
						- img->Recon_img[(i + Blk_Row * img->info.Blk_H) * (img->info.width) + (j + Blk_Col * img->info.Blk_W)];
					mse += pow(buff, 2);
				}
			}
			for (int i = 0; i < img->info.CbCr_Blk_H; i++) {
				for (int j = 0; j < img->info.CbCr_Blk_W; j++) {
					Cb_buff = img->Cb_Org_img[(i + Blk_Row * img->info.CbCr_Blk_H) * (img->info.CbCr_width) + (j + Blk_Col * img->info.CbCr_Blk_W)]
						- img->Cb_Recon_img[(i + Blk_Row * img->info.CbCr_Blk_H) * (img->info.CbCr_width) + (j + Blk_Col * img->info.CbCr_Blk_W)];
					Cb_mse += pow(Cb_buff, 2);
					Cr_buff = img->Cr_Org_img[(i + Blk_Row * img->info.CbCr_Blk_H) * (img->info.CbCr_width) + (j + Blk_Col * img->info.CbCr_Blk_W)]
						- img->Cr_Recon_img[(i + Blk_Row * img->info.CbCr_Blk_H) * (img->info.CbCr_width) + (j + Blk_Col * img->info.CbCr_Blk_W)];
					Cr_mse += pow(Cr_buff, 2);
				}
			}
		}
	}
	mse /= (img->info.width * img->info.height);		// /M*N
	Cb_mse /= (img->info.CbCr_width * img->info.CbCr_height);
	Cr_mse /= (img->info.CbCr_width * img->info.CbCr_height);
	mse_sum += (mse + Cb_mse + Cr_mse);
	mse_sum /= 3;

	img->info.psnr_sum += 10.0 * log10(pow(255.0, 2) / mse_sum);

	//printf("MSE : %lf\n", mse);
	//psnr = 10 * log10(255.0 * 255.0 / mse);
	//printf("PSNR : %lf\n", psnr);
}