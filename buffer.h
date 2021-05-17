#include "image_info.h"

enum mode
{
	DC,
	VERTICAL,
	HORIZONTAL,
	Max_Mode_num
};


typedef struct _Image_information
{
	int intra_flag;
	int dpcm_flag;
	int dcqp;
	int acqp;
	int period;

	int width;			 // ����/���� ������ ���� ũ��
	int height;			 // ����/���� ������ ���� ũ��
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
	unsigned char* Org_img;     // ���� ���� ���� ����	
	unsigned char* Cb_img;		// Chroma Blue
	unsigned char* Cr_img;		// Chroma Red

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

void initialization(Img_Buf* img, Pred_Buf* pred); //�Է� ���� ���� �Լ�

void Encoder(Img_Buf* img, Pred_Buf* pred, FILE* fp, FILE* pFile, FILE* bs, int frame_mode, int header_Flag);
void Prediction(Img_Buf* img, Pred_Buf* pred, int Row, int Col);
void Reconstruct(Img_Buf* img, Pred_Buf* pred, int Row, int Col);
void CbCr_Prediction(Img_Buf* img, int Row, int Col);		
void CbCr_Reconstruct(Img_Buf* img, int Row, int Col);
void Motion_Estimation(Img_Buf* img, Pred_Buf* pred, int Row, int Col);
void CbCr_Motion_Estimation(Img_Buf* img, int row, int col);
void make_MPM_flag(Img_Buf* img);
void DC_dpcm(Img_Buf* img);
void Motion_Compensation(Img_Buf* img, Pred_Buf* pred, int Row, int Col);
void CbCr_Motion_Compensation(Img_Buf* img, Pred_Buf* pred, int row, int col);
void Reordering(Img_Buf* img, int Blk_Row, int Blk_Col);
void Make_Zigzag_Array(Img_Buf* img);
void Make_Cosine_Array(Img_Buf* img);
void Make_Spiral_Search_Array(Img_Buf* img);
void Padding(Img_Buf* img);
void DCT(Img_Buf* img);
void IDCT(Img_Buf* img);
void Quantization(Img_Buf* img);
void deQuantization(Img_Buf* img);

void Bit_Form(Img_Buf* img, FILE* bs, int value);
void Byte_Writer(Img_Buf* img, FILE* bs, unsigned char uc_temp, int bit_element);
void Entropy(Img_Buf* img, FILE* bs, int header_Flag, int frame_mode);

void Decoder(Img_Buf* img, Pred_Buf* pred);





