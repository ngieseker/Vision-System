% Intrinsic and Extrinsic Camera Parameters
%
% This script file can be directly excecuted under Matlab to recover the camera intrinsic and extrinsic parameters.
% IMPORTANT: This file contains neither the structure of the calibration objects nor the image coordinates of the calibration points.
%            All those complementary variables are saved in the complete matlab data file Calib_Results.mat.
% For more information regarding the calibration model visit http://www.vision.caltech.edu/bouguetj/calib_doc/


%-- Focal length:
fc = [ 889.352351478452533 ; 888.867343424777800 ];

%-- Principal point:
cc = [ 511.500000000000000 ; 383.500000000000000 ];

%-- Skew coefficient:
alpha_c = 0.000000000000000;

%-- Distortion coefficients:
kc = [ -0.224274564616323 ; 0.159055886843828 ; -0.000427825703600 ; 0.000344620725921 ; 0.000000000000000 ];

%-- Focal length uncertainty:
fc_error = [ 1.967781586926205 ; 1.959676907896648 ];

%-- Principal point uncertainty:
cc_error = [ 0.000000000000000 ; 0.000000000000000 ];

%-- Skew coefficient uncertainty:
alpha_c_error = 0.000000000000000;

%-- Distortion coefficients uncertainty:
kc_error = [ 0.001601878517756 ; 0.002980661590524 ; 0.000255345976611 ; 0.000205520999638 ; 0.000000000000000 ];

%-- Image size:
nx = 1024;
ny = 768;


%-- Various other variables (may be ignored if you do not use the Matlab Calibration Toolbox):
%-- Those variables are used to control which intrinsic parameters should be optimized

n_ima = 25;						% Number of calibration images
est_fc = [ 1 ; 1 ];					% Estimation indicator of the two focal variables
est_aspect_ratio = 1;				% Estimation indicator of the aspect ratio fc(2)/fc(1)
center_optim = 0;					% Estimation indicator of the principal point
est_alpha = 0;						% Estimation indicator of the skew coefficient
est_dist = [ 1 ; 1 ; 1 ; 1 ; 0 ];	% Estimation indicator of the distortion coefficients


%-- Extrinsic parameters:
%-- The rotation (omc_kk) and the translation (Tc_kk) vectors for every calibration image and their uncertainties

%-- Image #1:
omc_1 = [ 1.925622e-03 ; 3.066558e+00 ; 1.278465e-01 ];
Tc_1  = [ -1.880730e+02 ; -2.536704e+02 ; 6.026735e+02 ];
omc_error_1 = [ 9.751203e-04 ; 2.539072e-03 ; 3.445365e-03 ];
Tc_error_1  = [ 1.582564e-01 ; 8.238077e-02 ; 1.333171e+00 ];

%-- Image #2:
omc_2 = [ -7.371259e-03 ; 2.991873e+00 ; 2.516321e-01 ];
Tc_2  = [ -1.224890e+02 ; -2.404694e+02 ; 5.765824e+02 ];
omc_error_2 = [ 9.948466e-04 ; 3.190799e-03 ; 4.299546e-03 ];
Tc_error_2  = [ 1.481185e-01 ; 1.161602e-01 ; 1.248099e+00 ];

%-- Image #3:
omc_3 = [ 6.185505e-02 ; 2.927023e+00 ; 2.067237e-01 ];
Tc_3  = [ -3.601432e+01 ; -2.539192e+02 ; 6.110702e+02 ];
omc_error_3 = [ 7.692741e-04 ; 3.944712e-03 ; 4.529685e-03 ];
Tc_error_3  = [ 9.799001e-02 ; 8.074679e-02 ; 1.363899e+00 ];

%-- Image #4:
omc_4 = [ -1.099717e-02 ; 2.971526e+00 ; 2.006030e-01 ];
Tc_4  = [ 7.649875e+01 ; -2.574338e+02 ; 6.373011e+02 ];
omc_error_4 = [ 6.995484e-04 ; 3.248207e-03 ; 4.060009e-03 ];
Tc_error_4  = [ 6.576869e-02 ; 6.755455e-02 ; 1.405538e+00 ];

%-- Image #5:
omc_5 = [ 1.289559e-01 ; 2.829289e+00 ; 2.210508e-01 ];
Tc_5  = [ 1.268300e+02 ; -2.780373e+02 ; 6.632683e+02 ];
omc_error_5 = [ 5.901173e-04 ; 1.971351e-03 ; 2.499721e-03 ];
Tc_error_5  = [ 8.870428e-02 ; 7.093299e-02 ; 1.435307e+00 ];

