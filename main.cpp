#include "buffer.h"

void main(int argc, char *argv[]) {

	FILE* fp;
	FILE* pFile;
	FILE* bs;
	Img_Buf img;
	Pred_Buf pred;

	img.info.intra_flag = atoi(argv[1]);
	img.info.dpcm_flag = atoi(argv[2]);
	img.info.dcqp = atoi(argv[3]);
	img.info.acqp = atoi(argv[4]);
	img.info.period = atoi(argv[5]);

	pFile = fopen(INPUTFILE, "rb");
	fp = fopen("C:\\Users\\icsp\\source\\repos\\Encoder\\Release\\Recon_Enc.YUV", "wb");
	bs = fopen(argv[6], "wb");
	int intra_term = img.info.period + 1;
	int frame_mode = 0, header_Flag = 1;
	initialization(&img, &pred);
	Make_Spiral_Search_Array(&img);
	Make_Zigzag_Array(&img);
	Make_Cosine_Array(&img);

	for (int currFrame = 0; currFrame < FRAME; currFrame++) {

		// Enc
		if (currFrame % intra_term == 0) frame_mode = 0;		// Intra
		else frame_mode = 1;									// Inter
		if (currFrame == (FRAME - 1))
			header_Flag = 2;

		Encoder(&img, &pred, fp, pFile, bs, frame_mode, header_Flag);						// 프레임모드 넣어야할듯
		header_Flag = 0;



	}
	fclose(pFile);
	fclose(fp);
	fclose(bs);

	// Dec
	//Decoder(&img, &pred);
}