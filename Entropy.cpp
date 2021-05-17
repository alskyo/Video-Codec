#include "buffer.h"

void Byte_Writer(Img_Buf* img, FILE* bs, unsigned char uc_temp, int bit_element) {
	unsigned char buffer; 
	// bit_element 는 Max 8
	for (int i = bit_element; i > 0; i--) {
		buffer = uc_temp;
		buffer = buffer & (unsigned char)pow(2, i - 1);
		buffer >>= i - 1;
		buffer <<= (--img->info.Bitcounter);
		img->info.bitstream = img->info.bitstream | buffer;
		if (img->info.Bitcounter == 0) {
			//printf("%d\n", img->info.bitstream);
			fwrite(&img->info.bitstream, sizeof(unsigned char), 1, bs);
			img->info.Bitcounter = 8;
			img->info.bitstream = 0;
		}
	}
}

void Bit_Form(Img_Buf* img, FILE* bs, int value_buf) {
	int sign, value;
	int val_buf;
	sign = (value_buf >= 0) ? 1 : 0;	// Sign bit is 0 in case of negative value and 1 in case of positive value.
	value = abs(value_buf);

	if (value == 0) Byte_Writer(img, bs, 0, 2);
	else if (value == 1) {
		// Total data length : 4
		Byte_Writer(img, bs, 2, 3);
		Byte_Writer(img, bs, sign, 1);
	}
	else if (value == 2 || value == 3) {
		// Total data length : 5
		value -= 2;
		Byte_Writer(img, bs, 3, 3);
		Byte_Writer(img, bs, sign, 1);
		Byte_Writer(img, bs, value, 1);
	}
	else if (value >= 4 && value <= 7) {
		// Total data length : 6
		value -= 4;
		Byte_Writer(img, bs, 4, 3);
		Byte_Writer(img, bs, sign, 1);
		Byte_Writer(img, bs, value, 2);
	}
	else if (value >= 8 && value <= 15) {
		// Total data length : 7
		value -= 8;
		Byte_Writer(img, bs, 5, 3);
		Byte_Writer(img, bs, sign, 1);
		Byte_Writer(img, bs, value, 3);
	}
	else if (value >= 16 && value <= 31) {
		// Total data length : 8
		value -= 16;
		Byte_Writer(img, bs, 6, 3);
		Byte_Writer(img, bs, sign, 1);
		Byte_Writer(img, bs, value, 4);
	}
	else if (value >= 32 && value <= 63) {
		// Total data length : 10
		value -= 32;
		Byte_Writer(img, bs, 14, 4);
		Byte_Writer(img, bs, sign, 1);
		Byte_Writer(img, bs, value, 5);
	}
	else if (value >= 64 && value <= 127) {
		// Total data length : 12
		value -= 64;
		Byte_Writer(img, bs, 30, 5);
		Byte_Writer(img, bs, sign, 1);
		Byte_Writer(img, bs, value, 6);
	}
	else if (value >= 128 && value <= 255) {
		// Total data length : 14
		value -= 128;
		Byte_Writer(img, bs, 62, 6);
		Byte_Writer(img, bs, sign, 1);
		Byte_Writer(img, bs, value, 7);
	}
	else if (value >= 256 && value <= 511) {
		// Total data length : 16
		value -= 256;
		Byte_Writer(img, bs, 126, 7);
		Byte_Writer(img, bs, sign, 1);
		Byte_Writer(img, bs, value, 8);
	}
	else if (value >= 512 && value <= 1023) {
		// Total data length : 18
		value -= 512;
		val_buf = (int)(value / 2);
		value -= val_buf * 2;
		Byte_Writer(img, bs, 254, 8);
		Byte_Writer(img, bs, sign, 1);
		Byte_Writer(img, bs, val_buf, 8);
		Byte_Writer(img, bs, value, 1);
	}
	else if (value >= 1024 && value <= 2047) {
		// Total data length : 20
		value -= 1024;
		val_buf = (int)(value / 4);
		value -= val_buf * 4;
		Byte_Writer(img, bs, 1, 1);
		Byte_Writer(img, bs, 254, 8);
		Byte_Writer(img, bs, sign, 1);
		Byte_Writer(img, bs, val_buf, 8);
		Byte_Writer(img, bs, value, 2);
	}
	//else if (value >= 2048 && value <= 4095) {
	//	// Total data length : 22
	//	value -= 2048;
	//	val_buf = (int)(value / 8);
	//	value -= val_buf * 8;
	//	Byte_Writer(img, bs, 3, 2);
	//	Byte_Writer(img, bs, 254, 8);
	//	Byte_Writer(img, bs, sign, 1);
	//	Byte_Writer(img, bs, val_buf, 8);
	//	Byte_Writer(img, bs, value, 3);
	//}
	else {
		printf("error\n");
		printf("%d\n", value);
	}
}