%-- Image #6:
omc_6 = [ 8.395488e-02 ; 2.837869e+00 ; 1.329165e-01 ];
Tc_6  = [ 3.738579e+02 ; -2.682233e+02 ; 6.470634e+02 ];
omc_error_6 = [ 7.207452e-04 ; 1.479694e-03 ; 1.747043e-03 ];
Tc_error_6  = [ 2.002546e-01 ; 8.435810e-02 ; 1.506441e+00 ];

%-- Image #7:
omc_7 = [ 3.048653e-02 ; 2.866108e+00 ; 1.158911e-01 ];
Tc_7  = [ 3.948041e+02 ; -1.659945e+02 ; 6.676662e+02 ];
omc_error_7 = [ 7.363555e-04 ; 1.519424e-03 ; 1.690423e-03 ];
Tc_error_7  = [ 1.931909e-01 ; 7.350685e-02 ; 1.581069e+00 ];

%-- Image #8:
omc_8 = [ -1.117762e-02 ; 2.799469e+00 ; -2.990446e-02 ];
Tc_8  = [ 4.077273e+02 ; 1.479934e+00 ; 7.001499e+02 ];
omc_error_8 = [ 7.460318e-04 ; 1.365193e-03 ; 1.716404e-03 ];
Tc_error_8  = [ 1.937487e-01 ; 1.012645e-01 ; 1.705359e+00 ];

%-- Image #9:
omc_9 = [ -1.195981e-02 ; 3.092079e+00 ; -7.444930e-02 ];
Tc_9  = [ 3.777336e+02 ; 4.642501e+01 ; 6.461919e+02 ];
omc_error_9 = [ 8.919304e-04 ; 1.843130e-03 ; 2.864651e-03 ];
Tc_error_9  = [ 1.661915e-01 ; 1.090340e-01 ; 1.609092e+00 ];

%-- Image #10:
omc_10 = [ -9.082558e-02 ; 3.044394e+00 ; -7.628217e-02 ];
Tc_10  = [ 2.674960e+02 ; 4.479034e+01 ; 6.378389e+02 ];
omc_error_10 = [ 7.339030e-04 ; 1.822258e-03 ; 2.918532e-03 ];
Tc_error_10  = [ 1.045064e-01 ; 9.113716e-02 ; 1.512828e+00 ];

%-- Image #11:
omc_11 = [ -4.068198e-02 ; 3.077281e+00 ; -7.096629e-02 ];
Tc_11  = [ 4.499561e+01 ; 2.284867e+01 ; 6.014546e+02 ];
omc_error_11 = [ 5.833427e-04 ; 2.800876e-03 ; 4.085965e-03 ];
Tc_error_11  = [ 5.198397e-02 ; 6.730111e-02 ; 1.387202e+00 ];

%-- Image #12:
omc_12 = [ -3.820627e-02 ; -3.103764e+00 ; 9.767784e-02 ];
Tc_12  = [ -1.521719e+02 ; 9.916179e-01 ; 5.445224e+02 ];
omc_error_12 = [ 7.735302e-04 ; 1.845449e-03 ; 2.673462e-03 ];
Tc_error_12  = [ 1.120042e-01 ; 5.912911e-02 ; 1.255502e+00 ];

%-- Image #13:
omc_13 = [ 4.119281e-01 ; -2.892826e+00 ; 4.450651e-01 ];
Tc_13  = [ -1.270898e+02 ; -2.267545e+02 ; 6.541557e+02 ];
omc_error_13 = [ 7.117138e-04 ; 1.279410e-03 ; 1.629659e-03 ];
Tc_error_13  = [ 1.114911e-01 ; 8.898890e-02 ; 1.447042e+00 ];

%-- Image #14:
omc_14 = [ 2.474220e-01 ; -3.052596e+00 ; 6.540991e-01 ];
Tc_14  = [ 7.577267e+01 ; -2.359595e+02 ; 7.200352e+02 ];
omc_error_14 = [ 5.765892e-04 ; 1.244579e-03 ; 1.849165e-03 ];
Tc_error_14  = [ 5.534229e-02 ; 1.322941e-01 ; 1.519301e+00 ];

%-- Image #15:
omc_15 = [ -7.037491e-02 ; 2.626421e+00 ; -6.141903e-01 ];
Tc_15  = [ 1.948245e+02 ; -2.362274e+02 ; 7.881274e+02 ];
omc_error_15 = [ 5.788180e-04 ; 1.003785e-03 ; 1.222961e-03 ];
Tc_error_15  = [ 1.170072e-01 ; 1.411128e-01 ; 1.629635e+00 ];

