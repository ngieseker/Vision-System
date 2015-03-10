% Intrinsic and Extrinsic Camera Parameters
%
% This script file can be directly excecuted under Matlab to recover the camera intrinsic and extrinsic parameters.
% IMPORTANT: This file contains neither the structure of the calibration objects nor the image coordinates of the calibration points.
%            All those complementary variables are saved in the complete matlab data file Calib_Results.mat.
% For more information regarding the calibration model visit http://www.vision.caltech.edu/bouguetj/calib_doc/


%-- Focal length:
fc = [ 1691.679367457322542 ; 1692.300740372626933 ];

%-- Principal point:
cc = [ 530.541421533724360 ; 391.603509178551064 ];

%-- Skew coefficient:
alpha_c = 0.000000000000000;

%-- Distortion coefficients:
kc = [ -0.387801554083676 ; 0.040076843739321 ; 0.002922412720361 ; 0.000029096851062 ; 0.000000000000000 ];

%-- Focal length uncertainty:
fc_error = [ 3.364716295896266 ; 3.275562765812249 ];

%-- Principal point uncertainty:
cc_error = [ 4.363647581018905 ; 3.926840192313283 ];

%-- Skew coefficient uncertainty:
alpha_c_error = 0.000000000000000;

%-- Distortion coefficients uncertainty:
kc_error = [ 0.006986459999518 ; 0.051604587109158 ; 0.000456222527578 ; 0.000481938707943 ; 0.000000000000000 ];

%-- Image size:
nx = 1024;
ny = 768;


%-- Various other variables (may be ignored if you do not use the Matlab Calibration Toolbox):
%-- Those variables are used to control which intrinsic parameters should be optimized

n_ima = 19;						% Number of calibration images
est_fc = [ 1 ; 1 ];					% Estimation indicator of the two focal variables
est_aspect_ratio = 1;				% Estimation indicator of the aspect ratio fc(2)/fc(1)
center_optim = 1;					% Estimation indicator of the principal point
est_alpha = 0;						% Estimation indicator of the skew coefficient
est_dist = [ 1 ; 1 ; 1 ; 1 ; 0 ];	% Estimation indicator of the distortion coefficients


%-- Extrinsic parameters:
%-- The rotation (omc_kk) and the translation (Tc_kk) vectors for every calibration image and their uncertainties

%-- Image #1:
omc_1 = [ -2.098741e+00 ; -2.111418e+00 ; 8.816482e-03 ];
Tc_1  = [ 1.598253e+02 ; -1.249156e+03 ; 5.663058e+03 ];
omc_error_1 = [ 4.605726e-03 ; 4.962280e-03 ; 8.832909e-03 ];
Tc_error_1  = [ 1.475138e+01 ; 1.324850e+01 ; 1.247282e+01 ];

%-- Image #2:
omc_2 = [ -2.115681e+00 ; -2.104112e+00 ; -6.588669e-03 ];
Tc_2  = [ 1.313063e+02 ; -9.487255e+02 ; 5.588728e+03 ];
omc_error_2 = [ 3.944854e-03 ; 4.430615e-03 ; 8.139605e-03 ];
Tc_error_2  = [ 1.449211e+01 ; 1.305151e+01 ; 1.235085e+01 ];

%-- Image #3:
omc_3 = [ -2.116020e+00 ; -2.106719e+00 ; 4.779519e-02 ];
Tc_3  = [ 1.304267e+02 ; -6.764405e+02 ; 5.637390e+03 ];
omc_error_3 = [ 3.676043e-03 ; 4.300700e-03 ; 8.111240e-03 ];
Tc_error_3  = [ 1.458124e+01 ; 1.311195e+01 ; 1.238292e+01 ];

%-- Image #4:
omc_4 = [ -2.094214e+00 ; -2.120358e+00 ; 7.166536e-02 ];
Tc_4  = [ 1.759933e+02 ; -1.801571e+02 ; 5.782310e+03 ];
omc_error_4 = [ 2.891493e-03 ; 3.729179e-03 ; 6.936934e-03 ];
Tc_error_4  = [ 1.493770e+01 ; 1.340773e+01 ; 1.247905e+01 ];

%-- Image #5:
omc_5 = [ -2.111387e+00 ; -2.108045e+00 ; 6.992211e-02 ];
Tc_5  = [ 1.890436e+02 ; 8.923106e+01 ; 5.808316e+03 ];
omc_error_5 = [ 2.578635e-03 ; 3.440935e-03 ; 6.517802e-03 ];
Tc_error_5  = [ 1.500652e+01 ; 1.347344e+01 ; 1.240019e+01 ];

%-- Image #6:
omc_6 = [ 2.095679e+00 ; 2.196893e+00 ; 4.726595e-02 ];
Tc_6  = [ -7.467643e+02 ; -5.374057e+02 ; 2.908581e+03 ];
omc_error_6 = [ 1.603740e-03 ; 1.727580e-03 ; 3.516730e-03 ];
Tc_error_6  = [ 7.581308e+00 ; 6.807803e+00 ; 6.282878e+00 ];

%-- Image #7:
omc_7 = [ 1.755566e+00 ; 1.393389e+00 ; 6.098262e-01 ];
Tc_7  = [ -1.039310e+02 ; -6.896115e+02 ; 3.697343e+03 ];
omc_error_7 = [ 2.271492e-03 ; 1.908314e-03 ; 2.791397e-03 ];
Tc_error_7  = [ 9.645346e+00 ; 8.549813e+00 ; 8.490011e+00 ];

