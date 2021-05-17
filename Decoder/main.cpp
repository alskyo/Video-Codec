#include "buffer.h"

void main(int argc, char* argv[]) {

	FILE* bs;
	FILE* fp;

	Img_Buf img;
	Pred_Buf pred;

	bs = fopen(argv[1], "rb");
	fp = fopen(argv[2], "wb");
	
	// Dec
	Decoder(&img, &pred, bs, fp);

	img.info.psnr_sum /= (img.info.frame);
	printf("football_psnr : %f || Intra_Pred : %d, DPCM : %d, QPDC : %d, QPAC : %d, Period : %d, FileSize : %d\n", 
		img.info.psnr_sum, img.info.intra_prediction_flag, img.info.dc_dpcm, img.info.qp_dc, img.info.qp_ac, img.info.intra_period, img.info.print);

	End(&img, &pred);
	fclose(fp);
	fclose(bs);
}