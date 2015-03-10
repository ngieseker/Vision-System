function [ ] = save_int_ex(fn, Tc_ext, Rc_ext)
	loading_calib
	if (nargin ~= 3)
		extrinsic_computation
	end
	K = [fc(1) 0 cc(1);0 fc(2) cc(2);0 0 1]
	cr = Rc_ext
	ct = Tc_ext
	save(fn, 'K', 'cr', 'ct');
