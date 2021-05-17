#include "buffer.h"
// 리더 모듈화
void Value_reader(Img_Buf* img, FILE* bs) {
	while (img->info.range_end == 0) {
		Byte_reader(img, bs);
		//Stream_check(img, bs);
	}
	img->info.range_end = 0;
	img->info.codeword = 0;
	img->info.cordword_len = 0;
	img->info.sign = -1;		// -1 은 값이 아직 안들어옴을 의미
	img->info.range = 0;
	Stream_check(img, bs);
}
void Byte_reader(Img_Buf* img, FILE* bs) {
	unsigned char buffer;
	for (int i = img->info.Bitcounter; i > 0; i--) {
		buffer = img->info.bitstream;
		buffer = buffer & (unsigned char)pow(2, i - 1);
		buffer >>= (i - 1);
		img->info.Bitcounter -= 1;
		Stream_check(img, bs);
		Bit_form(img, bs, buffer);
		if (img->info.range_end == 1) {
			if (img->info.sign == 1) img->info.value = img->info.range;
			else img->info.value = (-1) * img->info.range;
			break;
		}
	}
}
void Stream_check(Img_Buf* img, FILE* bs) {
	if (img->info.Bitcounter == 0) {
		fread(&img->info.bitstream, sizeof(unsigned char), 1, bs);
		img->info.Bitcounter = 8;
		img->info.file_size -= 1;
	}
}


void Bit_form(Img_Buf* img, FILE* bs, unsigned char temp) {
	
	if (img->info.sign == -1) {
		img->info.codeword *= 2;
		img->info.codeword += temp;
		img->info.cordword_len += 1;
		if (img->info.cordword_len == 2) {
			if (img->info.codeword == 0) {
				// Category 0
				img->info.range_end = 1;
			}
		}
		else {
			if (img->info.codeword == 2) {
				// Category 1
				img->info.sign = -2;		// -2는 코드를 받을 차례라는 의미
				img->info.range_len = 0;	// range 값 존재 x
				img->info.range += 1;
			}
			else if (img->info.codeword == 3) {
				// Category 2
				img->info.sign = -2;
				img->info.range_len = 1;
				img->info.range += 2;
			}
			else if (img->info.codeword == 4) {
				// Category 3
				img->info.sign = -2;
				img->info.range_len = 2;
				img->info.range += 4;
			}
			else if (img->info.codeword == 5) {
				// Category 4
				img->info.sign = -2;
				img->info.range_len = 3;
				img->info.range += 8;
			}
			else if (img->info.codeword == 6) {
				// Category 5
				img->info.sign = -2;
				img->info.range_len = 4;
				img->info.range += 16;
			}
			else if (img->info.codeword == 14) {
				// Category 6
				img->info.sign = -2;
				img->info.range_len = 5;
				img->info.range += 32;
			}
			else if (img->info.codeword == 30) {
				// Category 7
				img->info.sign = -2;
				img->info.range_len = 6;
				img->info.range += 64;
			}
			else if (img->info.codeword == 62) {
				// Category 8
				img->info.sign = -2;
				img->info.range_len = 7;
				img->info.range += 128;
			}
			else if (img->info.codeword == 126) {
				// Category 9
				img->info.sign = -2;
				img->info.range_len = 8;
				img->info.range += 256;
			}
			else if (img->info.codeword == 254) {
				// Category 10
				img->info.sign = -2;
				img->info.range_len = 9;
				img->info.range += 512;
			}
			else if (img->info.codeword == 510) {
				// Category 11
				img->info.sign = -2;
				img->info.range_len = 10;
				img->info.range += 1024;
			}
		}
	}
	else if (img->info.sign == -2) {
		img->info.sign = temp;
		if (img->info.range_len == 0) img->info.range_end = 1;
	}
	else {
		img->info.range += temp * pow(2, --img->info.range_len);
		// 여기부터
		if (img->info.range_len == 0) img->info.range_end = 1;
	}
}
// 읽고싶은 비트 수를 적어주면 값으로 변환해주는 함수
void bit_reader(Img_Buf* img, FILE* bs, int bit_elements) {
	unsigned char bit_buf;
	unsigned char sum_buf;

	bit_buf = 0, sum_buf = 0;
	img->info.bit_value = 0;

	for (int i = 0; i < bit_elements; i++) {
		bit_buf = img->info.bitstream;
		bit_buf = bit_buf & (unsigned char)pow(2, --img->info.Bitcounter);
		bit_buf >>= img->info.Bitcounter;
		bit_buf <<= bit_elements - i - 1;
		Stream_check(img, bs);
		sum_buf += bit_buf;
	}
	img->info.bit_value = sum_buf;
}

