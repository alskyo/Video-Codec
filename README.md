# Video-Codec
Video compression codec based on Mpeg-2. Keeping the ICSP format.
This code was developed in Feb 2020.

We implemented encoder and decoder for video compression and created applications that can be used using MFC.


The parameters you need to know before using are as follows.

Config.
Test sequence		: akiyo (300f), football (90f), and foreman (300f) (only CIF format).
DC QP		: 1, 8, and 16.
AC QP			: 1 and 16.
DC DPCM mode	: 0 and 6.
Intra period		: 0 to 10.
Intra prediction enable	: 0 and 1.

The sequence can be used by any image, but there may be some restrictions.
DPCM is used by 0 and 6 is not used.
Intra Prediction flag is not used by 0.


Please use only the mfc file when using it.
