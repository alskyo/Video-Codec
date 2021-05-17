#include "use_Array.h"
void Make_Spiral_Search_Array(Img_Buf* img) {
    int x, y, dx, dy;
    x = y = dx = 0;
    dy = -1;
    int t = SEARCH_RANGE;
    int range = SEARCH_RANGE * SEARCH_RANGE;
    for (int i = 0; i < range; i++) {
        if ((SEARCH_RANGE / 2 >= x) && (x >= -SEARCH_RANGE / 2) && (-SEARCH_RANGE / 2 <= y) && (y <= SEARCH_RANGE / 2)) {
            xspiral_Search_Array[i] = x;
            yspiral_Search_Array[i] = y;
        }
        if ((x == y) || ((x < 0) && (x == -y)) || ((x > 0) && (x == 1 - y))) {
            t = dx;
            dx = -dy;
            dy = t;
        }
        x += dx;
        y += dy;
    }
}

void Motion_Estimation(Img_Buf* img, Pred_Buf* pred, int row, int col) {
    unsigned char* Ref, * Src;

    int Sad_Buf = 0;
    int Sad_Min = 0;
    int Sad_Sum = 0;

    Src = img->Pad_img;
    Ref = img->Pad_img + (PADLEN + (row * img->info.Blk_H)) * img->info.Pad_W + PADLEN + col * img->info.Blk_W;


    for (int sp = 0; sp < SEARCH_RANGE * SEARCH_RANGE; sp++) {

        // Sum of Absolute Difference
        for (int i = 0; i < img->info.Blk_H; i++) {
            for (int j = 0; j < img->info.Blk_W; j++) {
                pred->Resi_Val[i * img->info.Blk_W + j]
                    = (img->Cur_blk[i * img->info.Blk_W + j] - Ref[(i + yspiral_Search_Array[sp]) * img->info.Pad_W + j + xspiral_Search_Array[sp]]);
            }
        }
        Sad_Sum = 0;
        for (int i = 0; i < BKSIZE; i++) {
            Sad_Sum += abs(pred->Resi_Val[i]);
        }
        if (sp == 0) Sad_Min = Sad_Sum,
            memcpy(img->Resi_blk, pred->Resi_Val, img->info.Blk_H * img->info.Blk_W * sizeof(int));
        if (Sad_Min > Sad_Sum) {
            Sad_Min = Sad_Sum;
            Sad_Buf = sp;
            memcpy(img->Resi_blk, pred->Resi_Val, img->info.Blk_H * img->info.Blk_W * sizeof(int));
        }
    }
    img->MVx[row * (img->info.width / img->info.Blk_W) + col] = xspiral_Search_Array[Sad_Buf];
    img->MVy[row * (img->info.width / img->info.Blk_W) + col] = yspiral_Search_Array[Sad_Buf];
}

void CbCr_Motion_Estimation(Img_Buf* img, int row, int col) {
    unsigned char* Cb_Ref, * Cb_Src;
    unsigned char* Cr_Ref, * Cr_Src;

    Cb_Src = img->Cb_Pad_img;
    Cb_Ref = img->Cb_Pad_img + (CbCr_PADLEN + (row * img->info.CbCr_Blk_H)) * img->info.CbCr_Pad_W + CbCr_PADLEN + col * img->info.CbCr_Blk_W;
    Cr_Src = img->Cr_Pad_img;
    Cr_Ref = img->Cr_Pad_img + (CbCr_PADLEN + (row * img->info.CbCr_Blk_H)) * img->info.CbCr_Pad_W + CbCr_PADLEN + col * img->info.CbCr_Blk_W;

    for (int i = 0; i < img->info.CbCr_Blk_H; i++) {
        for (int j = 0; j < img->info.CbCr_Blk_W; j++) {
            img->Cb_Resi_blk[i * img->info.CbCr_Blk_W + j] = img->Cb_Cur_blk[i * img->info.CbCr_Blk_W + j]
                - Cb_Ref[(i + (int)(img->MVy[row * (img->info.width / img->info.Blk_W) + col] / 2)) * img->info.CbCr_Pad_W + j + (int)(img->MVx[row * (img->info.width / img->info.Blk_W) + col] / 2)];
            img->Cr_Resi_blk[i * img->info.CbCr_Blk_W + j] = img->Cr_Cur_blk[i * img->info.CbCr_Blk_W + j]
                - Cr_Ref[(i + (int)(img->MVy[row * (img->info.width / img->info.Blk_W) + col] / 2)) * img->info.CbCr_Pad_W + j + (int)(img->MVx[row * (img->info.width / img->info.Blk_W) + col] / 2)];
        }
    }
}
