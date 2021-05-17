#include "buffer.h"

void End(Img_Buf* img, Pred_Buf* pred)
{
	free(img->Recon_img);
	free(img->Cb_Recon_img);
	free(img->Cr_Recon_img);

	free(img->ReconForInter_img);
	free(img->Cb_ReconForInter_img);
	free(img->Cr_ReconForInter_img);

	free(img->Quant_img);
	free(img->Cb_Quant_img);
	free(img->Cr_Quant_img);

	free(img->Pad_img);
	free(img->Cb_Pad_img);
	free(img->Cr_Pad_img);

	free(img->Reorder_blk);
	free(img->Cb_Reorder_blk);
	free(img->Cr_Reorder_blk);

	free(img->DC_buffer);
	free(img->Cb_DC_buffer);
	free(img->Cr_DC_buffer);
	
	free(img->AC_Flag);
	free(img->Cb_AC_Flag);
	free(img->Cr_AC_Flag);

	free(img->DPCM_mode);
	free(img->MPM_resi);
	free(img->MPM_flag);
	free(img->Best_IntraMode);

	free(img->MVx);
	free(img->MVy);

	free(img->DCT_blk);
	free(img->Cb_DCT_blk);
	free(img->Cr_DCT_blk);

	free(img->Recon_blk);
	free(img->Cb_Recon_blk);
	free(img->Cr_Recon_blk);

	free(img->Resi_blk);
	free(img->Cb_Resi_blk);
	free(img->Cr_Resi_blk);

	free(img->Quant_blk);
	free(img->Cb_Quant_blk);
	free(img->Cr_Quant_blk);

	free(img->Zigzag_arr);
	free(img->CbCr_Zigzag_arr);
}