#include "buffer.h"

void Decoder(Img_Buf* img, Pred_Buf* pred, FILE* bs, FILE* fp)
{


	img->info.frame = 0;
	FILE* oFile;
	oFile = fopen("C:\\Users\\icsp\\Desktop\\Foreman_bitstream\\football_cif(352X288)_90f.yuv", "rb");

	fseek(bs, 0, SEEK_END);
	img->info.file_size = ftell(bs);
	img->info.print = img->info.file_size;
	img->info.print /= 1024;
	fseek(bs, 0, SEEK_SET);

	img->info.file_size -= 14;		// Stream Header Byte

	while (img->info.file_size > 10) {
		//printf("%d\n", img->info.frame + 1);
		Entropy(img, pred, bs);		// frame_unit
		if(img->info.dc_dpcm == 0)
			Inverse_DC_dpcm(img);
		if (((img->info.frame % (img->info.intra_period + 1)) == 0) && (img->info.intra_prediction_flag == 1))
			make_BestIntra(img);
		// dequant 직전에 리오더링 해줘도 상관없을듯
		for (int Blk_Row = 0; Blk_Row < img->info.height / img->info.Blk_H; Blk_Row++) {
			for (int Blk_Col = 0; Blk_Col < img->info.width / img->info.Blk_W; Blk_Col++) {
				// Luma
				for (int i = 0; i < img->info.Blk_H; i++) {
					for (int j = 0; j < img->info.Blk_W; j++) {
						img->Reorder_blk[i * img->info.Blk_W + j]
							= img->Quant_img[(i + Blk_Row * img->info.Blk_H) * (img->info.width) + (j + Blk_Col * img->info.Blk_W)];
					}
				}
				// Chroma
				for (int i = 0; i < img->info.CbCr_Blk_H; i++) {
					for (int j = 0; j < img->info.CbCr_Blk_W; j++) {
						img->Cb_Reorder_blk[i * img->info.CbCr_Blk_W + j]
							= img->Cb_Quant_img[(i + Blk_Row * img->info.CbCr_Blk_H) * (img->info.CbCr_width) + (j + Blk_Col * img->info.CbCr_Blk_W)];
						img->Cr_Reorder_blk[i * img->info.CbCr_Blk_W + j]
							= img->Cr_Quant_img[(i + Blk_Row * img->info.CbCr_Blk_H) * (img->info.CbCr_width) + (j + Blk_Col * img->info.CbCr_Blk_W)];
					}
				}
				Reordering(img);		// include CbCr	// Quant blk 에 저장
				deQuantization(img);	// include CbCr // dct blk
				IDCT(img);				// include CbCr // resi blk
				if ((img->info.frame % (img->info.intra_period + 1)) == 0) {
					if (img->info.intra_prediction_flag == 1) {
						Reconstruct(img, pred, Blk_Row, Blk_Col);
						CbCr_Reconstruct(img, Blk_Row, Blk_Col);
					}
					else {
						for (int i = 0; i < img->info.Blk_H; i++)
						{
							for (int j = 0; j < img->info.Blk_W; j++)
							{
								if (img->Resi_blk[i * img->info.Blk_W + j] + (unsigned char)0 > 255)
									img->Recon_blk[i * img->info.Blk_W + j] = 255;
								else if (img->Resi_blk[i * img->info.Blk_W + j] + (unsigned char)0 < 0)
									img->Recon_blk[i * img->info.Blk_W + j] = 0;
								else
									img->Recon_blk[i * img->info.Blk_W + j] = img->Resi_blk[i * img->info.Blk_W + j] + (unsigned char)0;
							}
						}
						for (int i = 0; i < img->info.CbCr_Blk_H; i++) {
							for (int j = 0; j < img->info.CbCr_Blk_W; j++) {
								if (img->Cb_Resi_blk[i * img->info.CbCr_Blk_W + j] + (unsigned char)0 > 255)
									img->Cb_Recon_blk[i * img->info.CbCr_Blk_W + j] = 255;
								else if (img->Cb_Resi_blk[i * img->info.CbCr_Blk_W + j] + (unsigned char)0 < 0)
									img->Cb_Recon_blk[i * img->info.CbCr_Blk_W + j] = 0;
								else
									img->Cb_Recon_blk[i * img->info.CbCr_Blk_W + j] = img->Cb_Resi_blk[i * img->info.CbCr_Blk_W + j] + (unsigned char)0;

								if (img->Cr_Resi_blk[i * img->info.CbCr_Blk_W + j] + (unsigned char)0 > 255)
									img->Cr_Recon_blk[i * img->info.CbCr_Blk_W + j] = 255;
								else if (img->Cr_Resi_blk[i * img->info.CbCr_Blk_W + j] + (unsigned char)0 < 0)
									img->Cr_Recon_blk[i * img->info.CbCr_Blk_W + j] = 0;
								else
									img->Cr_Recon_blk[i * img->info.CbCr_Blk_W + j] = img->Cr_Resi_blk[i * img->info.CbCr_Blk_W + j] + (unsigned char)0;
							}
						}
					}
				}
				else {
					if (Blk_Row == 0 && Blk_Col == 0) Padding(img);
					Motion_Compensation(img, pred, Blk_Row, Blk_Col);
					CbCr_Motion_Compensation(img, pred, Blk_Row, Blk_Col);
				}
				for (int i = 0; i < img->info.Blk_H; i++)
					for (int j = 0; j < img->info.Blk_W; j++)
						img->Recon_img[(i + Blk_Row * img->info.Blk_H) * (img->info.width) + (j + Blk_Col * img->info.Blk_W)]
						= img->Recon_blk[i * img->info.Blk_W + j];

				for (int i = 0; i < img->info.CbCr_Blk_H; i++) {
					for (int j = 0; j < img->info.CbCr_Blk_W; j++) {
						img->Cb_Recon_img[(i + Blk_Row * img->info.CbCr_Blk_H) * (img->info.CbCr_width) + (j + Blk_Col * img->info.CbCr_Blk_W)]
							= img->Cb_Recon_blk[i * img->info.CbCr_Blk_W + j];
						img->Cr_Recon_img[(i + Blk_Row * img->info.CbCr_Blk_H) * (img->info.CbCr_width) + (j + Blk_Col * img->info.CbCr_Blk_W)]
							= img->Cr_Recon_blk[i * img->info.CbCr_Blk_W + j];
					}
				}
			}
		}
		PSNR(img, oFile);

		fwrite(img->Recon_img, sizeof(unsigned char), img->info.width * img->info.height, fp);
		fwrite(img->Cb_Recon_img, sizeof(unsigned char), img->info.CbCr_width * img->info.CbCr_height, fp);
		fwrite(img->Cr_Recon_img, sizeof(unsigned char), img->info.CbCr_width * img->info.CbCr_height, fp);

		// 여기다가 recon 설계
		memcpy(img->ReconForInter_img, img->Recon_img, img->info.height * img->info.width * sizeof(unsigned char));
		memcpy(img->Cb_ReconForInter_img, img->Cb_Recon_img, img->info.CbCr_height * img->info.CbCr_width * sizeof(unsigned char));
		memcpy(img->Cr_ReconForInter_img, img->Cr_Recon_img, img->info.CbCr_height * img->info.CbCr_width * sizeof(unsigned char));

		img->info.frame += 1;
	}


	fclose(oFile);
}
