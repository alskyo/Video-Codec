#include "buffer.h"

void Reordering(Img_Buf* img) {
    
    for (int i = 0; i < img->info.Blk_H; i++) {
        for (int j = 0; j < img->info.Blk_W; j++) {
            img->Quant_blk[img->Zigzag_arr[i * img->info.Blk_W + j]] 
                = img->Reorder_blk[i * img->info.Blk_W + j];
        }
    }
    // chroma
    for (int i = 0; i < img->info.CbCr_Blk_H; i++) {
        for (int j = 0; j < img->info.CbCr_Blk_W; j++) {
            img->Cb_Quant_blk[img->CbCr_Zigzag_arr[i * img->info.CbCr_Blk_W + j]]
                = img->Cb_Reorder_blk[i * img->info.CbCr_Blk_W + j];
            img->Cr_Quant_blk[img->CbCr_Zigzag_arr[i * img->info.CbCr_Blk_W + j]]
                = img->Cr_Reorder_blk[i * img->info.CbCr_Blk_W + j];
        }
    }
}

void Make_Zigzag_Array(Img_Buf* img) {
    int x = 0, y = 0;
    int temp = 0;
    int navi = 1;       //  Zero is left lower and One is right upper.

    while (true) {
        img->Zigzag_arr[y * img->info.Blk_W + x] = temp++;
        if ((x == (img->info.Blk_W - 1)) && (y == (img->info.Blk_H - 1))) break;
        if (navi == 1) {
            x++;
            y--;
            if (x >= img->info.Blk_W) {
                x = img->info.Blk_W - 1;
                y += 2;
                navi = 0;
            }
            else if (y < 0) {
                y = 0;
                navi = 0;
            }
        }
        else {
            x--;
            y++;
            if (y >= img->info.Blk_H) {
                x += 2;
                y = img->info.Blk_H - 1;
                navi = 1;
            }
            else if (x < 0) {
                x = 0;
                navi = 1;
            }
        }
    }
    // chroma
    x = 0, y = 0, temp = 0, navi = 0;
    while (true) {
        img->CbCr_Zigzag_arr[y * img->info.CbCr_Blk_W + x] = temp++;
        if ((x == (img->info.CbCr_Blk_W - 1)) && (y == (img->info.CbCr_Blk_H - 1))) break;
        if (navi == 1) {
            x++;
            y--;
            if (x >= img->info.CbCr_Blk_W) {
                x = img->info.CbCr_Blk_W - 1;
                y += 2;
                navi = 0;
            }
            else if (y < 0) {
                y = 0;
                navi = 0;
            }
        }
        else {
            x--;
            y++;
            if (y >= img->info.CbCr_Blk_H) {
                x += 2;
                y = img->info.CbCr_Blk_H - 1;
                navi = 1;
            }
            else if (x < 0) {
                x = 0;
                navi = 1;
            }
        }
    }
}