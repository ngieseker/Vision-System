function [t centroid pose m1 m2 m3 m4] = read_log(logfile)
	data = load(logfile);

	t  = data(:, 1);
	m1 = data(:, [2 3 4]);
	m2 = data(:, [5 6 7]);
	m3 = data(:, [8 9 10]);
	m4 = data(:, [11 12 13]);

	centroid = [mean(data(:, [2 5 8 11]), 2)...
	            mean(data(:, [3 6 9 12]), 2)...
	            mean(data(:, [4 7 10 13]), 2)];
            
    tnn = ~isnan(centroid(:,1));
    tnn(1) = 0;
    
    t = t(tnn);
    centroid = centroid(tnn,:);
    m1 = m1(tnn,:);
    m2 = m2(tnn,:);
    m3 = m3(tnn,:);
    m4 = m4(tnn,:);
    
    % Calculates marker difffffferencess lawl yeah 
    dist_12  = sqrt(sum((m1-m2).^2,2));
    mean_12  = mean(dist_12);
    std_12   = std(dist_12);
    range_12 = range(dist_12);
    
    dist_23  = sqrt(sum((m2-m3).^2,2));
    mean_23  = mean(dist_23);
    std_23   = std(dist_23);
    range_23 = range(dist_23);
    
    dist_34  = sqrt(sum((m3-m4).^2,2));
    mean_34  = mean(dist_34);
    std_34   = std(dist_34);
    range_34 = range(dist_34);
    
    dist_41  = sqrt(sum((m4-m1).^2,2));
    mean_41  = mean(dist_41);
    std_41   = std(dist_41);
    range_41 = range(dist_41);
    
    dist_13  = sqrt(sum((m1-m3).^2,2));
    mean_13  = mean(dist_13);
    std_13   = std(dist_13);
    range_13 = range(dist_13);
    
    dist_24  = sqrt(sum((m2-m4).^2,2));
    mean_24  = mean(dist_24);
    std_24   = std(dist_24);
    range_24 = range(dist_24);
        
    %calculates pose lawl yeah
    pose = zeros(size(t,1),3);
    for i = 1:size(t,1)
        marks = [m1(i,:); m2(i,:); m3(i,:); m4(i,:)];
        c = ones(size(marks(:,1)));

        coeff = [marks(:,1) marks(:,2) c]\marks(:,3);

        normal = [-coeff(1) -coeff(2) 1];
        normal = normal./norm(normal);

        forwardpoint = [marks(1,1) marks(1,2) marks(1,3)];
        dist = dot(centroid(i,:) - forwardpoint, normal) / dot(-normal, normal);
        forwardpoint = forwardpoint - normal * dist;

        xaxis = forwardpoint - centroid(i,:);
        xaxis = xaxis./norm(xaxis);
        yaxis = cross(normal, xaxis);

        rotmat = [xaxis; yaxis; normal];

        pose(i,:) = [atan2(rotmat(3,2), rotmat(3,3)) ...
                     atan2(-rotmat(3,1), sqrt(rotmat(3,2)^2 + rotmat(3,3)^2)) ...
                     atan2(rotmat(2,1), rotmat(1,1))];
    end

    fprintf('Markers 1,2: mean:%16.6f std:%16.6f range:%16.6f\n', mean_12, std_12, range_12);
    fprintf('Markers 2,3: mean:%16.6f std:%16.6f range:%16.6f\n', mean_23, std_23, range_23);
    fprintf('Markers 3,4: mean:%16.6f std:%16.6f range:%16.6f\n', mean_34, std_34, range_34);
    fprintf('Markers 4,1: mean:%16.6f std:%16.6f range:%16.6f\n', mean_41, std_41, range_41);
    fprintf('Markers 1,3: mean:%16.6f std:%16.6f range:%16.6f\n', mean_13, std_13, range_13);
    fprintf('Markers 2,4: mean:%16.6f std:%16.6f range:%16.6f\n', mean_24, std_24, range_24);
end
