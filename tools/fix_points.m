function [ Pout ] = fix_points( Pin )
    ax1 = Pin(2,:) - Pin(1,:);
    ax2 = -(Pin(4,:) - Pin(1,:));
    ax3 = -cross(ax1, ax2);
    
    R = [ax1/norm(ax1); ax2/norm(ax2); ax3/norm(ax3)]';
    [U D V] = svd(R);
    R = inv(U * V');
    
    Pout = Pin;
    for p = 1:size(Pin,1)
        Pout(p,:) = (R * Pout(p,:)')';
    end
    Pout = Pout - repmat(Pout(1,:), size(Pin,1), 1);
    
    %plot3(Pin(:,1), Pin(:,2), Pin(:,3), 'r.');
    %hold on;
    plot3(Pout(:,1), Pout(:,2), Pout(:,3), 'b.');
    grid on;
    axis equal;
end

