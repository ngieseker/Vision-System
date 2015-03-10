function [ ] = camplacement(cam, focal, height)
    if (nargin == 2)
        height = 0;
        use_height = 0;
    elseif (nargin == 3)
        use_height = 1;
    end
    
    if (cam == 1)
        % Flea3
        res = [640; 480; 1];
        focal = focal * 1.78;
    else
        % Imaging source
        res = [1024; 768; 1];
    end
    
% 	tall_h = 18.69; % tall height
% 	short_h = 14.63; % short height

    tall_h = 17.5;
    short_h = 15.5;

	l = 25.6; % side length
	short_w = 6; % short width
	long_w = 9.3; % long width

	width_diff_2 = (long_w-short_w)/2;

	trapazoid_h = sqrt(l^2 - width_diff_2^2);


	x = [0 short_w; -width_diff_2 short_w+width_diff_2]*1000;
	y = [0 0; trapazoid_h trapazoid_h]*1000;
	z = [0 0; 0 0]*1000;
	bot = surf(x,y,z);
    hold on;
        
	z = [4 4; 4 4]*1000;
	surf(x,y,z);
    
    z = [8 8; 8 8]*1000;
	surf(x,y,z);
    
    z = [13 13; 13 13]*1000;
	surf(x,y,z);

	z = [tall_h short_h; tall_h short_h]*1000;
	surf(x,y,z);
	
	x = [0 short_w ;0 short_w]*1000;
	y = [0 0; 0 0]*1000;
	z = [0 0; tall_h short_h]*1000;
	surf(x,y,z);

	x = [-width_diff_2 short_w+width_diff_2; -width_diff_2 short_w+width_diff_2]*1000;
	y = [trapazoid_h trapazoid_h; trapazoid_h trapazoid_h]*1000;
	surf(x,y,z);

	x = [0 0; -width_diff_2 -width_diff_2]*1000;
	y = [0 0; trapazoid_h trapazoid_h]*1000;
	z = [0 tall_h; 0 tall_h]*1000;
	surf(x,y,z);

	x = [short_w short_w; short_w+width_diff_2 short_w+width_diff_2]*1000;
	y = [0 0; trapazoid_h trapazoid_h]*1000;
	z = [0 short_h; 0 short_h]*1000;
	surf(x,y,z);


    K = [focal*res(1)/4.8 0.0000000 res(1)/2; ...
        0.0000000 focal*res(2)/3.6 res(2)/2; ...
        0.0000000 0.0000000 1.0000000];

	cor = zeros(0,3);

	function points = camvol(pos, fpos, points)
		rot = fpos - pos;
        
        pitch_up = atan2(rot(1), pos(3));
        yaw      = atan2(rot(2), pos(3));
        
        rot = rot ./ norm(rot);
        
        R1 = [cos(pi/2) -sin(pi/2)  0;...
              sin(pi/2)  cos(pi/2)  0;...
              0          0          1];
        R2 = [cos(pitch_up)  0  -sin(pitch_up);...
              0              1   0;...
              sin(pitch_up)  0   cos(pitch_up)];
        R3 = [1  0         0;...
              0  cos(yaw) -sin(yaw);...
              0  sin(yaw)  cos(yaw)];
        
        R = R3*R2*R1;
        

		origin = [0; 0; height];
		normal = [0; 0; 1];

		a = quiver3(pos(1), pos(2), pos(3), rot(1), rot(2), rot(3), dot(origin - pos, normal) / dot(rot, normal));
		a = plot3(pos(1), pos(2), pos(3),'-g.', 'MarkerSize', 25);

		point = [0; 0; 1];
		cone = (R)*inv(K)*point;
		conen = cone./norm(cone);
		points(end+1,1:3) = conen*dot(origin - pos, normal)/dot(conen, normal) + pos;
        
		point = res .* [0; 1; 1];
		cone = (R)*inv(K)*point;
		conen = cone./norm(cone);
		points(end+1,1:3) = conen*dot(origin - pos, normal)/dot(conen, normal) + pos;

		point = res .* [1; 1; 1];
		cone = (R)*inv(K)*point;
		conen = cone./norm(cone);
		points(end+1,1:3) = conen*dot(origin - pos, normal)/dot(conen, normal) + pos;

		point = res .* [1; 0; 1];
		cone = (R)*inv(K)*point;
		conen = cone./norm(cone);
		points(end+1,1:3) = conen*dot(origin - pos, normal)/dot(conen, normal) + pos;


        if (~use_height)
            points(end+1,1:3) = pos;
            DT = DelaunayTri(points);
            hf = convexHull(DT);
            CH = trisurf(hf, DT.X(:,1), DT.X(:,2), DT.X(:,3), 'FaceColor','r','FaceLighting','gouraud','LineStyle', '--');
        else
            CH = fill3(points(:,1), points(:,2), points(:,3), 'r');
        end
        
		alpha(CH, 0.2)
	end
	
	alpha(0.0001);
	alpha(bot, 0.2);

    
    cam_h_low = short_h;
    cam_h_high = tall_h;

	% Position of each pair of cameras as the proportion of their distance along the wall
	pair_1_p = 0;
	pair_2_p = 2/8;
	pair_3_p = 5/8;
	pair_4_p = 7/8;

	% Where each camera is facing
	pair_1_f = [5/8 1/4-1/16];
	pair_2_f = [5/8 7/16-1/16];
	pair_3_f = [5/8 9/16-1/16];
	pair_4_f = [5/8 3/4-1/16];

	p1_low  = { [short_w + pair_1_p*width_diff_2; pair_1_p*trapazoid_h; cam_h_low]*1000, [pair_1_f(1)*short_w;     pair_1_f(2)*trapazoid_h; 0]*1000 };
	p1_high = { [0 - pair_1_p*width_diff_2;       pair_1_p*trapazoid_h; cam_h_high]*1000,  [(1-pair_1_f(1))*short_w; pair_1_f(2)*trapazoid_h; 0]*1000 };

	p2_low  = { [short_w + pair_2_p*width_diff_2; pair_2_p*trapazoid_h; cam_h_low]*1000, [pair_2_f(1)*short_w;     pair_2_f(2)*trapazoid_h; 0]*1000 };
	p2_high = { [0 - pair_2_p*width_diff_2;       pair_2_p*trapazoid_h; cam_h_high]*1000,  [(1-pair_2_f(1))*short_w; pair_2_f(2)*trapazoid_h; 0]*1000 };

	p3_low  = { [short_w + pair_3_p*width_diff_2; pair_3_p*trapazoid_h; cam_h_low]*1000, [pair_3_f(1)*short_w;     pair_3_f(2)*trapazoid_h; 0]*1000 };
	p3_high = { [0 - pair_3_p*width_diff_2;       pair_3_p*trapazoid_h; cam_h_high]*1000,  [(1-pair_3_f(1))*short_w; pair_3_f(2)*trapazoid_h; 0]*1000 };

	p4_low  = { [short_w + pair_4_p*width_diff_2; pair_4_p*trapazoid_h; cam_h_low]*1000, [pair_4_f(1)*short_w;     pair_4_f(2)*trapazoid_h; 0]*1000 };
	p4_high = { [0 - pair_4_p*width_diff_2;       pair_4_p*trapazoid_h; cam_h_high]*1000,  [(1-pair_4_f(1))*short_w; pair_4_f(2)*trapazoid_h; 0]*1000 };


	low_source  = [p1_low{1}(1) - 1000;  p1_low{1}(2);  8000];
	high_source  = [p1_high{1}(1) + 1000; p1_high{1}(2); 8000];

