#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define _USE_MATH_DEFINES
#include <math.h>

#define BKLEN 8
#define BKSIZE (BKLEN * BKLEN)
#define CbCr_BKLEN (BKLEN / 2)
#define CbCr_BKSIZE (CbCr_BKLEN * CbCr_BKLEN)
#define SEARCH_RANGE 33
#define CbCr_SEARCH_RANGE ((SEARCH_RANGE - 1) / 2 + 1)
#define PADLEN ((SEARCH_RANGE - 1)/2)
#define CbCr_PADLEN ((SEARCH_RANGE - 1) / 4) 

enum mode
{
	DC,
	VERTICAL,
	HORIZONTAL,
	Max_Mode_num
};


typedef struct _Image_information
{
	int width;			 // ����/���� ������ ���� ũ��
	int height;			 // ����/���� ������ ���� ũ��
	int qp_dc;
	int qp_ac;
	int pixel_dpcm;
	int dc_dpcm;
	int intra_period;
	int intra_prediction_flag;
	int frame;
	int frame_mode;
	int range_end;
	int bit_value;
	int print;
	double psnr_sum;

	int codeword, sign, range, cordword_len, range_len, value;

	int file_size;

	int CbCr_width;		 // ũ�θ� ����
	int CbCr_height;	 // ũ�θ� ����
	int Pad_W;			 // �е� ����
	int Pad_H;			 // �е� ����
	int CbCr_Pad_W;		 // ũ�θ� �е� ����
	int CbCr_Pad_H;		 // ũ�θ� �е� ����
	int Blk_W;			 // ����� ���� ũ��
	int Blk_H;			 // ����� ���� ũ��
	int CbCr_Blk_W;		 // ũ�θ� ����� ���� ũ��
	int CbCr_Blk_H;		 // ũ�θ� ����� ���� ũ��

	unsigned char bitstream;
	int Bitcounter;			// bit counter
}Info;

typedef struct _Image_Buffer
{
	unsigned char* Recon_img;   // ���� ���� ���� ���� 
	unsigned char* Cb_Recon_img;
	unsigned char* Cr_Recon_img;

	unsigned char* ReconForInter_img;   // ���� ���� ���� ���� 
	unsigned char* Cb_ReconForInter_img;
	unsigned char* Cr_ReconForInter_img;

	unsigned char* Pad_img;		// �е� ���� ���� ����
	unsigned char* Cb_Pad_img;
	unsigned char* Cr_Pad_img;

	unsigned char* Cur_blk;     // ���� ��� ���� ����
	unsigned char* Cb_Cur_blk;     // ���� ��� ���� ����
	unsigned char* Cr_Cur_blk;     // ���� ��� ���� ����

	unsigned char* Recon_blk;     // ���� ��� ���� ����
	unsigned char* Cb_Recon_blk;
	unsigned char* Cr_Recon_blk;

	int* Resi_blk;    // ���� ��� ���� ���� (���� ��� = ���� ��� - ���� ���), ������ ���� ��� ���� ����
	int* Cb_Resi_blk;
	int* Cr_Resi_blk;

	unsigned char* Org_img;
	unsigned char* Cb_Org_img;
	unsigned char* Cr_Org_img;

	double* DCT_blk;     // ��ȯ ��� ���� ����
	double* Cb_DCT_blk;
	double* Cr_DCT_blk;

	int* Reorder_blk;     // Zig-zag ���� ����
	int* Cb_Reorder_blk;
	int* Cr_Reorder_blk;

	int* AC_Flag;     // AC_Flag ���� ����
	int* Cb_AC_Flag;
	int* Cr_AC_Flag;

	int* Quant_blk;     // ��ȯ ��� ���� ����
	int* Cb_Quant_blk;
	int* Cr_Quant_blk;

	int* Test_img;

	int* Quant_img;
	int* Cb_Quant_img;
	int* Cr_Quant_img;

	int* Zigzag_arr;
	int* CbCr_Zigzag_arr;

	int* Spiral_xPos;
	int* Spiral_yPos;

	int* DC_buffer;
	int* Cb_DC_buffer;
	int* Cr_DC_buffer;

	int* DPCM_mode;
	int* MPM_flag;
	int* MPM_resi;
	int* Best_IntraMode;   // ������ ���� ��� ���� ����
	int* MVx;
	int* MVy;
	Info info;

}Img_Buf;

typedef struct _Pred_Buffer
{
	unsigned char* Ver;
	unsigned char* Hor;
	unsigned char* Dc;

	int* Resi_Val;
	int* Resi_Ver;
	int* Resi_Hor;
	int* Resi_Dc;

}Pred_Buf;

void Decoder(Img_Buf* img, Pred_Buf* pred, FILE* bs, FILE* fp);
void initialization(Img_Buf* img, Pred_Buf* pred);

void Value_reader(Img_Buf* img, FILE* bs);
void Stream_check(Img_Buf* img, FILE* bs);
void Byte_reader(Img_Buf* img, FILE* bs);
void Bit_form(Img_Buf* img, FILE* bs, unsigned char temp);
void bit_reader(Img_Buf* img, FILE* bs, int bit_elements);
void frame_reader(Img_Buf* img, FILE* bs);
void Entropy(Img_Buf* img, Pred_Buf* pred, FILE* bs);

void Inverse_DC_dpcm(Img_Buf* img);
void make_BestIntra(Img_Buf* img);
void Reordering(Img_Buf* img);
void Make_Zigzag_Array(Img_Buf* img);

void deQuantization(Img_Buf* img);

void Make_Cosine_Array(Img_Buf* img);
void IDCT(Img_Buf* img);

void Reconstruct(Img_Buf* img, Pred_Buf* pred, int Row, int Col);
void CbCr_Reconstruct(Img_Buf* img, int Row, int Col);

void Motion_Compensation(Img_Buf* img, Pred_Buf* pred, int row, int col);
void CbCr_Motion_Compensation(Img_Buf* img, Pred_Buf* pred, int row, int col);
void Padding(Img_Buf* img);

void End(Img_Buf* img, Pred_Buf* pred);
void PSNR(Img_Buf* img, FILE* oFile);