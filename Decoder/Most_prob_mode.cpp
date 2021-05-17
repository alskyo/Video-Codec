#include "buffer.h"

void make_BestIntra(Img_Buf* img) {

	img->Best_IntraMode[0] = img->MPM_resi[0]; 
	for (int i = 1; i < (img->info.width / img->info.Blk_W) * (img->info.height / img->info.Blk_H); i++) {
		if (img->MPM_flag[i] == 0) {
			img->Best_IntraMode[i] = img->Best_IntraMode[i - 1];
		}
		else {
			switch (img->Best_IntraMode[i - 1]) {
			case 0:
				img->Best_IntraMode[i] = img->MPM_resi[i] == 0 ? 1 : 2;
				break;
			case 1:
				img->Best_IntraMode[i] = img->MPM_resi[i] == 0 ? 0 : 2;
				break;
			case 2:
				img->Best_IntraMode[i] = img->MPM_resi[i];
				break;
			}
		}
	}
}