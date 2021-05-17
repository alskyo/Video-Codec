#include "buffer.h"

// Inter Prediction 을 위한 패딩
void Padding(Img_Buf* img) {
	//원본 영상을 패딩 영상에 저장
	for (int i = 0; i < img->info.height; i++)
		for (int j = 0; j < img->info.width; j++)
			img->Pad_img[(PADLEN + i) * img->info.Pad_W + j + PADLEN] = img->ReconForInter_img[i * img->info.width + j];

	for (int i = 0; i < img->info.CbCr_height; i++)
		for (int j = 0; j < img->info.CbCr_width; j++)
			img->Cb_Pad_img[(CbCr_PADLEN + i) * img->info.CbCr_Pad_W + j + CbCr_PADLEN] = img->Cb_ReconForInter_img[i * img->info.CbCr_width + j],
			img->Cr_Pad_img[(CbCr_PADLEN + i) * img->info.CbCr_Pad_W + j + CbCr_PADLEN] = img->Cr_ReconForInter_img[i * img->info.CbCr_width + j];
	
	for (int i = 0; i < PADLEN; i++) {
		for (int j = 0; j < img->info.width; j++) {
			// Top
			img->Pad_img[i * img->info.Pad_W + (PADLEN + j)] = img->ReconForInter_img[j];
			// Bottom
			img->Pad_img[(i + PADLEN + img->info.height) * img->info.Pad_W + (PADLEN + j)] 
				= img->ReconForInter_img[(img->info.height - 1) * img->info.width + j];
		}
	}

	for (int i = 0; i < CbCr_PADLEN; i++) {
		for (int j = 0; j < img->info.CbCr_width; j++) {
			// Top
			img->Cb_Pad_img[i * img->info.CbCr_Pad_W + (CbCr_PADLEN + j)] = img->Cb_ReconForInter_img[j];
			img->Cr_Pad_img[i * img->info.CbCr_Pad_W + (CbCr_PADLEN + j)] = img->Cr_ReconForInter_img[j];
			// Bottom
			img->Cb_Pad_img[(i + CbCr_PADLEN + img->info.CbCr_height) * img->info.CbCr_Pad_W + (CbCr_PADLEN + j)] 
				= img->Cb_ReconForInter_img[(img->info.CbCr_height - 1) * img->info.CbCr_width + j];
			img->Cr_Pad_img[(i + CbCr_PADLEN + img->info.CbCr_height) * img->info.CbCr_Pad_W + (CbCr_PADLEN + j)]
				= img->Cr_ReconForInter_img[(img->info.CbCr_height - 1) * img->info.CbCr_width + j];
		}
	}

	for (int i = 0; i < img->info.height; i++) {
		for (int j = 0; j < PADLEN; j++) {
			// Left
			img->Pad_img[(PADLEN + i) * img->info.Pad_W + j] = img->ReconForInter_img[i * img->info.width];
			// Right
			img->Pad_img[(PADLEN + i) * img->info.Pad_W + img->info.width + PADLEN + j] = img->ReconForInter_img[img->info.width * (i + 1) - 1];
		}
	}

	for (int i = 0; i < img->info.CbCr_height; i++) {
		for (int j = 0; j < CbCr_PADLEN; j++) {
			// Left
			img->Cb_Pad_img[(CbCr_PADLEN + i) * img->info.CbCr_Pad_W + j] = img->Cb_ReconForInter_img[i * img->info.CbCr_width];
			img->Cr_Pad_img[(CbCr_PADLEN + i) * img->info.CbCr_Pad_W + j] = img->Cr_ReconForInter_img[i * img->info.CbCr_width];
			// Right
			img->Cb_Pad_img[(CbCr_PADLEN + i) * img->info.CbCr_Pad_W + img->info.CbCr_width + CbCr_PADLEN + j] 
				= img->Cb_ReconForInter_img[img->info.CbCr_width * (i + 1) - 1];
			img->Cr_Pad_img[(CbCr_PADLEN + i) * img->info.CbCr_Pad_W + img->info.CbCr_width + CbCr_PADLEN + j] 
				= img->Cr_ReconForInter_img[img->info.CbCr_width * (i + 1) - 1];
		}
	}

	for (int i = 0; i < PADLEN; i++) {
		for (int j = 0; j < PADLEN; j++) {
			// Left_Top_Corner
			img->Pad_img[i * img->info.Pad_W + j] = img->ReconForInter_img[0];
			// Right_Top_Corner
			img->Pad_img[img->info.width + PADLEN + i * img->info.Pad_W + j] = img->ReconForInter_img[img->info.width - 1];
			// Left_Bottom_Corner
			img->Pad_img[(PADLEN + img->info.height + i) * img->info.Pad_W + j] 
				= img->ReconForInter_img[img->info.width * (img->info.height - 1)];
			// Right_Bottom_Corner
			img->Pad_img[(PADLEN + img->info.height + i) * img->info.Pad_W + PADLEN + img->info.width + j] 
				= img->ReconForInter_img[img->info.height * img->info.width - 1];
		}
	}

	for (int i = 0; i < CbCr_PADLEN; i++) {
		for (int j = 0; j < CbCr_PADLEN; j++) {
			// Left_Top_Corner
			img->Cb_Pad_img[i * img->info.CbCr_Pad_W + j] = img->Cb_ReconForInter_img[0];
			img->Cr_Pad_img[i * img->info.CbCr_Pad_W + j] = img->Cr_ReconForInter_img[0];
			// Right_Top_Corner
			img->Cb_Pad_img[img->info.CbCr_width + CbCr_PADLEN + i * img->info.CbCr_Pad_W + j] = img->Cb_ReconForInter_img[img->info.CbCr_width - 1];
			img->Cr_Pad_img[img->info.CbCr_width + CbCr_PADLEN + i * img->info.CbCr_Pad_W + j] = img->Cr_ReconForInter_img[img->info.CbCr_width - 1];
			// Left_Bottom_Corner
			img->Cb_Pad_img[(CbCr_PADLEN + img->info.CbCr_height + i) * img->info.CbCr_Pad_W + j] 
				= img->Cb_ReconForInter_img[img->info.CbCr_width * (img->info.CbCr_height - 1)];
			img->Cr_Pad_img[(CbCr_PADLEN + img->info.CbCr_height + i) * img->info.CbCr_Pad_W + j]
				= img->Cr_ReconForInter_img[img->info.CbCr_width * (img->info.CbCr_height - 1)];
			// Right_Bottom_Corner
			img->Cb_Pad_img[(CbCr_PADLEN + img->info.CbCr_height + i) * img->info.CbCr_Pad_W + CbCr_PADLEN + img->info.CbCr_width + j] 
				= img->Cb_ReconForInter_img[img->info.CbCr_height * img->info.CbCr_width - 1];
			img->Cr_Pad_img[(CbCr_PADLEN + img->info.CbCr_height + i) * img->info.CbCr_Pad_W + CbCr_PADLEN + img->info.CbCr_width + j]
				= img->Cr_ReconForInter_img[img->info.CbCr_height * img->info.CbCr_width - 1];
		}
	}
}