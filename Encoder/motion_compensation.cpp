#include "buffer.h"

void Motion_Compensation(Img_Buf* img, Pred_Buf* pred, int row, int col) {
    unsigned char* Ref, * Src;
    unsigned char ipred[BKSIZE];

    Src = img->Pad_img;
    Ref = img->Pad_img + (PADLEN + (row * img->info.Blk_H)) * img->info.Pad_W + PADLEN + col * img->info.Blk_W;

    for (int i = 0; i < img->info.Blk_H; i++) {
        for (int j = 0; j < img->info.Blk_W; j++) {
            ipred[i * img->info.Blk_W + j]
                = Ref[(i + img->MVy[row * (img->info.width / img->info.Blk_W) + col]) * img->info.Pad_W + j + img->MVx[row * (img->info.width / img->info.Blk_W) + col]];
        }
    }

    for (int i = 0; i < img->info.Blk_H; i++)
    {
        for (int j = 0; j < img->info.Blk_W; j++)
        {
            if (img->Resi_blk[i * img->info.Blk_W + j] + ipred[i * img->info.Blk_W + j] > 255)
                img->Recon_blk[i * img->info.Blk_W + j] = 255;
            else if (img->Resi_blk[i * img->info.Blk_W + j] + ipred[i * img->info.Blk_W + j] < 0)
                img->Recon_blk[i * img->info.Blk_W + j] = 0;
            else
                img->Recon_blk[i * img->info.Blk_W + j] = img->Resi_blk[i * img->info.Blk_W + j] + ipred[i * img->info.Blk_W + j];
        }
    }
}

void CbCr_Motion_Compensation(Img_Buf* img, Pred_Buf* pred, int row, int col) {
    unsigned char Cb_pred[CbCr_BKSIZE];
    unsigned char Cr_pred[CbCr_BKSIZE];

    unsigned char* Ref, * Src;
    unsigned char* Cb_Ref, * Cb_Src;
    unsigned char* Cr_Ref, * Cr_Src;

    Cb_Src = img->Cb_Pad_img;
    Cb_Ref = img->Cb_Pad_img + (CbCr_PADLEN + (row * img->info.CbCr_Blk_H)) * img->info.CbCr_Pad_W + CbCr_PADLEN + col * img->info.CbCr_Blk_W;
    Cr_Src = img->Cr_Pad_img;
    Cr_Ref = img->Cr_Pad_img + (CbCr_PADLEN + (row * img->info.CbCr_Blk_H)) * img->info.CbCr_Pad_W + CbCr_PADLEN + col * img->info.CbCr_Blk_W;

    for (int i = 0; i < img->info.CbCr_Blk_H; i++) {
        for (int j = 0; j < img->info.CbCr_Blk_W; j++) {
            Cb_pred[i * img->info.CbCr_Blk_W + j]
                = Cb_Ref[(i + (int)(img->MVy[row * (img->info.width / img->info.Blk_W) + col] / 2 )) * img->info.CbCr_Pad_W + j + (int)(img->MVx[row * (img->info.width / img->info.Blk_W) + col] / 2 )];
            Cr_pred[i * img->info.CbCr_Blk_W + j]
                = Cr_Ref[(i + (int)(img->MVy[row * (img->info.width / img->info.Blk_W) + col] / 2 )) * img->info.CbCr_Pad_W + j + (int)(img->MVx[row * (img->info.width / img->info.Blk_W) + col] / 2 )];
        }
    }

    for (int i = 0; i < img->info.CbCr_Blk_H; i++)
    {
        for (int j = 0; j < img->info.CbCr_Blk_W; j++)
        {
            if (img->Cb_Resi_blk[i * img->info.CbCr_Blk_W + j] + Cb_pred[i * img->info.CbCr_Blk_W + j] > 255)
                img->Cb_Recon_blk[i * img->info.CbCr_Blk_W + j] = 255;
            else if (img->Cb_Resi_blk[i * img->info.CbCr_Blk_W + j] + Cb_pred[i * img->info.CbCr_Blk_W + j] < 0)
                img->Cb_Recon_blk[i * img->info.CbCr_Blk_W + j] = 0;
            else
                img->Cb_Recon_blk[i * img->info.CbCr_Blk_W + j] = img->Cb_Resi_blk[i * img->info.CbCr_Blk_W + j] + Cb_pred[i * img->info.CbCr_Blk_W + j];

            if (img->Cr_Resi_blk[i * img->info.CbCr_Blk_W + j] + Cr_pred[i * img->info.CbCr_Blk_W + j] > 255)
                img->Cr_Recon_blk[i * img->info.CbCr_Blk_W + j] = 255;
            else if (img->Cr_Resi_blk[i * img->info.CbCr_Blk_W + j] + Cr_pred[i * img->info.CbCr_Blk_W + j] < 0)
                img->Cr_Recon_blk[i * img->info.CbCr_Blk_W + j] = 0;
            else
                img->Cr_Recon_blk[i * img->info.CbCr_Blk_W + j] = img->Cr_Resi_blk[i * img->info.CbCr_Blk_W + j] + Cr_pred[i * img->info.CbCr_Blk_W + j];
        }
    }
}