void frame_reader(Img_Buf* img, FILE* bs) {
	unsigned char bit_buf;

	// Macro Block Reader
	// MPM_flag
	Stream_check(img, bs);
	for (int Blk_Row = 0; Blk_Row < img->info.height / img->info.Blk_H; Blk_Row++) {
		for (int Blk_Col = 0; Blk_Col < img->info.width / img->info.Blk_W; Blk_Col++) {
			
			if (Blk_Row == 0 && Blk_Col == 0) {
				if ((img->info.frame % (img->info.intra_period + 1)) == 0)		// Intra_frame
				{
					if (img->info.intra_prediction_flag == 1) {
						bit_reader(img, bs, 1);
						img->MPM_flag[Blk_Row * img->info.width / img->info.Blk_W + Blk_Col] = img->info.bit_value;
						bit_reader(img, bs, 1);
						img->MPM_resi[Blk_Row * img->info.width / img->info.Blk_W + Blk_Col] = img->info.bit_value;
					}
				}
				else  // Inter_frame
				{
					bit_reader(img, bs, 1);		// MV mode flag (just read)
					Value_reader(img, bs);		
					img->MVx[Blk_Row * img->info.width / img->info.Blk_W + Blk_Col] = img->info.value;  // MV read
					Value_reader(img, bs);
					img->MVy[Blk_Row * img->info.width / img->info.Blk_W + Blk_Col] = img->info.value;  
				}
			}
			else {
				if ((img->info.frame % (img->info.intra_period + 1)) == 0)		// Intra_frame
				{
					if (img->info.intra_prediction_flag == 1) {
						bit_reader(img, bs, 1);
						img->MPM_flag[Blk_Row * img->info.width / img->info.Blk_W + Blk_Col] = img->info.bit_value;
						if (img->info.bit_value == 1) {
							bit_reader(img, bs, 1);
							img->MPM_resi[Blk_Row * img->info.width / img->info.Blk_W + Blk_Col] = img->info.bit_value;
						}
					}
				}
				else  // Inter_frame 
				{
					Value_reader(img, bs);		// MV read
					img->MVx[Blk_Row * img->info.width / img->info.Blk_W + Blk_Col] = img->info.value;
					Value_reader(img, bs);
					img->MVy[Blk_Row * img->info.width / img->info.Blk_W + Blk_Col] = img->info.value;
				}
			}
			Value_reader(img, bs);		// DC value
			img->Quant_img[(Blk_Row * img->info.Blk_H) * img->info.width + Blk_Col * img->info.Blk_W] = img->info.value;
			bit_reader(img, bs, 1);		// AC_flag read
			img->AC_Flag[Blk_Row * (img->info.width / img->info.Blk_W) + Blk_Col] = img->info.bit_value;
			if (img->info.bit_value == 0) {
				for (int i = 0; i < img->info.Blk_H; i++) {
					for (int j = 0; j < img->info.Blk_W; j++) {
						if (i != 0 || j != 0) {
							Value_reader(img, bs);		// ACs read
							img->Quant_img[(i + Blk_Row * img->info.Blk_H) * (img->info.width) + (j + Blk_Col * img->info.Blk_W)]
								= img->info.value;
						}
					}
				}
			}
			else {
				for (int i = 0; i < img->info.Blk_H; i++)
					for (int j = 0; j < img->info.Blk_W; j++)
						if (i != 0 || j != 0) img->Quant_img[(i + Blk_Row * img->info.Blk_H) * (img->info.width) + (j + Blk_Col * img->info.Blk_W)] = 0;
			}
		}
	}
	// Chroma
	for (int Blk_Row = 0; Blk_Row < img->info.height / img->info.Blk_H; Blk_Row++) {
		for (int Blk_Col = 0; Blk_Col < img->info.width / img->info.Blk_W; Blk_Col++) {

			Value_reader(img, bs);		// DC value
			img->Cb_Quant_img[(Blk_Row * img->info.CbCr_Blk_H) * img->info.CbCr_width + Blk_Col * img->info.CbCr_Blk_W] = img->info.value;
			bit_reader(img, bs, 1);		// AC_flag read
			img->Cb_AC_Flag[Blk_Row * (img->info.width / img->info.Blk_W) + Blk_Col] = img->info.bit_value;
			if (img->info.bit_value == 0) {
				for (int i = 0; i < img->info.CbCr_Blk_H; i++) {
					for (int j = 0; j < img->info.CbCr_Blk_W; j++) {
						if (i != 0 || j != 0) {
							Value_reader(img, bs);		// ACs read
							img->Cb_Quant_img[(i + Blk_Row * img->info.CbCr_Blk_H) * (img->info.CbCr_width) + (j + Blk_Col * img->info.CbCr_Blk_W)]
								= img->info.value;
						}
					}
				}
			}
			else {
				for (int i = 0; i < img->info.CbCr_Blk_H; i++)
					for (int j = 0; j < img->info.CbCr_Blk_W; j++)
						if (i != 0 || j != 0) img->Cb_Quant_img[(i + Blk_Row * img->info.CbCr_Blk_H) * (img->info.CbCr_width) + (j + Blk_Col * img->info.CbCr_Blk_W)] = 0;
			}
		}
	}
	for (int Blk_Row = 0; Blk_Row < img->info.height / img->info.Blk_H; Blk_Row++) {
		for (int Blk_Col = 0; Blk_Col < img->info.width / img->info.Blk_W; Blk_Col++) {
			Value_reader(img, bs);		// DC value
			img->Cr_Quant_img[(Blk_Row * img->info.CbCr_Blk_H) * img->info.CbCr_width + Blk_Col * img->info.CbCr_Blk_W] = img->info.value;
			bit_reader(img, bs, 1);		// AC_flag read
			img->Cr_AC_Flag[Blk_Row * (img->info.width / img->info.Blk_W) + Blk_Col] = img->info.bit_value;
			if (img->info.bit_value == 0) {
				for (int i = 0; i < img->info.CbCr_Blk_H; i++) {
					for (int j = 0; j < img->info.CbCr_Blk_W; j++) {
						if (i != 0 || j != 0) {
							Value_reader(img, bs);		// ACs read
							img->Cr_Quant_img[(i + Blk_Row * img->info.CbCr_Blk_H) * (img->info.CbCr_width) + (j + Blk_Col * img->info.CbCr_Blk_W)]
								= img->info.value;
						}
					}
				}
			}
			else {
				for (int i = 0; i < img->info.CbCr_Blk_H; i++)
					for (int j = 0; j < img->info.CbCr_Blk_W; j++)
						if (i != 0 || j != 0) img->Cr_Quant_img[(i + Blk_Row * img->info.CbCr_Blk_H) * (img->info.CbCr_width) + (j + Blk_Col * img->info.CbCr_Blk_W)] = 0;
			}
		}
	}
}

