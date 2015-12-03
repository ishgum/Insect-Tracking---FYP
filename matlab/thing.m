%%
% Feel free to edit
% Todo:
% Add row for min/max focal length
% Add delay due to framerate
%%
%clc
%clear
%close all;
format shortg
%disp('                                           New Run')
%% iterator

%for(
%% Consts
% you can break this by making the matrix singular for some vals
Cost_resolution = 0.00005 ;    %arb, seconds per pixel, below <0.0002 works
Uav_lag = 0.2 ;         % seconds
Bug_speed = 5        ;   % ms-1
Min_px = 5         ;    % one dimension of bug in image, in px
Bug_size = 0.005    ;     % one dim of bug in reality, m
Min_dst = 10; %m
% Max_dst = 10
FOS = 2;                    % fos 2 = image is twice what bug can move
                            % fos 4 = bug can only move halfway to edge
Shutter_lag = 1/25; %include in total somewhere.
Fix_res = 2048;%400;%2048;
Fix_lag =1;%1/25;
Fix_bug_px = 5;
%% Focal_length
               %fl  hfov  vfov
focal_lookup = [10,  66.8, 47.5
                11,  61.9, 43.6
                12,  57.6, 40.3
                14,  50.5, 34.9
                15,  47.5, 32.7
                17,  42.4, 29
                18,  40.3, 27.5
                19,  38.3, 26.1
                20,  36.5, 24.8
                24,  30.8, 20.8
                28,  26.5, 17.9
                30,  24.8, 16.7
                35,  21.4, 14.3
                45,  16.7, 11.2
                50,  15,   10.1
                55,  13.7, 9.1
                60,  12.6, 8.4
                70,  10.8, 7.2
                75,  10.1, 6.7
                80,  9.4,  6.3
                85,  8.9,  5.9
                90,  8.4,  5.6
                100, 7.6,  5
                105, 7.2,  4.8
                120, 6.3,  4.2
                125, 6,    4
                135, 5.6,  3.7
                150, 5,    3.4
                170, 4.4,  3
                180, 4.2,  2.8
                200, 3.8,  2.5
                210, 3.6,  2.4
                300, 2.5,  1.7
                400, 1.9,  1.3
                500, 1.5,  1
                600, 1.3,  0.8
                800, 0.9,  0.6];

%% debug
% 1/Bug_size
% %Bug_speed/Cost_resolution
% Min_px*Bug_speed
% Bug_size
%fprintf('cost %10.5i',Cost_resolution)
%% Hacking for nonlin
%  Width = Res * Bug_size / Min_px
%% System of eqns
% Processing_lag = Cost_resolution * Resolution
% Lag = Processing_lag + Uav_lag
% Width = 2*FOS*(Bug_speed*Lag)
% Resolution = Min_px * Width / Bug_size

A = [1 0 0 -Cost_resolution
    -1 1 0 0
    0 -FOS*Bug_speed 1 0
    0 0 -Min_px/Bug_size 1];
b = [0
    Uav_lag
    Bug_size
    0];
%Det_A = det(A)
C = A\b;
FOV = 2*atand(C(3)/(2*Min_dst));
[junk Focal_index] = min(abs(focal_lookup(:,3)-FOV));
Meet_Constraints = [C;Min_px;Bug_speed*C(2);Bug_speed*100*C(2)/C(3);...
    Min_dst;FOV; focal_lookup(Focal_index,1)];

%% Processing_lag = C(1)
% Total_lag = C(2)
% Width_of_image_in_m = C(3)
% Resolution_one_side_in_px = C(4)

%% Adjust resolution manually
A2 = [1 0 0 -Cost_resolution
    -1 1 0 0
    0 -FOS*Bug_speed 1 0
    0 0 0 1];
b2 = [0
    Uav_lag
    Bug_size % or 0, doesn't make much difference
    Fix_res];
%Det_A2 = det(A2)
C2 = A2\b2;
FOV = 2*atand(C2(3)/(2*Min_dst));
[junk Focal_index] = min(abs(focal_lookup(:,3)-FOV));
Max_resolution = [C2;C2(4)*Bug_size/C2(3);Bug_speed*C2(2);...
    Bug_speed*100*C2(2)/C2(3);Min_dst;FOV;focal_lookup(Focal_index,1)];
%% Adjust FPS manually
A3 = [1 0 0 0
    0 1 0 0
    0 -FOS*Bug_speed 1 0
    0 0 -Min_px/Bug_size 1];
b3 = [0
    Fix_lag
    Bug_size
    0];
%Det_A2 = det(A2)
C3 = A3\b3;
FOV = 2*atand(C3(3)/(2*Min_dst));
[junk Focal_index] = min(abs(focal_lookup(:,3)-FOV));
Fixed_lag = [C3;C3(4)*Bug_size/C3(3);Bug_speed*C3(2);...
    Bug_speed*100*C3(2)/C3(3);Min_dst;FOV;focal_lookup(Focal_index,1)];
%% Fix two things
A4 = [1 0 0 0
    0 1 0 0
    0 -FOS*Bug_speed 1 0
    0 0 0 1];
b4 = [0
    Uav_lag
    Bug_size
    Fix_res];
%Det_A2 = det(A2)
C4 = A4\b4;
C4 = [0;0;8.192;2048];
FOV = 2*atand(C4(3)/(2*Min_dst));
[junk Focal_index] = min(abs(focal_lookup(:,2)-FOV));
Two = [C4;C4(4)*Bug_size/C4(3);Bug_speed*C4(2);...
    Bug_speed*100*C4(2)/C4(3);Min_dst;FOV;focal_lookup(Focal_index,1)];
%% disp('                                       With fixed')
% Processing_lag = C(1)
% Total_lag = C(2)
% Width_of_image_in_m = C(3)
% Resolution_one_side_in_px = C(4)
% Resolution_one_side_in_px*Bug_size/Width_of_image_in_m
%% Camera deets
% using pythag

%% Create table

row_names = {'Processing_lag (s)';'Total lag (proc + UAV) (s)';...
    'Width of image (m)';'Resolution (px)';...
    'Minimum bug (px)'; 'Distance moved during lag (m)';...
    'Distance moved during lag (%)'; 'Bug-Camera Distance (m)';...
    'FOV at dst (deg)'; 'Suggested Focal length at dst'};
T = table(Meet_Constraints,Max_resolution,Fixed_lag,Two,'RowNames', row_names)