% 	fprintf('Cable lengths:\n');
% 	fprintf('  Pair 1, low: %6.2fm, %d repeaters\n', cableLength(low_source,  p1_low{1}),  ceil(cableLength(low_source,  p1_low{1})/10)-1);
% 	fprintf('  Pair 1, high:%6.2fm, %d repeaters\n', cableLength(high_source, p1_high{1}), ceil(cableLength(high_source, p1_high{1})/10)-1);
% 	fprintf('  Pair 2, low: %6.2fm, %d repeaters\n', cableLength(low_source,  p2_low{1}),  ceil(cableLength(low_source,  p2_low{1})/10)-1);
% 	fprintf('  Pair 2, high:%6.2fm, %d repeaters\n', cableLength(high_source, p2_high{1}), ceil(cableLength(high_source, p2_high{1})/10)-1);
% 	fprintf('  Pair 3, low: %6.2fm, %d repeaters\n', cableLength(low_source,  p3_low{1}),  ceil(cableLength(low_source,  p3_low{1})/10)-1);
% 	fprintf('  Pair 3, high:%6.2fm, %d repeaters\n', cableLength(high_source, p3_high{1}), ceil(cableLength(high_source, p3_high{1})/10)-1);
% 	fprintf('  Pair 4, low: %6.2fm, %d repeaters\n', cableLength(low_source,  p4_low{1}),  ceil(cableLength(low_source,  p4_low{1})/10)-1);
% 	fprintf('  Pair 4, high:%6.2fm, %d repeaters\n', cableLength(high_source, p4_high{1}), ceil(cableLength(high_source, p4_high{1})/10)-1);


 	cor = camvol(p1_low{1},  p1_low{2},  []);
 	cor = camvol(p1_high{1}, p1_high{2}, []);
 	cor = camvol(p2_low{1},  p2_low{2},  []);
 	cor = camvol(p2_high{1}, p2_high{2}, []);
 	cor = camvol(p3_low{1},  p3_low{2},  []);
 	cor = camvol(p3_high{1}, p3_high{2}, []);
 	cor = camvol(p4_low{1},  p4_low{2},  []);
 	cor = camvol(p4_high{1}, p4_high{2}, []);
	

	% a = plot3(cor(:,1), cor(:,2), cor(:,3), '--b');
	 
	hold off;
	axis equal;
	grid on;
	axis on;
    xlabel('x');
    ylabel('y');
    zlabel('z');

	function l = cableLength(s, c)
		c = c - s;
		l = c(3) + sqrt(c(1)^2 + c(2)^2);
		l = l / 1000;
	end

	function R = axisAngle2rotMat(v, a)
		x = v(1);
		y = v(2);
		z = v(3);
		vx = [0 -z y; z 0 -x; -y x 0];
		vv = [x*x x*y x*z; y*x y*y y*z; z*x z*y z*z];
		
		test = vrrotvec2mat([v;a])
		R = eye(3)*cos(a) + sin(a)*vx + (1-cos(a))*vv
 	end
end
