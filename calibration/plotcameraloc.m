squaresx = 10;
squaresy = 14;
width = 16;

cd cam2;
load('cam2.mat');
K2 = K;
c2r = cr;
c2t = ct;

cd ../cam3;
load('cam3.mat');
K3 = K;
c3r = cr;
c3t = ct;

cd ../cam4;
load('cam4.mat');
K4 = K;
c4r = cr;
c4t = ct;
cd ..;

point = [1024/2; 768/2; 1];

n = [0;0;1];
o = [0;0;0];

c2pos = -inv(c2r)*c2t;
c2rot = inv(c2r)*inv(K2)*point;
c2rotn = c2rot ./ norm(c2rot);
c3pos = -inv(c3r)*c3t;
c3rot = inv(c3r)*inv(K3)*point;
c3rotn = c3rot ./ norm(c3rot);
c4pos = -inv(c4r)*c4t;
c4rot = inv(c4r)*inv(K4)*point;
c4rotn = c4rot ./ norm(c4rot);

quiver3(c2pos(1), c2pos(2), c2pos(3), c2rotn(1), c2rotn(2), c2rotn(3), dot(o-c2pos,n)/dot(c2rotn,n), 'b');
hold on;
quiver3(c3pos(1), c3pos(2), c3pos(3), c3rotn(1), c3rotn(2), c3rotn(3), dot(o-c3pos,n)/dot(c3rotn,n), 'g');
quiver3(c4pos(1), c4pos(2), c4pos(3), c4rotn(1), c4rotn(2), c4rotn(3), dot(o-c4pos,n)/dot(c4rotn,n), 'r');


point = [0; 0; 1];
c2cone = inv(c2r)*inv(K2)*point;
c2conen = c2cone./norm(c2cone);
quiver3(c2pos(1), c2pos(2), c2pos(3), c2conen(1), c2conen(2), c2conen(3), dot(o-c2pos,n)/dot(c2conen,n), ':b', 'LineWidth', 2);
cor(1,:) = c2conen*dot(o-c2pos,n)/dot(c2conen,n)+c2pos;
cor(5,:) = c2conen*dot(o-c2pos,n)/dot(c2conen,n)+c2pos;
point = [0; 768; 1];
c2cone = inv(c2r)*inv(K2)*point;
c2conen = c2cone./norm(c2cone);
quiver3(c2pos(1), c2pos(2), c2pos(3), c2conen(1), c2conen(2), c2conen(3), dot(o-c2pos,n)/dot(c2conen,n), ':b', 'LineWidth', 2);
cor(2,:) = c2conen*dot(o-c2pos,n)/dot(c2conen,n)+c2pos;
point = [1024; 768; 1];
c2cone = inv(c2r)*inv(K2)*point;
c2conen = c2cone./norm(c2cone);
quiver3(c2pos(1), c2pos(2), c2pos(3), c2conen(1), c2conen(2), c2conen(3), dot(o-c2pos,n)/dot(c2conen,n), ':b', 'LineWidth', 2);
cor(3,:) = c2conen*dot(o-c2pos,n)/dot(c2conen,n)+c2pos;
point = [1024; 0; 1];
c2cone = inv(c2r)*inv(K2)*point;
c2conen = c2cone./norm(c2cone);
quiver3(c2pos(1), c2pos(2), c2pos(3), c2conen(1), c2conen(2), c2conen(3), dot(o-c2pos,n)/dot(c2conen,n), ':b', 'LineWidth', 2);
cor(4,:) = c2conen*dot(o-c2pos,n)/dot(c2conen,n)+c2pos;
plot3(cor(:,1), cor(:,2), cor(:,3), '--b');

