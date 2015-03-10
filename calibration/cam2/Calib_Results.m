% Intrinsic and Extrinsic Camera Parameters
%
% This script file can be directly excecuted under Matlab to recover the camera intrinsic and extrinsic parameters.
% IMPORTANT: This file contains neither the structure of the calibration objects nor the image coordinates of the calibration points.
%            All those complementary variables are saved in the complete matlab data file Calib_Results.mat.
% For more information regarding the calibration model visit http://www.vision.caltech.edu/bouguetj/calib_doc/


%-- Focal length:
fc = [ 1694.424331711923060 ; 1697.737796256193860 ];

%-- Principal point:
cc = [ 496.578906519597524 ; 342.685791380842204 ];

%-- Skew coefficient:
alpha_c = 0.000000000000000;

%-- Distortion coefficients:
kc = [ -0.402006805904044 ; 0.188141309310434 ; -0.002185847109901 ; 0.000446855929261 ; 0.000000000000000 ];

%-- Focal length uncertainty:
fc_error = [ 2.639287267359086 ; 2.577296538462834 ];

%-- Principal point uncertainty:
cc_error = [ 3.813488857520954 ; 3.180905075755346 ];

%-- Skew coefficient uncertainty:
alpha_c_error = 0.000000000000000;

%-- Distortion coefficients uncertainty:
kc_error = [ 0.005970922771177 ; 0.046351439377919 ; 0.000314051373300 ; 0.000416127576785 ; 0.000000000000000 ];

%-- Image size:
nx = 1024;
ny = 768;


%-- Various other variables (may be ignored if you do not use the Matlab Calibration Toolbox):
%-- Those variables are used to control which intrinsic parameters should be optimized

n_ima = 21;						% Number of calibration images
est_fc = [ 1 ; 1 ];					% Estimation indicator of the two focal variables
est_aspect_ratio = 1;				% Estimation indicator of the aspect ratio fc(2)/fc(1)
center_optim = 1;					% Estimation indicator of the principal point
est_alpha = 0;						% Estimation indicator of the skew coefficient
est_dist = [ 1 ; 1 ; 1 ; 1 ; 0 ];	% Estimation indicator of the distortion coefficients


%-- Extrinsic parameters:
%-- The rotation (omc_kk) and the translation (Tc_kk) vectors for every calibration image and their uncertainties

%-- Image #1:
omc_1 = [ 1.948828e+00 ; 1.889728e+00 ; 6.933073e-01 ];
Tc_1  = [ -1.793317e+02 ; -1.156005e+02 ; 6.417317e+02 ];
omc_error_1 = [ 1.832729e-03 ; 1.495016e-03 ; 2.844611e-03 ];
Tc_error_1  = [ 1.499293e+00 ; 1.231309e+00 ; 1.134665e+00 ];

%-- Image #2:
omc_2 = [ 1.907594e+00 ; 1.957223e+00 ; 5.992834e-01 ];
Tc_2  = [ -1.714870e+02 ; -2.866160e+01 ; 6.311307e+02 ];
omc_error_2 = [ 1.879577e-03 ; 1.378110e-03 ; 2.974764e-03 ];
Tc_error_2  = [ 1.455696e+00 ; 1.206977e+00 ; 1.103962e+00 ];

%-- Image #3:
omc_3 = [ 1.809227e+00 ; 2.017047e+00 ; 5.743320e-01 ];
Tc_3  = [ -1.489840e+02 ; -1.067148e+01 ; 6.239410e+02 ];
omc_error_3 = [ 1.882117e-03 ; 1.404251e-03 ; 2.963503e-03 ];
Tc_error_3  = [ 1.429043e+00 ; 1.186709e+00 ; 1.085612e+00 ];

%-- Image #4:
omc_4 = [ 1.712440e+00 ; 1.741666e+00 ; 8.916906e-01 ];
Tc_4  = [ -1.433800e+02 ; -1.905008e+01 ; 5.956365e+02 ];
omc_error_4 = [ 2.046669e-03 ; 1.328321e-03 ; 2.505972e-03 ];
Tc_error_4  = [ 1.372492e+00 ; 1.134588e+00 ; 1.058252e+00 ];