void Entropy(Img_Buf* img, Pred_Buf* pred, FILE* bs) {
	unsigned char input_stream, uc_temp;

	if (img->info.frame == 0) {
		// Stream Header
		fseek(bs, 5, SEEK_CUR);									// 0 & ICSP
		fread(&input_stream, sizeof(unsigned char), 1, bs);		// Height 상위 비트
		img->info.height = input_stream * pow(2, 8);
		fread(&input_stream, sizeof(unsigned char), 1, bs);		// Height 하위 비트
		img->info.height += input_stream;
		fread(&input_stream, sizeof(unsigned char), 1, bs);		// Width 상위 비트
		img->info.width = input_stream * pow(2, 8);
		fread(&input_stream, sizeof(unsigned char), 1, bs);		// Width 하위 비트
		img->info.width += input_stream;
		fread(&input_stream, sizeof(unsigned char), 1, bs);		// QP_DC
		img->info.qp_dc = input_stream;
		//printf("%d\n", img->info.qp_dc);
		fread(&input_stream, sizeof(unsigned char), 1, bs);		// QP_AC
		img->info.qp_ac = input_stream;
		//printf("%d\n", img->info.qp_ac);
		fread(&input_stream, sizeof(unsigned char), 1, bs);		// DPCM
		img->info.pixel_dpcm = input_stream;
		img->info.pixel_dpcm >>= 4;
		img->info.dc_dpcm = input_stream - (img->info.pixel_dpcm * pow(2, 4));
		fread(&input_stream, sizeof(unsigned char), 1, bs);		// MV_pred + Intra(-1)
		img->info.intra_period = input_stream;
		fread(&input_stream, sizeof(unsigned char), 1, bs);		// period + Intra_pred
		input_stream >>= 6;
		img->info.intra_prediction_flag = input_stream % 2;
		img->info.intra_period = img->info.intra_period * 2 + (int)(input_stream / 2);
		initialization(img, pred);
		Make_Zigzag_Array(img);
		Make_Cosine_Array(img);
	}
	frame_reader(img, bs);
}