#include "buffer.h"

void make_MPM_flag(Img_Buf* img) {

	img->MPM_flag[0] = 0;		// (0,0) Block Exception Processing
	img->MPM_resi[0] = img->Best_IntraMode[0];

	for (int i = 1; i < (img->info.width / img->info.Blk_W) * (img->info.height / img->info.Blk_H); i++) {
		if (img->Best_IntraMode[i - 1] == img->Best_IntraMode[i]) {
			img->MPM_flag[i] = 0;
			img->MPM_resi[i] = 0;		// Just trash value
		}
		else {
			img->MPM_flag[i] = 1;
			switch (img->Best_IntraMode[i - 1]) {
			case 0 :
				img->MPM_resi[i] = img->Best_IntraMode[i] == 1 ? 0 : 1;
				break;
			case 1 :
				img->MPM_resi[i] = img->Best_IntraMode[i] == 0 ? 0 : 1;
				break;
			case 2 : 
				img->MPM_resi[i] = img->Best_IntraMode[i];
				break;
			}
		}
	}
}