%-- Image #16:
omc_16 = [ -1.507006e-01 ; 2.825002e+00 ; -5.827535e-01 ];
Tc_16  = [ 4.000064e+02 ; -2.871370e+02 ; 8.431859e+02 ];
omc_error_16 = [ 7.821259e-04 ; 1.272001e-03 ; 1.474633e-03 ];
Tc_error_16  = [ 2.187438e-01 ; 2.029374e-01 ; 1.943579e+00 ];

%-- Image #17:
omc_17 = [ 2.687835e-01 ; -2.424034e+00 ; 1.542713e-01 ];
Tc_17  = [ 3.358587e+02 ; -2.375434e+02 ; 6.178120e+02 ];
omc_error_17 = [ 5.476799e-04 ; 1.667161e-03 ; 2.172799e-03 ];
Tc_error_17  = [ 2.074915e-01 ; 8.978199e-02 ; 1.488626e+00 ];

%-- Image #18:
omc_18 = [ 3.795218e-01 ; -2.619865e+00 ; 6.547263e-01 ];
Tc_18  = [ 3.366183e+02 ; -9.557940e+00 ; 6.052774e+02 ];
omc_error_18 = [ 7.259990e-04 ; 1.763483e-03 ; 2.239729e-03 ];
Tc_error_18  = [ 1.086383e-01 ; 9.171629e-02 ; 1.446914e+00 ];

%-- Image #19:
omc_19 = [ 3.658583e-01 ; -2.534207e+00 ; 7.898648e-01 ];
Tc_19  = [ 1.400430e+02 ; 1.011433e+01 ; 5.816696e+02 ];
omc_error_19 = [ 5.327676e-04 ; 1.041482e-03 ; 1.401112e-03 ];
Tc_error_19  = [ 4.914820e-02 ; 7.180150e-02 ; 1.296004e+00 ];

%-- Image #20:
omc_20 = [ 2.556133e-02 ; -2.834817e+00 ; 9.558764e-01 ];
Tc_20  = [ -4.825142e+01 ; -1.634474e+01 ; 6.104304e+02 ];
omc_error_20 = [ 6.219554e-04 ; 1.071780e-03 ; 1.486788e-03 ];
Tc_error_20  = [ 8.008430e-02 ; 6.595526e-02 ; 1.237718e+00 ];

%-- Image #21:
omc_21 = [ 1.616993e-01 ; -2.595580e+00 ; 3.570373e-01 ];
Tc_21  = [ -8.966288e+01 ; -1.181292e+02 ; 5.356485e+02 ];
omc_error_21 = [ 5.197056e-04 ; 9.279979e-04 ; 1.064247e-03 ];
Tc_error_21  = [ 7.670822e-02 ; 4.404027e-02 ; 1.217320e+00 ];

%-- Image #22:
omc_22 = [ -5.159195e-01 ; 2.800057e+00 ; -4.201671e-01 ];
Tc_22  = [ 3.026828e+01 ; -8.701564e+01 ; 6.918109e+02 ];
omc_error_22 = [ 5.520118e-04 ; 2.000161e-03 ; 2.634339e-03 ];
Tc_error_22  = [ 7.450004e-02 ; 7.355238e-02 ; 1.403240e+00 ];

%-- Image #23:
omc_23 = [ NaN ; NaN ; NaN ];
Tc_23  = [ NaN ; NaN ; NaN ];
omc_error_23 = [ NaN ; NaN ; NaN ];
Tc_error_23  = [ NaN ; NaN ; NaN ];

%-- Image #24:
omc_24 = [ 2.842990e-01 ; -2.827441e+00 ; 4.041968e-01 ];
Tc_24  = [ 1.676794e+02 ; -1.096115e+02 ; 3.324307e+02 ];
omc_error_24 = [ 3.059659e-04 ; 9.576301e-04 ; 1.311839e-03 ];
Tc_error_24  = [ 3.671789e-02 ; 3.830148e-02 ; 7.565667e-01 ];

%-- Image #25:
omc_25 = [ 4.897238e-02 ; -2.946919e+00 ; 1.233556e-01 ];
Tc_25  = [ -6.009029e+00 ; -1.137520e+02 ; 3.118424e+02 ];
omc_error_25 = [ 3.327532e-04 ; 1.053506e-03 ; 1.318573e-03 ];
Tc_error_25  = [ 3.101990e-02 ; 2.584711e-02 ; 6.802283e-01 ];

