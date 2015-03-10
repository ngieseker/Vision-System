% Intrinsic and Extrinsic Camera Parameters
%
% This script file can be directly excecuted under Matlab to recover the camera intrinsic and extrinsic parameters.
% IMPORTANT: This file contains neither the structure of the calibration objects nor the image coordinates of the calibration points.
%            All those complementary variables are saved in the complete matlab data file Calib_Results.mat.
% For more information regarding the calibration model visit http://www.vision.caltech.edu/bouguetj/calib_doc/


%-- Focal length:
fc = [ 1719.804303870196236 ; 1720.068886960818645 ];

%-- Principal point:
cc = [ 542.204134094175970 ; 355.858735831354977 ];

%-- Skew coefficient:
alpha_c = 0.000000000000000;

%-- Distortion coefficients:
kc = [ -0.403981780788214 ; 0.100117379115587 ; 0.002692192052163 ; -0.000610420677572 ; 0.000000000000000 ];

%-- Focal length uncertainty:
fc_error = [ 1.949798952496592 ; 1.873640834460206 ];

%-- Principal point uncertainty:
cc_error = [ 2.270740816487633 ; 1.900683537089427 ];

%-- Skew coefficient uncertainty:
alpha_c_error = 0.000000000000000;

%-- Distortion coefficients uncertainty:
kc_error = [ 0.004646022285824 ; 0.046101654769623 ; 0.000247872165556 ; 0.000187294863795 ; 0.000000000000000 ];

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
omc_1 = [ -1.898442e+00 ; -1.919557e+00 ; 2.132132e-01 ];
Tc_1  = [ -7.630325e+01 ; -8.269431e+01 ; 7.023741e+02 ];
omc_error_1 = [ 9.880334e-04 ; 1.010053e-03 ; 1.735492e-03 ];
Tc_error_1  = [ 9.272859e-01 ; 7.768539e-01 ; 7.508484e-01 ];

%-- Image #2:
omc_2 = [ -2.065354e+00 ; -2.070907e+00 ; -4.914437e-01 ];
Tc_2  = [ -1.030030e+02 ; -4.820392e+01 ; 4.795317e+02 ];
omc_error_2 = [ 6.969148e-04 ; 1.087364e-03 ; 1.922176e-03 ];
Tc_error_2  = [ 6.349309e-01 ; 5.357510e-01 ; 5.955379e-01 ];

%-- Image #3:
omc_3 = [ -1.853342e+00 ; -1.863681e+00 ; 7.760679e-02 ];
Tc_3  = [ -2.285752e+02 ; -1.280891e+02 ; 7.779720e+02 ];
omc_error_3 = [ 1.064982e-03 ; 9.824072e-04 ; 1.839405e-03 ];
Tc_error_3  = [ 1.038859e+00 ; 8.782592e-01 ; 9.322220e-01 ];

%-- Image #4:
omc_4 = [ -1.896155e+00 ; -1.882252e+00 ; 6.951848e-02 ];
Tc_4  = [ -2.208038e+02 ; -9.242823e+01 ; 7.923030e+02 ];
omc_error_4 = [ 1.058690e-03 ; 1.017186e-03 ; 1.871058e-03 ];
Tc_error_4  = [ 1.053337e+00 ; 8.918127e-01 ; 9.375767e-01 ];

%-- Image #5:
omc_5 = [ -1.955430e+00 ; -1.910800e+00 ; -2.526672e-02 ];
Tc_5  = [ -2.394187e+02 ; -2.138237e+01 ; 7.699873e+02 ];
omc_error_5 = [ 1.044441e-03 ; 1.101909e-03 ; 1.945355e-03 ];
Tc_error_5  = [ 1.022004e+00 ; 8.701131e-01 ; 9.371048e-01 ];

%-- Image #6:
omc_6 = [ -1.660385e+00 ; -1.880113e+00 ; 3.478631e-01 ];
Tc_6  = [ -1.822625e+02 ; -6.796855e+01 ; 8.326267e+02 ];
omc_error_6 = [ 1.075855e-03 ; 1.094479e-03 ; 1.588814e-03 ];
Tc_error_6  = [ 1.105097e+00 ; 9.305774e-01 ; 9.051191e-01 ];

%-- Image #7:
omc_7 = [ -2.036425e+00 ; -2.243280e+00 ; -5.893094e-01 ];
Tc_7  = [ -1.428207e+02 ; -1.940699e+01 ; 6.348955e+02 ];
omc_error_7 = [ 7.605231e-04 ; 1.204643e-03 ; 2.160806e-03 ];
Tc_error_7  = [ 8.369123e-01 ; 7.094694e-01 ; 7.919340e-01 ];

%-- Image #8:
omc_8 = [ -2.122210e+00 ; -2.119254e+00 ; -5.657511e-01 ];
Tc_8  = [ -7.258843e+01 ; -7.422146e+00 ; 6.175377e+02 ];
omc_error_8 = [ 7.107837e-04 ; 1.200402e-03 ; 2.152638e-03 ];
Tc_error_8  = [ 8.138427e-01 ; 6.827831e-01 ; 7.495937e-01 ];

%-- Image #9:
omc_9 = [ 2.076749e+00 ; 1.967019e+00 ; -5.935074e-02 ];
Tc_9  = [ -1.340638e+02 ; -7.992950e+00 ; 7.075185e+02 ];
omc_error_9 = [ 1.167365e-03 ; 1.155756e-03 ; 2.659399e-03 ];
Tc_error_9  = [ 9.363486e-01 ; 7.870551e-01 ; 7.941139e-01 ];