void Entropy(Img_Buf* img, FILE* bs, int header_Flag, int frame_mode) {

	unsigned char bitstream, uc_temp;
	char icsp;
	int width = WIDTH;
	int height = HEIGHT;
	unsigned char qp_dc = img->info.dcqp;
	unsigned char qp_ac = img->info.acqp;
	int value = 0;

	// Stream Header
	if (header_Flag == 1) {
		bitstream = 0;
		fwrite(&bitstream, sizeof(unsigned char), 1, bs);
		icsp = 'I';
		fwrite(&icsp, sizeof(char), 1, bs);
		icsp = 'C';
		fwrite(&icsp, sizeof(char), 1, bs);
		icsp = 'S';
		fwrite(&icsp, sizeof(char), 1, bs);
		icsp = 'P';
		fwrite(&icsp, sizeof(char), 1, bs);
		height /= 256;
		fwrite(&height, sizeof(unsigned char), 1, bs);
		height = HEIGHT % 256;
		fwrite(&height, sizeof(unsigned char), 1, bs);
		width /= 256;
		fwrite(&width, sizeof(unsigned char), 1, bs);
		width = WIDTH % 256;
		fwrite(&width, sizeof(unsigned char), 1, bs);
		fwrite(&qp_dc, sizeof(unsigned char), 1, bs);
		fwrite(&qp_ac, sizeof(unsigned char), 1, bs);
		bitstream = PIXEL_DPCM_VALUE;
		bitstream <<= 4;
		bitstream = bitstream | img->info.dpcm_flag;
		fwrite(&bitstream, sizeof(unsigned char), 1, bs);
		bitstream = MV_PRED;
		bitstream <<= 5;
		uc_temp = img->info.period;
		uc_temp >>= 1;
		bitstream = bitstream | uc_temp;
		fwrite(&bitstream, sizeof(unsigned char), 1, bs);
		bitstream = img->info.period;
		bitstream <<= 7;
		uc_temp = img->info.intra_flag;
		uc_temp <<= 6;
		bitstream = bitstream | uc_temp;
		fwrite(&bitstream, sizeof(unsigned char), 1, bs);
	}
	// Luma ************************************************************************************************************
	for (int Blk_Row = 0; Blk_Row < img->info.height / img->info.Blk_H; Blk_Row++) {
		for (int Blk_Col = 0; Blk_Col < img->info.width / img->info.Blk_W; Blk_Col++) {
			// Macro-block Header
			if (frame_mode == 0) {
				// Intra Prediction
				if (Blk_Row == 0 && Blk_Col == 0) {
					if (img->info.intra_flag == 1) {
						uc_temp = 0;
						Byte_Writer(img, bs, uc_temp, 1);	// 값, 비트 수		// MPM flag (0,0)예외처리
						uc_temp = img->MPM_resi[0];			// 0로 추정
						Byte_Writer(img, bs, uc_temp, 1);	// 값, 비트 수		// MPM resi (0,0)예외처리
					}
				}
				else {
					if (img->info.intra_flag == 1) {
						uc_temp = img->MPM_flag[Blk_Row * (img->info.width / img->info.Blk_W) + Blk_Col];	// 0 : origin, 1 : resi
						Byte_Writer(img, bs, uc_temp, 1);
						// MPM_Flag가 1이면 resi 값 넣기
						if (uc_temp == 1) {
							uc_temp = img->MPM_resi[Blk_Row * (img->info.width / img->info.Blk_W) + Blk_Col];
							Byte_Writer(img, bs, uc_temp, 1);
						}
					}
				}
			}
			else {
				//Inter Prediction
				if (Blk_Row == 0 && Blk_Col == 0) {
					uc_temp = 1;			// MV modeflag 8by8 -> 1  이거 한번 확인해봐야할듯 4by4
					Byte_Writer(img, bs, uc_temp, 1);
				}
				value = img->MVx[Blk_Row * (img->info.width / img->info.Blk_W) + Blk_Col];
				Bit_Form(img, bs, value);
				value = img->MVy[Blk_Row * (img->info.width / img->info.Blk_W) + Blk_Col];
				Bit_Form(img, bs, value);
			}
			// DC Value
			value = img->Quant_img[(Blk_Row * img->info.Blk_H) * img->info.width + Blk_Col * img->info.Blk_W];
			Bit_Form(img, bs, value);
			uc_temp = img->AC_Flag[Blk_Row * (img->info.width / img->info.Blk_W) + Blk_Col];
			Byte_Writer(img, bs, uc_temp, 1);
			if (uc_temp == 0) {
				// ACs [63]
				for (int i = 0; i < img->info.Blk_H; i++) {
					for (int j = 0; j < img->info.Blk_W; j++) {
						if (i != 0 || j != 0) {
							value = img->Quant_img[(i + Blk_Row * img->info.Blk_H) * (img->info.width) + (j + Blk_Col * img->info.Blk_W)];
							Bit_Form(img, bs, value);
						}
					}
				}
			}
			// AC_Flag 가 1이면 ACs는 pass
		}
	}
	// Chroma Blue ************************************************************************************************************
	for (int Blk_Row = 0; Blk_Row < img->info.height / img->info.Blk_H; Blk_Row++) {
		for (int Blk_Col = 0; Blk_Col < img->info.width / img->info.Blk_W; Blk_Col++) {
			// Macro-block Header
			// DC Value
			value = img->Cb_Quant_img[(Blk_Row * img->info.CbCr_Blk_H) * img->info.CbCr_width + Blk_Col * img->info.CbCr_Blk_W];
			Bit_Form(img, bs, value);
			uc_temp = img->Cb_AC_Flag[Blk_Row * (img->info.CbCr_width / img->info.CbCr_Blk_W) + Blk_Col];
			Byte_Writer(img, bs, uc_temp, 1);
			if (uc_temp == 0) {
				// ACs [15]
				for (int i = 0; i < img->info.CbCr_Blk_H; i++) {
					for (int j = 0; j < img->info.CbCr_Blk_W; j++) {
						if (i != 0 || j != 0) {
							value = img->Cb_Quant_img[(i + Blk_Row * img->info.CbCr_Blk_H) * (img->info.CbCr_width) + (j + Blk_Col * img->info.CbCr_Blk_W)];
							Bit_Form(img, bs, value);
						}
					}
				}
			}
			// AC_Flag 가 1이면 ACs는 pass
		}
	}
	// Chroma Red ************************************************************************************************************
	for (int Blk_Row = 0; Blk_Row < img->info.height / img->info.Blk_H; Blk_Row++) {
		for (int Blk_Col = 0; Blk_Col < img->info.width / img->info.Blk_W; Blk_Col++) {
			// Macro-block Header
			// DC Value
			value = img->Cr_Quant_img[(Blk_Row * img->info.CbCr_Blk_H) * img->info.CbCr_width + Blk_Col * img->info.CbCr_Blk_W];
			Bit_Form(img, bs, value);
			uc_temp = img->Cr_AC_Flag[Blk_Row * (img->info.CbCr_width / img->info.CbCr_Blk_W) + Blk_Col];
			Byte_Writer(img, bs, uc_temp, 1);
			if (uc_temp == 0) {
				// ACs [15]
				for (int i = 0; i < img->info.CbCr_Blk_H; i++) {
					for (int j = 0; j < img->info.CbCr_Blk_W; j++) {
						if (i != 0 || j != 0) {
							value = img->Cr_Quant_img[(i + Blk_Row * img->info.CbCr_Blk_H) * (img->info.CbCr_width) + (j + Blk_Col * img->info.CbCr_Blk_W)];
							Bit_Form(img, bs, value);
						}
					}
				}
			}
			// AC_Flag 가 1이면 ACs는 pass
		}
	}
	if ((header_Flag == 2) && (img->info.Bitcounter != 8)) {
		fwrite(&img->info.bitstream, sizeof(unsigned char), 1, bs);
	}
}