point = [0; 0; 1];
c3cone = inv(c3r)*inv(K2)*point;
c3conen = c3cone./norm(c3cone);
quiver3(c3pos(1), c3pos(2), c3pos(3), c3conen(1), c3conen(2), c3conen(3), dot(o-c3pos,n)/dot(c3conen,n), ':g', 'LineWidth', 2);
cor(1,:) = c3conen*dot(o-c3pos,n)/dot(c3conen,n)+c3pos;
cor(5,:) = c3conen*dot(o-c3pos,n)/dot(c3conen,n)+c3pos;
point = [0; 768; 1];
c3cone = inv(c3r)*inv(K2)*point;
c3conen = c3cone./norm(c3cone);
quiver3(c3pos(1), c3pos(2), c3pos(3), c3conen(1), c3conen(2), c3conen(3), dot(o-c3pos,n)/dot(c3conen,n), ':g', 'LineWidth', 2);
cor(2,:) = c3conen*dot(o-c3pos,n)/dot(c3conen,n)+c3pos;
point = [1024; 768; 1];
c3cone = inv(c3r)*inv(K2)*point;
c3conen = c3cone./norm(c3cone);
quiver3(c3pos(1), c3pos(2), c3pos(3), c3conen(1), c3conen(2), c3conen(3), dot(o-c3pos,n)/dot(c3conen,n), ':g', 'LineWidth', 2);
cor(3,:) = c3conen*dot(o-c3pos,n)/dot(c3conen,n)+c3pos;
point = [1024; 0; 1];
c3cone = inv(c3r)*inv(K2)*point;
c3conen = c3cone./norm(c3cone);
quiver3(c3pos(1), c3pos(2), c3pos(3), c3conen(1), c3conen(2), c3conen(3), dot(o-c3pos,n)/dot(c3conen,n), ':g', 'LineWidth', 2);
cor(4,:) = c3conen*dot(o-c3pos,n)/dot(c3conen,n)+c3pos;
plot3(cor(:,1), cor(:,2), cor(:,3), '--g');

point = [0; 0; 1];
c4cone = inv(c4r)*inv(K2)*point;
c4conen = c4cone./norm(c4cone);
quiver3(c4pos(1), c4pos(2), c4pos(3), c4conen(1), c4conen(2), c4conen(3), dot(o-c4pos,n)/dot(c4conen,n), ':r', 'LineWidth', 2);
cor(1,:) = c4conen*dot(o-c4pos,n)/dot(c4conen,n)+c4pos;
cor(5,:) = c4conen*dot(o-c4pos,n)/dot(c4conen,n)+c4pos;
point = [0; 768; 1];
c4cone = inv(c4r)*inv(K2)*point;
c4conen = c4cone./norm(c4cone);
quiver3(c4pos(1), c4pos(2), c4pos(3), c4conen(1), c4conen(2), c4conen(3), dot(o-c4pos,n)/dot(c4conen,n), ':r', 'LineWidth', 2);
cor(2,:) = c4conen*dot(o-c4pos,n)/dot(c4conen,n)+c4pos;
point = [1024; 768; 1];
c4cone = inv(c4r)*inv(K2)*point;
c4conen = c4cone./norm(c4cone);
quiver3(c4pos(1), c4pos(2), c4pos(3), c4conen(1), c4conen(2), c4conen(3), dot(o-c4pos,n)/dot(c4conen,n), ':r', 'LineWidth', 2);
cor(3,:) = c4conen*dot(o-c4pos,n)/dot(c4conen,n)+c4pos;
point = [1024; 0; 1];
c4cone = inv(c4r)*inv(K2)*point;
c4conen = c4cone./norm(c4cone);
quiver3(c4pos(1), c4pos(2), c4pos(3), c4conen(1), c4conen(2), c4conen(3), dot(o-c4pos,n)/dot(c4conen,n), ':r', 'LineWidth', 2);
cor(4,:) = c4conen*dot(o-c4pos,n)/dot(c4conen,n)+c4pos;
plot3(cor(:,1), cor(:,2), cor(:,3), '--r');


plot3(c2pos(1), c2pos(2), c2pos(3),'-b.', 'MarkerSize', 25);
plot3(c3pos(1), c3pos(2), c3pos(3),'-g.', 'MarkerSize', 25);
plot3(c4pos(1), c4pos(2), c4pos(3),'-r.', 'MarkerSize', 25);

cor = [0, 0, 0;
	   0, 16*14, 0;
	   16*10, 16*14, 0;
	   16*10, 0, 0;
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
plot3(0,0,0, '-r.', 'MarkerSize', 15);

zl = zlim;
zlim([0 zl(2)]);
axis(gca,'vis3d');
grid on;
hold off;