%-- Image #10:
omc_10 = [ 2.289757e+00 ; 2.079672e+00 ; 3.999427e-01 ];
Tc_10  = [ -4.458656e+01 ; -7.642097e+01 ; 6.508693e+02 ];
omc_error_10 = [ 1.313486e-03 ; 9.482916e-04 ; 2.342288e-03 ];
Tc_error_10  = [ 8.625675e-01 ; 7.186962e-01 ; 7.654788e-01 ];

%-- Image #11:
omc_11 = [ -2.235224e+00 ; -2.059162e+00 ; -4.065955e-01 ];
Tc_11  = [ -1.060808e+02 ; -9.617884e+01 ; 7.242787e+02 ];
omc_error_11 = [ 1.090452e-03 ; 1.188912e-03 ; 2.421734e-03 ];
Tc_error_11  = [ 9.643319e-01 ; 8.089103e-01 ; 8.773811e-01 ];

%-- Image #12:
omc_12 = [ -1.763361e+00 ; -1.914598e+00 ; 3.974917e-01 ];
Tc_12  = [ -8.979915e+01 ; -1.092032e+02 ; 8.065908e+02 ];
omc_error_12 = [ 1.058750e-03 ; 9.916815e-04 ; 1.675287e-03 ];
Tc_error_12  = [ 1.067727e+00 ; 8.919934e-01 ; 8.421475e-01 ];

%-- Image #13:
omc_13 = [ -2.096551e+00 ; -1.931595e+00 ; 6.265960e-01 ];
Tc_13  = [ -1.239577e+02 ; -4.839783e+01 ; 8.341879e+02 ];
omc_error_13 = [ 1.282575e-03 ; 8.872812e-04 ; 1.924003e-03 ];
Tc_error_13  = [ 1.101955e+00 ; 9.240116e-01 ; 8.652111e-01 ];

%-- Image #14:
omc_14 = [ -1.689129e+00 ; -1.950019e+00 ; 4.455185e-01 ];
Tc_14  = [ -3.082982e+01 ; -1.046581e+02 ; 8.302026e+02 ];
omc_error_14 = [ 1.020137e-03 ; 1.028672e-03 ; 1.646882e-03 ];
Tc_error_14  = [ 1.097808e+00 ; 9.158561e-01 ; 8.456650e-01 ];

%-- Image #15:
omc_15 = [ 2.082490e+00 ; 2.238556e+00 ; -2.446170e-01 ];
Tc_15  = [ -1.844785e+02 ; -1.080380e+02 ; 7.973637e+02 ];
omc_error_15 = [ 1.142798e-03 ; 1.550161e-03 ; 2.711131e-03 ];
Tc_error_15  = [ 1.058264e+00 ; 8.930096e-01 ; 9.134199e-01 ];

%-- Image #16:
omc_16 = [ 1.864982e+00 ; 1.505744e+00 ; 1.301476e+00 ];
Tc_16  = [ -1.485364e+02 ; -3.783220e+01 ; 5.430255e+02 ];
omc_error_16 = [ 1.378628e-03 ; 7.941062e-04 ; 1.523078e-03 ];
Tc_error_16  = [ 7.294006e-01 ; 6.114048e-01 ; 7.376121e-01 ];

%-- Image #17:
omc_17 = [ -1.641520e+00 ; -1.639053e+00 ; 3.205824e-01 ];
Tc_17  = [ -1.102666e+02 ; -1.138129e+02 ; 8.491034e+02 ];
omc_error_17 = [ 1.030758e-03 ; 1.009034e-03 ; 1.466446e-03 ];
Tc_error_17  = [ 1.124609e+00 ; 9.413518e-01 ; 9.000928e-01 ];

%-- Image #18:
omc_18 = [ -1.645081e+00 ; -1.535775e+00 ; 9.703922e-02 ];
Tc_18  = [ -1.280876e+02 ; -1.040262e+02 ; 8.505823e+02 ];
omc_error_18 = [ 9.828970e-04 ; 1.035740e-03 ; 1.434403e-03 ];
Tc_error_18  = [ 1.125568e+00 ; 9.458122e-01 ; 9.368342e-01 ];

%-- Image #19:
omc_19 = [ -1.972091e+00 ; -1.951553e+00 ; 1.028857e-01 ];
Tc_19  = [ -1.085499e+02 ; -6.614928e+01 ; 5.680810e+02 ];
omc_error_19 = [ 9.376455e-04 ; 9.899828e-04 ; 1.755642e-03 ];
Tc_error_19  = [ 7.500126e-01 ; 6.310835e-01 ; 6.367412e-01 ];

%-- Image #20:
omc_20 = [ -1.899909e+00 ; -2.052913e+00 ; -7.758221e-03 ];
Tc_20  = [ -1.197900e+02 ; -6.813018e+01 ; 5.227615e+02 ];
omc_error_20 = [ 8.676432e-04 ; 1.045100e-03 ; 1.775642e-03 ];
Tc_error_20  = [ 6.905685e-01 ; 5.838399e-01 ; 6.091208e-01 ];

%-- Image #21:
omc_21 = [ -2.062877e+00 ; -2.132717e+00 ; -1.652125e-01 ];
Tc_21  = [ -1.189890e+02 ; -7.327029e+01 ; 4.394549e+02 ];
omc_error_21 = [ 7.990676e-04 ; 9.886857e-04 ; 1.915285e-03 ];
Tc_error_21  = [ 5.837312e-01 ; 4.951797e-01 ; 5.444169e-01 ];

