function rotMatrix = eulerAngles2rotationMatrix(rotAngles)

% rotMatrix = eulerAngles2rotationMatrix(rotAngles)
%
% This function converts rotation angles into rotation matrix
%
% rotAngles : [rx ry rz] in degrees
% rotMatrix : data(3xN)*R format
%
% Code written and all rights reserved by Dr. Ajmal Mian
%
% Copyright : This code is written by Ajmal Saeed Mian {ajmal@csse.uwa.edu.au}
%              Computer Science, The University of Western Australia. The code
%              may be used, modified and distributed for research purposes with
%              acknowledgement of the author and inclusion this copyright information.
%
% Disclaimer : This code is provided as is without any warrantly.

psi = deg2rad(rotAngles(1));
theta = deg2rad(rotAngles(2));
phi = deg2rad(rotAngles(3));

rotMatrix = [cos(theta)*cos(phi) sin(psi)*sin(theta)*cos(phi)-cos(psi)*sin(phi) cos(psi)*sin(theta)*cos(phi)+sin(psi)*sin(phi); ...
             cos(theta)*sin(phi) sin(psi)*sin(theta)*sin(phi)+cos(psi)*cos(phi) cos(psi)*sin(theta)*sin(phi)-sin(psi)*cos(phi); ...
             -sin(theta) sin(psi)*cos(theta) cos(psi)*cos(theta)];

rotMatrix = rotMatrix'; 