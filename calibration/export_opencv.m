%NOTE: 
% You have to be in the folder with your calibration data.
% You also need the calibration toolbox and the folder with
% this script in the Matlab path.
function [fc, cc, alpha_c, kc, Tc_ext, Rc_ext] = export_opencv(filename, Tc_ext, Rc_ext)
	loading_calib;
	if (nargin < 3)
		extrinsic_computation;
	end

	if (nargin == 0 || isequal(filename,''))
		filename = 'camModel.yaml';
	end

	intrinsic = [fc(1) alpha_c cc(1); ...
	             0     fc(2)   cc(2); ...
	             0     0       1];

	fn = fopen(filename, 'w+');

	fprintf(fn, '%%YAML:1.0\n');
	fprintf(fn, 'intrinsic: !!opencv-matrix\n');
	fprintf(fn, '   rows: 3\n');
	fprintf(fn, '   cols: 3\n');
	fprintf(fn, '   dt: d\n');
	fprintf(fn, '   data: [\n'); 
	fprintf(fn, '      %12.7f, %12.7f, %12.7f,\n',  intrinsic(1,:)); 
	fprintf(fn, '      %12.7f, %12.7f, %12.7f,\n',  intrinsic(2,:)); 
	fprintf(fn, '      %12.7f, %12.7f, %12.7f ]\n', intrinsic(3,:)); 
	fprintf(fn, 'distCoeff: !!opencv-matrix\n');
	fprintf(fn, '   rows: 1\n');
	fprintf(fn, '   cols: 5\n');
	fprintf(fn, '   dt: d\n');
	fprintf(fn, '   data: [ %f, %f, %f, %f, %f]\n', kc); 

	fprintf(fn, 'Rc_ext: !!opencv-matrix\n');
	fprintf(fn, '   rows: 3\n');
	fprintf(fn, '   cols: 3\n');
	fprintf(fn, '   dt: d\n');
	fprintf(fn, '   data: [\n'); 
	fprintf(fn, '      %12.7f, %12.7f, %12.7f,\n',  Rc_ext(1,:)); 
	fprintf(fn, '      %12.7f, %12.7f, %12.7f,\n',  Rc_ext(2,:)); 
	fprintf(fn, '      %12.7f, %12.7f, %12.7f ]\n', Rc_ext(3,:)); 
	fprintf(fn, 'Tc_ext: !!opencv-matrix\n');
	fprintf(fn, '   rows: 3\n');
	fprintf(fn, '   cols: 1\n');
	fprintf(fn, '   dt: d\n');
	fprintf(fn, '   data: [ %f, %f, %f]\n', Tc_ext); 
end
