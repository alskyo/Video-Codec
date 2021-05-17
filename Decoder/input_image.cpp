#include "buffer.h"

void initialization(Img_Buf* img, Pred_Buf* pred) {

	//FILE* pFile;

	img->info.range_end = 0;
	img->info.codeword = 0;
	img->info.cordword_len = 0;
	img->info.sign = -1;		// -1 은 값이 아직 안들어옴을 의미
	img->info.range = 0;

	img->info.Bitcounter = 0;
	img->info.bitstream = 0;

	img->info.psnr_sum = 0.0;

	img->info.CbCr_width = (img->info.width / 2);
	img->info.CbCr_height = (img->info.height / 2);

	img->info.Blk_H = BKLEN;
	img->info.Blk_W = BKLEN;

	img->info.CbCr_Blk_H = CbCr_BKLEN;
	img->info.CbCr_Blk_W = CbCr_BKLEN;

	img->info.CbCr_Pad_W = img->info.width / 2 + (SEARCH_RANGE - 1) / 2;
	img->info.CbCr_Pad_H = img->info.height / 2 + (SEARCH_RANGE - 1) / 2;

	img->info.Pad_W = img->info.width + SEARCH_RANGE - 1;
	img->info.Pad_H = img->info.height + SEARCH_RANGE - 1;

	// Frame
	img->Org_img = (unsigned char*)calloc(img->info.width * img->info.height, sizeof(unsigned char));
	img->Cb_Org_img = (unsigned char*)calloc(img->info.CbCr_width * img->info.CbCr_height, sizeof(unsigned char));
	img->Cr_Org_img = (unsigned char*)calloc(img->info.CbCr_width * img->info.CbCr_height, sizeof(unsigned char));

	img->Recon_img = (unsigned char*)calloc(img->info.width * img->info.height, sizeof(unsigned char));
	img->Cb_Recon_img = (unsigned char*)calloc(img->info.CbCr_width * img->info.CbCr_height, sizeof(unsigned char));
	img->Cr_Recon_img = (unsigned char*)calloc(img->info.CbCr_width * img->info.CbCr_height, sizeof(unsigned char));

	img->ReconForInter_img = (unsigned char*)calloc(img->info.width * img->info.height, sizeof(unsigned char));
	img->Cb_ReconForInter_img = (unsigned char*)calloc(img->info.CbCr_width * img->info.CbCr_height, sizeof(unsigned char));
	img->Cr_ReconForInter_img = (unsigned char*)calloc(img->info.CbCr_width * img->info.CbCr_height, sizeof(unsigned char));

	img->Quant_img = (int*)calloc(img->info.height * img->info.width, sizeof(int));
	img->Cb_Quant_img = (int*)calloc(img->info.CbCr_height * img->info.CbCr_width, sizeof(int));
	img->Cr_Quant_img = (int*)calloc(img->info.CbCr_height * img->info.CbCr_width, sizeof(int));

	img->Pad_img = (unsigned char*)calloc(img->info.Pad_H * img->info.Pad_W, sizeof(unsigned char));
	img->Cb_Pad_img = (unsigned char*)calloc(img->info.CbCr_Pad_H * img->info.CbCr_Pad_W, sizeof(unsigned char));
	img->Cr_Pad_img = (unsigned char*)calloc(img->info.CbCr_Pad_H * img->info.CbCr_Pad_W, sizeof(unsigned char));

	img->Reorder_blk = (int*)calloc(img->info.Blk_H * img->info.Blk_W, sizeof(int));
	img->Cb_Reorder_blk = (int*)calloc(img->info.CbCr_Blk_H * img->info.CbCr_Blk_W, sizeof(int));
	img->Cr_Reorder_blk = (int*)calloc(img->info.CbCr_Blk_H * img->info.CbCr_Blk_W, sizeof(int));

	img->DC_buffer = (int*)calloc((img->info.width / img->info.Blk_W) * (img->info.height / img->info.Blk_H), sizeof(int));
	img->Cb_DC_buffer = (int*)calloc((img->info.width / img->info.Blk_W) * (img->info.height / img->info.Blk_H), sizeof(int));
	img->Cr_DC_buffer = (int*)calloc((img->info.width / img->info.Blk_W) * (img->info.height / img->info.Blk_H), sizeof(int));

	img->AC_Flag = (int*)calloc((img->info.width / img->info.Blk_W) * (img->info.height / img->info.Blk_H), sizeof(int));
	img->Cb_AC_Flag = (int*)calloc((img->info.width / img->info.Blk_W) * (img->info.height / img->info.Blk_H), sizeof(int));
	img->Cr_AC_Flag = (int*)calloc((img->info.width / img->info.Blk_W) * (img->info.height / img->info.Blk_H), sizeof(int));

	img->DPCM_mode = (int*)calloc((img->info.width / img->info.Blk_W) * (img->info.height / img->info.Blk_H), sizeof(int));
	img->MPM_resi = (int*)calloc((img->info.width / img->info.Blk_W) * (img->info.height / img->info.Blk_H), sizeof(int));
	img->MPM_flag = (int*)calloc((img->info.width / img->info.Blk_W) * (img->info.height / img->info.Blk_H), sizeof(int));
	img->Best_IntraMode = (int*)calloc((img->info.width / img->info.Blk_W) * (img->info.height / img->info.Blk_H), sizeof(int));
	img->MVx = (int*)calloc((img->info.width / img->info.Blk_W) * (img->info.height / img->info.Blk_H), sizeof(int));
	img->MVy = (int*)calloc((img->info.width / img->info.Blk_W) * (img->info.height / img->info.Blk_H), sizeof(int));
	//img->Spiral_xPos = (int*)calloc((SEARCH_RANGE * SEARCH_RANGE), sizeof(int));
	//img->Spiral_yPos = (int*)calloc((SEARCH_RANGE * SEARCH_RANGE), sizeof(int));

	// Block
	img->Cur_blk = (unsigned char*)calloc(img->info.Blk_H * img->info.Blk_W, sizeof(unsigned char));
	img->Cb_Cur_blk = (unsigned char*)calloc(img->info.CbCr_Blk_H * img->info.CbCr_Blk_W, sizeof(unsigned char));
	img->Cr_Cur_blk = (unsigned char*)calloc(img->info.CbCr_Blk_H * img->info.CbCr_Blk_W, sizeof(unsigned char));

	img->DCT_blk = (double*)calloc(img->info.Blk_H * img->info.Blk_W, sizeof(double)); 
	img->Cb_DCT_blk = (double*)calloc(img->info.CbCr_Blk_H * img->info.CbCr_Blk_W, sizeof(double));
	img->Cr_DCT_blk = (double*)calloc(img->info.CbCr_Blk_H * img->info.CbCr_Blk_W, sizeof(double));

	img->Recon_blk = (unsigned char*)calloc(img->info.Blk_H * img->info.Blk_W, sizeof(unsigned char)); 
	img->Cb_Recon_blk = (unsigned char*)calloc(img->info.CbCr_Blk_H * img->info.CbCr_Blk_W, sizeof(unsigned char));
	img->Cr_Recon_blk = (unsigned char*)calloc(img->info.CbCr_Blk_H * img->info.CbCr_Blk_W, sizeof(unsigned char));

	img->Resi_blk = (int*)calloc(img->info.Blk_H * img->info.Blk_W, sizeof(int)); 
	img->Cb_Resi_blk = (int*)calloc(img->info.CbCr_Blk_H * img->info.CbCr_Blk_W, sizeof(int));
	img->Cr_Resi_blk = (int*)calloc(img->info.CbCr_Blk_H * img->info.CbCr_Blk_W, sizeof(int));

	img->Quant_blk = (int*)calloc(img->info.Blk_H * img->info.Blk_W, sizeof(int));
	img->Cb_Quant_blk = (int*)calloc(img->info.CbCr_Blk_H * img->info.CbCr_Blk_W, sizeof(int));
	img->Cr_Quant_blk = (int*)calloc(img->info.CbCr_Blk_H * img->info.CbCr_Blk_W, sizeof(int));

	img->Zigzag_arr = (int*)calloc(img->info.Blk_H * img->info.Blk_W, sizeof(int));
	img->CbCr_Zigzag_arr = (int*)calloc(img->info.CbCr_Blk_H * img->info.CbCr_Blk_W, sizeof(int));

	//pFile = fopen(INPUTFILE, "rb");
	//fread(img->Org_img, sizeof(unsigned char), (img->info.width * img->info.height), pFile);
	//fclose(pFile);
	
}