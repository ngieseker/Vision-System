function analyze()
    % constants
    tdiff = 45.6828749771118;
    hdiff = 660-962;
    ltdiff = 2.61489703178404; 
	flight_start = 185;%151;
	flight_end = 286;
	
	xoff = 307.81914444;
	yoff = -275.1351105575;
	lxxoff=-2.7771608366;
	lyyoff=-11.5975814625;
    
	fig_idx = 1000;
    
    
    % read MAAV
    rawdata = load('07.16.12.00.28');

    mt = rawdata(:,1);
    mt = mt-mt(1);
    mr = -rad2deg(rawdata(:,2));
    mp = rad2deg(rawdata(:,7));
    my = rad2deg(rawdata(:,12));
    mh = rawdata(:,17).*1000;
    
    %READ ASS DATA
    rawdata = load('pose_data_28.log');
    lt = rawdata(:,1)/1000000;
    lt = lt-lt(1);
    lxx = rawdata(:,2)*1000;
    lyy = rawdata(:,3)*1000;
    lr = -rad2deg(rawdata(:,5));
    lp = rad2deg(rawdata(:,6));
    ly = -rad2deg(rawdata(:,7));

	lt = lt+ltdiff;
    
    %read our data
    [t centroid pose m1 m2 m3 m4] = read_log('2012.07.15-20.29.21.log');
    t = t-t(1);
    t = t+tdiff;
    
    r = rad2deg(pose(:,1));
    p = rad2deg(pose(:,2));
    y = rad2deg(pose(:,3));
    h = centroid(:,3)+hdiff;
    
    r = r-mean(r)+mean(mr);
    p = p-mean(p)+mean(mp);
    y = y-mean(y)+mean(ly);
    
	xx = centroid(:,1);
	yy = centroid(:,2);
	
	r = sgolayfilt(r, 3, 11);
	p = sgolayfilt(p, 3, 11);
	y = sgolayfilt(y, 3, 11);
  
    %calc flight range
    hr = and(t>=flight_start, t<=flight_end);  
    mrange = and(mt>=flight_start, mt<=flight_end); 
    lrange = and(lt>=flight_start, lt<=flight_end); 
    t = t(hr);
    mt = mt(mrange);
    lt = lt(lrange);	
    mhr = and(mt>=152.936, mt<=283.656);
    
    r = r(hr);
    p = p(hr);
    y = y(hr);
    h = h(hr);
   	
	xx = xx(hr);
	yy = yy(hr);

	xx = xx-xoff;
    yy = yy-yoff;

    mr = mr(mrange);
    mp = mp(mrange);
    my = my(mrange);
    mh = mh(mrange);

	lxx = lxx(lrange);
	lyy = lyy(lrange);
	ly = ly(lrange);

	lxx = lxx-lxxoff;
	lyy = lyy-lyyoff;

	% transform
	transfrom = [13.5149  -23.9231   40.0000];
	%interpolate
    ti = min(t(1),mt(1)):1/1000:max(t(end), mt(end));
    
    ri = interp1(t, r, ti, 'linear', 0);
    mri = interp1(mt, mr, ti, 'linear', 0);
    
    pi = interp1(t, p, ti, 'linear');
    mpi = interp1(mt, mp, ti, 'linear');
    
    yi = interp1(t, y, ti, 'linear');
    lyi = interp1(lt, ly, ti, 'linear');

	xxi = interp1(t, xx, ti, 'linear');
    lxxi = interp1(lt, lxx, ti, 'linear');

	yyi = interp1(t, yy, ti, 'linear');
	lyyi = interp1(lt, lyy, ti, 'linear');

    hti = t(150):1/1000:t(end-150);
	hti = ti;
    hi = interp1(t, h, hti, 'linear');
    mhi = interp1(mt(mhr), mh(mhr), hti, 'linear');


	% transform XYYYYYYYYYYYYYYYZ
	tempsomething = [xxi-lxxi; yyi-lyyi; repmat(100, size(xxi));]';
	
	for i = 1:size(tempsomething,2)
		rotmat = eulerAngles2rotationMatrix([mri(i) mpi(i) yi(i)]);
		tempsomething(i,:) = (rotmat*tempsomething(i,:)')';
	end
	
	tempsomething = tempsomething(and(~isnan(tempsomething(:,1)), ~isnan(tempsomething(:,2))), :);
	transfrom = mean(tempsomething,1);

	for i = 1:size(xxi,2)
		rotmat = eulerAngles2rotationMatrix([mri(i) mpi(i) yi(i)]);
		tp = inv(rotmat) * transfrom';
		tp = tp*.8;
		xxi(i) = xxi(i) - tp(1);
		yyi(i) = yyi(i) - tp(2);
		%hi(i) =  hi(i) - tp(3);
	end

	%xxi = interp1(t, xx, ti, 'linear');
	%yyi = interp1(t, yy, ti, 'linear');
    %hi = interp1(t, h, hti, 'linear');

	%plot shit here
    figure (fig_idx);
        plot(t, r, 'b');
        hold on;
        plot(mt, mr, 'r');
        hold off;
        title('Roll');
    fig_idx = fig_idx+1;
    
    figure (fig_idx);
        plot(t, p, 'b');
        hold on;
        plot(mt, mp, 'r');
        hold off;
        title('Pitch');
    fig_idx = fig_idx+1;

    figure (fig_idx);
        plot(t, y, 'b');
        hold on;
        plot(lt, ly, 'r');
        hold off;
        title('Yaw');
    fig_idx = fig_idx+1;
    
    figure (fig_idx);
        plot(t, h, 'b');
        hold on;
        plot(mt(mhr), mh(mhr), 'r');
        hold off;
        title('Height');
    fig_idx = fig_idx+1;
   
    figure (fig_idx);
        plot(t, xx, 'b');
        hold on;
        plot(lt, lxx, 'r');
        hold off;
        title('X');
    fig_idx = fig_idx+1;

    figure (fig_idx);
        plot(t, yy, 'b');
        hold on;
        plot(lt, lyy, 'r');
        hold off;
        title('Y');
    fig_idx = fig_idx+1;
    
	figure (fig_idx);
        plot(xxi, yyi, 'b');
        hold on;
        plot(lxx, lyy, 'r');
        hold off;
        title('XY');
    fig_idx = fig_idx+1;

    %plot data comp
    figure (fig_idx);
        rdiff = abs(ri-mri);
		temp = ~isnan(rdiff);
		rdiff = rdiff(temp);
		l_ti = ti(temp);
        plot(l_ti, rdiff);
        title('Roll Comp');
        fprintf('Roll Comp: mean:%16.6f std:%16.6f range:%16.6f\n', mean(rdiff), std(rdiff), range(rdiff));
    fig_idx = fig_idx+1;
    
    figure (fig_idx);
        pdiff = abs(pi-mpi);
		temp = ~isnan(pdiff);
		pdiff = pdiff(temp);
		l_ti = ti(temp);
        plot(l_ti, pdiff);
        title('Pitch Comp');
        fprintf('Pitch Comp: mean:%16.6f std:%16.6f range:%16.6f\n', mean(pdiff), std(pdiff), range(pdiff));
    fig_idx = fig_idx+1;

    figure (fig_idx);
        ydiff = abs(yi-lyi);
		temp = ~isnan(ydiff);
		ydiff = ydiff(temp);
		l_ti = ti(temp);
        plot(l_ti, ydiff);
        title('Yaw Comp');
        fprintf('Yaw Comp: mean:%16.6f std:%16.6f range:%16.6f\n', mean(ydiff), std(ydiff), range(ydiff));
    fig_idx = fig_idx+1;
    
    figure (fig_idx);
        xxdiff = abs(xxi-lxxi);
		temp = ~isnan(xxdiff);
		xxdiff = xxdiff(temp);
		l_ti = ti(temp);
        plot(l_ti, xxdiff);
        title('X Comp');
        fprintf('X Comp: mean:%16.6f std:%16.6f range:%16.6f\n', mean(xxdiff), std(xxdiff), range(xxdiff));
    fig_idx = fig_idx+1;
    
    figure (fig_idx);
        yydiff = abs(yyi-lyyi);
		temp = ~isnan(yydiff);
		yydiff = yydiff(temp);
		l_ti = ti(temp);
        plot(l_ti, yydiff);
        title('Y Comp');
        fprintf('Y Comp: mean:%16.6f std:%16.6f range:%16.6f\n', mean(yydiff), std(yydiff), range(yydiff));
    fig_idx = fig_idx+1;

    figure (fig_idx);
        hdiff = abs(hi-mhi);
		temp = ~isnan(hdiff);
        hdiff = hdiff(temp);
		l_ti = hti(temp);
        plot(l_ti, hdiff);
        title('Height Comp');
        fprintf('Height Comp: mean:%16.6f std:%16.6f range:%16.6f\n', mean(hdiff), std(hdiff), range(hdiff));
    fig_idx = fig_idx+1;
    

    hi = interp1(t, h, ti, 'linear');
    mhi = interp1(mt(mhr), mh(mhr), ti, 'linear');

	figure (fig_idx);
        plot3(xxi, yyi, hi, 'b');
        hold on;
        plot3(lxxi, lyyi, mhi, 'r');
        hold off;
		axis equal;
		grid on;
        title('XY');
    fig_idx = fig_idx+1;

end