%-- Image #5:
omc_5 = [ 2.073109e+00 ; 1.686175e+00 ; 8.348571e-01 ];
Tc_5  = [ -1.553225e+02 ; -4.326489e+01 ; 6.227150e+02 ];
omc_error_5 = [ 2.081406e-03 ; 1.189678e-03 ; 2.799317e-03 ];
Tc_error_5  = [ 1.436104e+00 ; 1.186250e+00 ; 1.095027e+00 ];

%-- Image #6:
omc_6 = [ -2.490456e+00 ; -1.783972e+00 ; -5.010215e-01 ];
Tc_6  = [ -1.455528e+02 ; -4.193606e+01 ; 6.512697e+02 ];
omc_error_6 = [ 1.777653e-03 ; 1.657156e-03 ; 3.789640e-03 ];
Tc_error_6  = [ 1.482021e+00 ; 1.235080e+00 ; 1.088743e+00 ];

%-- Image #7:
omc_7 = [ 1.490212e+00 ; 2.379931e+00 ; -8.467492e-02 ];
Tc_7  = [ -7.134081e+01 ; -1.013615e+02 ; 6.356707e+02 ];
omc_error_7 = [ 1.221355e-03 ; 2.053555e-03 ; 3.361344e-03 ];
Tc_error_7  = [ 1.435741e+00 ; 1.196461e+00 ; 9.673879e-01 ];

%-- Image #8:
omc_8 = [ 1.439414e+00 ; 1.878501e+00 ; 6.515815e-01 ];
Tc_8  = [ -7.465223e+00 ; -8.121678e+01 ; 5.072110e+02 ];
omc_error_8 = [ 1.897696e-03 ; 1.720371e-03 ; 2.413328e-03 ];
Tc_error_8  = [ 1.139282e+00 ; 9.496197e-01 ; 8.338452e-01 ];

%-- Image #9:
omc_9 = [ -1.514625e+00 ; -2.093047e+00 ; 4.512508e-01 ];
Tc_9  = [ -1.041168e+02 ; -1.047788e+02 ; 6.336445e+02 ];
omc_error_9 = [ 1.816680e-03 ; 1.937257e-03 ; 2.568552e-03 ];
Tc_error_9  = [ 1.433187e+00 ; 1.195293e+00 ; 8.858898e-01 ];

%-- Image #10:
omc_10 = [ -2.161678e+00 ; -2.165418e+00 ; -3.574715e-01 ];
Tc_10  = [ -1.034573e+02 ; -5.747311e+01 ; 4.187967e+02 ];
omc_error_10 = [ 1.335818e-03 ; 1.797888e-03 ; 3.359635e-03 ];
Tc_error_10  = [ 9.495563e-01 ; 7.962945e-01 ; 7.133242e-01 ];

%-- Image #11:
omc_11 = [ 1.963196e+00 ; 1.839919e+00 ; 8.446459e-01 ];
Tc_11  = [ -6.380684e+01 ; -6.579539e+01 ; 4.052450e+02 ];
omc_error_11 = [ 2.120082e-03 ; 1.294008e-03 ; 2.756522e-03 ];
Tc_error_11  = [ 9.222054e-01 ; 7.615688e-01 ; 7.053177e-01 ];

%-- Image #12:
omc_12 = [ 2.169201e+00 ; 2.063914e+00 ; 5.601027e-01 ];
Tc_12  = [ -5.286414e+01 ; -7.628604e+01 ; 4.717590e+02 ];
omc_error_12 = [ 2.103938e-03 ; 1.390849e-03 ; 3.293028e-03 ];
Tc_error_12  = [ 1.068847e+00 ; 8.824635e-01 ; 7.752561e-01 ];

