function [sock] = live_plot()
	MSG_INIT        = 1;
	MSG_MARKERS     = 2;
	MSG_POSITION    = 3;
	MSG_ORIENTATION = 4;

	global point;
	global ray;
    
	function handleMessage(s, e)
        avail = get(s, 'BytesAvailable');
        if (avail ~= 0)
            header = fread(s, 1, 'uint8');

			switch header
				case MSG_INIT
					offset = initialize(s);
				case MSG_MARKERS
					offset = updateMarkers(s);
				case MSG_POSITION
					offset = updatePosition(s);
				case MSG_ORIENTATION
					offset = updateOrientation(s);
                otherwise
					fprintf('Unrecognized header %d\n', header);
            end

			fread(s, 1024 - offset, 'uint8');
        end
	end

	function offset = initialize(s)
        offset = 1;
		numCams = fread(s, 1, 'uint8');
        offset = offset + 1;

		% Each row is one camera
		% Column 1 is the intrinsic matrix, column 2 is the translation vector, column 3 is the rotation matrix
		cams = cell(numCams, 2);
		for c = 1:numCams
			K = [];
			for i = 1:3
				K(i,1:3) = fread(s, 3, 'double');
                offset = offset + 3*8;
			end
			cams{c,1} = K;

			t = fread(s, 3, 'double');
            offset = offset + 3*8;
			cams{c,2} = t;

			r = [];
			for i = 1:3
				r(i,1:3) = fread(s, 3, 'double');
                offset = offset + 3*8;
			end
			cams{c,3} = r;
		end


		center = [1024/2; 768/2; 1];
		origin = [0; 0; 0];
		normal = [0; 0; 1];

		hold off;
		for c = 1:numCams
			if c == 1
				color = [1 0 0];
			elseif c == 2
				color = [0 1 0];
			elseif c == 3
				color = [0 0 1];
			end

			K = cams{c,1};
			T = cams{c,2};
			R = cams{c,3};

			pos = -inv(cams{c,3}) * cams{c,2};
			rot = inv(cams{c,3}) * inv(cams{c,1}) * center;
			rot = rot ./ norm(rot);

			a = quiver3(pos(1), pos(2), pos(3), rot(1), rot(2), rot(3), dot(origin - pos, normal) / dot(rot, normal));
			set(a, 'Color', color);
			hold on;
			a = plot3(pos(1), pos(2), pos(3),'-g.', 'MarkerSize', 25);
			set(a, 'Color', color);


			point = [0; 0; 1];
			cone = inv(R)*inv(K)*normal;
			conen = cone./norm(cone);
			a = quiver3(pos(1), pos(2), pos(3), conen(1), conen(2), conen(3), dot(origin - pos, normal)/dot(conen, normal), ':b', 'LineWidth', 2);
			set(a, 'Color', color);
			cor(1,:) = conen*dot(origin - pos, normal)/dot(conen, normal)+pos;
			cor(5,:) = conen*dot(origin - pos, normal)/dot(conen, normal)+pos;

			point = [0; 768; 1];
			cone = inv(R)*inv(K)*point;
			conen = cone./norm(cone);
			a = quiver3(pos(1), pos(2), pos(3), conen(1), conen(2), conen(3), dot(origin - pos, normal)/dot(conen, normal), ':b', 'LineWidth', 2);
			set(a, 'Color', color);
			cor(2,:) = conen*dot(origin - pos, normal)/dot(conen, normal)+pos;

			point = [1024; 768; 1];
			cone = inv(R)*inv(K)*point;
			conen = cone./norm(cone);
			a = quiver3(pos(1), pos(2), pos(3), conen(1), conen(2), conen(3), dot(origin - pos, normal)/dot(conen, normal), ':b', 'LineWidth', 2);
			set(a, 'Color', color);
			cor(3,:) = conen*dot(origin - pos, normal)/dot(conen, normal)+pos;

			point = [1024; 0; 1];
			cone = inv(R)*inv(K)*point;
			conen = cone./norm(cone);
			a = quiver3(pos(1), pos(2), pos(3), conen(1), conen(2), conen(3), dot(origin - pos, normal)/dot(conen, normal), ':b', 'LineWidth', 2);
			set(a, 'Color', color);
			cor(4,:) = conen*dot(origin - pos, normal)/dot(conen, normal)+pos;

			a = plot3(cor(:,1), cor(:,2), cor(:,3), '--b');
			set(a, 'Color', color);
		end

		squaresx = 6;% 10;
		squaresy = 5;% 14;
		width = 101;% 16;

		cor = [0, 0, 0;
			   0, width*squaresy, 0;
			   width*squaresx, width*squaresy, 0;
			   width*squaresx, 0, 0;
			   0, 0, 0];

		for i = 1:squaresx-1
			temp = [i*width, 0, 0;
					i*width, squaresy*width, 0];
			plot3(temp(:,1), temp(:,2), temp(:,3), '-c');
		end
		for i = 1:squaresy-1
			temp = [0, i*width, 0;
					squaresx*width, i*width, 0];
			plot3(temp(:,1), temp(:,2), temp(:,3), '-c');
		end

		plot3(cor(:,1), cor(:,2), cor(:,3), '-k');
		plot3(0,0,0, '-b.', 'MarkerSize', 10);

		point = plot3(0,0,0, 'r.', 'MarkerSize', 15);
	
		fixedpoint = plot3(0,0,0, 'g.', 'MarkerSize', 15);

		%ray = quiver3(0, 0, 0, 1, 1, 1, 1, 'k', 'LineWidth', 1);

		xl = xlim;
		xm = max(abs(xl));
		yl = ylim;
		ym = max(abs(yl));
		zl = zlim;
		%xlim([-xm xm]);
		%ylim([-ym ym]);
		zlim([0 zl(2)]);

		%axis(gca, 'vis3d');
		axis(gca, 'equal');
		grid on;
	end

	function offset = updateMarkers(s)
        offset = 1;
		numMarks = fread(s, 1, 'uint8');
        offset = offset + 1;
        
		marks = zeros(numMarks, 3);
		rays = zeros(1, 6);
		
		for i = 1:numMarks
			marks(i,1:3) = fread(s, 3, 'double');
            offset = offset + 3*8;

			numRays = fread(s, 1, 'uint8');
			offset = offset + 1;
			if i == 1
				rays = zeros(numRays, 6);
			end
			for j = 1:numRays
				if i == 1
					rays(j,1:6) = fread(s, 6, 'double');
				else
					fread(s, 6, 'double');
				end
				offset = offset + 6*8;
			end
        end
        
        % Moving average
        if numMarks == 4
            moving_avg(:,:,cur_point+1) = marks;
            cur_point = mod(cur_point+1, avg_points);
            marks = mean(moving_avg, 3);
        end

		set(point, 'XData', marks(:,1));
		set(point, 'YData', marks(:,2));
		set(point, 'ZData', marks(:,3));

        %set(ray, 'UData', rays(:,4));
		%set(ray, 'VData', rays(:,5));
		%set(ray, 'WData', rays(:,6));
		%set(ray, 'XData', rays(:,1));
		%set(ray, 'YData', rays(:,2));
		%set(ray, 'ZData', rays(:,3));
		%set(ray, 'AutoScale', 'off');
        %set(ray, 'AutoScaleFactor', 5000);

		% fixed point calculations

		c = ones(size(marks(:,1)));

		coeff = [marks(:,1) marks(:,2) c]\marks(:,3);

		normal = [-coeff(1) -coeff(2) 1];
	 	normal = normal./norm(normal);
		
		centroid = [mean(marks(:,1)) mean(marks(:,2)) mean(marks(:,3))];

		forwardpoint = [marks(1,1) marks(1,2) marks(1,3)];
		dist = dot(centroid - forwardpoint, normal) / dot(-normal, normal);
		forwardpoint = forwardpoint - normal * dist;

		xaxis = forwardpoint - centroid;
		xaxis = xaxis./norm(xaxis);
		yaxis = cross(normal, xaxis);

		rotmat = [xaxis; yaxis; normal];
		
		fixedmarks = [ 1*scale 0 0; 0 1*scale 0; -1*scale 0 0; 0 -1*scale 0];

		for i = 1:4
			fixedmarks(i,:) = centroid + fixedmarks(i,:)*rotmat;	
		end
		
		set(fixedpoint, 'XData', fixedmarks(:,1));
		set(fixedpoint, 'YData', fixedmarks(:,2));
		set(fixedpoint, 'ZData', fixedmarks(:,3));

		% plot3(centroid(1), centroid(2), centroid(3), 'b.', 'MarkerSize', 10);

		drawnow;
	end

	function offset = updatePosition(s)
		% blah
	end

	function offset = updateOrientation(s)
		%blah
    end

    avg_points = 1;
    cur_point = 0;
    moving_avg = zeros(4,3,avg_points);

	point = 0;
	ray = 0;
	fixedpoint = 0;

	% scale of square (mm) 
	scale = 230;
    
	sock = tcpip('localhost', 12346);
	%sock = tcpip('10.42.0.1', 12346);
	set(sock, 'InputBufferSize', 1024*100, 'Terminator', '', 'ByteOrder', 'littleEndian');
	sock.BytesAvailableFcnMode = 'byte';
	sock.BytesAvailableFcnCount = 1024;
	sock.BytesAvailableFcn = @handleMessage;
	fopen(sock);

end