%-- Image #8:
omc_8 = [ -1.727444e+00 ; -1.801389e+00 ; 7.302955e-01 ];
Tc_8  = [ -9.616161e+02 ; -2.931716e+02 ; 4.305287e+03 ];
omc_error_8 = [ 2.269259e-03 ; 1.917000e-03 ; 3.174751e-03 ];
Tc_error_8  = [ 1.112063e+01 ; 1.008437e+01 ; 8.167776e+00 ];

%-- Image #9:
omc_9 = [ -1.683293e+00 ; -1.815243e+00 ; 8.117810e-01 ];
Tc_9  = [ -9.058202e+02 ; -4.319029e+02 ; 4.398956e+03 ];
omc_error_9 = [ 2.317034e-03 ; 1.906500e-03 ; 3.135513e-03 ];
Tc_error_9  = [ 1.137873e+01 ; 1.028580e+01 ; 8.232510e+00 ];

%-- Image #10:
omc_10 = [ -1.595175e+00 ; -2.096683e+00 ; 5.337716e-01 ];
Tc_10  = [ -4.126061e+02 ; -6.528723e+02 ; 4.317808e+03 ];
omc_error_10 = [ 1.947380e-03 ; 1.979546e-03 ; 3.405123e-03 ];
Tc_error_10  = [ 1.113356e+01 ; 1.000777e+01 ; 8.335372e+00 ];

%-- Image #11:
omc_11 = [ 1.781246e+00 ; 1.380052e+00 ; 3.123286e-01 ];
Tc_11  = [ -8.776374e+02 ; -3.846622e+02 ; 5.599044e+03 ];
omc_error_11 = [ 2.157131e-03 ; 1.947031e-03 ; 3.066318e-03 ];
Tc_error_11  = [ 1.445052e+01 ; 1.303396e+01 ; 1.264789e+01 ];

%-- Image #12:
omc_12 = [ 2.081082e+00 ; 1.640103e+00 ; -8.940144e-01 ];
Tc_12  = [ -4.564549e+02 ; -3.396497e+01 ; 5.116911e+03 ];
omc_error_12 = [ 1.802542e-03 ; 2.167362e-03 ; 3.664023e-03 ];
Tc_error_12  = [ 1.318682e+01 ; 1.184389e+01 ; 9.739759e+00 ];

%-- Image #13:
omc_13 = [ -1.975426e+00 ; -2.032860e+00 ; 6.449074e-01 ];
Tc_13  = [ -1.226077e+03 ; -1.370751e+02 ; 4.943729e+03 ];
omc_error_13 = [ 2.298173e-03 ; 1.782366e-03 ; 3.758707e-03 ];
Tc_error_13  = [ 1.275211e+01 ; 1.162510e+01 ; 9.862590e+00 ];

%-- Image #14:
omc_14 = [ 1.866367e+00 ; 1.808467e+00 ; 6.828880e-01 ];
Tc_14  = [ -1.007568e+03 ; -4.584853e+02 ; 3.741298e+03 ];
omc_error_14 = [ 2.202377e-03 ; 1.727515e-03 ; 3.451808e-03 ];
Tc_error_14  = [ 9.755415e+00 ; 8.801395e+00 ; 8.847979e+00 ];

%-- Image #15:
omc_15 = [ 2.132505e+00 ; 2.152220e+00 ; 1.884329e-01 ];
Tc_15  = [ -1.171745e+03 ; -7.084065e+02 ; 3.644221e+03 ];
omc_error_15 = [ 2.075195e-03 ; 2.405098e-03 ; 4.791877e-03 ];
Tc_error_15  = [ 9.615639e+00 ; 8.647433e+00 ; 8.247121e+00 ];

%-- Image #16:
omc_16 = [ 2.181529e+00 ; 2.099155e+00 ; 9.574250e-02 ];
Tc_16  = [ -1.135588e+03 ; -7.488520e+02 ; 3.641463e+03 ];
omc_error_16 = [ 1.957204e-03 ; 2.253779e-03 ; 4.639472e-03 ];
Tc_error_16  = [ 9.547751e+00 ; 8.589218e+00 ; 8.243228e+00 ];

%-- Image #17:
omc_17 = [ 2.202437e+00 ; 2.140609e+00 ; 1.297131e-01 ];
Tc_17  = [ -1.097329e+03 ; -5.463282e+02 ; 3.574342e+03 ];
omc_error_17 = [ 2.093007e-03 ; 2.234287e-03 ; 4.692960e-03 ];
Tc_error_17  = [ 9.356494e+00 ; 8.449963e+00 ; 7.977398e+00 ];

%-- Image #18:
omc_18 = [ 2.201282e+00 ; 2.171482e+00 ; 1.382907e-01 ];
Tc_18  = [ -9.905400e+02 ; -3.979911e+02 ; 3.606070e+03 ];
omc_error_18 = [ 2.171896e-03 ; 2.160946e-03 ; 4.576380e-03 ];
Tc_error_18  = [ 9.407843e+00 ; 8.499647e+00 ; 7.946017e+00 ];

%-- Image #19:
omc_19 = [ 2.211881e+00 ; 2.208328e+00 ; 9.295494e-02 ];
Tc_19  = [ -1.127131e+03 ; -2.691507e+02 ; 3.688474e+03 ];
omc_error_19 = [ 2.287601e-03 ; 2.298131e-03 ; 4.682531e-03 ];
Tc_error_19  = [ 9.566465e+00 ; 8.716266e+00 ; 8.279525e+00 ];