%-- Image #13:
omc_13 = [ 1.572251e+00 ; 1.865590e+00 ; 5.343782e-01 ];
Tc_13  = [ 5.875647e+01 ; -4.326248e+01 ; 5.599326e+02 ];
omc_error_13 = [ 2.006613e-03 ; 1.678885e-03 ; 2.697082e-03 ];
Tc_error_13  = [ 1.254674e+00 ; 1.051274e+00 ; 8.990649e-01 ];

%-- Image #14:
omc_14 = [ 1.977439e+00 ; 2.074859e+00 ; 1.721269e-01 ];
Tc_14  = [ -1.104210e+02 ; -2.833349e+01 ; 6.050475e+02 ];
omc_error_14 = [ 1.767960e-03 ; 1.617747e-03 ; 3.637395e-03 ];
Tc_error_14  = [ 1.374339e+00 ; 1.140906e+00 ; 9.849146e-01 ];

%-- Image #15:
omc_15 = [ 2.016294e+00 ; 1.974788e+00 ; 2.516294e-01 ];
Tc_15  = [ 1.249234e+02 ; -2.076297e+02 ; 1.317512e+03 ];
omc_error_15 = [ 2.264955e-03 ; 2.202140e-03 ; 4.072900e-03 ];
Tc_error_15  = [ 2.990102e+00 ; 2.483057e+00 ; 2.343190e+00 ];

%-- Image #16:
omc_16 = [ 1.921263e+00 ; 1.427327e+00 ; -2.511455e-01 ];
Tc_16  = [ 2.563868e+01 ; 6.717371e+01 ; 1.104224e+03 ];
omc_error_16 = [ 1.972521e-03 ; 1.731707e-03 ; 2.941592e-03 ];
Tc_error_16  = [ 2.488777e+00 ; 2.060740e+00 ; 1.736300e+00 ];

%-- Image #17:
omc_17 = [ 1.433381e+00 ; 2.104283e+00 ; -8.656777e-01 ];
Tc_17  = [ -9.900453e+01 ; 1.853240e+01 ; 1.066313e+03 ];
omc_error_17 = [ 1.226662e-03 ; 2.412979e-03 ; 2.837253e-03 ];
Tc_error_17  = [ 2.396128e+00 ; 2.004714e+00 ; 1.532637e+00 ];

%-- Image #18:
omc_18 = [ 1.696295e+00 ; 2.482829e+00 ; -8.115475e-01 ];
Tc_18  = [ -1.130788e+02 ; -7.642944e+01 ; 1.076314e+03 ];
omc_error_18 = [ 1.222361e-03 ; 2.708145e-03 ; 3.518667e-03 ];
Tc_error_18  = [ 2.422203e+00 ; 2.029549e+00 ; 1.574604e+00 ];

%-- Image #19:
omc_19 = [ -1.330803e+00 ; -2.171854e+00 ; -3.602866e-02 ];
Tc_19  = [ -9.440673e+01 ; -1.532530e+02 ; 9.742232e+02 ];
omc_error_19 = [ 1.496819e-03 ; 2.291624e-03 ; 2.892135e-03 ];
Tc_error_19  = [ 2.206177e+00 ; 1.835346e+00 ; 1.560910e+00 ];

%-- Image #20:
omc_20 = [ -1.872087e+00 ; -1.793279e+00 ; -1.853305e-02 ];
Tc_20  = [ -1.793108e+01 ; -9.859165e+01 ; 1.166669e+03 ];
omc_error_20 = [ 1.798253e-03 ; 2.144447e-03 ; 3.596023e-03 ];
Tc_error_20  = [ 2.630887e+00 ; 2.192074e+00 ; 1.795389e+00 ];

%-- Image #21:
omc_21 = [ 1.172985e+00 ; 2.278685e+00 ; -1.261662e+00 ];
Tc_21  = [ 3.151071e+01 ; -4.484921e+01 ; 1.094733e+03 ];
omc_error_21 = [ 1.286358e-03 ; 2.561745e-03 ; 2.751929e-03 ];
Tc_error_21  = [ 2.463189e+00 ; 2.057425e+00 ; 1.533708e+00 ];

