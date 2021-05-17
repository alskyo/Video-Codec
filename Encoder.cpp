#include "buffer.h"

void Encoder(Img_Buf* img, Pred_Buf* pred, FILE* fp, FILE* pFile, FILE* bs, int frame_mode, int header_Flag) {

    fread(img->Org_img, sizeof(unsigned char), (img->info.width * img->info.height), pFile);               // for Luma
    fread(img->Cb_img, sizeof(unsigned char), (img->info.CbCr_width * img->info.CbCr_height), pFile);         // for Chroma Blue
    fread(img->Cr_img, sizeof(unsigned char), (img->info.CbCr_width * img->info.CbCr_height), pFile);         // for Chroma Red
    //printf("Mode : %d \n", frame_mode);

    for (int Blk_Row = 0; Blk_Row < img->info.height / img->info.Blk_H; Blk_Row++) {
        for (int Blk_Col = 0; Blk_Col < img->info.width / img->info.Blk_W; Blk_Col++) {
            for (int i = 0; i < img->info.Blk_H; i++) {
                for (int j = 0; j < img->info.Blk_W; j++) {
                    img->Cur_blk[i * img->info.Blk_W + j]
                        = img->Org_img[(i + Blk_Row * img->info.Blk_H) * (img->info.width) + (j + Blk_Col * img->info.Blk_W)];
                }
            }
            // Chroma
            for (int i = 0; i < img->info.CbCr_Blk_H; i++) {
                for (int j = 0; j < img->info.CbCr_Blk_W; j++) {
                    img->Cb_Cur_blk[i * img->info.CbCr_Blk_W + j]
                        = img->Cb_img[(i + Blk_Row * img->info.CbCr_Blk_H) * (img->info.CbCr_width) + (j + Blk_Col * img->info.CbCr_Blk_W)];
                    img->Cr_Cur_blk[i * img->info.CbCr_Blk_W + j]
                        = img->Cr_img[(i + Blk_Row * img->info.CbCr_Blk_H) * (img->info.CbCr_width) + (j + Blk_Col * img->info.CbCr_Blk_W)];
                }
            }
            // frame_mode Zero is Intra prediction and One is Inter Prediction
            if (frame_mode == 0) {
                if (img->info.intra_flag == 1) {
                    Prediction(img, pred, Blk_Row, Blk_Col);            //Resiblk
                    CbCr_Prediction(img, Blk_Row, Blk_Col);            //Cb, Cr Resiblk
                }
                else {
                    for (int i = 0; i < img->info.Blk_H; i++)
                        for (int j = 0; j < img->info.Blk_W; j++)
                            img->Resi_blk[i * img->info.Blk_W + j] = img->Cur_blk[i * img->info.Blk_W + j];
                    for (int i = 0; i < img->info.CbCr_Blk_H; i++) {
                        for (int j = 0; j < img->info.CbCr_Blk_W; j++) {
                            img->Cb_Resi_blk[i * img->info.CbCr_Blk_W + j]
                                = img->Cb_Cur_blk[i * img->info.CbCr_Blk_W + j];
                            img->Cr_Resi_blk[i * img->info.CbCr_Blk_W + j]
                                = img->Cr_Cur_blk[i * img->info.CbCr_Blk_W + j];
                        }
                    }
                }
            }
            else {
                if (Blk_Row == 0 && Blk_Col == 0) Padding(img);         //Padimg, include CbCr
                Motion_Estimation(img, pred, Blk_Row, Blk_Col);         //Resiblk
                CbCr_Motion_Estimation(img, Blk_Row, Blk_Col);         //Cb_Resiblk, Cr_Resiblk
            }

            DCT(img);                                       //DCTBLK, include CbCr
            Quantization(img);                                 //quantblk, include CbCr
            Reordering(img, Blk_Row, Blk_Col);                     //reorder, include CbCr

            // Quant img에 저장한 것을 바탕으로 후처리 -> ENTROPY
            for (int i = 0; i < img->info.Blk_H; i++)
                for (int j = 0; j < img->info.Blk_W; j++)
                    img->Quant_img[(i + Blk_Row * img->info.Blk_H) * (img->info.width) + (j + Blk_Col * img->info.Blk_W)]
                    = img->Reorder_blk[i * img->info.Blk_W + j];
            for (int i = 0; i < img->info.CbCr_Blk_H; i++) {
                for (int j = 0; j < img->info.CbCr_Blk_W; j++) {
                    img->Cb_Quant_img[(i + Blk_Row * img->info.CbCr_Blk_H) * (img->info.CbCr_width) + (j + Blk_Col * img->info.CbCr_Blk_W)]
                        = img->Cb_Reorder_blk[i * img->info.CbCr_Blk_W + j];
                    img->Cr_Quant_img[(i + Blk_Row * img->info.CbCr_Blk_H) * (img->info.CbCr_width) + (j + Blk_Col * img->info.CbCr_Blk_W)]
                        = img->Cr_Reorder_blk[i * img->info.CbCr_Blk_W + j];
                }
            }
            deQuantization(img);                              //dctblk, include CbCr
            IDCT(img);                                       //dct or resi, include CbCr
            if (frame_mode == 0) {
                if (img->info.intra_flag == 1) {
                    Reconstruct(img, pred, Blk_Row, Blk_Col);
                    CbCr_Reconstruct(img, Blk_Row, Blk_Col);
                }
                else {
                    for (int i = 0; i < img->info.Blk_H; i++)
                    {
                        for (int j = 0; j < img->info.Blk_W; j++)
                        {
                            if (img->Resi_blk[i * img->info.Blk_W + j] + (unsigned char)0 > 255)
                                img->Recon_blk[i * img->info.Blk_W + j] = 255;
                            else if (img->Resi_blk[i * img->info.Blk_W + j] + (unsigned char)0 < 0)
                                img->Recon_blk[i * img->info.Blk_W + j] = 0;
                            else
                                img->Recon_blk[i * img->info.Blk_W + j] = img->Resi_blk[i * img->info.Blk_W + j] + (unsigned char)0;
                        }
                    }
                    for (int i = 0; i < img->info.CbCr_Blk_H; i++) {
                        for (int j = 0; j < img->info.CbCr_Blk_W; j++) {
                            if (img->Cb_Resi_blk[i * img->info.CbCr_Blk_W + j] + (unsigned char)0 > 255)
                                img->Cb_Recon_blk[i * img->info.CbCr_Blk_W + j] = 255;
                            else if (img->Cb_Resi_blk[i * img->info.CbCr_Blk_W + j] + (unsigned char)0 < 0)
                                img->Cb_Recon_blk[i * img->info.CbCr_Blk_W + j] = 0;
                            else
                                img->Cb_Recon_blk[i * img->info.CbCr_Blk_W + j] = img->Cb_Resi_blk[i * img->info.CbCr_Blk_W + j] + (unsigned char)0;

                            if (img->Cr_Resi_blk[i * img->info.CbCr_Blk_W + j] + (unsigned char)0 > 255)
                                img->Cr_Recon_blk[i * img->info.CbCr_Blk_W + j] = 255;
                            else if (img->Cr_Resi_blk[i * img->info.CbCr_Blk_W + j] + (unsigned char)0 < 0)
                                img->Cr_Recon_blk[i * img->info.CbCr_Blk_W + j] = 0;
                            else
                                img->Cr_Recon_blk[i * img->info.CbCr_Blk_W + j] = img->Cr_Resi_blk[i * img->info.CbCr_Blk_W + j] + (unsigned char)0;
                        }
                    }
                }
            }
            else {
                Motion_Compensation(img, pred, Blk_Row, Blk_Col);
                CbCr_Motion_Compensation(img, pred, Blk_Row, Blk_Col);
            }
            for (int i = 0; i < img->info.Blk_H; i++)
                for (int j = 0; j < img->info.Blk_W; j++)
                    img->Recon_img[(i + Blk_Row * img->info.Blk_H) * (img->info.width) + (j + Blk_Col * img->info.Blk_W)]
                    = img->Recon_blk[i * img->info.Blk_W + j];

            for (int i = 0; i < img->info.CbCr_Blk_H; i++) {
                for (int j = 0; j < img->info.CbCr_Blk_W; j++) {
                    img->Cb_Recon_img[(i + Blk_Row * img->info.CbCr_Blk_H) * (img->info.CbCr_width) + (j + Blk_Col * img->info.CbCr_Blk_W)]
                        = img->Cb_Recon_blk[i * img->info.CbCr_Blk_W + j];
                    img->Cr_Recon_img[(i + Blk_Row * img->info.CbCr_Blk_H) * (img->info.CbCr_width) + (j + Blk_Col * img->info.CbCr_Blk_W)]
                        = img->Cr_Recon_blk[i * img->info.CbCr_Blk_W + j];
                }
            }
        }
    }
    // D Q 이후의 처리 위치 ---------------------------------------------------------
    if (frame_mode == 0)
        make_MPM_flag(img);
    if (img->info.dpcm_flag == 0) {
        DC_dpcm(img);      // Quantimg   
    }

    Entropy(img, bs, header_Flag, frame_mode);
    // ------------------------------------------------------------------------------
    memcpy(img->ReconForInter_img, img->Recon_img, img->info.height * img->info.width * sizeof(unsigned char));
    memcpy(img->Cb_ReconForInter_img, img->Cb_Recon_img, img->info.CbCr_height * img->info.CbCr_width * sizeof(unsigned char));
    memcpy(img->Cr_ReconForInter_img, img->Cr_Recon_img, img->info.CbCr_height * img->info.CbCr_width * sizeof(unsigned char));

    //fwrite(img->Recon_img, sizeof(unsigned char), img->info.width * img->info.height, fp);
    //fwrite(img->Cb_Recon_img, sizeof(unsigned char), img->info.CbCr_height * img->info.CbCr_width, fp);
    //fwrite(img->Cr_Recon_img, sizeof(unsigned char), img->info.CbCr_height * img->info.CbCr_width, fp);

}